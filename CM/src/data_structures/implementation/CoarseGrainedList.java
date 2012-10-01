package data_structures.implementation;

import data_structures.Sorted;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class CoarseGrainedList<T extends Comparable<T>> implements Sorted<T> {
    private Node head;
    private Lock lock = new ReentrantLock();
    private int size;

    public CoarseGrainedList() {
        head = new Node(Integer.MIN_VALUE);
        head.next = new Node(Integer.MAX_VALUE);
        size = 0;
    }

    public void add(T t) {
        Node pred, curr;
        int key = t.hashCode();

        lock.lock();
        try {
            pred = head;
            curr = pred.next;
            while(curr.key < key) {
                pred = curr;
                curr = curr.next;
            }

            Node node = new Node(t);
            node.next = curr;
            pred.next = node;
            size++;
        }
        finally {
            lock.unlock();
        }
    }

    public void remove(T t) {
        Node pred, curr;
        int key = t.hashCode();
        lock.lock();
        try {
            pred = head;
            curr = pred.next;
            while(curr.key < key) {
                pred = curr;
                curr = curr.next;
            }
            if(key == curr.key) {
                pred.next = curr.next;
                size--;
            }
            else {
                System.out.println("Element not found, skipping");
            }
        }
        finally {
            lock.unlock();
        }
    }

    public String toString() {
        return "CGL - size: " + size + " - value of head: " + head.key +
                " - value of head's pred: " + head.next.key;
    }

    class Node {
        int key;
        T value;
        Node next;

        public Node(T t) {
            key = t.hashCode();
            value = t;
        }

        public Node(int newKey) {
            key = newKey;
        }
    }
}
