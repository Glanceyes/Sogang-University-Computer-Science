package ac.sogang.cs.chap2;

public class Break1 {

	public static void main(String[] args) {
		int n, sum = 0;
		while(true) {
			n = (int) (11 * Math.random()) + 10;
			System.out.print(n + " ");
			if (n == 15) // if n is 15, break out of a loop
				break;
			sum += n; //sum = sum + n
		}
		System.out.println("\nsum : " + sum);
	}

}
