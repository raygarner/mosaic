#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

void
output_chord(int key_root, int mode, int chord_degree, const char extensions[],
             int extlen)
{
	int alter, n, chord_root, i;
	char ext;

	alter = get_correct_accidental(key_root, mode);
	chord_root = apply_steps(I, mode, key_root, chord_degree);
	n = chord_root;
	print_note(alter, n);
	putchar(' ');
	n = apply_steps(chord_degree, mode, chord_root, III);
	print_note(alter, n);
	putchar(' ');
	n = apply_steps(chord_degree, mode, chord_root, V);
	print_note(alter, n);
	putchar(' ');
	for (i = 0; i < extlen; i++) {
		ext = extensions[i];
		n = apply_steps(chord_degree, mode, chord_root, atoi(&ext)-1);
		print_note(alter, n);
		putchar(' ');
	}
	printf("- ");
	print_note(alter, key_root);
	putchar(' ');
	printf("%s\n", MODES[mode]);
}

int
main(int argc, char *argv[])
{
	char *extensions;
	int extlen, note, mode, chord_degree, key_freq[TONES][DEGREES];

	if (argc > 3 && argv[3][0] == ALL_KEYS) {
		init_key_field(key_freq, TRUE);
		extensions = argv[2];
		extlen = strlen(extensions);
	} else if (argc > 2) {
		if (argv[2][0] == ALL_KEYS) {
			init_key_field(key_freq, TRUE);
			extensions = NULL;
			extlen = 0;
		} else {
			init_key_field(key_freq, FALSE);
			read_key_list(key_freq, TRUE);
			extensions = argv[2];
			extlen = strlen(extensions);
		}
	} else if (argc > 1) {
		extensions = NULL;
		extlen = 0;
		init_key_field(key_freq, FALSE);
		read_key_list(key_freq, TRUE);
	} else {
		printf("Please pass chord degree and (optionally) "
		       "extensions\n");
		printf("eg: crd 1 7\n");
		return 1;
	}
	chord_degree = atoi(argv[1]) - 1;
	for (note = 0; note < TONES; note++) {
		for (mode = 0; mode < DEGREES; mode++) {
			if (key_freq[note][mode])
				output_chord(note, mode, chord_degree, \
				             extensions, extlen);
		}
	}
	return 0;
}
