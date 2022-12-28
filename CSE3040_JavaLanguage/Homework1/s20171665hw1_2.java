package hw1;

public class s20171665hw1_2 {

	public static void main(String[] args) {
		int win_num[] = {7, 18, 32, 37, 44}, i, j, result = 0, counter = 0, ran_num[] = {};
		
		while(result < 3) {
			ran_num = new int[5];
			result = 0;
			for(i = 0; i < 5; i++) {
				ran_num[i] = (int)(50 * Math.random());
				for (j = 0; j < i; j++) {
					if(ran_num[i] == ran_num[j]) {
						i--;
						break;
					}
				}
			}
			for (i = 0; i < 5; i++) {
				for(j = 0; j < 5; j++) {
					if(ran_num[i] == win_num[j]) {
						result++;
						break;
					}
				}
			}
			counter++;
		}
		for (i = 0; i < 5; i++) {
			System.out.println(ran_num[i]);
		}
		System.out.println("The number of iterations is " + counter);
	}

}
