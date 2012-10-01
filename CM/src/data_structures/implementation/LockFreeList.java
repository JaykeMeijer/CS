package data_structures.implementation;

import data_structures.Sorted;
import java.util.concurrent.atomic.AtomicMarkableReference;

public class LockFreeList<T extends Comparable<T>> implements Sorted<T> {
    private Node head;
    private int size;

    public LockFreeList() {
        head = new Node(Integer.MIN_VALUE);
        Node tail = new Node(Integer.MAX_VALUE);
        while(!head.next.compareAndSet(null, tail, false, false));
        size = 0;
    }

    public void add(T t) {
        int key = t.hashCode();
        while(true) {
            Window window = find(head, key);
            Node pred = window.pred, curr = window.curr;
            Node node = new Node(t);
            node.next = new AtomicMarkableReference<Node>(curr, false);
            if(pred.next.compareAndSet(curr, node, false, false)) {
                size++;
                return;
            }
        }
    }

    public void remove(T t) {
        int key = t.hashCode();
        boolean snip;
        while(true) {
            Window window = find(head, key);
            Node pred = window.pred, curr = window.curr;
            if(curr.key != key) {
                System.out.println("Element not found, skipping");
                return;
            }
            else {
                Node succ = curr.next.getReference();
                snip = curr.next.compareAndSet(succ, succ, false, true);
                if(!snip)
                    continue;
                pred.next.compareAndSet(curr, succ, false, false);
                size--;
                return;
            }
        }
    }

    public String toString() {
        return "LFL - size: " + size + " - value of head: " + head.key +
                " - value of head's pred: " + head.next.getReference().key;
    }

    /* Find the two nodes affected in the required operation. */
    private Window find(Node head, int key) {
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
                if(curr.key >= key)
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

    class Node {
        int key;
        T value;
        AtomicMarkableReference<Node> next;

        Node(T t) {
            key = t.hashCode();
            value = t;
            next = new AtomicMarkableReference<Node>(null, false);
        }

        Node(int newKey) {
            key = newKey;
            value = null;
            next = new AtomicMarkableReference<Node>(null, false);
        }
    }
}
