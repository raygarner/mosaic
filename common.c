#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

const int MAJOR_SCALE[DEGREES] = { TONE, TONE, SEMITONE, TONE, TONE, TONE, 
                                   SEMITONE };
const int ALTERED_SCALE[DEGREES] = { SEMITONE, TONE, SEMITONE, TONE, TONE,
                                     TONE, TONE };
const char *MODES[DEGREES] = { "Ionian", "Dorian", "Phrygian", "Lydian", 
                               "Mixolydian", "Aeolian", "Locrian" };
const char *ALT_MODES[DEGREES] = { "Altered 1", "Altered 2", "Altered 3",
                                   "Altered 4", "Altered 5", "Altered 6",
                                   "Altered 7" };
const char *NOTES[TONES] = { "Cn", "C+", "Dn", "E-", "En", "Fn", "F+", "Gn", "A-",
                             "An", "B-", "Bn" };

int
read_accidental(char a)
{
	switch (a) {
	case '+':
	case '#':
		return SHARP;
	case '-':
	case 'b':
		return FLAT;
	case 'n': /* fallthrough */
	case WILDCARD_ICON: 
		return 0;
	}
	printf("Invalid input %c for accidental\n", a);
	exit(ERROR);
}

int
read_note(char n)
{
	switch (tolower(n)) {
	case WILDCARD_ICON:
		return X;
	case 'c':
		return C;
	case 'd':
		return D;
	case 'e':
		return E;
	case 'f':
		return F;
	case 'g':
		return G;
	case 'a':
		return A;
	case 'b':
		return B;
	}
	printf("Invalid input format for note: %c\n", n);
	exit(ERROR);
}

int
read_tone(char n, char a)
{
	return clock_mod((read_note(n) + read_accidental(a)), TONES);
}

int
read_mode(const char *input)
{
	int m;
	char copy[2];

	if (input == NULL)
		return ERROR;
	copy[0] = toupper(input[0]);
	copy[1] = tolower(input[1]);
	for (m = 0; m < DEGREES; m++)
		if (strncmp(copy, MODES[m], 2) == 0)
			return m;
	return ERROR;
}

void
init_key_field(int key_field[TONES][DEGREES], int def)
{
	int n, m;
	
	for (n = 0; n < TONES; n++)
		for (m = 0; m < DEGREES; m++)
			key_field[n][m] = def;
}

int
clock_mod(int x, int mod)
{
	if (x == X)
		return x;
	return x < 0 ? mod - (abs(x) % mod) : x % mod;
}

int
custom_step(int degree, int note, int mode, const int intervals[DEGREES])
{
	return (note + intervals[(degree + mode) % DEGREES]) % TONES;
}

int
step(int degree, int note, int mode)
{
	return (note + MAJOR_SCALE[(degree + mode) % DEGREES]) % TONES;
}

void
read_key_list(int key_field[TONES][DEGREES], int val)
{
	char c, buf[BUFLEN];
	int note, mode, i;

	while ((c = getchar()) != EOF) {
		if (isspace(c))
			continue;
		note = read_tone(c, getchar());
		scanf(STRINP, buf);
		mode = read_mode(buf);
		if (note == X) {
			for (i = 0; i < TONES; i++)
				key_field[i][mode]= val;
		} else {
			key_field[note][mode] = val;
		}
	}
}

void
print_matching_keys(int key_freq[TONES][DEGREES], int len)
{
	int n, m, alter;

	for (n = 0; n < TONES; n++) {
		for (m = 0; m < DEGREES; m++) {
			if (key_freq[n][m] == len) {
				alter = get_correct_accidental(n, m);
				print_note(alter, n);
				putchar(' ');
				printf("%s\n", MODES[m]);
			}
		}
	}
}

int
calc_degree(int note, int root, int mode)
{
	int d, cn = root;

	for (d = 0; d < DEGREES; d++) {
		if (cn == note)
			return d;
		cn = step(d, cn, mode);
	}
	return ERROR;
}

int
is_diatonic(int note, int root, int mode)
{
	return calc_degree(note, root, mode) == ERROR ? FALSE : TRUE;
}

int
is_accidental(int note)
{
	return !is_diatonic(note, C, IONIAN);
}

