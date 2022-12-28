package ac.sogang.cs.chap2;

public class Continue {

	public static void main(String[] args) {
		int n = 1;
		while (n != 0) {
			n = (int)(5 * Math.random())-2;
			System.out.print(n + " ");
			if(n < 0) {
				System.out.println("n is negative number!");
				continue;
			}
			System.out.println("square root : " + Math.sqrt(n));
		}
	}
}