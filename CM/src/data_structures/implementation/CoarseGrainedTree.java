package data_structures.implementation;

import data_structures.Sorted;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class CoarseGrainedTree<T extends Comparable<T>> implements Sorted<T> {
    private Node head = null;
    private Lock lock = new ReentrantLock();

    public void add(T t) {
        Node curr;

        lock.lock();
        try {
            if(head == null) {
                // No head is available yet.
                head = new Node(t);
            } else { // Traverse the tree until a free leaf is found.
                curr = head;
                while(true) {
                    if(curr.compareTo(t) >= 0) {
                         // Smaller than or equal to the current node. New item
                         // has to go on the left.
                        if(curr.left == null) {
                            // No leaf node yet, item is new leaf.
                            curr.left = new Node(t);
                            return;
                        } else { // There is a subtree. Continue traversal.
                            curr = curr.left;
                        }
                    } else {
                        // Larger than the current node. Item goes on the right.
                        if(curr.right == null) {
                            // No leaf node yet, item is new leaf.
                            curr.right = new Node(t);
                            return;
                        } else { // There is a subtree. Continue traversal.
                            curr = curr.right;
                        }
                    }
                }
            }
        } finally {
            lock.unlock();
        }
    }

    public void remove(T t) {
        Node curr, pred = null;

        lock.lock();
        try {
            // Traverse the tree to find the node to remove
            curr = head;
            while(true) {
                if(curr == null) {
                    // Element is not in tree
                    return;
                } else if(curr.compareTo(t) == 0) {
                    // Element found, now remove
                    if(curr.left != null && curr.right != null) {
                        // Node has two children
                        T newValue = findAndRemoveMinimalValue(curr);
                        curr.value = newValue;
                    } else if(curr.left != null) {
                        // Node only has a left child
                        replaceNodeWith(curr, pred, curr.left);
                    } else if(curr.right != null) {
                        // Node only has a right child
                        replaceNodeWith(curr, pred, curr.right);
                    } else {
                        // Childless node, remove link from pred
                        replaceNodeWith(curr, pred, null);
                    }
                    return;
                } else {
                    // Element not yet found, continue traversal
                    pred = curr;
                    curr = curr.compareTo(t) >= 0 ? curr.left : curr.right;
                }
            }
        } finally {
            lock.unlock();
        }
    }

    public String toString() {
        return head == null ? "(null)" : head.stringify(Node.TABSIZE);
    }

    /**
     * Find the minimal value of the given subtree and remove it.
     *
     * The minimal value is the node found by only traversing left. For the
     * removal the parent of this subtree is needed as well.
     *
     * @param   Node    pred  The parent of the subtree.
     * @return  T             The item from the node that was just removed.
     */
    private T findAndRemoveMinimalValue(Node parent) {
        Node pred = parent, curr = parent.right;

        // Find the node with the smallest value in the (sub)tree.
        while(true) {
            if(curr.left != null) {
                parent = curr;
                curr = curr.left;
            } else {
                if(curr.right != null) {
                    // Node still has a right child.
                    replaceNodeWith(curr, parent, curr.right);
                } else {
                    // Node is childless.
                    replaceNodeWith(curr, parent, null);
                }

                return curr.value;
            }
        }
    }

    /*
     * Replace the current node with some replacement.
     *
     * @param   Node    curr        The node to replace.
     * @param   Node    parent      The parent of the current node.
     * @param   Node    replacement The node to use as replacement.
     */
    private void replaceNodeWith(Node curr, Node parent, Node replacement) {
        if(parent == null) {
            // Node is the current head, replacement is the new head
            head = replacement;
        } else {
            // Node is not the head, replacement now replaces current node as
            // child of parent
            if(curr == parent.left)
                parent.left = replacement;
            else
                parent.right = replacement;
        }
    }

    class Node implements Comparable<T> {
        final static int TABSIZE = 4;
        T value = null;
        Node left = null, right = null;

        public Node(T t) {
            value = t;
        }

        public int compareTo(T t) {
            return value.compareTo(t);
        }

        public boolean isLeaf() {
            return left == null && right == null;
        }

        public String stringify(int indent_length) {
            if(this.isLeaf())
                return value.toString();

            String indent = new String();

            for(int i = 0; i < indent_length; i++)
                indent += ' ';

            return String.format("%s\n%s< %s\n%s> %s", value, indent,
                left == null ? "(null)" : left.stringify(indent_length + TABSIZE),
                indent,
                right == null ? "(null)" : right.stringify(indent_length + TABSIZE));
        }
    }
}
