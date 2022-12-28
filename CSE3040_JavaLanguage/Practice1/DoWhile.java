package ac.sogang.cs.chap2;

public class DoWhile {

	public static void main(String[] args) {
		int i = 0, n;
		long sum = 0;
		n = (int) (6 * Math.random())+10;
		System.out.println("Random number : " + n);
		do {
			sum += i;
			i++;
		}
		while (i<= n);
		System.out.println("\n i : " + i);
		System.out.println("1 ~ n sum: " + sum);
	}
}
