package data_structures.implementation;

import data_structures.Sorted;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class CoarseGrainedList<T extends Comparable<T>> implements Sorted<T> {
    private CGLNode head;
    private Lock lock = new ReentrantLock();
    private int size;

    public CoarseGrainedList() {
        head = new CGLNode<T>(Integer.MIN_VALUE);
        head.next = new CGLNode<T>(Integer.MAX_VALUE);
        size = 0;
    }

    public void add(T t) {
        CGLNode pred, curr;
        int key = t.hashCode();

        lock.lock();
        try {
            pred = head;
            curr = pred.next;
            while(curr.key < key) {
                pred = curr;
                curr = curr.next;
            }

            CGLNode node = new CGLNode<T>(t);
            node.next = curr;
            pred.next = node;
            size++;
        }
        finally {
            lock.unlock();
        }
    }

    public void remove(T t) {
        CGLNode pred, curr;
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
}

class CGLNode<T extends Comparable<T>> {
    int key;
    T value;
    CGLNode next;

    public CGLNode(T t) {
        key = t.hashCode();
        value = t;
    }

    public CGLNode(int newKey) {
        key = newKey;
    }
}
