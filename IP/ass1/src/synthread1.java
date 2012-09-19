/* The primary program, that will invoke the other thread. */
class Synthread1 {
    public static void main(String[] args) {
        MyThread t = new MyThread();
        t.start();
        for(int i = 0; i < 10; i++)
            display("Bonjour monde\n");
    }

    /* Display a message by printing it one character at a time. This function
     * is synchronized to provide mutual exclusion.
     * 
     * @param   str The string to print
     * @return  none
     */
    synchronized static public void display(String str) {
        for(int i = 0; i < str.length(); i++)
            System.out.print(str.charAt(i));
    }
}

/* The extra thread that prints the second message. */
class MyThread extends Thread {
    private int argument;

    MyThread() {}

    public void run() {
        for(int i = 0; i < 10; i++)
            Synthread1.display("Hello world\n");
    }
}
