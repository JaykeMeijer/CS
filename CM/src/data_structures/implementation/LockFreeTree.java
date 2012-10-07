package data_structures.implementation;

import data_structures.Sorted;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.atomic.AtomicStampedReference;

public class LockFreeTree<T extends Comparable<T>> implements Sorted<T> {

    final static int CLEAN = 1, MARKED = 2, IFLAG = 3, DFLAG = 4;

    class Update {
        AtomicStampedReference<Info> inforef;

        public Update(Info info, int state) {
            inforef = new AtomicStampedReference<Info>(info, state);
        }

        public boolean attemptStamp(Info expectedInfo, int newState) {
            return inforef.attemptStamp(expectedInfo, newState);
        }

        public int getState() {
            return inforef.getStamp();
        }

        public Info getInfo() {
            return inforef.getReference();
        }

        public Info get(int[] stateHolder) {
            return inforef.get(stateHolder);
        }

        public boolean compareAndSet(Info er, Info nr, int es, int ns) {
            return inforef.compareAndSet(er, nr, es, ns);
        }
    }

    abstract class Node implements Comparable<T> {
        AtomicReference<T> value;

        abstract public String toString();
    }

    class Internal extends Node {
        AtomicReference<Node> left, right;
        Update update;

        public int compareTo(T t) {
            return value.get().compareTo(t);
        }

        public String toString() {
            return "Internal node";
        }
    }

    class Leaf extends Node {
        public int compareTo(T t) {
            return value.get().compareTo(t);
        }

        public String toString() {
            return "Leaf node";
        }
    }

    abstract class Info {
        Leaf l;
    }

    class IInfo extends Info {
        Internal p, newInternal;
    }

    class DInfo extends Info {
        Internal gp, p;
        Update pupdate;
    }

    class SearchTuple {
        Internal gp, p;
        Leaf l;
        Update pupdate, gpupdate;

        public SearchTuple(Internal gp, Internal p, Leaf l,
                Update pupdate, Update gpupdate) {
            this.gp = gp;
            this.p = p;
            this.l = l;
            this.pupdate = pupdate;
            this.gpupdate = gpupdate;
        }
    }

    private SearchTuple search(T t) {
        return null;
    }

    public void add(T t) {
        throw new UnsupportedOperationException();
    }

    private void helpInsert(IInfo op) {
        return;
    }

    public void remove(T t) {
        throw new UnsupportedOperationException();
    }

    private void helpDelete(DInfo op) {
        return;
    }

    private void helpMarked(DInfo op) {
        return;
    }

    private void help(Update u) {
        return;
    }

    private void casChild(Internal parent, Node old, Node newnode) {
        return;
    }

    public String toString() {
        throw new UnsupportedOperationException();
    }
}
