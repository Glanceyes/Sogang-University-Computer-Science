package chap7;


import java.awt.*; 
import java.awt.event.*; 



public class thSunWi extends Frame implements ActionListener { 
	private Canvas cvas; 
	public thSunWi() { 
		cvas = new Canvas();         
		add("Center", cvas); // 프레임에 그림상자 cvas를 배치 
		cvas.setBackground(Color.white);  // cvas 바탕색 -> 하양 
		Panel p = new Panel(); // 프레임에 패널 p를 배치 
		Button pen = new Button("Penguin"); 
		Button car = new Button("Car"); 
		Button end = new Button("Exit"); 
		p.add(pen); 
		p.add(car); 
		p.add(end); 
		pen.addActionListener(this); 
		car.addActionListener(this); 
		end.addActionListener(this); 
		add("South", p); 
	} 
	public void actionPerformed(ActionEvent evt) {  
		if (evt.getActionCommand().equals("Penguin")) {
			thIdong pgwin = new thIdong(cvas, "src\\pgwin.gif", 0);
			pgwin.setPriority(Thread.NORM_PRIORITY);
			pgwin.start();
		}
		else if (evt.getActionCommand().equals("Car")) {
			thIdong cha = new thIdong(cvas, "src\\CHA.GIF", 1);
			cha.setPriority(Thread.NORM_PRIORITY + 2);
			cha.start();
		}
		else if (evt.getActionCommand().equals("Exit")) 
			System.exit(0);                
	}
	
	public static void main(String[] args) { 
		Frame f = new thSunWi(); 
		f.setSize(500, 300); 
		WindowDestroyer listener = new WindowDestroyer();  
		f.addWindowListener(listener); 
		f.setVisible(true);  
	}
	class thIdong extends Thread {  
		private Canvas cvas; 
		private int x = 0, y, l=10;    // img in x coordinate 
		private Image img; 
		private Graphics g;
		public thIdong(Canvas cv, String imgfile, int n) { 
			cvas = cv; 
			y = 30 + 100*n; 
			img = Toolkit.getDefaultToolkit().getImage(imgfile);       
			g = cvas.getGraphics();    
			g.setXORMode(cvas.getBackground()); 
		} 
		public void run() {  
			while (true) // infinite while loop 
				IDong(); 
		}
		
		public void IDong() {  
			g = cvas.getGraphics();            // img를 그리고 
			g.drawImage(img , x, y, cvas); // 0.03 초 동안 쉰 후 
			try { Thread.sleep(30); } 
			catch(InterruptedException e) {}    // img를 지운다 
			g.drawImage(img , x, y, cvas);     
			Dimension  d = cvas.getSize(); 
			if (x >= d.width-50)     // reaching right side of window 
			{ x = d.width-50; l = -l; } 
			if (x<0)                 // reaching left side of window 
			{ x = 0; l = -l; } 
			x += l; 
		} 
	}
}