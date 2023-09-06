#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "common.h"

enum { FIRST, SECOND, THIRD, FOURTH, FIFTH, SIXTH };
enum { THIRD_TONES = 4, FOURTH_TONES = 5, TRITONE = 6, FIFTH_TONES = 7 };

typedef struct Path Path;
struct Path {
	int faults;
	Node *head;
};

Path improve_bass_line(Node*, Node*, int, int);

int
is_primary_degree(int note, int root, int mode)
{
	switch (calc_degree(note, root, mode)) {
	case I: /* fallthrough */
	case IV: /* fallthrough */
	case V: /* fallthrough */
		return TRUE;
	}
	return FALSE;
}

int
add_middle_note(int bass_note, int mld_note, int root, int mode)
{
	int bass_degree, first_degree, mld_degree, mld_function, first_pitch,
	    third_pitch, fifth_pitch, inverted;

	bass_degree = calc_degree(bass_note, root, mode);
	if (is_primary_degree(bass_note, root, mode)) {
		first_degree = bass_degree;
		inverted = FALSE;
	} else {
		first_degree = clock_mod(bass_degree - THIRD, DEGREES);
		inverted = TRUE;
	}
	mld_degree = calc_degree(mld_note, root, mode);
	mld_function = clock_mod(mld_degree - first_degree, DEGREES);
	first_pitch = apply_steps(FIRST, mode, root, first_degree);
	third_pitch = apply_steps(first_degree, mode, first_pitch, THIRD);
	fifth_pitch = apply_steps(first_degree, mode, first_pitch, FIFTH);
	switch (mld_function) {
	case FIRST:
		return inverted ? fifth_pitch : third_pitch;
	case THIRD:
		return inverted ? first_pitch : fifth_pitch;
	case FIFTH:
		return inverted ? first_pitch : third_pitch;
	}
	return ERROR;
}

int
min(int a, int b)
{
	return a < b ? a : b;
}

int
calc_next_mid_note(int adj_note, int bass_note, int root, int mode)
{
	int chord_degree, first, third, fifth, dfirst, dthird, dfifth;

	chord_degree = calc_degree(bass_note, root, mode);
	/* if root in bass */
	if (is_primary_degree(bass_note, root, mode)) {
		first = bass_note;
		third = apply_steps(chord_degree, mode, first, THIRD);
		fifth = apply_steps(chord_degree, mode, first, FIFTH);
	} else { /* if third in bass */
		first = apply_steps(chord_degree, mode, bass_note, -THIRD);
		third = bass_note;
		fifth = apply_steps(chord_degree-THIRD, mode, first, FIFTH);
	}
	dfirst = abs(min_tone_diff(adj_note, first));
	dthird = abs(min_tone_diff(adj_note, third));
	dfifth = abs(min_tone_diff(adj_note, fifth));
	if (dthird <= dfirst && dthird <= dfifth)
		return third;
	else if (dfifth <= dfirst && dfifth <= dthird)
		return fifth;
	else
		return first;
}

int
decide_mid_note(const Node *mid_tail, int bass_note, int mld_note, int root, 
                int mode)
{
	if (mid_tail)
		return calc_next_mid_note(mid_tail->data, bass_note, root, 
		                          mode);
	else
		return add_middle_note(bass_note, mld_note, root, mode);
}

/* starts at final chord adding most complete note */
/* works back adding nearest chord tone */
Node *
generate_middle_line(Node *bass_tail, Node *mld_tail, int root, int mode)
{
	Node *mid_head = NULL, *mid_tail = NULL;
	int mid_note;

	while (bass_tail && mld_tail) {
		mid_note = decide_mid_note(mid_tail, bass_tail->data, 
		                           mld_tail->data, root, mode);
		mid_head = prepend_node(mid_head, mid_note);
		if (mid_tail == NULL)
			mid_tail = mid_head;
		bass_tail = bass_tail->prev;
		mld_tail = mld_tail->prev;
	}
	return mid_head;
}

int
pick_primary_chord(int mld_degree)
{
	switch (mld_degree) {
	case I: /* I or IV */
		return I;
	case II:
		return V;
	case III:
		return I;
	case IV:
		return IV;
	case V: /* I or V */
		return I;
	case VI:
		return IV;
	case VII:
		return V;
	}
	return -1;
}

int
negative(int x)
{
	return x < 0;
}

int
is_consecutive_pitch(const Node *line)
{
	return line->data == line->prev->data;
}

int
is_parallel_octaves(const Node *part_a, const Node *part_b)
{
	return part_a->data == part_b->data &&
	       part_a->prev->data == part_b->prev->data;
}

int
is_tritone_leap(const Node *line)
{
	return abs(min_tone_diff(line->data, line->prev->data)) == TRITONE;
}

int
is_fifth(int mld_note, int bass_note, int bass_degree, int mode)
{
	return mld_note == apply_steps(bass_degree, mode, bass_note, FIFTH);
}

