package data_structures.implementation;

import data_structures.Sorted;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class FineGrainedTree<T extends Comparable<T>> implements Sorted<T> {
    // Dummy node for locking purposes, the actual root is head.left
    private Node head = new Node(null);

	public void add(T t) {
        Node curr, pred = head, next;
        boolean left;

        head.lock();
        try {
            if(head.left == null) {
                // No head is available yet.
                head.left = new Node(t);
            } else { // Traverse the tree until a free leaf is found.
                curr = head.left;
                curr.lock();
                try {
                    while(true) {
                        /* Determine whether to move right or left */
                        left = curr.compareTo(t) >= 0;
                        next = left ? curr.left : curr.right;
                        if(next == null) {
                            // No leaf node yet, item is new leaf.
                            if(left)
                                curr.left = new Node(t);
                            else
                                curr.right = new Node(t);
                            return;
                        } else { // There is a subtree. Continue traversal.
                            pred.unlock();
                            pred = curr;
                            curr = next;
                        }
                        curr.lock();
                    }
                } finally {
                    curr.unlock();
                }
            }
        } finally {
            pred.unlock();
        }
    }

    public void remove(T t) {
        Node curr, pred = head;

        head.lock();
        try {
            curr = head.left;

            if(curr == null) {
                return;
            }

            // Traverse the tree to find the node to remove
            curr.lock();
            try {
                while(true) {
                    if(curr.compareTo(t) == 0) {
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
                    } else { // Element not yet found, continue traversal.
                        pred.unlock();
                        pred = curr;
                        curr = curr.compareTo(t) >= 0 ? curr.left : curr.right;

                        if(curr == null)
                            return;

                        curr.lock();
                    }
                }
            } finally {
                if(curr != null)
                    curr.unlock();
            }
        } finally {
            pred.unlock();
        }
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

        curr.lock();
        try {
            while(true) { //Find the node with the smallest value in the (sub)tree.
                if(curr.left != null) { // There is a left child, continue traverse
                    pred = curr;
                    curr = curr.left;
                    curr.lock();
                    pred.unlock();
                } else {
                    replaceNodeWith(curr, pred, curr.right);
                    return curr.value;
                }
            }
        } finally {
            curr.unlock();
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
        // Node is not head. Replacement now replaces
        // current node as child of parent.
        if(parent.value == null || curr == parent.left) {
            // Left child of parent.
            parent.left = replacement;
        } else {
            // Right child of parent.
            parent.right = replacement;
        }
    }

    public String toString() {
        return head.isLeaf() ? "(null)" : head.left.stringify(Node.TABSIZE);
    }

    class Node implements Comparable<T> {
        final static int TABSIZE = 4;
        T value = null;
        Node left = null, right = null;
        Lock lock = new ReentrantLock();

        public Node(T t) {
            value = t;
        }

        public int compareTo(T t) {
            return value.compareTo(t);
        }

        public void lock() {
            lock.lock();
        }

        public void unlock() {
            lock.unlock();
        }

        public boolean isLeaf() {
            return left == null && right == null;
        }

        public String stringify() {
            return stringify(TABSIZE);
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