int
is_correct_accidental(int root, int mode, int accidental)
{
	int d, cn = root;

	if (clock_mod(root+mode,TONES) == C+1 || 
	clock_mod(root+mode,TONES) == F+1 || clock_mod(root+mode,TONES) == B)
		return TRUE;
	for (d = 0; d < DEGREES; d++) {
		if (is_accidental(cn) && \
		is_diatonic(clock_mod(cn+accidental*-1, TONES), root, mode) &&\
		is_accidental(clock_mod(cn+accidental*-2, TONES))) {
			return FALSE;
		}
		cn = step(d, cn, mode);
	}
	return TRUE;
}

int
get_correct_accidental(int root, int mode)
{
	return is_correct_accidental(root, mode, SHARP) ? SHARP : FLAT;
}

Node *
prepend_node(Node *head, int data)
{
	Node *new = xmalloc(sizeof(Node));

	new->prev = NULL;
	new->next = head;
	new->data = data;
	if (head)
		head->prev = new;
	return new;
}

Node *
append_node(Node *tail, int data)
{
	Node *new = xmalloc(sizeof(Node));

	new->prev = tail;
	new->data = data;
	new->next = NULL;
	if (tail)
		tail->next = new;
	return new;
}

Node *
pop_head(Node *head)
{
	Node *new_head = head->next;

	free(head);
	return new_head;
}

void
print_list(const Node *head, int tonal, int root, int mode)
{
	int alter;

	if (tonal)
		alter = get_correct_accidental(root, mode);
	while (head) {
		if (tonal) {
			print_note(alter, head->data);
			putchar(' ');
		} else {
			printf("%s ", NOTES[head->data]);
		}
		head = head->next;
	}
}

void
delete_list(Node *head)
{
	Node *n;

	while (head) {
		n = head->next;
		free(head);
		head = n;
	}
}

void
delete_list_from_tail(Node *tail)
{
	Node *n;

	while (tail) {
		n = tail->prev;
		free(tail);
		tail = n;
	}
}

Node *
copy_list(Node *src_head)
{
	Node *dest_head = NULL, *dest_tail = NULL;

	while (src_head) {
		dest_tail = append_node(dest_tail, src_head->data);
		if (dest_head == NULL)
			dest_head = dest_tail;
		src_head = src_head->next;
	}
	return dest_head;
}

Node *
copy_list_from_tail(Node *src_tail)
{
	Node *dest_head = NULL;

	while (src_tail) {
		dest_head = prepend_node(dest_head, src_tail->data);
		src_tail = src_tail->prev;
	}
	return dest_head;
}

int
apply_steps(int degree, int mode, int note, int steps)
{
	int dir, s, i;

	dir = steps < 0 ? DOWN: UP;
	degree = steps < 0 ? clock_mod(degree - 1, DEGREES) : degree;
	for (s = 0; s != steps; s += dir) {
		i = MAJOR_SCALE[clock_mod(degree + mode, DEGREES)];
		note = clock_mod(note + dir * i, TONES);
		degree = clock_mod(degree + dir, DEGREES);
	}
	return note;
}

int
min_tone_diff(int note_a, int note_b)
{
	int down_steps = clock_mod(note_a - note_b, TONES);
	int up_steps = clock_mod(note_b - note_a, TONES);

	if (down_steps < up_steps)
		return 0-down_steps;
	else
		return up_steps;
}

void
print_note(int sign, int note)
{
	char alter, buf[3];

	/* if flat key */
	if (sign == FLAT)
		alter = '-';
	else /* if sharp key */
		alter = '+';
	if (is_accidental(note)) {
		if (NOTES[note][1] == alter) { /* if sign is correct */
			printf("%s", NOTES[note]);
		} else {
			note += sign * -1;
			buf[0] = NOTES[note][0];
			buf[1] = alter;
			buf[2] = '\0';
			printf("%s", buf);
		}
	} else {
		printf("%s", NOTES[note]);
	}
}

void *
xmalloc(size_t len)
{
	void *p;

	if (!(p = malloc(len)))
		die("malloc: %s\n", strerror(errno));
	return p;
}

void *
xcalloc(size_t len, size_t size)
{
	void *p;

	if (!(p = calloc(len, size)))
		die("calloc: %s\n", strerror(errno));
	return p;
}

void
die(const char *errstr, ...)
{
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(1);
}

