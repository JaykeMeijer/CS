package data_structures.implementation;

import data_structures.Sorted;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.atomic.AtomicStampedReference;

public class LockFreeTree<Key extends Comparable<Key>> implements Sorted<Key> {
    final static int CLEAN = 1, MARK = 2, IFLAG = 3, DFLAG = 4;
    Internal root;

    class Update {
        AtomicStampedReference<Info> inforef;

        Update(Info info, int state) {
            inforef = new AtomicStampedReference<Info>(info, state);
        }

        int getState() {
            return inforef.getStamp();
        }

        Info getInfo() {
            return inforef.getReference();
        }

        Info get(int[] stateHolder) {
            return inforef.get(stateHolder);
        }

        boolean compareAndSet(Info er, Info nr, int es, int ns) {
            return inforef.compareAndSet(er, nr, es, ns);
        }
    }

    abstract class Node implements Comparable<Key> {
        final static int TABSIZE = 4;
        Key key;

        abstract boolean isLeaf();
        abstract String toString(int indent_length);

        public int compareTo(Key k) {
            if(k == null)
                return -1;

            if(key == null)
                return 1;

            return key.compareTo(k);
        }
    }

    class Internal extends Node {
        AtomicReference<Node> left, right;
        Update update;

        Internal(Key k, Node l, Node r) {
            key = k;
            left = new AtomicReference<Node>(l);
            right = new AtomicReference<Node>(r);
            update = new Update(null, CLEAN);
        }

        boolean isLeaf() {
            return false;
        }

        Node getLeft() {
            return left.get();
        }

        Node getRight() {
            return right.get();
        }

        String toString(int indent_length) {
            String indent = new String();

            for(int i = 0; i < indent_length; i++)
                indent += ' ';

            return String.format("%s\n%s< %s\n%s> %s", key, indent,
                left.get().toString(indent_length + TABSIZE),
                indent, right.get().toString(indent_length + TABSIZE));
        }
    }

    class Leaf extends Node {
        Leaf(Key k) {
            key = k;
        }

        boolean isLeaf() {
            return true;
        }

        String toString(int indent_length) {
            return String.valueOf(key);
        }
    }

    abstract class Info {
        Leaf l;
    }

    class IInfo extends Info {
        Internal p, newInternal;

        IInfo(Internal p, Leaf l, Internal newInternal) {
            this.p = p;
            this.l = l;
            this.newInternal = newInternal;
        }
    }

    class DInfo extends Info {
        Internal gp, p;
        Update pupdate;

        DInfo(Internal gp, Internal p, Leaf l, Update pupdate) {
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
        Info info;
        int[] stateHolder = new int[1];

        SearchTuple(Internal gp, Internal p, Leaf l,
                Update pupdate, Update gpupdate) {
            this.gp = gp;
            this.p = p;
            this.l = l;
            info = pupdate.get(stateHolder);
            this.pupdate = new Update(info, stateHolder[0]);

            if(gpupdate != null) {
                info = gpupdate.get(stateHolder);
                this.gpupdate = new Update(info, stateHolder[0]);
            } else {
                this.gpupdate = null;
            }
        }
    }

    public LockFreeTree() {
        Leaf left = new Leaf(null), right = new Leaf(null);
        root = new Internal(null, left, right);
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
                return;
            }

            if(r.pupdate.getState() != CLEAN) {
                help(r.pupdate);
            } else {
                newSibling = new Leaf(r.l.key);
                newInternal = cmp < 0
                    ? new Internal(k, newSibling, newLeaf)
                    : new Internal(r.l.key, newLeaf, newSibling);
                op = new IInfo(r.p, r.l, newInternal);
                expInfo = r.pupdate.get(expState);

                //iflag CAS step
                if(r.p.update.compareAndSet(expInfo, op, expState[0], IFLAG)) {
                    helpInsert(op);
                    return;
                }

                help(r.p.update);
            }
        }
    }

    private void helpInsert(IInfo op) {
        // ichild CAS step
        casChild(op.p, op.l, op.newInternal);

        // iunflag CAS step
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
                return;
            }

            if(r.gpupdate.getState() != CLEAN) {
                help(r.gpupdate);
            } else if(r.pupdate.getState() != CLEAN) {
                help(r.pupdate);
            } else {
                op = new DInfo(r.gp, r.p, r.l, r.pupdate);
                expInfo = r.gpupdate.get(expState);

                // dflag CAS step
                if(r.gp.update.compareAndSet(expInfo, op, expState[0], DFLAG)) {
                    if(helpDelete(op))
                        return;
                } else {
                    // Failed to perform dflag CAS. First help other operation
                    help(r.gp.update);
                }
            }
        }
    }

    private boolean helpDelete(DInfo op) {
        int[] stateHolder = new int[1];
        Info info = op.pupdate.get(stateHolder);

        // mark CAS step
        if(op.p.update.compareAndSet(info, op, stateHolder[0], MARK)) {
            helpMarked(op);
            return true;
        } else {
            // Check if another thread marked the parent already
            info = op.p.update.get(stateHolder);

            if(info == op && stateHolder[0] == MARK) {
                helpMarked(op);
                return true;
            }

            help(op.p.update);

            // Failed to mark the parent node. Remove DFLAG and restart.
            // backtrack CAS step
            op.gp.update.compareAndSet(op, op, DFLAG, CLEAN);
            return false;
        }
    }

    private void helpMarked(DInfo op) {
        Node right = op.p.getRight(),
             other = right == op.l ? op.p.getLeft() : right;

        // dchild CAS step
        casChild(op.gp, op.p, other);

        // dunflag CAS step
        op.gp.update.compareAndSet(op, op, DFLAG, CLEAN);
    }

    private void help(Update u) {
        int[] stateHolder = new int[1];
        Info info = u.get(stateHolder);

        switch(stateHolder[0]) {
            case IFLAG:
                helpInsert((IInfo)info);
                break;
            case MARK:
                helpMarked((DInfo)info);
                break;
            case DFLAG:
                helpDelete((DInfo)info);
                break;
        }
    }

    private void casChild(Internal parent, Node old, Node newNode) {
        if(newNode.compareTo(parent.key) < 0)
            parent.left.compareAndSet(old, newNode);
        else
            parent.right.compareAndSet(old, newNode);
    }

    public String toString() {
        return root.toString(Node.TABSIZE);
    }
}
