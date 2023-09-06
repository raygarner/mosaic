#include <stdio.h>
#include <ctype.h>

#include "common.h"

#define PARTS 3

enum { BASS_CLEF, TREBLE_CLEF };
enum { BASS, MIDDLE, MELODY };

const char *ID[PARTS] = { "bass", "middle", "melody" };
const char *NAME[PARTS] = { "low", "mid", "high" };
const int OCTAVE[PARTS] = { 3, 5, 6 };

void
write_headers()
{
	printf("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
	printf("<!DOCTYPE score-partwise PUBLIC\n");
	printf("\t\"-//Recordare//DTD MusicXML 4.0 Partwise//EN\"\n");
	printf("\t\"http://www.musicxml.org/dtds/partwise.dtd\">\n");
}

void
write_part_def(const char *id, const char *name, const char *indent)
{
	printf("%s<score-part id=\"%s\">\n", indent, id);
	printf("%s\t<part-name>%s</part-name>\n", indent, name);
	printf("%s</score-part>\n", indent);
}

int
octave_change(int x, int y)
{
	int interval = min_tone_diff(x, y);

	if (interval < 0 && x < y)
		return -1;
	else if (interval > 0 && x > y)
		return 1;
	else
		return 0;
}

void
write_part_line(const char *id, Node *head_note, int octave, int clef, 
                const char *indent)
{
	char note;
	int alter, base_octave = octave;

	printf("%s<part id=\"%s\">\n", indent, id);
	printf("%s\t<measure number=\"1\">\n", indent);
	if (clef == BASS_CLEF) {
		printf("%s\t\t<attributes>\n", indent);
		printf("%s\t\t\t<clef>\n", indent);
		printf("%s\t\t\t\t<sign>F</sign>\n", indent);
		printf("%s\t\t\t\t<line>4</line>\n", indent);
		printf("%s\t\t\t</clef>\n", indent);
		printf("%s\t\t</attributes>\n", indent);
	}
	while (head_note) {
		printf("%s\t\t<note>\n", indent);
		printf("%s\t\t\t<pitch>\n", indent);
		if (is_accidental(head_note->data)) {
			note = NOTES[head_note->data-1][0];
			alter = SHARP;
		} else {
			note = NOTES[head_note->data][0];
			alter = NATURAL;
		}
		printf("%s\t\t\t\t<step>%c</step>\n", indent, note);;
		printf("%s\t\t\t\t<alter>%d</alter>\n", indent, alter);
		/* calc octave */
		if (head_note->prev && head_note->prev->data != X)
			octave += octave_change(head_note->prev->data, 
			                        head_note->data);
		printf("%s\t\t\t\t<octave>%d</octave>\n", indent, octave); 
		printf("%s\t\t\t</pitch>\n", indent);
		printf("%s\t\t\t<duration>1</duration>\n", indent);
		if (head_note->next && head_note->next->data != X)
			printf("%s\t\t\t<type>quarter</type>\n", indent);
		else
			printf("%s\t\t\t<type>whole</type>\n", indent);
		printf("%s\t\t</note>\n", indent);
		head_note = head_note->next;
		if (head_note->data == X) {
			head_note = head_note->next;
			octave = base_octave;
		}
	}
	printf("%s\t</measure>\n", indent);
	printf("%s</part>\n", indent);
}

void
read_line(char c, Node **head, Node **tail)
{
	char buf[BUFLEN];

	do {
		if (isspace(c))
			continue;
		*tail = append_node(*tail, read_tone(c, getchar()));
		if (*head == NULL)
			*head = *tail;
	} while ((c = getchar()) != '-');
	while (isspace(c = getchar())) {}
	*tail = append_node(*tail, X);
	read_tone(c, getchar());
	scanf(STRINP, buf);
	read_mode(buf);
}

int
main()
{
	Node *melody_head = NULL, *melody_tail = NULL, *middle_head = NULL,
	     *middle_tail = NULL, *bass_head = NULL, *bass_tail = NULL;
	int i;
	char c;

	write_headers();
	printf("<score-partwise version=\"4.0\">\n");
	printf("\t<part-list>\n");
	for (i = PARTS-1; i >= 0; i--)
		write_part_def(ID[i], NAME[i], "\t\t");
	printf("\t</part-list>\n");
	c = getchar();
	while (c != EOF) {
		read_line(c, &melody_head, &melody_tail);
		c = getchar();
		read_line(c, &middle_head, &middle_tail);
		c = getchar();
		read_line(c, &bass_head, &bass_tail);
		c = getchar();
		while (isspace(c) && c != EOF)
			c = getchar();
	}
	write_part_line(ID[MELODY], melody_head, OCTAVE[MELODY], TREBLE_CLEF, "\t");
	write_part_line(ID[MIDDLE], middle_head, OCTAVE[MIDDLE], TREBLE_CLEF, "\t");
	write_part_line(ID[BASS], bass_head, OCTAVE[BASS], BASS_CLEF, "\t");
	delete_list(melody_head);
	delete_list(middle_head);
	delete_list(bass_head);
	printf("</score-partwise>\n");
	return 0;
}
