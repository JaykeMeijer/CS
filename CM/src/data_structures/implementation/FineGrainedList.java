package data_structures.implementation;

import data_structures.Sorted;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class FineGrainedList<T extends Comparable<T>> implements Sorted<T> {
    private Node head;
    private int size;

    public FineGrainedList() {
        head = new Node(Integer.MIN_VALUE);
        head.next = new Node(Integer.MAX_VALUE);
        size = 0;
    }

    public void add(T t) {
        int key = t.hashCode();
        head.lock();
        Node pred = head;
        try {
            Node curr = pred.next;
            curr.lock();
            try  {
                while(curr.key < key) {
                    pred.unlock();
                    pred = curr;
                    curr = curr.next;
                    curr.lock();
                }
                Node newNode = new Node(t);
                newNode.next = curr;
                pred.next = newNode;
                size++;
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
        int key = t.hashCode();
        head.lock();
        try {
            pred = head;
            Node curr = pred.next;
            curr.lock();
            try {
                while(curr.key < key) {
                    pred.unlock();
                    pred = curr;
                    curr = curr.next;
                    curr.lock();
                }
                if(curr.key == key) {
                    pred.next = curr.next;
                    size--;
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
        return "FGL - size: " + size + " - value of head: " + head.key +
                " - value of head's pred: " + head.next.key;
    }

    class Node {
        int key;
        T value;
        Node next;
        private Lock lock;

        public Node(T t) {
            key = t.hashCode();
            value = t;
            lock = new ReentrantLock();
        }

        public Node(int newKey) {
            key = newKey;
            lock = new ReentrantLock();
        }

        public void lock() {
            lock.lock();
        }

        public void unlock() {
            lock.unlock();
        }
    }
}
