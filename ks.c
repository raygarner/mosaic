#include <stdio.h>

#include "common.h"

int
main(int argc, char *argv[])
{
	int n, m, a;

	if (argc < 4) {
		printf("Please pass key and accidental as args\n");
		printf("eg: ks an ionian +\n");
		return ERROR;
	}
	n = clock_mod(read_note(argv[1][0]) + \
	              read_accidental(argv[1][1]), TONES);
	m = read_mode(argv[2]);
	a = read_accidental(argv[3][0]);
	if (is_correct_accidental(n, m, a))
		printf("Valid accidental\n");
	else
		printf("Invalid accidental\n");
	return 0;
}
