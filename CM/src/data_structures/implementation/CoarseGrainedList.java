package data_structures.implementation;

import data_structures.Sorted;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class CoarseGrainedList<T extends Comparable<T>> implements Sorted<T> {
    private Node head;
    private Lock lock = new ReentrantLock();
    private int size;

    public CoarseGrainedList() {
        head = new FirstNode();
        head.next = new LastNode();
        size = 0;
    }

    public void add(T t) {
        Node pred, curr;

        lock.lock();
        try {
            pred = head;
            curr = pred.next;
            while(curr.compareTo(t) < 0) {
                pred = curr;
                curr = curr.next;
            }

            Node node = new ListNode(t);
            node.next = curr;
            pred.next = node;
            size++;
        } finally {
            lock.unlock();
        }
    }

    public void remove(T t) {
        Node pred, curr;

        lock.lock();
        try {
            pred = head;
            curr = pred.next;
            while(curr.compareTo(t) < 0) {
                pred = curr;
                curr = curr.next;
            }
            if(curr.compareTo(t) == 0) {
                pred.next = curr.next;
                size--;
            } else {
                System.out.println("Element not found, skipping");
            }
        } finally {
            lock.unlock();
        }
    }

    public String toString() {
        return "CGL - size: " + size + " - head: " + head +
                " - head's pred: " + head.next;
    }

    abstract class Node {
        T value;
        Node next;

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
