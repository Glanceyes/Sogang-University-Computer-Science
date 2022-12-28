package ac.sogang.cs.chap2;

public class Break2 {

	public static void main(String[] args) {
		int i, n;
		char c = 'A';
		WHILEFOR:
			while(true)
			{
				n = (int)(6 * Math.random()) + 5;
				System.out.print(n + " ");
				for (i = 1; i <= n; i++)
				{
					System.out.print(c + " ");
					if(c == 'Q')
						break WHILEFOR;
					c++;
				}
				System.out.println();
			}
	}
}
