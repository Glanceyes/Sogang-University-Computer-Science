package chap7;

import java.awt.*;
import java.util.*;

public class TimerTest extends Frame{

	public TimerTest() {
		setLayout(new GridLayout(2,3));
		add(new ClockCanvas("Seoul", 16));
		add(new ClockCanvas("Chicago", 1));
		add(new ClockCanvas("London", 7));
		add(new ClockCanvas("Moscow", 10));
		add(new ClockCanvas("Paris", 8));
		add(new ClockCanvas("Beijing", 15));
	}
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		Frame f = new TimerTest();
		f.setSize(450, 300);
		f.setBackground(Color.gray);
		WindowDestroyer listener = new WindowDestroyer();
		f.addWindowListener(listener);
		f.setVisible(true);
	}
}

interface Timed{
	public void tick(Timer t);
}

class Timer extends Thread{
	public Timer (Timed t, int i) {
		target = t; interval = i;
		setDaemon(true);
	}
	private Timed target;
	private int interval;
}

class ClockCanvas extends Canvas implements Timed{
	public ClockCanvas(String c, int off) {
		city = c; offset = off;
		new Timer(this, 1000).start();
		setSize(125, 125);
	}
	public int baseX = 10;
	public int baseY = 10;
	public int clockW = 100;
	public int clockH = 100;
	public int center = baseX + clockH / 2;
	public void paint(Graphics g) {
		g.setColor(Color.white);
		g.fillOval(baseX, baseY, clockW, clockH);
		double hourAngle = 2 * Math.PI * (seconds - 3 * 60 * 60) / (12 * 60 * 60);
		double minuteAngle = 2 * Math.PI * (seconds - 15 * 60) / (60 * 60);
		double secondAngle = 2 * Math.PI * (seconds - 15) / 60;
		g.setColor(Color.black);
		g.drawLine(center, center, center + (int)(30 * Math.cos(hourAngle)),center + (int)(30 * Math.sin(hourAngle)));
		g.drawLine(center, center, center + (int)(40 * Math.cos(minuteAngle)),center + (int)(40 * Math.sin(minuteAngle)));
		g.setColor(Color.blue);
		g.drawLine(center, center, center + (int)(45 * Math.cos(secondAngle)),center + (int)(45 * Math.sin(secondAngle)));
		g.setColor(Color.black);
		g.drawString(city, baseX, baseY+clockH+10);
	}
	public void tick(Timer t) {
		Calendar rightNow = Calendar.getInstance();
		seconds = (rightNow.get(Calendar.HOUR) - LOCAL + offset)* 60 * 60 +
		rightNow.get(Calendar.MINUTE) * 60 +
		rightNow.get(Calendar.SECOND);
		repaint();
	}
	private int seconds = 0;
	private String city;
	private int offset;
	private final int LOCAL = 16;
}
