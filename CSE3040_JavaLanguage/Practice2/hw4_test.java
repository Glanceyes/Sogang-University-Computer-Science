package hw4;

import java.awt.*; 
import java.util.ArrayList;
import java.awt.event.*; 

public class s20171665hw4 extends Frame implements ActionListener {  
	private Canvas canvas;
	private Ball b;
	private int index = 0;
	public s20171665hw4() {  
		canvas = new Canvas(); 
		add("Center", canvas); 
		Panel p = new Panel(); 
		Button s = new Button("Start"); 
		Button c = new Button("Close"); 
		p.add(s); 
		p.add(c); 
		s.addActionListener(this); 
		c.addActionListener(this); 
		add("South", p); 
	} 
	
	public void actionPerformed(ActionEvent evt) {  
		if (evt.getActionCommand() == "Start") {  
			if(index == 0) {
				b = new Ball(canvas); 
				b.start();
				index++;
			}
			else if(index >= 1 && index < 5) {
				b.updateFlag(index);
				index++;
			}
		} 
		else if (evt.getActionCommand() == "Close") 
			System.exit(0); 
	} 
	public static void main(String[] args) {  
		Frame f = new s20171665hw4(); 
		f.setSize(400, 300); 
		WindowDestroyer listener = new WindowDestroyer();  
		f.addWindowListener(listener); 
		f.setVisible(true);  
	}
}


class Ball extends Thread {  
	private Canvas box; 
	private static final int XSIZE = 20; 
	private static final int YSIZE = 20; 
	private ArrayList<Integer> x = new ArrayList<Integer>();
	private ArrayList<Integer> y = new ArrayList<Integer>();
	private int index = 0;
	private int updateIndexFlag = 0;
	private ArrayList<Integer> dx = new ArrayList<Integer>(); 
	private ArrayList<Integer> dy = new ArrayList<Integer>(); 
	private Graphics g;
	public Ball(Canvas c) { 
		box = c; 
		x.add(box.getSize().width/2);
		y.add(box.getSize().height/2);
		int i = 0;
		while((i = (int)(Math.random() * 10 + 1)) == 0);
		dx.add(i);
		while((i = (int)(Math.random() * 10 + 1)) == 0);
		dy.add(i);
	} 
	
	public void updateFlag(int index) {
		this.index = index;
		updateIndexFlag = 1;
	}
	
	public void updateIndex() {
		x.add(box.getSize().width/2);
		y.add(box.getSize().height/2);
		int i = 0;
		while((i = (int)(Math.random() * 10 + 1)) == 0);
		dx.add(i);
		while((i = (int)(Math.random() * 10 + 1)) == 0);
		dy.add(i);
		draw();
		updateIndexFlag = 0;
	}
	public void draw() { 
		g = box.getGraphics(); 
		for (int i = 0; i <= index; i++) {
			g.fillOval(x.get(i), y.get(i), XSIZE, YSIZE); 
		}
		g.dispose(); 
	} 
	public void move() {  
		g = box.getGraphics(); 
		g.setXORMode(box.getBackground());
		Dimension d = box.getSize(); 
		for(int i = 0; i <= index; i++) {
			int tempX = x.get(i), tempY = x.get(i), tempDX = dx.get(i), tempDY = dy.get(i);
			g.fillOval(tempX, tempY, XSIZE, YSIZE); 
			x.set(i, tempX + tempDX); 
			y.set(i, tempY + tempDY); 
			if (x.get(i) < 0) { 
				x.set(i, 0); 
				dx.set(i, -tempDX); 
			} 
			if (x.get(i) + XSIZE >= d.width) { 
				x.set(i, d.width - XSIZE); 
				dx.set(i, -tempDX); 
			}
			if (x.get(i) < 0) { 
				y.set(i, 0); 
				dy.set(i, -tempDY); 
			} 
			if (x.get(i) + YSIZE >= d.height) { 
				y.set(i, d.height - YSIZE); 
				dy.set(i, -tempDY); 
			} 
			g.fillOval(x.get(i), y.get(i), XSIZE, YSIZE); 
		}
		g.dispose();
	}  
	public void run() {  
		draw(); 
		while(true) {
			if(updateIndexFlag == 0) move();
			else if(updateIndexFlag == 1) updateIndex();
			try { Thread.sleep(30); } 
			catch(InterruptedException e) {}
		} 
	}
}
