package data_structures.implementation;

import data_structures.Sorted;
import java.util.concurrent.atomic.AtomicMarkableReference;

public class LockFreeList<T extends Comparable<T>> implements Sorted<T> {
    private Node head;

    public LockFreeList() {
        head = new FirstNode();
        Node tail = new LastNode();
        while(!head.next.compareAndSet(null, tail, false, false));
    }

    public void add(T t) {
        while(true) {
            // Find the nodes affected by the addition
            Window window = find(t);
            Node pred = window.pred, curr = window.curr;

            // Create new node and add it.
            Node node = new ListNode(t);
            node.next = new AtomicMarkableReference<Node>(curr, false);
            if(pred.next.compareAndSet(curr, node, false, false))
                return;
        }
    }

    public void remove(T t) {
        boolean snip;
        while(true) {
            // Find the nodes affected by the remove
            Window window = find(t);
            Node pred = window.pred, curr = window.curr;
            if(curr.compareTo(t) == 0) {
                // Node located, remove it
                Node succ = curr.next.getReference();
                snip = curr.next.compareAndSet(succ, succ, false, true);
                if(!snip)
                    // Failed to cut it free, retry
                    continue;
                pred.next.compareAndSet(curr, succ, false, false);
            }
            return;
        }
    }

    public String toString() {
        return "LFL: " + head;
    }

    /* Find the two nodes affected in the required operation. */
    private Window find(T t) {
        Node pred = null, curr = null, succ = null;
        boolean[] marked = {false};
        boolean snip;
        retry: while(true) {
            pred = head;
            curr = pred.next.getReference();
            while(true) {
                succ = curr.next.get(marked);
                while(marked[0]) {
                    // Node is marked, try to remove it. If this fails, start
                    // over from the beginning.
                    snip = pred.next.compareAndSet(curr, succ, false, false);
                    if(!snip)
                        continue retry;
                    curr = succ;
                    succ = curr.next.get(marked);
                }
                if(curr.compareTo(t) >= 0)
                    // Position found, return affected nodes.
                    return new Window(pred, curr);

                // Current node is smaller, continue traversal.
                pred = curr;
                curr = succ;
            }
        }
    }

    class Window {
        public Node pred, curr;

        Window(Node myPred, Node myCurr) {
            pred = myPred;
            curr = myCurr;
        }
    }

    abstract class Node {
        T value;
        AtomicMarkableReference<Node> next = 
                new AtomicMarkableReference<Node>(null, false);

        abstract int compareTo(T t);
        public String toString() {
            String nextNode = "";

            if (next != null)
                nextNode += " - " + next.getReference();
            return "Node " + value + nextNode;
        }
    }

    class ListNode extends Node {
        public ListNode(T t) {
            value = t;
        }

        int compareTo(T t) {
            return value.compareTo(t);
        }
    }

    class FirstNode extends Node {
        int compareTo(T t) {
            return -1;
        }
    }

    class LastNode extends Node {
        int compareTo(T t) {
            return 1;
        }
    }
}
