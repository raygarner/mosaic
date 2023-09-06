#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"

int
degree_val(int d, int m)
{
	int n, v = 0;
	
	for (n = 0; n < d-1; n++)
		v += MAJOR_SCALE[(n+m)%DEGREES];
	return v;
}

int
correct_alteration(int d, int m, int alter)
{
	int x = degree_val(d, m) - degree_val(d, IONIAN);
	
	switch (alter) {
	case NATURAL:
		return x == alter;
	case FLAT:
		return x < 0;
	case SHARP:
		return x > 0;
	}
	return ERROR;
}

int
main(int argc, char *argv[])
{
	int m, n, i, len, key_field[TONES][DEGREES], alter, d;
	char tmp;

	if (argc < 2) {
		printf("Please pass alterations to major scale degrees\n");
		printf("eg: int -3-7n6 (flat 3rd, flat 7th, natural 6th\n");
		return 1;
	}
	if (argc > 2 && argv[2][0] == ALL_KEYS) {
		init_key_field(key_field, 0);
	} else {
		init_key_field(key_field, -999);
		read_key_list(key_field, 0);
	}
	len = strlen(argv[1]);
	if (len % 2) {
		printf("Please pass alterations to major scale degrees\n");
		printf("eg: int -3-7n6 (flat 3rd, flat 7th, natural 6th)\n");
		return 1;
	}
	for (i = 0; i < len-1; i+=2) {
		for (m = 0; m < DEGREES; m++) { /* for each mode */
			alter = read_accidental(argv[1][i]);
			tmp = argv[1][i+1];
			d = atoi(&tmp);
			if (correct_alteration(d, m, alter)) {
				for (n = 0; n < TONES; n++)
					key_field[n][m]++;
			}
		}
	}
	print_matching_keys(key_field, len/2);
	return 0;
}
