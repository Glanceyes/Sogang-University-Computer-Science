package chap7;

import java.awt.*;

public class thTimer1 extends Frame{

	public thTimer1(String str) {
		super(str);
		add(new timerCanvas());
	}
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		Frame f = new thTimer1("Timer");
		f.setSize(500, 300);;
		WindowDestroyer listener = new WindowDestroyer();
		f.addWindowListener(listener);
		f.setVisible(true);
	}

}

class timerCanvas extends Canvas {
	public timerCanvas() {
		new Timer(this, 1000).start();
		setSize(400, 250);
	}
}

class Timer extends Thread{
	private timerCanvas tcvas;
	private int msec;
	public Timer(timerCanvas t, int ms) {
		tcvas = t;
		msec = ms;
	}
	
	public void run() {
		while(true) {
			try { sleep(msec);}
			catch(InterruptedException e) {
				PalCho();
			}
		}
	}
	
	private boolean pal = false;
	public void PalCho() {
		if (pal) tcvas.setBackground(Color.red);
		else tcvas.setBackground(Color.green);
		pal = !pal;
		tcvas.repaint();
	}
}