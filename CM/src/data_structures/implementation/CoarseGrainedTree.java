package data_structures.implementation;

import data_structures.Sorted;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class CoarseGrainedTree<T extends Comparable<T>> implements Sorted<T> {
    private Node head;
    private Lock lock = new ReentrantLock();
    private int size;

    public CoarseGrainedTree() {
        head = null;
        size = 0;
    }

    public void add(T t) {
        int key = t.hashCode();
        Node curr;

        lock.lock();
        try {
            if(head == null) {
                /* No head is available yet. */
                head = new Node(t);
                size++;
                return;
            }
            else {
                /* Traverse the tree until a free leaf is found. */
                curr = head;
                while(true) {
                    if(curr.key >= key) {
                        /*
                         * Smaller than or equal to the current node. New item
                         * has to go on the left.
                         */
                        if(curr.left == null) {
                            /* No leaf node yet, item is new leaf. */
                            curr.left = new Node(t);
                            size++;
                            return;
                        }
                        else {
                            /* There is a leaf node. Continue traversal. */
                            curr = curr.left;
                            continue;
                        }
                    }
                    else {
                    /* Larger than the current node. Item goes on the right. */
                        if(curr.right == null) {
                            /* No leaf node yet, item is new leaf. */
                            curr.right = new Node(t);
                            size++;
                            return;
                        }
                        else {
                            /* There is a leaf node. Continue traversal. */
                            curr = curr.right;
                            continue;
                        }
                    }
                }
            }
        }
        finally {
            lock.unlock();
        }
    }

    public void remove(T t) {
        int key = t.hashCode();
        Node curr, parent;

        lock.lock();
        try {
            /* Traverse the tree to find the node to remove. */
            parent = null;
            curr = head;
            while(true) {
                if(curr == null) {
                    /* Element not in tree. */
                    System.out.println("Element not found, skipping.");
                    return;
                }
                else if(curr.key == key) {
                    /* Element found, now remove. */
                    if(curr.left == null && curr.right == null) {
                        /* Childless node, remove link from parent */
                        removeChildlessNode(curr, parent);
                        return;
                    }
                    else if(curr.left != null && curr.right == null) {
                        /* Node has a left child. */
                        removeNodeWithLeftChild(curr, parent);
                        return;
                    }
                    else if(curr.left == null && curr.right != null) {
                        /* Node has a right child. */
                        removeNodeWithRightChild(curr, parent);
                        return;
                    }
                    else {
                        /* Node has two children. */
                        T newValue = findAndRemoveMinimalValue(curr.right,curr);
                        Node newCurr = new Node(newValue);
                        newCurr.left = curr.left;
                        newCurr.right = curr.right;
                        if(parent == null) {
                            /*
                             * Node is head. New head is lowest node larger than
                             * current head.
                             */
                            head = newCurr;
                        }
                        else {
                            /*
                             * The removed node was not the head. Attach the
                             * new subtree to the parent.
                             */
                            if(parent.key <= curr.key)
                                parent.right = newCurr;
                            else
                                parent.left = newCurr;
                        }
                        return;
                    }
                }
                else {
                    /* Element not yet found, continue traversal. */
                    parent = curr;
                    if(curr.key > key)
                        curr = curr.left;
                    else
                        curr = curr.right;
                }
            }
        }
        finally{
            lock.unlock();
        }
    }

    public String toString() {
        return "CGT - size: " + size + " - Head: " + head;
    }

    /*
     * Find the minimal value of the given subtree and remove it. The minimal
     * value is the node found by only traversing left. For the removal the
     * parent of this subtree is needed as well.
     *
     * @param   Node    tree    The head of the subtree to traverse.
     * @param   Node    parent  The parent of the subtree.
     * @return  T               The item from the node that was just removed.
     */
    private T findAndRemoveMinimalValue(Node tree, Node subtreeParent) {
        Node curr, parent;
        T t;

        parent = subtreeParent;
        curr = tree;
        while(true) {
            /* Find the node with the smallest value in the (sub)tree. */
            if(curr.left != null) {
                parent = curr;
                curr = curr.left;
            }
            else {
                t = curr.value;
                if(curr.right != null) {
                    /* Node still has a right child. */
                    removeNodeWithRightChild(curr, parent);
                    return t;
                }
                else {
                    /* Node is childless. */
                    removeChildlessNode(curr, parent);
                    return t;
                }
            }
        }
    }

    /*
     * Remove a node that has no children. This is done by removing its
     * reference from the parent node.
     *
     * @param   Node    curr    The node to remove.
     * @param   Node    parent  The parent of the node that was just removed.
     */
    private void removeChildlessNode(Node curr, Node parent) {
        if(parent == null) {
            /* Current node is head. */
            head = null;
            size--;
            return;
        }
        else {
            if(curr.key <= parent.key) {
                /* Left child of parent. */
                parent.left = null;
                size--;
                return;
            }
            else {
                /* Right child of parent. */
                parent.right = null;
                size--;
                return;
            }
        }
    }

    /*
     * Remove a node that has only a left child. This is done by changing its
     * reference in the parent node to that of its child.
     *
     * @param   Node    curr    The node to remove.
     * @param   Node    parent  The parent of the node that was just removed.
     */
    private void removeNodeWithLeftChild(Node curr, Node parent) {
        if(parent == null) {
            /* Node is head. Child is new head. */
            head = curr.left;
            size--;
            return;
        }
        else {
            /*
             * Node is not head. Left child now replaces
             * current node as child of parent.
             */
            if(curr.key <= parent.key) {
                /* Left child of parent. */
                parent.left = curr.left;
                size--;
                return;
            }
            else {
                /* Right child of parent. */
                parent.right = curr.left;
                size--;
                return;
            }
        }
    }

    /*
     * Remove a node that has only a right child. This is done by changing its
     * reference in the parent node to that of its child.
     *
     * @param   Node    curr    The node to remove.
     * @param   Node    parent  The parent of the node that was just removed.
     */
    private void removeNodeWithRightChild(Node curr, Node parent) {
        if(parent == null) {
            /* Node is head. Child is new head. */
            head = curr.right;
            size--;
            return;
        }
        else {
            /*
             * Node is not head. Right child now replaces
             * current node as child of parent.
             */
            if(curr.key <= parent.key) {
                /* Left child of parent. */
                parent.left = curr.right;
                size--;
                return;
            }
            else {
                /* Right child of parent. */
                parent.right = curr.right;
                size--;
                return;
            }
        }
    }

    class Node {
        int key;
        T value = null;
        Node left = null, right = null;

        public Node(T t) {
            key = t.hashCode();
            value = t;
        }

        public Node(int newKey) {
            key = newKey;
        }
    }
}
