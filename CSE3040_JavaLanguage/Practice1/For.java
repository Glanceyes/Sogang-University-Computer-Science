package ac.sogang.cs.chap2;

public class For {

	public static void main(String[] args) {
		// i, sum (integer type variable)
		int i, sum = 0;
		
		// for loop of sum 1 ~ 10
		for (i = 1 ; i <= 10; i++) {
			System.out.print(i + ""); 
			sum += i;
		}
		System.out.println();
		System.out.println("1 ~ 10 sum : " + sum);
	}

}