int
is_parallel_fifth(const Node *bass_note, const Node *mld_note, int root, 
                  int mode)
{
	int bass_degree, prev_bass_degree, prev_mld, prev_bass;

	bass_degree = calc_degree(bass_note->data, root, mode);
	prev_bass_degree = calc_degree(bass_note->prev->data, root, mode);
	prev_mld = mld_note->prev->data;
	prev_bass = bass_note->prev->data;
	return is_fifth(mld_note->data, bass_note->data, bass_degree, mode) &&
	       is_fifth(prev_mld, prev_bass, prev_bass_degree, mode);
}

int
is_same_dir(int a, int b)
{
	return negative(a) == negative(b);
}

int
is_double_leap_same_dir(const Node *bass_note)
{
	int ia, ib;

	if (bass_note == NULL || bass_note->prev == NULL ||
	bass_note->prev->prev == NULL)
		return FALSE;
	ia = min_tone_diff(bass_note->data, bass_note->prev->data);
	ib = min_tone_diff(bass_note->prev->data, bass_note->prev->prev->data);
	return (abs(ia) > TONE && abs(ib) > TONE && is_same_dir(ia, ib));
}

int
is_siren(const Node *bass_note)
{
	if (bass_note == NULL || bass_note->prev == NULL ||
	bass_note->prev->prev == NULL || bass_note->prev->prev->prev == NULL)
		return FALSE;
	return (bass_note->data == bass_note->prev->prev->data &&
	bass_note->prev->data == bass_note->prev->prev->prev->data);
}

int
is_big_leap(const Node *bass_note)
{
	int i;

	if (bass_note->next == NULL)
		return 0;
	i = min_tone_diff(bass_note->data, bass_note->prev->data);
	return i > THIRD_TONES;
}

int
faulty_note(Node *bass_note, Node *mld_note, int root, int mode)
{
	int fault = 0;

	if (!(bass_note->prev && mld_note->prev))
		return 0;
	fault += is_consecutive_pitch(bass_note);
	fault += is_parallel_octaves(bass_note, mld_note);
	fault += is_tritone_leap(bass_note);
	fault += is_parallel_fifth(bass_note, mld_note, root, mode);
	fault += is_double_leap_same_dir(bass_note);
	fault += is_siren(bass_note);
	fault += is_big_leap(bass_note);
	//fault += bass_note->data == mld_note->data;
	//bass_degree = calc_degree(bass_note->data, root, mode);
	/*
	//fault += mld_note->data == \
		apply_steps(bass_degree, mode, bass_note->data, FIFTH);
	*/
	return fault;
}

int
count_faults(Node *bass_note, Node *mld_note, int root, int mode)
{
	int c = 0, range = 0, leaps = 0, steps = 0, interval;

	while (bass_note && mld_note) {
		if (bass_note->prev) {
			interval = min_tone_diff(bass_note->data, 
			                         bass_note->prev->data);
			range += interval;
			if (abs(interval) > TONE) {
				leaps++;
			} else if (interval != 0){
				steps++;
			} 		
		}
		c += faulty_note(bass_note, mld_note, root, mode);
		bass_note = bass_note->prev;
		mld_note = mld_note->prev;
	}
	if (abs(range) > TONES)
		c += abs(range) - TONES;
	c += abs(leaps - steps);
	return c;
}

int
alt_chord_choice(int chord, int mld_degree)
{
	switch (mld_degree) {
	case I:
		return chord == I ? IV : I;
	case V:
		return chord == I ? V : I;
	}
	return chord;
}

Path
get_alt_path(Node *bass_note, Node *mld_note, int root, int mode)
{
	Path ret;
	Node *alt_head;
	int new_degree, old_degree, mld_degree;

	ret.faults = 9999;
	ret.head = NULL;
	mld_degree = calc_degree(mld_note->data, root, mode);
	if ((mld_degree == I || mld_degree == V) == FALSE)
		return ret;
	alt_head = copy_list(bass_note);
	alt_head->prev = bass_note->prev;
	old_degree = calc_degree(bass_note->data, root, mode);
	new_degree = alt_chord_choice(old_degree, mld_degree);
	alt_head->data = apply_steps(I, mode, root, new_degree);
	ret = improve_bass_line(alt_head->next, mld_note->next, root, mode);
	delete_list(alt_head);
	return ret;

}

Path
get_inverted_alt_path(Node *bass_note, Node *mld_note, int root, int mode)
{
	Path ret;
	Node *alt_head;
	int new_degree, old_degree, alt_note, mld_degree;

	ret.faults = 9999;
	ret.head = NULL;
	mld_degree = calc_degree(mld_note->data, root, mode);
	if ((mld_degree == I || mld_degree == V) == FALSE)
		return ret;
	alt_head = copy_list(bass_note);
	alt_head->prev = bass_note->prev;
	old_degree = calc_degree(bass_note->data, root, mode);
	new_degree = alt_chord_choice(old_degree, mld_degree);
	alt_note = apply_steps(I, mode, root, new_degree);
	if (mld_note->data == alt_note) {
		alt_head->data = apply_steps(new_degree, mode, alt_note, THIRD);
		ret = improve_bass_line(alt_head->next, mld_note->next, root, 
		                        mode);
	}
	delete_list(alt_head);
	return ret;
}

