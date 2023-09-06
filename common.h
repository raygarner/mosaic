#define DEGREES 7
#define TONES   12
#define BUFLEN  16
#define ERROR   -1
#define ALL_KEYS '-'
#define WILDCARD_ICON 'x'
#define STRINP "%16s"

enum { NOT_PRESENT, PRESENT };

enum { IONIAN, DORIAN, PHYRIGIAN, LYDIAN, MIXOLYDIAN, AEOLIAN, LOCRIAN };

enum { SEMITONE = 1, TONE = 2 };

enum { FALSE, TRUE };

enum {
	X = -999, /* wildcard */
	C = 0,
	D = 2,
	E = 4,
	F = 5,
	G = 7,
	A = 9,
	B = 11,
	LAST_NOTE = 999
};

enum { DOWN = -1, SAME = 0, UP = 1 };

enum { FLAT = -1, NATURAL = 0, SHARP = 1 };

enum { I, II, III, IV, V, VI, VII };

typedef struct Node Node;
struct Node {
	int data;
	Node *prev;
	Node *next;
};

extern const int MAJOR_SCALE[DEGREES];
extern const int ALTERED_SCALE[DEGREES];
extern const char *MODES[DEGREES];
extern const char *ALT_MODES[DEGREES];
extern const char *NOTES[TONES];

int read_accidental(char);
int read_note(char);
int read_tone(char, char);
int read_mode(const char *);
int clock_mod(int, int);
int custom_step(int, int, int, const int[DEGREES]);
int step(int, int, int);
void init_key_field(int[TONES][DEGREES], int);
void read_key_list(int[TONES][DEGREES], int);
void print_matching_keys(int[TONES][DEGREES], int);
void print_mode(int);
int calc_degree(int note, int root, int mode);
int is_diatonic(int, int, int);
int is_accidental(int);
int is_correct_accidental(int, int, int);
int get_correct_accidental(int, int);
Node *prepend_node(Node *, int);
Node *append_node(Node *, int);
Node *pop_head(Node *);
void print_list(const Node *, int, int, int);
void delete_list(Node *);
void delete_list_from_tail(Node *);
Node *copy_list(Node *);
Node *copy_list_from_tail(Node *);
int apply_steps(int, int, int, int);
int min_tone_diff(int, int);
void print_note(int, int);
void *xmalloc(size_t len);
void *xcalloc(size_t len, size_t size);
void die(const char *errstr, ...);

