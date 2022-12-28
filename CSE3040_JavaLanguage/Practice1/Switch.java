package ac.sogang.cs.chap2;

public class Switch {

	public static void main(String[] args) {
		int n;
		// n : 10 ~ 20 random number(integer)
		n = (int)( 11 * Math.random()) + 10;
		System.out.println("Random number : " + n);
		System.out.println("n % 5 : " + (n % 5) + "\n");
		switch(n % 5)
		{
			case 0 : System.out.println("remainder 0"); break;
			case 1 : System.out.println("remainder 1"); break;
			case 2 : System.out.println("remainder 2"); break;
			default: System.out.println("remainder 3 or 4"); break;
		}
	}

}