Path
get_inverted_path(Node *bass_note, Node *mld_note, int root, int mode)
{
	Path ret;
	Node *alt_head;
	int old_degree;

	ret.faults = 9999;
	ret.head = NULL;
	if (bass_note->data != mld_note->data)
		return ret;
	alt_head = copy_list(bass_note);
	alt_head->prev = bass_note->prev;
	old_degree = calc_degree(bass_note->data, root, mode);
	alt_head->data = apply_steps(old_degree, mode, bass_note->data, THIRD);
	ret = improve_bass_line(alt_head->next, mld_note->next, root, mode);
	delete_list(alt_head);
	return ret;
}

Path
get_default_path(Node *bass_note, Node *mld_note, int root, int mode)
{
	Node *alt_head;
	Path ret;

	alt_head = copy_list(bass_note);
	alt_head->prev = bass_note->prev;
	ret = improve_bass_line(alt_head->next, mld_note->next, root, mode);
	delete_list(alt_head);
	return ret;
}

Path *
handle_path_choice(Path *a, Path *b, Path *c, Path *d)
{
	if (a->faults <= b->faults && 
	a->faults <= c->faults && 
	a->faults <= d->faults) {
		delete_list(b->head);
		delete_list(c->head);
		delete_list(d->head);
		return a;
	} else if (b->faults <= a->faults && 
	b->faults <= c->faults && 
	b->faults <= d->faults) {
		delete_list(a->head);
		delete_list(c->head);
		delete_list(d->head);
		return b;
	} else if (c->faults <= a->faults && 
	c->faults <= b->faults && 
	c->faults <= d->faults) {
		delete_list(a->head);
		delete_list(b->head);
		delete_list(d->head);
		return c;
	} else {
		delete_list(a->head);
		delete_list(b->head);
		delete_list(c->head);
		return d;
	}
}

Path
improve_bass_line(Node *bass_note, Node *mld_note, int root, int mode)
{
	Path a_path, b_path, c_path, def_path, full_path;

	if (bass_note->next == NULL || mld_note->next == NULL) {
		full_path.head = copy_list_from_tail(bass_note);
		full_path.faults = count_faults(bass_note, mld_note, root, 
		                                mode);
		return full_path;
	}
	a_path = get_alt_path(bass_note, mld_note, root, mode);
	b_path = get_inverted_alt_path(bass_note, mld_note, root, mode);
	c_path = get_inverted_path(bass_note, mld_note, root, mode);
	def_path = get_default_path(bass_note, mld_note, root, mode);
	return *handle_path_choice(&a_path, &b_path, &c_path, &def_path);
}

Node *
generate_bass_line(Node *melody_tail, int root, int mode)
{
	Node *bass_head = NULL, *melody_note = melody_tail;
	int bass_note, mld_degree, chord;

	while (melody_note) {
		mld_degree = calc_degree(melody_note->data, root, mode);
		chord = pick_primary_chord(mld_degree);
		bass_note = apply_steps(0, mode, root, chord);
		bass_head = prepend_node(bass_head, bass_note);
		melody_note = melody_note->prev;
	}
	return bass_head;
}

int
main()
{
	char c, buf[BUFLEN];
	int root = -1, mode = -1, alter;
	Node *melody_tail = NULL, *melody_head = NULL, *bass_head = NULL,
	     *mid_head = NULL, *bass_tail = NULL;
	Path improved_bass;

	c = getchar();
	while (c != EOF) {
		do {
			if (isspace(c))
				continue;
			melody_tail = append_node(melody_tail, 
			                          read_tone(c, getchar()));
			if (melody_head == NULL)
				melody_head = melody_tail;
		} while ((c = getchar()) != '-');
		while (isspace(c = getchar())) {}
		root = read_tone(c, getchar());
		scanf(STRINP, buf);
		mode = read_mode(buf);
		bass_head = generate_bass_line(melody_tail, root, mode);
		improved_bass = improve_bass_line(bass_head, melody_head, root, 
		                                  mode);
		delete_list(bass_head);
		bass_head = improved_bass.head;
		bass_tail = bass_head;
		while (bass_tail->next)
			bass_tail = bass_tail->next;
		mid_head = generate_middle_line(bass_tail, melody_tail, root, 
		                                mode);
		alter = get_correct_accidental(root, mode);
		print_list(melody_head, TRUE, root, mode);
		printf("- ");
		print_note(alter, root);
		putchar(' ');
		printf("%s\n", MODES[mode]);
		print_list(mid_head, TRUE, root, mode);
		printf("- ");
		print_note(alter, root);
		putchar(' ');
		printf("%s\n", MODES[mode]);
		print_list(bass_head, TRUE, root, mode);
		printf("- ");
		print_note(alter, root);
		putchar(' ');
		printf("%s\n", MODES[mode]);
		putchar('\n');
		getchar();
		c = getchar();
		delete_list(bass_head);
		delete_list(melody_head);
		delete_list(mid_head);
		melody_head = NULL;
		melody_tail = NULL;
		bass_head = NULL;
		bass_tail = NULL;
	}
	return 0;
}
