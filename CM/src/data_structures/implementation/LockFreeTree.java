package data_structures.implementation;

import data_structures.Sorted;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.atomic.AtomicStampedReference;

public class LockFreeTree<Key extends Comparable<Key>> implements Sorted<Key> {

    final static int CLEAN = 1, MARK = 2, IFLAG = 3, DFLAG = 4;
    Node root = null;

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

    abstract class Node implements Comparable<Key> {
        AtomicReference<Key> key;

        abstract public String toString();
        abstract boolean isLeaf();

        Key getKey() {
            return key.get();
        }
    }

    class Internal extends Node {
        AtomicReference<Node> left, right;
        Update update;

        public Internal(Key k, Node l, Node r) {
            key = new AtomicReference<Key>(k);
            left = new AtomicReference<Node>(l);
            right = new AtomicReference<Node>(r);
            update = new Update(null, CLEAN);
        }

        public int compareTo(Key k) {
            return key.get().compareTo(k);
        }

        public String toString() {
            return "Internal node";
        }

        public boolean isLeaf() {
            return false;
        }

        public Node getLeft() {
            return left.get();
        }

        public Node getRight() {
            return right.get();
        }
    }

    class Leaf extends Node {
        public Leaf(Key k) {
            key = new AtomicReference<Key>(k);
        }

        public int compareTo(Key k) {
            return key.get().compareTo(k);
        }

        public String toString() {
            return "Leaf node";
        }

        public boolean isLeaf() {
            return true;
        }
    }

    abstract class Info {
        Leaf l;
    }

    class IInfo extends Info {
        Internal p, newInternal;

        public IInfo(Internal p, Leaf l, Internal newInternal) {
            this.p = p;
            this.l = l;
            this.newInternal = newInternal;
        }
    }

    class DInfo extends Info {
        Internal gp, p;
        Update pupdate;

        public DInfo(Internal gp, Internal p, Leaf l, Update pupdate) {
            this.gp = gp;
            this.p = p;
            this.l = l;
            this.pupdate = pupdate;
        }
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

    private SearchTuple search(Key k) {
        Node l = root;
        Internal p = null, gp = null;
        Update gpupdate = null, pupdate = null;

        while(!l.isLeaf()) {
            gp = p;
            p = (Internal)l;
            gpupdate = pupdate;
            pupdate = p.update;
            l = l.compareTo(k) > 0 ? p.getLeft() : p.getRight();
        }

        return new SearchTuple(gp, p, (Leaf)l, pupdate, gpupdate);
    }

    public void add(Key k) {
        Internal newInternal;
        Leaf newSibling, newLeaf = new Leaf(k);
        Update result;
        IInfo op;
        SearchTuple r;
        int cmp;
        int[] expState = new int[1];
        Info expInfo;

        while(true) {
            r = search(k);
            cmp = r.l.compareTo(k);

            // Do not allow double key occurrences
            if(cmp == 0) {
                // FIXME: remove print statements
                System.out.println("Attempted to insert double key: " + k);
                return;
            }

            if(r.pupdate.getState() != CLEAN) {
                help(r.pupdate);
            } else {
                newSibling = new Leaf(r.l.getKey());
                newInternal = cmp < 0
                    ? new Internal(k, newSibling, newLeaf)
                    : new Internal(r.l.getKey(), newLeaf, newSibling);
                op = new IInfo(r.p, r.l, newInternal);
                expInfo = r.pupdate.get(expState);

                if(r.p.update.compareAndSet(expInfo, op, expState[0], IFLAG)) {
                    helpInsert(op);
                    return;
                }

                help(r.p.update);
            }
        }
    }

    private void helpInsert(IInfo op) {
        casChild(op.p, op.l, op.newInternal);
        op.p.update.compareAndSet(op, op, IFLAG, CLEAN);
    }

    public void remove(Key k) {
        DInfo op;
        SearchTuple r;
        int[] expState = new int[1];
        Info expInfo;

        while(true) {
            r = search(k);
            if(r.l.compareTo(k) != 0) {
                // FIXME: remove print statements
                System.out.println("Key not found: " + k);
                return;
            }

            if(r.gpupdate.getState() != CLEAN) {
                help(r.gpupdate);
            } else if(r.pupdate.getState() != CLEAN) {
                help(r.pupdate);
            } else {
                op = new DInfo(r.gp, r.p, r.l, r.pupdate);
                expInfo = r.gpupdate.get(expState);

                if(r.gp.update.compareAndSet(expInfo, op, expState[0], DFLAG)) {
                    if(helpDelete(op))
                        return;
                }

                help(r.gp.update);
            }
        }
    }

    private boolean helpDelete(DInfo op) {
        //if(op == null)
        //    return true;

        Update result;
        int[] stateHolder = new int[1];
        Info infoHolder = op.pupdate.get(stateHolder);

        if(op.p.update.compareAndSet(infoHolder, op, stateHolder[0], MARK)) {
            helpMarked(op);
            return true;
        } else {
            // Check if another thread helped
            infoHolder = op.p.update.get(stateHolder);

            if(infoHolder == op && stateHolder[0] == MARK) {
                helpMarked(op);
                return true;
            }

            help(op.p.update);
            op.gp.update.compareAndSet(op, op, DFLAG, CLEAN);
            return false;
        }
    }

    private void helpMarked(DInfo op) {
        //if(op == null)
        //    return true;

        Node other = op.p.getRight() == op.l ? op.p.getLeft() : op.p.getRight();

        casChild(op.gp, op.p, other);
        op.gp.update.compareAndSet(op, op, DFLAG, CLEAN);
    }

    private void help(Update u) {
        switch(u.getState()) {
            case IFLAG:
                helpInsert((IInfo)u.getInfo());
                break;
            case MARK:
                helpMarked((DInfo)u.getInfo());
                break;
            case DFLAG:
                helpDelete((DInfo)u.getInfo());
        }
    }

    private void casChild(Internal parent, Node old, Node newNode) {
        if(newNode.getKey().compareTo(parent.getKey()) < 0)
            parent.left.compareAndSet(old, newNode);
        else
            parent.right.compareAndSet(old, newNode);
    }

    public String toString() {
        throw new UnsupportedOperationException();
    }
}
