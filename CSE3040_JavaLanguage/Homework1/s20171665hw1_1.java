package hw1;

public class s20171665hw1_1 {

	public static void main(String[] args) {
		int i, sum = 0;
		
		for (i = 0; i <= 1000; i++) {
			if(i % 2 == 1){
				sum += i;
			}
		}
		System.out.println("The sum of all odd numbers from 0 to 1000 is " + sum);

	}

}
