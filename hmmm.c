/*
 * hmmm.c
 *
 * A simpleminded word generator based on hidden markov models
 * by Jonathan H N Chin <jc254@newton.cam.ac.uk>, 21 July 2000
 *
 * Given a list, one word per line, it will output "similar" words.
 *
 * This program has not been optimised at all.
 * It needs ~70Mb to run, assuming 4-byte floats.
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 28	/* [a-z] + space + terminator */

/* declare this globally; it won't fit on the stack... */
float A[SIZE][SIZE][SIZE][SIZE][SIZE];

#define CHATTER(stuff)					\
	do{						\
		if (!quiet){				\
			fprintf(stderr, (stuff));	\
			fflush(stderr);			\
		}					\
	}while(0)

int main(int argc, char *argv[]){
	char spinner[] = "-\\|/";
	int s = 0, words = 0, quiet = 0, input, count, depth, seed;
	int gdepth, hdepth, idepth;
	int g, h, i, j, k;

	if (argc==5 && !strcmp("-q", argv[1])){
		quiet = 1;
	}

	CHATTER("\n   A simpleminded random wordalike generator.\n");

	if (argc<4 || argc>5 || (argc==5 && !quiet)){
		fprintf(stderr,
			"\n   Usage:"
			"\n      hmmm [-q] COUNT DEPTH SEED <WORDS\n"
			"\n      where:"
			"\n         -q    = turn off chatter"
			"\n         COUNT = how many words to generate"
			"\n         DEPTH = integer from 1..4 inclusive"
			"\n         SEED  = a random integer"
			"\n         WORDS = a list, one item per line, "
			"containing [A-Za-z ]\n"
			"\n   Warning:"
			"\n      needs nearly 70Mb of RAM to run.\n\n");
		fflush(stderr);
		exit(1);
	}

	count = atoi(argv[argc==4 ? 1 : 2]);
	if (count<1){
		CHATTER("\n   bogus count\n");
		count = 1;
	}

	depth = atoi(argv[argc==4 ? 2 : 3]);
	if (depth<1 || depth>4){
		CHATTER("\n   bogus depth\n");
		depth = 3;
	}

	gdepth = hdepth = idepth = SIZE;
	switch (depth){
		/* fallthrough */
		case 1: idepth = 1;
		case 2: hdepth = 1;
		case 3: gdepth = 1;
	}

	seed = atoi(argv[argc==4 ? 3 : 4]);
	if (seed<0){
		CHATTER("\n   bogus seed\n");
		seed = 0;
	}

/*-------------------------------------------------------------------------*/
if (!quiet){
	fprintf(stderr,
		"\n   count=%d, depth=%d, seed=%d\n"
		"\n   initialising arrays      "
		"[                            ]"
		"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b",
		count, depth, seed);
	fflush(stderr);
}
/*=========================================================================*/

	for (g = 0; g<gdepth; g++){
		for (h = 0; h<hdepth; h++){
			for (i = 0; i<idepth; i++){
				for (j = 0; j<SIZE; j++){
					for (k = 0; k<SIZE; k++)
						A[g][h][i][j][k] = 0;
					if (depth==1) CHATTER(".");
				}
				if (depth==2) CHATTER(".");
			}
			if (depth==3) CHATTER(".");
		}
		if (depth==4) CHATTER(".");
	}
	CHATTER("\n   reading data              ");

	g = h = i = j = k = 0;
	while ((input = getchar())!=EOF){
		input = tolower(input);
		if (isalpha(input)){
			A[g][h][i][j][1+input-'a'] += 1;
			if (depth>3) g = h;
			if (depth>2) h = i;
			if (depth>1) i = j;
			j = 1+input-'a';
		} else if (input==' ') {
			A[g][h][i][j][SIZE-1] += 1;
			if (depth>3) g = h;
			if (depth>2) h = i;
			if (depth>1) i = j;
			j = SIZE-1;
		} else if (input=='\n') {

			A[g][h][i][j][0] += 1;
			g = h = i = j = 0;
/*-------------------------------------------------------------------------*/
if (!quiet){
			fprintf(stderr, "\b%c", spinner[s]);
			fflush(stderr);
}
/*=========================================================================*/
			s = (s+1)%4;
			words++;
		}
	}

	A[0][0][0][0][0] = 0;	/* don't care about empty lines */

/*-------------------------------------------------------------------------*/
if (!quiet){
	fprintf(stderr,
		"\bgot %d line%s of input\n   calculating percentiles  "
		"[                            ]"
		"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b",
		words,
		words==1 ? "" : "s");
	fflush(stderr);
}
/*=========================================================================*/

	for (g = 0; g<gdepth; g++){
		for (h = 0; h<hdepth; h++){
			for (i = 0; i<idepth; i++){
				for (j = 0; j<SIZE; j++){
					float max = 0, pc = 0;

					for (k = 0; k<SIZE; k++){
						max += A[g][h][i][j][k];
					}
					for (k = 0; k<SIZE; k++){
						A[g][h][i][j][k] /= max;
						A[g][h][i][j][k] += pc;
						pc = A[g][h][i][j][k];
					}
					if (depth==1) CHATTER(".");
				}
				if (depth==2) CHATTER(".");
			}
			if (depth==3) CHATTER(".");
		}
		if (depth==4) CHATTER(".");
	}

	CHATTER("\n   generating words\n\n");

	srand(seed);

	while (count-->0){
		float n;

		g = h = i = j = k = 0;
loop:
		if (depth>3) g = h;
		if (depth>2) h = i;
		if (depth>1) i = j;
		j = k;
		n = ((float)rand())/((float)RAND_MAX);

		for (k = 0; k<SIZE && n>A[g][h][i][j][k]; k++)
			;

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
				CHATTER("Internal Error!\n");
				exit(2);
			}
		}
	}
	
	CHATTER("\n   finished\n\n");
	exit(0);
}

