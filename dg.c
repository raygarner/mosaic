/* dg 577655 */
/* convert tab to chord diagram */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define STRINGS 6
#define FRETS 5
#define STRING_WIDTH 3
#define REST -1
#define REST_SYMBOL 'x'
#define INF INT_MAX
#define OPEN 0

void
print_fret(char c)
{
	int i;

	for (i = 0; i < STRING_WIDTH * STRINGS; i++)
		putchar(c);
	putchar('\n');
}

int
read_fret(char c)
{
	char tmp[2];
	
	if (tolower(c) == REST_SYMBOL)
		return REST;
	tmp[1] = '\0';
	tmp[0] = c;
	return atoi(tmp);
}

int
main(int argc, char *argv[])
{
	int i, lowest = INF, chord[STRINGS], s, f;
	
	if (argc < 2) {
		printf("Please pass tab as argument\n");
		printf("eg: md 577655\n");
		return -1;
	}
	if (strlen(argv[1]) != STRINGS) {
		printf("Please use 6 digits for tab\n");
		printf("eg: md 577655\n");
		return -1;
	}
	for (i = 0; i < STRINGS; i++) {
		f = read_fret(argv[1][i]);
		if (f < lowest && f != REST && f != OPEN)
			lowest = f;
		chord[i] = f;
	}
	for (i = 0; i < STRINGS; i++) {
		if (chord[i] == REST)
			printf(" x ");
		else if (chord[i] == OPEN)
			printf(" o ");
		else
			printf("   ");
	}
	putchar('\n');
	print_fret('-');
	for (f = lowest; f < lowest+FRETS; f++) { 
		for (s = 0; s < STRINGS; s++) {
			if (chord[s] == f)
				printf(" O ");
			else
				printf(" | ");
		}
		if (f == lowest)
			printf("  %d", lowest);
		putchar('\n');
		print_fret('-');
	}
	return 0;
}
