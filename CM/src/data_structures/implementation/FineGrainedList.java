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
        head.lock();
        Node pred = head;
        try {
            Node curr = pred.next;
            curr.lock();
            try  {
                while(curr.compareTo(t) < 0) {
                    pred.unlock();
                    pred = curr;
                    curr = curr.next;
                    curr.lock();
                }
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
        Node pred = null;
        head.lock();
        try {
            pred = head;
            Node curr = pred.next;
            curr.lock();
            try {
                while(curr.compareTo(t) < 0) {
                    pred.unlock();
                    pred = curr;
                    curr = curr.next;
                    curr.lock();
                }
                if(curr.compareTo(t) == 0) {
                    pred.next = curr.next;
                } else {
                    System.out.println("Element not found, skipping");
                }
            } finally {
                curr.unlock();
            }
        } finally {
            pred.unlock();
        }
    }

    public String toString() {
        return "FGL - head: " + head + " - head's next: " + head.next;
    }

    abstract class Node {
        T value;
        Node next;
        private Lock lock = new ReentrantLock();

        abstract int compareTo(T t);
        abstract public String toString();

        public void lock() {
            lock.lock();
        }

        public void unlock() {
            lock.unlock();
        }
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
