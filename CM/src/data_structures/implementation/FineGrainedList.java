package data_structures.implementation;

import data_structures.Sorted;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class FineGrainedList<T extends Comparable<T>> implements Sorted<T> {
    private FGLNode head;
    private int size;

    public FineGrainedList() {
        head = new FGLNode<T>(Integer.MIN_VALUE);
        head.next = new FGLNode<T>(Integer.MAX_VALUE);
        size = 0;
    }

    public void add(T t) {
        int key = t.hashCode();
        head.lock();
        FGLNode pred = head;
        try {
            FGLNode curr = pred.next;
            curr.lock();
            try  {
                while(curr.key < key) {
                    pred.unlock();
                    pred = curr;
                    curr = curr.next;
                    curr.lock();
                }
                FGLNode newNode = new FGLNode<T>(t);
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
        FGLNode pred = null;
        int key = t.hashCode();
        head.lock();
        try {
            pred = head;
            FGLNode curr = pred.next;
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
                }
                else {
                    System.out.println("Element not found, skipping");
                }
            }
            finally {
                curr.unlock();
            }
        }
        finally {
            pred.unlock();
        }
    }

    public String toString() {
        return "FGL - size: " + size + " - value of head: " + head.key +
                " - value of head's pred: " + head.next.key;
    }
}

class FGLNode<T extends Comparable<T>> {
    int key;
    T value;
    FGLNode next;
    private Lock lock;

    public FGLNode(T t) {
        key = t.hashCode();
        value = t;
        lock = new ReentrantLock();
    }

    public FGLNode(int newKey) {
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
