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
            Window window = find(t);
            Node pred = window.pred, curr = window.curr;
            Node node = new ListNode(t);
            node.next = new AtomicMarkableReference<Node>(curr, false);
            if(pred.next.compareAndSet(curr, node, false, false))
                return;
        }
    }

    public void remove(T t) {
        boolean snip;
        while(true) {
            Window window = find(t);
            Node pred = window.pred, curr = window.curr;
            if(curr.compareTo(t) != 0) {
                System.out.println("Element not found, skipping");
            } else {
                Node succ = curr.next.getReference();
                snip = curr.next.compareAndSet(succ, succ, false, true);
                if(!snip)
                    continue;
                pred.next.compareAndSet(curr, succ, false, false);
            }
            return;
        }
    }

    public String toString() {
        return "LFL - head: " + head + " - head's next: "
                + head.next.getReference();
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
                    snip = pred.next.compareAndSet(curr, succ, false, false);
                    if(!snip)
                        continue retry;
                    curr = succ;
                    succ = curr.next.get(marked);
                }
                if(curr.compareTo(t) >= 0)
                    return new Window(pred, curr);
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
        abstract public String toString();
    }

    class ListNode extends Node {
        public ListNode(T t) {
            value = t;
        }

        int compareTo(T t) {
            return value.compareTo(t);
        }

        public String toString() {
            return "List node";
        }
    }

    class FirstNode extends Node {
        int compareTo(T t) {
            return -1;
        }

        public String toString() {
            return "First node in the list";
        }
    }

    class LastNode extends Node {
        int compareTo(T t) {
            return 1;
        }

        public String toString() {
            return "Last node in the list";
        }
    }
}
