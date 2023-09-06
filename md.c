#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "common.h"

void check_relative_modes(int, int, int[TONES][DEGREES]);
int process_notes(const char[], int, int[TONES][DEGREES]);

void
check_relative_modes(int tonic, int mode, int key_freq[TONES][DEGREES])
{
	int current_note = tonic, d;
	
	for (d = 0; d < DEGREES; d++) {
		key_freq[current_note][(d+mode)%DEGREES]++;
		current_note = step(d, current_note, mode);
	}
}

int
process_notes(const char notes[], int len, int key_freq[TONES][DEGREES])
{
	int note_count = 0, note, m, i = 0;

	while (i < len) {
		note = read_tone(notes[i], notes[i+1]);
		note_count++;
		for (m = 0; m < DEGREES; m++)
			check_relative_modes(note, m, key_freq);
		i += 2;
	}
	return note_count;
}

int
main(int argc, char *argv[])
{
	int key_freq[TONES][DEGREES], note_count;

	if (argc < 2) {
		printf("Please pass notes as args\n");
		printf("eg: md cnf+\n");
		return -1;
	}
	if (argc > 2 && argv[2][0] == ALL_KEYS) {
		init_key_field(key_freq, 0); /* all keys are allowed */
	} else {
		init_key_field(key_freq, -999);
		read_key_list(key_freq, 0);
	}
	note_count = process_notes(argv[1], strlen(argv[1]), key_freq);
	print_matching_keys(key_freq, note_count);
	return 0;
}
