package chap7;

import java.awt.*;
import java.awt.event.*;

public class Bounce extends Frame implements ActionListener{

	private Canvas canvas;
	public Bounce() {
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
		if(evt.getActionCommand() == "start") {
			Ball b = new Ball(canvas);
			b.bounce();
		}
		else if(evt.getActionCommand() == "Close") {
			System.exit(0);
		}
	}
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		Frame f = new Bounce();
		f.setSize(400, 300);
		WindowDestroyer listener = new WindowDestroyer();
		f.addWindowListener(listener);
		f.setVisible(true);
	}

	private Canvas box;
	private static final int XSIZE = 20;
	private static final int YSIZE = 20;
	private int x = 0;
	private int y = 0;
	private int dx = 2;
	private int dy = 2;
	
	class Ball{
		public Ball(Canvas c) {
			box = c;
		}
		public void draw() {
			Graphics g = box.getGraphics();
			g.fillOval(x, y, XSIZE, YSIZE);
			g.dispose();
		}
		public void move() {
			Graphics g = box.getGraphics();
			g.setXORMode(box.getBackground());
			g.fillOval(x, y, XSIZE, YSIZE);
			x += dx;  y += dy;
			Dimension d = box.getSize(); 
			if (x < 0) { x = 0; dx = -dx; }
			if (x + XSIZE >= d.width) { x = d.width - XSIZE; dx = -dx; } 
			if (y < 0) { y = 0; dy = -dy; }
			if (y + YSIZE >= d.height) { y = d.height - YSIZE; dy = -dy; } 
			g.fillOval(x, y, XSIZE, YSIZE); 
			g.dispose();
		}
		public void bounce() {
			draw(); 
			for (int i = 1; i <= 1000; i++) {  
				move(); 
				try { Thread.sleep(5); } catch(InterruptedException e) {}
			}
		}
	}
}

