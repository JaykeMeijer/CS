package data_structures.implementation;

import data_structures.Sorted;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class FineGrainedList<T extends Comparable<T>> implements Sorted<T> {
    private Node head;

    public FineGrainedList() {
        head = new FirstNode();
        head.next = new LastNode();
    }

    public void add(T t) {
        Node pred = head;
        head.lock();
        try {
            Node curr = pred.next;
            curr.lock();
            try  {
                while(curr.compareTo(t) < 0) {
                    // Current node smaller, keep traversing.
                    pred.unlock();
                    pred = curr;
                    curr = curr.next;
                    curr.lock();
                }

                // Location found, create node and add it.
                Node newNode = new ListNode(t);
                newNode.next = curr;
                pred.next = newNode;
            }
            finally {
                curr.unlock();
            }
        }
        finally {
            pred.unlock();
        }
    }

    public void remove(T t) {
        Node pred = head;
        head.lock();
        try {
            Node curr = pred.next;
            curr.lock();
            try {
                while(curr.compareTo(t) < 0) {
                    // Current node smaller, keep traversing.
                    pred.unlock();
                    pred = curr;
                    curr = curr.next;
                    curr.lock();
                }
                if(curr.compareTo(t) == 0)
                    // Element found, remove it.
                    pred.next = curr.next;
            } finally {
                curr.unlock();
            }
        } finally {
            pred.unlock();
        }
    }

    public String toString() {
        return "FGL: " + head;
    }

    abstract class Node {
        T value;
        Node next;
        private Lock lock = new ReentrantLock();

        abstract int compareTo(T t);

        public void lock() {
            lock.lock();
        }

        public void unlock() {
            lock.unlock();
        }

        public String toString() {
            String nextNode = "";

            if (next != null)
                nextNode += " - " + next;
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
