#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <termios.h>
#include <sys/time.h>
#include <unistd.h>

/* game over flag & board state */
int game_over, b[4][4];
FILE *fp;

void init(){
	int i, j;
	game_over = 0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			b[i][j] = 0;
}


void rotate(int b[4][4]){
	/* user code */ 
	int b_copy1[4][4] = {0};

	int column = 3;
	for (int i = 0; i<4; i++, column--) {
		int row = 0;
		for (int j = 0; j<4; j++) {
			b_copy1[row++][column] = b[i][j];
		}
	}
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			b[i][j] = b_copy1[i][j];

}

int set_board(int dir, int b[4][4]){

	/* user code init */
	int b_copy1[4][4] = {0};
	int b_copy2[4][4] = {0};
	for (int i = 0; i<4; i++)
		for (int j = 0; j<4; j++)
			b_copy1[i][j] = b[i][j];

	for (int i = 0; i < dir; i++) {
		rotate(b);
	}


	for (int row = 0; row < 4; row++) {
		int counter = 0;
		do {
			for (int q = 2; q >= 0; q--) {
				if (b[row][q] == 0) continue;
				if (b[row][q + 1] == 0) {
					b[row][q + 1] = b[row][q];
					b[row][q] = 0;
				}
				if (b[row][q + 1] == b[row][q]){
					if (b_copy2[row][q] != 1 && b_copy2[row][q + 1] != 1){
						b[row][q + 1] *= 2;
						b[row][q] = 0;
						b_copy2[row][q + 1] = 1; 
					}
				}
			}
			counter++;
		} while(counter < 3);
		for( int q = 0; q < 4; q++){
			b_copy2[row][q] = 0; 
		}
	}

	for (int y = 0; y < 4 - dir; y++){
		rotate(b);
	}
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			if (b[i][j] == 0 && b_copy1[i][j] == 0){
				continue;
			}
			else {
				if (b[i][j] != b_copy1[i][j]) return 1;
			}
		}
	}
	return 0;
}

int is_game_over(){
	/* user code */

	int i, j, k = 0;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 3; j++) {
			if (b[i][j] == b[i][j + 1]) k = 1;
			if (b[j][i] == b[j + 1][i]) k = 1;
		}
	}
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (b[i][j] == 0) k = 1;
		}
	}


	if (k != 1){
		return 1;
	}
	/* if game over return 1, else then return 0 */

	return 0;
}

void draw_board(int tot, int command){
	int i, j, k, c[8][8], score;
	/* console clear */
	system("clear");

	score = 0;
	/* user code */
	
	int tot_copy = 0;

	for (i = 0; i <= 3; i++) {
		for (j = 0; j <= 3; j++) {
			if (score < b[i][j]) score = b[i][j];
			tot_copy += b[i][j];
		}
	}
	if (tot_copy != tot){ 
		printf("Error!");
	}

	/* calculate score & check sum of all block equasls variable tot */



	printf("    Score : %d\n", score);
	fprintf(fp, "%d %d\n", score, command);

	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
			c[i][j] = 32;
			for(k=0; k<50; k++){
				if( b[i][j] == (1<<k) ){
					c[i][j] = 32 + (k%6);
					break;
				}
			}

		}
	}
	for(i=0; i<4; i++){
		for(j=0; j<4; j++)
			fprintf(fp, "%d ", b[i][j]);
		fprintf(fp, "\n");
	}

	for(i=0; i<4; i++){
		puts("");
		puts("");
		for(j=0; j<4; j++)
			printf("\033[%dm%5d\033[0m", c[i][j], b[i][j]);
		puts("");
		puts("");
	}
}

int make_two_or_four(){
	/* user code */
	int x, y, t = 0;//2»ý¼ºÈ®·ü 2/3 4´Â 1/3
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (b[i][j] == 0) t = 1;
		}
	}

	if (t == 1){
		srand(time(NULL));
		int random = rand() % 3;
		if (random == 0){ 
			t = 4;
		}
		else{
			t = 2;
		}
	/* return 2 or 4 that makes in this times */
		while (1) {
			x = rand() % 4;
			y = rand() % 4;

			if (b[x][y] == 0) {
				b[x][y] = t;
			return t;
			}	
		}
	}
	/* return 2 or 4 that makes in this times */

	/* if can not make two or four, then return 0 */
	return 0;	
}


int getch(void)
{
	char   ch;
	int   error;
	static struct termios Otty, Ntty;

	fflush(stdout);
	tcgetattr(0, &Otty);
	Ntty = Otty;
	Ntty.c_iflag = 0;
	Ntty.c_oflag = 0;
	Ntty.c_lflag &= ~ICANON;
#if 1
	Ntty.c_lflag &= ~ECHO;
#else
	Ntty.c_lflag |= ECHO;
#endif
	Ntty.c_cc[VMIN] = 0;
	Ntty.c_cc[VTIME] = 1;

#if 1
#define FLAG TCSAFLUSH
#else
#define FLAG TCSANOW
#endif

	if (0 == (error = tcsetattr(0, FLAG, &Ntty)))
	{
		error = read(0, &ch, 1);
		error += tcsetattr(0, FLAG, &Otty);
	}

	return (error == 1 ? (int)ch : -1);
}


int GetCommand() {
	int ch = getch();

	switch (ch)
	{
	case 'd':
	case 'D': return 0;
	case 'w':
	case 'W': return 1;
	case 'a':
	case 'A': return 2;
	case 's':
	case 'S': return 3;
	default: return -1;
	}

	return -1;
}


int main() {
	int command = -1;
	fp = fopen("output.txt", "w");
	init();
	srand(time(NULL));

	int tot = 0;

	/* make init board */
	/* user code */
	tot += make_two_or_four();
	tot += make_two_or_four();

	draw_board(tot, -1);

	
	do {
		command = GetCommand();

		if (command != -1) {
			if( set_board(command, b) ){

				/* user code */
				tot += make_two_or_four();
				game_over = is_game_over();

				draw_board(tot, command);
			}
		}

	} while (!game_over);

	system("clear");
	printf("game over!\n");
	fclose( fp );
}
