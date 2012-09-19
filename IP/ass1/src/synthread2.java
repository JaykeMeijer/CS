class Synthread2 {
    public static Integer sem1 = 0, sem2 = 0;

    public static void main(String[] args) {
        MyThread2 t = new MyThread2();
        t.start();

        display("ab");
        synchronized (sem1) {
            sem1.notify();
        }

        for(int i = 0; i < 9; i++) {
            synchronized (sem2) {
                try {
                    sem2.wait();
                }
                catch(InterruptedException e) {
                    System.out.println("InterruptedExeption occured, exiting");
                    System.exit(1);
                }
            }
            display("ab");
            synchronized (sem1) {
                sem1.notify();
            }
        }
    }

    static public void display(String str) {
        for(int i = 0; i < str.length(); i++)
            System.out.print(str.charAt(i));
    }
}

class MyThread2 extends Thread {
    private int argument;

    MyThread2() {}

    public void run() {
        for(int i = 0; i < 10; i++) {
            synchronized (Synthread2.sem1) {
                try {
                    Synthread2.sem1.wait();
                }
                catch(InterruptedException e) {
                    System.out.println("InterruptedExeption occured, exiting");
                    System.exit(1);
                }
            }
            Synthread2.display("cd\n");
            synchronized (Synthread2.sem2) {
                Synthread2.sem2.notify();
            }
        }
    }
}
