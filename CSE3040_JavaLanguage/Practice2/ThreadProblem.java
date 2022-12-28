package chap7;

public class ThreadProblem {

    public static void main(String [] args) throws Exception {

          ManageThread mt = new ManageThread();

          mt.startThread();

    }

}

class ManageThread {

    SharedData sd = new SharedData(); // 공유되는 객체

    TestThread xxx= new TestThread("첫번째 Thread", sd);

    TestThread yyy= new TestThread("두번째 Thread", sd);

    Thread t1 = new Thread(xxx);

    Thread t2 = new Thread(yyy);

 

    public void startThread() {

          t1.start();

          t2.start();

    }

 

 

}

class TestThread implements Runnable {

String name;

SharedData sd;

 

TestThread(String name, SharedData sd) {

        this.name = name;

        this.sd = sd;

}

public void run() {

        System.out.println(name + " push data : " + sd.push('a'));

        System.out.println(name + " push data : " + sd.push('b'));

        System.out.println(name + " push data : " + sd.push('c'));

        System.out.println(name + " pop data : " + sd.pop());

        System.out.println(name + " pop data : " + sd.pop());

        System.out.println(name + " pop data : " + sd.pop());

}

}

 

class SharedData {

int stackPointer = 0;

char [] stack = new char[100];

 

public synchronized char push(char data) {

        stack[stackPointer] = data;

        doForALongJob();

        stackPointer = stackPointer + 1;

        return data;

}

 

public char pop() {
	synchronized (this) { // synchronized block

        stackPointer = stackPointer - 1;

        doForALongJob();

        return stack[stackPointer];
	}
}

 

public void doForALongJob() {

        for (long i = 0; i < 5000000; i++) {}

}

}
