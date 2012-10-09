package data_structures.implementation;

import data_structures.Sorted;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class CoarseGrainedList<T extends Comparable<T>> implements Sorted<T> {
    private Node head;
    private Lock lock = new ReentrantLock();

    public CoarseGrainedList() {
        head = new FirstNode();
        head.next = new LastNode();
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
            if(curr.compareTo(t) == 0)
                pred.next = curr.next;
        } finally {
            lock.unlock();
        }
    }

    public String toString() {
        return "CGL: " + head;
    }

    abstract class Node {
        T value;
        Node next;

        abstract int compareTo(T t);
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
