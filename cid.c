#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "common.h"

#define TRIAD_TONES 3
#define INVALID 999
#define NO_SIGN 99

enum { MIN_THIRD = 3, MAJ_THIRD = 4 , MIN_FIFTH_TONES = 5, TRITONE = 6 };

int 
get_third_index(int j, int k)
{
	return TRIAD_TONES - (j + k);
}

/*  1 = c major */
/* -1 = c minor */
/*  100 = c diminished */
int
id_triad(int x, int y, int z)
{
	int triad[TRIAD_TONES] = {x, y, z}, j, k, d, root = -1, third, jd, kd;

	/* find root by checking for fifth interval */
	for (j = 0; j < TRIAD_TONES; j++) {
		for (k = 0; k < TRIAD_TONES; k++) {
			d = min_tone_diff(triad[j], triad[k]);
			switch (d) {
			case MIN_FIFTH_TONES:
				root = triad[k];
				break;
			case -MIN_FIFTH_TONES:
				root = triad[j];
				break;
			case TRITONE:
			case -TRITONE:
				third = get_third_index(j, k);
				jd = min_tone_diff(triad[j], triad[third]);
				kd = min_tone_diff(triad[k], triad[third]);
				if (jd == MIN_THIRD)
					return (triad[j] + 1) * 100;
				else if (kd == MIN_THIRD)
					return (triad[k] + 1) * 100;
				else
					return INVALID;
			}
		}
		if (root != -1)
			break;
	}
	if (root == -1)
		return INVALID;
	/* check tonality by checking interval between third and root */
	for (j = 0; j < TRIAD_TONES; j++) {
		if (triad[j] == clock_mod(root + MIN_THIRD, TONES))
			return 0 - (root + 1);
		else if (triad[j] == clock_mod(root + MAJ_THIRD, TONES))
			return root + 1;
	}
	return INVALID;
}

int
main()
{
	char c, buf[BUFLEN];
	int note, pitches[TRIAD_TONES], i, chord, tonality, sign = NO_SIGN, 
	    cont = TRUE, root, mode, n, a, s;

	c = getchar();
	while (1) {
		if (c == EOF)
			break;
		i = 0;
		while (i < TRIAD_TONES) {
			while (isspace(c)) {
				c = getchar();
			}
			if (c == EOF)
				exit(0);
			a = getchar();
			note = read_tone(c, a);
			s = read_accidental(a);
			if (s != NATURAL)
				sign = s;
			pitches[i] = note;
			i++;
			c = getchar();
		}
		chord = id_triad(pitches[0], pitches[1], pitches[2]);
		if (chord == INVALID) {
			printf("Not a valid triad\n");
			return 1;
		}
		if (chord < 0) {
			tonality = AEOLIAN;
			chord = abs(chord);
		} else if (chord > TONES+1) {
			tonality = LOCRIAN;
			chord = chord / 100;
		} else {
			tonality = IONIAN;
		}
		chord--;
		/* eat whitespace and mode data */
		while (isspace(c)) {
			c = getchar();
		}
		if (cont == FALSE)
			break;
		if (c == '-') {
			/* eat mode data */
			getchar();
			n = getchar();
			a = getchar();
			root = read_tone(n, a);
			scanf(STRINP, buf);
			mode = read_mode(buf);
			c = getchar();
		} else {
			root = chord;
			mode = tonality;
		}
		if (sign == NO_SIGN) {
			if (is_correct_accidental(root, mode, FLAT))
				sign = FLAT;
			else
				sign = SHARP;
		}
		print_note(sign, chord);
		putchar(' ');
		switch (tonality) {
		case AEOLIAN:
			printf("min");
			break;
		case IONIAN:
			printf("maj");
			break;
		case LOCRIAN:
			printf("dim");
			break;
		}
		putchar('\n');
	}
	return 0;
}
