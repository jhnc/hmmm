/*
 * hmmm.c
 *
 * A simpleminded word generator based on hidden markov models
 * by Jonathan H N Chin <jc254@newton.cam.ac.uk>, 22 July 2000
 *
 * Given a list, one word per line, it will output "similar" words.
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 28	/* [a-z] + space + terminator */

/* declare these globally; they don't fit on the stack... */
unsigned short A[SIZE][SIZE][SIZE][SIZE][SIZE];
unsigned max[SIZE][SIZE][SIZE][SIZE];

int verbose = 0;
void chatter(char *stuff){
	if (verbose){
		fprintf(stderr, stuff);
		fflush(stderr);	
	}
}

int main(int argc, char *argv[]){
	char *spinner[] = {"\b-", "\b\\", "\b|", "\b/"};
	int s = 0, words = 0, input, count, depth, seed;
	int gdepth = SIZE, hdepth = SIZE, idepth = SIZE;
	int g, h, i, j, k;

	if (argc==5 && !strcmp("-v", argv[1])){
		verbose = 1;
	}

	chatter("\n  A simpleminded random wordalike generator.\n");

	if (argc<4 || argc>5 || (argc==5 && !verbose)){
		fprintf(stderr,
			"\n  Usage:"
			"\n    hmmm [-v] COUNT DEPTH SEED <WORDS\n"
			"\n    where:"
			"\n      -v    = be verbose"
			"\n      COUNT = how many words to generate"
			"\n      DEPTH = integer from 1..4 inclusive"
			"\n      SEED  = a random integer"
			"\n      WORDS = a list, one item per line, "
			"containing [A-Za-z ]\n"
			"\n  Warning:"
			"\n    needs nearly 70Mb of RAM to run.\n\n");
		fflush(stderr);
		exit(1);
	}

	count = atoi(argv[argc==4 ? 1 : 2]);
	if (count<1){
		chatter("\n  bogus count\n");
		count = 1;
	}

	depth = atoi(argv[argc==4 ? 2 : 3]);
	if (depth<1 || depth>4){
		chatter("\n  bogus depth\n");
		depth = 3;
	}
	switch (depth){
		/* fallthrough */
		case 1: idepth = 1;
		case 2: hdepth = 1;
		case 3: gdepth = 1;
	}

	seed = atoi(argv[argc==4 ? 3 : 4]);
	if (seed<0){
		chatter("\n  bogus seed\n");
		seed = 0;
	}

/*-------------------------------------------------------------------------*/
if (verbose){
	fprintf(stderr, "  count=%d, depth=%d, seed=%d\n",
		count, depth, seed);
	fflush(stderr);
}
/*=========================================================================*/

	chatter("  reading data   ");
	g = h = i = j = k = 0;
	while ((input = getchar())!=EOF){
		input = tolower(input);
		if (isalpha(input)){
			max[g][h][i][j] += 1;
			A[g][h][i][j][1+input-'a'] += 1;
			if (depth>3) g = h;
			if (depth>2) h = i;
			if (depth>1) i = j;
			j = 1+input-'a';
		} else if (input==' ') {
			max[g][h][i][j] += 1;
			A[g][h][i][j][SIZE-1] += 1;
			if (depth>3) g = h;
			if (depth>2) h = i;
			if (depth>1) i = j;
			j = SIZE-1;
		} else if (input=='\n') {
			max[g][h][i][j] += 1;
			A[g][h][i][j][0] += 1;
			g = h = i = j = 0;
			chatter(spinner[s]);
			s = (s+1)%4;
			words++;
		}
	}
	A[0][0][0][0][0] = 0;	/* don't care about empty lines */
/*-------------------------------------------------------------------------*/
if (verbose){
	fprintf(stderr, "\b\b- got %d line%s of input",
		words,
		words==1 ? "" : "s");
	fflush(stderr);
}
/*=========================================================================*/

	chatter("\n  generating words\n\n");
	srand(seed);
	while (count-->0){
		float n, where;

		g = h = i = j = k = 0;
loop:
		if (depth>3) g = h;
		if (depth>2) h = i;
		if (depth>1) i = j;
		j = k;
		n = ((float)rand())/((float)RAND_MAX);

		for (k = where = 0; k<SIZE; k++){
			where += ((float)A[g][h][i][j][k]/max[g][h][i][j]);
			if (where>=n)
				break;
		}

		if (k==0){
			putchar('\n');
		} else {
			int letter = k+'a'-1;

			if (islower(letter)){
				putchar(letter);
				goto loop;
			} else if (k==(SIZE-1)){
				putchar(' ');
				goto loop;
			} else {
				chatter("Internal Error!\n");
				exit(2);
			}
		}
	}
	
	exit(0);
}

