#include <tiams.h>
#include "blocks.h"
#include "resources.h"
#include "tr.h"
#include "../translations/lang_en.h"
#include "string_frame_builder.h"

/*
Defines
*/

#define SIZE           4

#define VERSION        19                        // Version 1.9
#define DEVELOPER      1                         // 0: release version, 1: developer version

#define UP             1
#define DOWN           2
#define LEFT           3
#define RIGHT          4

#define VALID          1
#define INVALID        0

#define MAX_FRAMES     5
#define MAX_UNDOS      20

#define NUM_CONFIG_IDS 12                        // Number of possible configuration combinations

#define NORMAL_MOVE    0
#define UNDO_MOVE      1


/*
Typedefs
*/

typedef struct {
	int x;
	int y;
} block_pos;

typedef struct {                                 // struct for one highscore entry
	char name[12];
	long int score;
	int random;
	USHORT year;
	USHORT month;
	USHORT day;
	USHORT hour;
	USHORT minute;
	USHORT second;
} highscore;

typedef struct {                                 // This is the old struct for one highscore entry. It was used in version 1.8 and lower and it is now used to import old highscore files.
	char name[12];
	long int score;
} highscore_old;

typedef struct {
	int mode;                                    // can be MODE_*, default is MODE_CLASSIC
	int size;                                    // grid size (from 2 to 4), default is 4
	int animations;                              // 0: animations off, 1: animations on
	int undo;                                    // 0: undo is off, 1: undo is on
	int config_id;                               // Each combination of config options has its number which will be used when accessing the highscore
} options;

typedef struct {
	int frame;                                   // current animation frame number. -1 means that no animation is running
	int max_frames;                              // maximal frame number
} animation_state;

typedef struct {                                 // Struct for a specific game state which includes the grid and some other information, but not the highscore or the game configuration
	int moves;                                   // number of moves in the current game
	int won;                                     // 0: game is not won, 1: game is won
	int game_over;
	int keep_playing;
	int empty_cells;                             // Number of empty cells in the grid
	long int score;                              // Score in the current game
	long int grid[4][4];                         // grid[x][y];
} play_state;

typedef struct {                                 // struct for the complete program state
	int ready_for_action;                        // 3A5T3R 3GG
	char key[12];                                // 3A5T3R 3GG
	highscore highscore_list[NUM_CONFIG_IDS][5]; // five highscore entries in each highscore list
	long int best_scores[NUM_CONFIG_IDS];        // best score (unrelated to the highscore list)
	options new_options;                         // new settings of the game. they will be applied when a new game is started
	options current_options;                     // current settings of the game
	long int random_state;                       // State of the random number generator
	animation_state animation;                   // State of the current animation
	int undo_array_index;                        // Index of the current top element in the undo array. If the undo array is full the programm will start overwrite the undo array again, starting from the first element
	int undos_left;                              // Number of undos left
	int undos_left_old;                          // Old number of remaning undos, used for the update function
	int undo_game_over;                          // Disable undo if the game is over
	int grid_merged[4][4];                       // Store if a tile was already merged to prevent merging a tile twice in a single turn
	play_state play_states[MAX_UNDOS + 2];       // Array of play_states, used for undo. Allocate one additional element, that can be trashed if nothing has changed in a move.
} game_state;

typedef struct {                                 // struct for a bitmap with a predefined size
	WORD NumRows;
	WORD NumCols;
	BYTE Data[54];
} MYBITMAP;

typedef struct {                                 // struct for an icon with a predefined size
	WORD NumRows;
	WORD NumCols;
	BYTE Data[66];
} MYICON;


/*
Functionprototypes
*/

static void AP_app (pFrame self, PEvent e);

int game_won (game_state *game_ptr);
int game_over (game_state *game_ptr);

int move (game_state *game_ptr, int direction);
void rotate (game_state *game_ptr, int rotation_number);
int move_up (game_state *game_ptr);

int find_pairs_vertically (game_state *game_ptr);

void check_and_add_to_highsore_list (game_state *game_ptr);
int can_be_new_highscore_entry (game_state *game_ptr, long int score, int config_id);
int check_for_duplicate (game_state *game_ptr, highscore *highscore_entry, int config_id);
void add_to_highscore_list (game_state *game_ptr, highscore *new_highscore_entry, int config_id);
void show_highscore_dialog (game_state *game_ptr);
char * translate_no_name (char *name);

void draw_game (game_state *game_ptr);
void draw_update_game (game_state *game_ptr, int undo);
void draw_best_score (game_state *game_ptr);
void draw_block (int i, int j, long int value, int grid_offset);
void draw_game_over (game_state *game_ptr, int grid_offset);

int increment_undo_index (int index);
int decrement_undo_index (int index);

void new_game (game_state *game_ptr);
void create_random_tile (game_state *game_ptr);
void create_new_tile (game_state *game_ptr, long int value);
block_pos find_random_empty_cell (game_state *game_ptr);

void set_defaults (game_state *game_ptr);
void set_merge_info_null (game_state *game_ptr);

void show_options_dialog (game_state *game_ptr);
void save_highscore (game_state *game_ptr);
void load_highscore (game_state *game_ptr);
int check_old_highscore_entries (highscore_old highscore_list[][5], int highscore_list_size);
int check_highscore_entries (highscore highscore_list[12][5]);
void merge_old_highscore_entries (game_state *game_ptr, highscore_old highscore_list[][5], int highscore_list_size);
void merge_highscore_entries (game_state *game_ptr, highscore highscore_list[NUM_CONFIG_IDS][5]);
void merge_best_scores (game_state *game_ptr, long int best_score_list[], int best_score_list_size);
void show_help_dialog (void);
void show_about_dialog (void);

void easter_egg (game_state *game_ptr);
void easter_egg_show_text (game_state *game_ptr, int category);

int random_range (game_state *game_ptr, int low, int high, long int *random_state_ptr);
long int random_raw (game_state *game_ptr, long int *random_state_ptr);

DWORD no_call_back (WORD DlgId, DWORD Value);


/*
Icon of the app
*/

static const MYICON app_icon = {22, 22, {
0x00,0x00,0x00,0xE8,0xED,0x5C,0xA8,0xA9,0x50,0xC8,0xA9,0x9C,0xA8,0xA9,0x84,0xA8,
0xA9,0x44,0xEE,0xED,0x5C,0x00,0x00,0x00,0x00,0xFC,0x00,0x03,0x87,0x00,0x0E,0x01,
0xC0,0x18,0x00,0x60,0x30,0x00,0x30,0x60,0x00,0x18,0xE0,0x00,0x1C,0x80,0x00,0x04,
0xBD,0xDA,0xB4,0xA5,0x52,0xA4,0x39,0x53,0x30,0x25,0x53,0x10,0x25,0x52,0x90,0x25,
0xDA,0xB0
}};


/*
App object frame
*/

static FRAME(botr_info_frame, OO_SYSTEM_FRAME, 0, OO_APP_FLAGS, 5)
	ATTR(OO_APP_FLAGS, APP_INTERACTIVE)
	ATTR(OO_APP_NAME, BOTR_APP_NAME)
	ATTR(OO_APP_PROCESS_EVENT, &AP_app)
	ATTR(OO_APP_DEFAULT_MENU, &app_menu)
	ATTR(OO_APP_ICON, &app_icon)
ENDFRAME

static STRING_FRAME(botr_string_frame, OO_SYSTEM_FRAME, &botr_info_frame, OO_APPSTRING, NUM_STRINGS)
	APP_STRING_LIST
ENDFRAME


/*
Global variables
*/

static pFrame botr_frame = (pFrame)&botr_string_frame; // Must be 1st!
static WINDOW appW;
static game_state current_game;
static const int grid_offset_table[] = {29, 19, 10};
static const int game_version = VERSION;
static const int leet_delta[10] = {-13, -1, 12, -7, 3, 0, 2, -9, 10, -111}; // 3A5T3R 3GG
static int leet_i;
static char leet_char;


/*
Images of the blocks
*/

static const MYBITMAP block_0 = {18, 18, {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00
}};

static const MYBITMAP block_2 = {18, 18, {
0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0x00,0x3F,0xFF,0x00,0x00,0x03,0x00,0x00,
0x03,0x00,0x00,0x03,0x00,0x00,0x03,0x00,0x3F,0xFF,0x00,0x3F,0xFF,0x00,0x30,0x00,
0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x3F,0xFF,0x00,0x3F,0xFF,0x00,
0x00,0x00,0x00,0x00,0x00,0x00
}};

static const MYBITMAP block_4 = {18, 18, {
0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x30,
0x00,0x00,0x30,0xC0,0x00,0x30,0xC0,0x00,0x3F,0xFE,0x00,0x3F,0xFE,0x00,0x00,0xC0,
0x00,0x00,0xC0,0x00,0x00,0xC0,0x00,0x00,0xC0,0x00,0x00,0xC0,0x00,0x00,0xC0,0x00,
0x00,0x00,0x00,0x00,0x00,0x00
}};

static const MYBITMAP block_8 = {18, 18, {
0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0x00,0x3F,0xFF,0x00,0x30,0x03,0x00,0x30,
0x03,0x00,0x30,0x03,0x00,0x30,0x03,0x00,0x3F,0xFF,0x00,0x3F,0xFF,0x00,0x30,0x03,
0x00,0x30,0x03,0x00,0x30,0x03,0x00,0x30,0x03,0x00,0x3F,0xFF,0x00,0x3F,0xFF,0x00,
0x00,0x00,0x00,0x00,0x00,0x00
}};

static const MYBITMAP block_16 = {18, 18, {
0x00,0x00,0x00,0x00,0x00,0x00,0x3E,0x7F,0x00,0x3E,0x7F,0x00,0x06,0x60,0x00,0x06,
0x60,0x00,0x06,0x60,0x00,0x06,0x60,0x00,0x06,0x7F,0x00,0x06,0x7F,0x00,0x06,0x63,
0x00,0x06,0x63,0x00,0x06,0x63,0x00,0x06,0x63,0x00,0x06,0x7F,0x00,0x06,0x7F,0x00,
0x00,0x00,0x00,0x00,0x00,0x00
}};

static const MYBITMAP block_32 = {18, 18, {
0xAA,0xAA,0x80,0x00,0x00,0x40,0xBF,0x3F,0x00,0x3F,0x3F,0x40,0x83,0x03,0x00,0x53,
0x53,0x40,0xAB,0x2B,0x00,0x43,0x03,0x40,0x9F,0x3F,0x00,0x5F,0x3F,0x40,0x83,0x30,
0x00,0x53,0x35,0x40,0xAB,0x32,0x80,0x03,0x30,0x40,0xBF,0x3F,0x00,0x3F,0x3F,0x40,
0x80,0x00,0x00,0x55,0x55,0x40
}};

static const MYBITMAP block_64 = {18, 18, {
0xAA,0xAA,0x80,0x00,0x05,0x40,0xBF,0x32,0x80,0x3F,0x35,0x40,0xB0,0x32,0x80,0x35,
0x35,0x40,0xB2,0xB2,0x80,0x30,0x30,0x40,0xBF,0x36,0x00,0x3F,0x3F,0x40,0xB3,0x3F,
0x00,0x33,0x06,0x40,0xB3,0x26,0x80,0x33,0x56,0x40,0xBF,0x26,0x80,0x3F,0x56,0x40,
0x80,0x20,0x80,0x55,0x55,0x40
}};

static const MYBITMAP block_128 = {18, 18, {
0xAA,0xAA,0x80,0x00,0x00,0x40,0xB7,0xDF,0x00,0x37,0xDF,0x40,0xB0,0xDB,0x00,0x34,
0xDB,0x40,0xB2,0xDB,0x00,0x30,0xDB,0x40,0xB7,0xDF,0x00,0x37,0xDF,0x40,0xB6,0x1B,
0x00,0x36,0x5B,0x40,0xB6,0x9B,0x00,0x36,0x1B,0x40,0xB7,0xDF,0x00,0x37,0xDF,0x40,
0x80,0x00,0x00,0x55,0x55,0x40
}};

static const MYBITMAP block_256 = {18, 18, {
0xAA,0xAA,0x80,0x00,0x00,0x40,0xBD,0xEF,0x00,0x05,0x08,0x40,0xA5,0x2A,0x80,0x55,
0x49,0x40,0xA5,0x2A,0x80,0x05,0x08,0x40,0xBD,0xEF,0x00,0x20,0x29,0x40,0xAA,0xA9,
0x00,0x25,0x29,0x40,0xAA,0xA9,0x00,0x25,0x29,0x40,0xA0,0x29,0x00,0x3D,0xEF,0x40,
0x80,0x00,0x00,0x55,0x55,0x40
}};

static const MYBITMAP block_512 = {18, 18, {
0xFF,0xFF,0xC0,0xFF,0xFF,0xC0,0xC3,0x10,0xC0,0xDF,0xDE,0xC0,0xDF,0xDE,0xC0,0xDF,
0xDE,0xC0,0xDF,0xDE,0xC0,0xDF,0xDE,0xC0,0xC3,0xD0,0xC0,0xFB,0xD7,0xC0,0xFB,0xD7,
0xC0,0xFB,0xD7,0xC0,0xFB,0xD7,0xC0,0xFB,0xD7,0xC0,0xFB,0xD7,0xC0,0xC3,0xD0,0xC0,
0xFF,0xFF,0xC0,0xFF,0xFF,0xC0
}};

static const MYBITMAP block_1024 = {18, 18, {
0xFF,0xFF,0xC0,0x88,0x8B,0xC0,0xEA,0xEB,0xC0,0xEA,0xEB,0xC0,0xEA,0xEB,0xC0,0xEA,
0xEB,0xC0,0xEA,0xEB,0xC0,0xEA,0xEA,0xC0,0xEA,0x88,0x40,0xEA,0xBE,0xC0,0xEA,0xBE,
0xC0,0xEA,0xBE,0xC0,0xEA,0xBE,0xC0,0xEA,0xBE,0xC0,0xEA,0xBE,0xC0,0xEA,0xBE,0xC0,
0xE8,0x8E,0xC0,0xFF,0xFF,0xC0
}};

static const MYBITMAP block_2048 = {18, 18, {
0xFF,0xFF,0xC0,0x80,0xC0,0x40,0xFE,0xDF,0x40,0xFE,0xDF,0x40,0x80,0xDF,0x40,0xBF,
0xDF,0x40,0xBF,0xDF,0x40,0x80,0xC0,0x40,0xFF,0xFF,0xC0,0xFF,0xFF,0xC0,0xBF,0xC0,
0x40,0xBF,0xDF,0x40,0xB7,0xDF,0x40,0x80,0xC0,0x40,0xF7,0xDF,0x40,0xF7,0xDF,0x40,
0xF7,0xC0,0x40,0xFF,0xFF,0xC0
}};

static const MYBITMAP block_4096 = {18, 18, {
0xFF,0xFF,0xC0,0xBC,0x44,0x40,0xBD,0x55,0xC0,0xBD,0x55,0xC0,0xBD,0x55,0xC0,0xBD,
0x55,0xC0,0xBD,0x55,0xC0,0xAD,0x55,0xC0,0x85,0x45,0xC0,0xED,0x74,0x40,0xED,0x75,
0x40,0xED,0x75,0x40,0xED,0x75,0x40,0xED,0x75,0x40,0xED,0x75,0x40,0xED,0x75,0x40,
0xEC,0x44,0x40,0xFF,0xFF,0xC0
}};

static const MYBITMAP block_8192 = {18, 18, {
0xFF,0xFF,0xC0,0x84,0x44,0x40,0xB7,0x57,0x40,0xB7,0x57,0x40,0xB7,0x57,0x40,0xB7,
0x57,0x40,0xB7,0x57,0x40,0xB7,0x57,0x40,0x87,0x44,0x40,0xB7,0x75,0xC0,0xB7,0x75,
0xC0,0xB7,0x75,0xC0,0xB7,0x75,0xC0,0xB7,0x75,0xC0,0xB7,0x75,0xC0,0xB7,0x75,0xC0,
0x87,0x44,0x40,0xFF,0xFF,0xC0
}};

static const MYBITMAP block_16384 = {18, 18, {
0xFF,0xFF,0xC0,0x84,0x1F,0xC0,0xF5,0xFF,0xC0,0xF5,0xFF,0xC0,0xF4,0x1F,0xC0,0xF5,
0xDF,0xC0,0xF5,0xDF,0xC0,0xF4,0x17,0xC0,0xFF,0xFF,0xC0,0xFF,0xFF,0xC0,0x84,0x17,
0xC0,0xF5,0xD7,0xC0,0xF5,0xD5,0xC0,0xC4,0x10,0x40,0xF5,0xDD,0xC0,0xF5,0xDD,0xC0,
0x84,0x1D,0xC0,0xFF,0xFF,0xC0
}};

static const MYBITMAP block_32768 = {18, 18, {
0xFF,0xFF,0xC0,0x84,0x1F,0xC0,0xF7,0xDF,0xC0,0xF7,0xDF,0xC0,0xC4,0x1F,0xC0,0xF5,
0xFF,0xC0,0xF5,0xFF,0xC0,0x84,0x17,0xC0,0xFF,0xFF,0xC0,0xFF,0xFF,0xC0,0x84,0x10,
0x40,0xF5,0xF7,0x40,0xF5,0xF7,0x40,0xF4,0x10,0x40,0xF5,0xD7,0x40,0xF5,0xD7,0x40,
0xF4,0x10,0x40,0xFF,0xFF,0xC0
}};

static const MYBITMAP block_65536 = {18, 18, {
0xFF,0xFF,0xC0,0x82,0x1F,0xC0,0xBE,0xFF,0xC0,0xBE,0xFF,0xC0,0x82,0x1F,0xC0,0xBB,
0xDF,0xC0,0xBB,0xDF,0xC0,0x82,0x17,0xC0,0xFF,0xFF,0xC0,0xFF,0xFF,0xC0,0x82,0x10,
0x40,0xBF,0xD7,0xC0,0xBF,0xD7,0xC0,0x83,0x10,0x40,0xFB,0xD7,0x40,0xFB,0xD7,0x40,
0x82,0x10,0x40,0xFF,0xFF,0xC0
}};

static const MYBITMAP block_131072 = {18, 18, {
0xFF,0xFF,0xC0,0x86,0x10,0xC0,0xF7,0xDE,0xC0,0xF7,0xDE,0xC0,0xF7,0x1E,0xC0,0xF7,
0xDE,0xC0,0xF7,0xDE,0xC0,0xF6,0x1E,0xC0,0xFF,0xFF,0xC0,0xFF,0xFF,0xC0,0x82,0x10,
0x40,0xBA,0xDF,0x40,0xBB,0xDF,0x40,0xBB,0xD0,0x40,0xBB,0xD7,0xC0,0xBB,0xD7,0xC0,
0x83,0xD0,0x40,0xFF,0xFF,0xC0
}};

static const MYBITMAP block_unicorn = {18, 18, {
0x00,0x03,0x00,0x00,0x07,0x00,0x00,0xCE,0x00,0x03,0xFC,0x00,0x0E,0x38,0x00,0x18,
0x18,0x00,0x20,0x6C,0x00,0x60,0x66,0x00,0xC0,0x02,0x00,0x80,0x03,0x80,0x01,0x00,
0x80,0x01,0x80,0x80,0x01,0xC2,0xC0,0x01,0x7B,0x40,0x03,0x1C,0xC0,0x06,0x07,0x80,
0x06,0x00,0x00,0x04,0x00,0x00
}};


/*
Main function, called by the OS with the event as a parameter
*/

static void AP_app (pFrame self, PEvent e) {
	Access_AMS_Global_Variables;
	WIN_RECT appWR;
	game_state *current_game_ptr;
	short key;
	WORD opts;
	WORD fullscreen_flag;
	HANDLE pack_storage; // Handle to save and restore the current game state when the FlashApp is moved

	current_game_ptr = &current_game;

	switch (e->command) {
		case CM_INSTALL:
			set_defaults (current_game_ptr);
			new_game (current_game_ptr);
			break;
		case CM_DEFAULTS:
			set_defaults (current_game_ptr);
			new_game (current_game_ptr);
			break;
		case CM_PACK:
			pack_storage = HeapAlloc (sizeof (game_state)); // Allocate memory for current_game

			if (pack_storage != H_NULL) {
				game_state *pack_game;
				pack_game = HeapDeref (pack_storage);
				*pack_game = *current_game_ptr;
			}

			OO_appSetPublicStorage (pack_storage);
			break;
		case CM_UNPACK:
			pack_storage = OO_appGetPublicStorage ();

			if (pack_storage != H_NULL) {
				game_state *pack_game;
				pack_game = HeapDeref (pack_storage);
				*current_game_ptr = *pack_game;;
				HeapFree (pack_storage);
			} else {
				set_defaults (current_game_ptr);
				new_game (current_game_ptr);
			}
			break;
		case CM_START:
			appWR = *(e->info.startInfo.startRect);

			MO_currentOptions ();
			if (MO_option[MO_OPT_SPLIT_SCREEN] == D_MODE_SPLIT_FULL) {
				fullscreen_flag = WF_NOBORDER;
			} else {
				fullscreen_flag = 0;
			}

			if (WinOpen (&appW, &appWR, fullscreen_flag)) {
				WinAttr (&appW, A_REPLACE);
				WinClr (&appW);
			} else {
				EV_quit ();
			}
			break;
		case CM_ACTIVATE:
			EV_defaultHandler (e);
			WinBeginPaint (&appW);
			WinActivate (&appW);
			draw_game (current_game_ptr);

			// 3A5T3R 3GG
			leet_i = 0;
			leet_char = 114;
			break;
		case CM_DEACTIVATE:
			WinEndPaint (&appW);
			EV_defaultHandler (e);
			break;
		case CM_QUIT:
			if (appW.Next) {
				WinClose (&appW);
				appW.Next = NULL;
			}
			break;
		case CM_KEY_PRESS:
			key = e->info.keyInfo.keyCode;

			if ((current_game_ptr->play_states[current_game_ptr->undo_array_index].game_over == 0) && (key == KB_UP || key == KB_DOWN || key == KB_LEFT || key == KB_RIGHT)) {
				current_game_ptr->play_states[increment_undo_index (current_game_ptr->undo_array_index)] = current_game_ptr->play_states[current_game_ptr->undo_array_index]; // Copy the current play state to the next element
				current_game_ptr->undo_array_index = increment_undo_index (current_game_ptr->undo_array_index);	// Set the undo array index to the new element

				if (move (&current_game, key)) { // Perform the move
					if (current_game_ptr->undos_left < MAX_UNDOS) {
						current_game_ptr->undos_left++;
					}
					current_game_ptr->play_states[current_game_ptr->undo_array_index].game_over = game_over (current_game_ptr);
					draw_update_game (current_game_ptr, NORMAL_MOVE); // Update the grid on the screen
					current_game_ptr->undos_left_old = current_game_ptr->undos_left;
				} else {
					current_game_ptr->undo_array_index = decrement_undo_index (current_game_ptr->undo_array_index); // Decrement the undo index again if nothing has changed to revert the game to the old (identical) state. TODO: restliche Vergleiche wirklich noetig?
				}

				set_merge_info_null (current_game_ptr); // Reset the merge info

				if (current_game_ptr->play_states[current_game_ptr->undo_array_index].won == 0) {
					current_game_ptr->play_states[current_game_ptr->undo_array_index].won = game_won (current_game_ptr);
				}

				current_game_ptr->play_states[current_game_ptr->undo_array_index].game_over = game_over (current_game_ptr);

				// User wins the game
				if (current_game_ptr->play_states[current_game_ptr->undo_array_index].won == 1 && current_game_ptr->play_states[current_game_ptr->undo_array_index].keep_playing == 0
				    && current_game_ptr->play_states[current_game_ptr->undo_array_index].game_over == 0) {
				    easter_egg_show_text (current_game_ptr, 3);
					opts = 1;
					if (KB_ENTER == Dialog (&dialog_win_notification, -1, -1, NULL, &opts) && opts == 1) {
						current_game_ptr->play_states[current_game_ptr->undo_array_index].keep_playing = 1;
					} else {
						current_game_ptr->play_states[current_game_ptr->undo_array_index].game_over = 1;
						current_game_ptr->undo_game_over = 1;
						check_and_add_to_highsore_list (current_game_ptr);
					}

				// User got a 2048 tile, but no further moves are possible
				} else if (current_game_ptr->play_states[current_game_ptr->undo_array_index].won == 1 && current_game_ptr->play_states[current_game_ptr->undo_array_index].keep_playing == 0
				           && current_game_ptr->play_states[current_game_ptr->undo_array_index].game_over == 1 && current_game_ptr->current_options.undo == 0) {
					easter_egg_show_text (current_game_ptr, 4);
					Dialog (&dialog_win_and_game_over, -1, -1, NULL, NULL);
					check_and_add_to_highsore_list (current_game_ptr);

				// User got a 2048 tile in the undo mode, but no further moves are possible
				} else if (current_game_ptr->play_states[current_game_ptr->undo_array_index].won == 1 && current_game_ptr->play_states[current_game_ptr->undo_array_index].keep_playing == 0
				           && current_game_ptr->play_states[current_game_ptr->undo_array_index].game_over == 1 && current_game_ptr->current_options.undo == 1) {
					opts = 1;
					if (KB_ENTER == Dialog (&dialog_undo_win_and_game_over, -1, -1, NULL, &opts) && opts == 1 && current_game_ptr->undos_left > 0) {
						current_game_ptr->undo_array_index = decrement_undo_index (current_game_ptr->undo_array_index);
						current_game_ptr->undos_left--;
						draw_update_game (current_game_ptr, UNDO_MOVE);
						current_game_ptr->undos_left_old = current_game_ptr->undos_left;
					} else {
						draw_game_over (current_game_ptr, grid_offset_table[current_game_ptr->current_options.size - 2]);
						current_game_ptr->undo_game_over = 1;
						check_and_add_to_highsore_list (current_game_ptr);
					}

				// Game is lost
				} else if ( ((current_game_ptr->play_states[current_game_ptr->undo_array_index].won == 1 && current_game_ptr->play_states[current_game_ptr->undo_array_index].keep_playing == 1)
				            || (current_game_ptr->play_states[current_game_ptr->undo_array_index].won == 0 && current_game_ptr->play_states[current_game_ptr->undo_array_index].keep_playing == 0))
				            && current_game_ptr->play_states[current_game_ptr->undo_array_index].game_over == 1 && current_game_ptr->current_options.undo == 0) {
					easter_egg_show_text (current_game_ptr, 2);
					Dialog (&dialog_game_over, -1, -1, NULL, NULL);
					check_and_add_to_highsore_list (current_game_ptr);

				// Game is lost in the undo mode
				} else if ( ((current_game_ptr->play_states[current_game_ptr->undo_array_index].won == 1 && current_game_ptr->play_states[current_game_ptr->undo_array_index].keep_playing == 1)
				            || (current_game_ptr->play_states[current_game_ptr->undo_array_index].won == 0 && current_game_ptr->play_states[current_game_ptr->undo_array_index].keep_playing == 0))
				            && current_game_ptr->play_states[current_game_ptr->undo_array_index].game_over == 1 && current_game_ptr->current_options.undo == 1) {
					opts = 1;
					if (KB_ENTER == Dialog (&dialog_undo_game_over, -1, -1, NULL, &opts) && opts == 1 && current_game_ptr->undos_left > 0 ) {
						current_game_ptr->undo_array_index = decrement_undo_index (current_game_ptr->undo_array_index);
						current_game_ptr->undos_left--;
						draw_update_game (current_game_ptr, UNDO_MOVE);
						current_game_ptr->undos_left_old = current_game_ptr->undos_left;
					} else {
						draw_game_over (current_game_ptr, grid_offset_table[current_game_ptr->current_options.size - 2]);
						current_game_ptr->undo_game_over = 1;
						check_and_add_to_highsore_list (current_game_ptr);
					}
				}

			} else if ((key == KB_DELETE) && (current_game_ptr->current_options.undo == 1) && current_game_ptr->undo_game_over == 0 && current_game_ptr->undos_left > 0) {
				current_game_ptr->undo_array_index = decrement_undo_index (current_game_ptr->undo_array_index);
				current_game_ptr->undos_left--;
				draw_update_game (current_game_ptr, UNDO_MOVE);
				current_game_ptr->undos_left_old = current_game_ptr->undos_left;
			} else {
				// 3A5T3R 3GG
				if (key == leet_char) {
					leet_char += leet_delta[leet_i];
					leet_i++;
					if (!leet_char) {
						leet_char = 114;
						leet_i = 0;
						easter_egg (current_game_ptr);
					}
				} else if (key == 114) {
					leet_i = 1;
					leet_char = 114 + leet_delta[0];
				} else {
					leet_char = 114;
					leet_i = 0;
					EV_defaultHandler (e);
				}
			}
			break;
		case CM_WPAINT:
			DrawWinBorder (&appW, &appW.Window);
			WinClr (&appW);
			draw_game (current_game_ptr);
			break;
		case ACM_NEW:
			new_game (current_game_ptr);
			WinClr (&appW);
			draw_game (current_game_ptr);
			easter_egg_show_text (current_game_ptr, 1);
			break;
		case ACM_HIGHSCORE:
			show_highscore_dialog (current_game_ptr);
			break;
		case ACM_OPTIONS:
			show_options_dialog (current_game_ptr);
			break;
		case ACM_SAVE:
			save_highscore (current_game_ptr);
			break;
		case ACM_LOAD:
			load_highscore (current_game_ptr);
			break;
		case ACM_HELP:
			show_help_dialog ();
			break;
		case ACM_ABOUT:
			show_about_dialog ();
			break;
		default:
			EV_defaultHandler (e);
			break;
	}
}


/*
Check if game is won (check for the magical 2048 tile)
*/

int game_won (game_state *game_ptr) {
	int won;
	int i;
	int j;

	won = 0;
	i = 0;
	while (i < game_ptr->current_options.size) {
		j = 0;
		while (j < game_ptr->current_options.size) {
			if (game_ptr->play_states[game_ptr->undo_array_index].grid[i][j] == 2048) {
				won = 1;
			}
			j++;
		}
		i++;
	}

	return won;
}


/*
Check if game is over/lost.
Returns 1 if game is lost and 0 if game is not lost.
*/

int game_over (game_state *game_ptr) {
	int result;

	result = 1;

	if (game_ptr->play_states[game_ptr->undo_array_index].empty_cells > 0 || find_pairs_vertically (game_ptr) == 1) {
		return 0;
	}

	rotate (game_ptr, 1);
	if (find_pairs_vertically (game_ptr) == 1) {
		result = 0;
	}
	rotate (game_ptr, 3);

	return result;
}


/*
Move tiles in the specified direction and join tiles if possible
*/

int move (game_state *game_ptr, int direction) {
	int changed;

	switch (direction) {
		case KB_UP:
			changed = move_up (game_ptr);
			break;
		case KB_DOWN:
			rotate (game_ptr, 2);
			changed = move_up (game_ptr);
			rotate (game_ptr, 2);
			break;
		case KB_LEFT:
			rotate (game_ptr, 1);
			changed = move_up (game_ptr);
			rotate (game_ptr, 3);
			break;
		case KB_RIGHT:
			rotate (game_ptr, 3);
			changed = move_up (game_ptr);
			rotate (game_ptr, 1);
			break;
		default:
			changed = 0;
			break;
	}

	return changed;
}


/*
Rotate the grid clockwise, rotation_number specifies how many times the grid is rotated
*/

void rotate (game_state *game_ptr, int rotation_number) {
	int current_index;
	int i;
	int j;
	long int tmp;

	current_index = game_ptr->undo_array_index;

	if (rotation_number < 0) {
		rotation_number = 4 - ((-rotation_number) % 4);
	} else {
		rotation_number = rotation_number % 4;
	}

	if (rotation_number != 0) {
		while (rotation_number-- != 0) {
			i = 0;
			while (i < game_ptr->current_options.size / 2) {
				j = i;
				while (j < game_ptr->current_options.size - i - 1) {
					tmp = game_ptr->play_states[current_index].grid[i][j];
					game_ptr->play_states[current_index].grid[i][j] = game_ptr->play_states[current_index].grid[j][game_ptr->current_options.size - i - 1];
					game_ptr->play_states[current_index].grid[j][game_ptr->current_options.size - i - 1] = game_ptr->play_states[current_index].grid[game_ptr->current_options.size - i - 1][game_ptr->current_options.size - j - 1];
					game_ptr->play_states[current_index].grid[game_ptr->current_options.size - i - 1][game_ptr->current_options.size - j - 1] = game_ptr->play_states[current_index].grid[game_ptr->current_options.size - j - 1][i];
					game_ptr->play_states[current_index].grid[game_ptr->current_options.size - j - 1][i] = tmp;
					j++;
				}
				i++;
			}
		}
	}
}


/*
Move tiles up and join tiles if possible
*/

int move_up (game_state *game_ptr) {
	int changed;
	int current_index;
	int i;
	int j;
	int k;

	changed = 0;
	current_index = game_ptr->undo_array_index;

	j = 1;
	while (j < game_ptr->current_options.size) {
		i = 0;
		while (i < game_ptr->current_options.size) {
			if (game_ptr->play_states[current_index].grid[i][j] != 0) {
				k = j - 1;
				while (1) {
					if (k < 0) {
						k++;
						game_ptr->play_states[current_index].grid[i][k] = game_ptr->play_states[current_index].grid[i][j];
						game_ptr->play_states[current_index].grid[i][j] = 0;
						changed = 1;
						break;
					}
					if (game_ptr->play_states[current_index].grid[i][k] != 0) {
						if (game_ptr->play_states[current_index].grid[i][k] == game_ptr->play_states[current_index].grid[i][j] && game_ptr->grid_merged[i][k] == 0) {
							game_ptr->play_states[current_index].grid[i][k] *= 2;
							game_ptr->play_states[current_index].grid[i][j] = 0;
							game_ptr->play_states[current_index].score += game_ptr->play_states[current_index].grid[i][k];
							game_ptr->grid_merged[i][k] = 1;
							game_ptr->play_states[current_index].empty_cells++;
							changed = 1;
						} else if (j - k != 1) {
							k++;
							game_ptr->play_states[current_index].grid[i][k] = game_ptr->play_states[current_index].grid[i][j];
							game_ptr->play_states[current_index].grid[i][j] = 0;
							changed = 1;
						}
						break;
					}
					k--;
				}
			}
			i++;
		}
		j++;
	}

	if (game_ptr->best_scores[game_ptr->current_options.config_id] < game_ptr->play_states[current_index].score) {
		game_ptr->best_scores[game_ptr->current_options.config_id] = game_ptr->play_states[current_index].score;
	}

	if (changed == 1) {
		create_random_tile (game_ptr);
		game_ptr->play_states[current_index].moves++;
	}

	return changed;
}


/*
Check if there are vertical pairs in the grid.
Returns 1 if a pair was found and 0 if no pair was found.
*/

int find_pairs_vertically (game_state *game_ptr) {
	int i;
	int j;

	i = 0;
	while (i < game_ptr->current_options.size) {
		j = 0;
		while (j < game_ptr->current_options.size - 1) {
			if (game_ptr->play_states[game_ptr->undo_array_index].grid[i][j] == game_ptr->play_states[game_ptr->undo_array_index].grid[i][j + 1]) {
				return 1;
			}
			j++;
		}
		i++;
	}

	return 0;
}


/*
If the score is high enough to get into the highscore list, ask for the user's name and add the new score to the highscore list
*/

void check_and_add_to_highsore_list (game_state *game_ptr) {
	highscore new_highscore_entry;

	if (can_be_new_highscore_entry (game_ptr, game_ptr->play_states[game_ptr->undo_array_index].score, -1) == 1) {
		new_highscore_entry.score = game_ptr->play_states[game_ptr->undo_array_index].score;

		new_highscore_entry.name[0] = '\0';
		if (KB_ENTER == Dialog (&dialog_new_highscore, -1, -1, new_highscore_entry.name, NULL)) {
			if (new_highscore_entry.name[0] == '\0') { // empty name string -> "no name"
				strcpy (new_highscore_entry.name, "no name");
			}
			DateAndTime_Get (&new_highscore_entry.year, &new_highscore_entry.month, &new_highscore_entry.day, &new_highscore_entry.hour, &new_highscore_entry.minute, &new_highscore_entry.second);
			do {
				new_highscore_entry.random = random_range (game_ptr, 1, INT_MAX, NULL);
			} while (check_for_duplicate (game_ptr, &new_highscore_entry, -1) == 1); // create new random values until a value is found which has not be used before because every highscore entry should be unique
			add_to_highscore_list (game_ptr, &new_highscore_entry, -1);
		}
	}
}


/*
Check if the score is high enough to get into the highscore list.
Returns 1 if the score is high enough and 0 if the score is too low.
*/

int can_be_new_highscore_entry (game_state *game_ptr, long int score, int config_id) {
	if (config_id == -1) {
		config_id = game_ptr->current_options.config_id;
	}

	if (game_ptr->highscore_list[config_id][4].score < score) {
		return 1;
	} else {
		return 0;
	}
}


/*
Check if there is a highscore entry which equal to the highscore entry given in the parameters.
The function returns 1 if a duplicate was found and 0 if no duplicate was found.
*/

int check_for_duplicate (game_state *game_ptr, highscore *highscore_entry, int config_id) {
	int i;

	if (config_id == -1) {
		config_id = game_ptr->current_options.config_id;
	}

	i = 0;
	while (i < 5) {
		if (highscore_entry->score == game_ptr->highscore_list[config_id][i].score
		    && highscore_entry->random == game_ptr->highscore_list[config_id][i].random
		    && highscore_entry->year == game_ptr->highscore_list[config_id][i].year
		    && highscore_entry->month == game_ptr->highscore_list[config_id][i].month
		    && highscore_entry->day == game_ptr->highscore_list[config_id][i].day
		    && highscore_entry->hour == game_ptr->highscore_list[config_id][i].hour
		    && highscore_entry->minute == game_ptr->highscore_list[config_id][i].minute
		    && highscore_entry->second == game_ptr->highscore_list[config_id][i].second
		    && strncmp (highscore_entry->name, game_ptr->highscore_list[config_id][i].name, 11) == 0) {
			return 1;
		}
		i++;
	}

	return 0;
}


/*
Add a highscore entry to the highscore list
*/

void add_to_highscore_list (game_state *game_ptr, highscore *new_highscore_entry, int config_id) {
	int i;
	int j;

	if (config_id == -1) {
		config_id = game_ptr->current_options.config_id;
	}

	i = 4;
	while (i >= 0) {
		if (game_ptr->highscore_list[config_id][i].score >= new_highscore_entry->score) {
			break;
		}
		i--;
	}
	i++;

	if (i < 5) {
		j = 3;
		while (j >= i) {
			game_ptr->highscore_list[config_id][j + 1] = game_ptr->highscore_list[config_id][j];
			j--;
		}

		game_ptr->highscore_list[config_id][i] = *new_highscore_entry;
	}
}


/*
Show a dialog with the highscore list
*/

void show_highscore_dialog (game_state *game_ptr) {
	char highscore_string[256];
	highscore *curr_hs;

	curr_hs = game_ptr->highscore_list[game_ptr->current_options.config_id];

	sprintf (highscore_string, "1. %-11s %6li\n2. %-11s %6li\n3. %-11s %6li\n4. %-11s %6li\n5. %-11s %6li",
		translate_no_name (curr_hs[0].name), curr_hs[0].score,
		translate_no_name (curr_hs[1].name), curr_hs[1].score,
		translate_no_name (curr_hs[2].name), curr_hs[2].score,
		translate_no_name (curr_hs[3].name), curr_hs[3].score,
		translate_no_name (curr_hs[4].name), curr_hs[4].score);

	DlgNotice (XR_stringPtr (S_HIGHSCORE_TITLE), highscore_string);
}


/*
Check if the highscore name is "no name" and, if this is the case, translate "no name" to the current language.
Returns the translation of "no name" if the name is "no name" or the unchanged name if not.
*/

char * translate_no_name (char *name) {
	if (strcmp (name, "no name") == 0) {
		return XR_stringPtr (S_NO_NAME);
	} else {
		return name;
	}
}


/*
Draw the current gamestate to the main window
*/

void draw_game (game_state *game_ptr) {
	WIN_RECT line;
	char out_str[15];
	int grid_offset;
	int offset;
	int i;
	int j;

	grid_offset = grid_offset_table[game_ptr->current_options.size - 2];

	line.x0 = grid_offset;
	line.y0 = grid_offset;
	line.x1 = game_ptr->current_options.size * 19 + grid_offset;
	line.y1 = grid_offset;

	i = 1;
	while (i <= game_ptr->current_options.size + 1) {
		WinLine (&appW, &line);
		line.y0 += 19;
		line.y1 += 19;
		i++;
	}

	line.x0 = grid_offset;
	line.y0 = grid_offset;
	line.x1 = grid_offset;
	line.y1 = game_ptr->current_options.size * 19 + grid_offset;

	i = 1;
	while (i <= game_ptr->current_options.size + 1) {
		WinLine (&appW, &line);
		line.x0 += 19;
		line.x1 += 19;
		i++;
	}

	i = 0;
	while (i < game_ptr->current_options.size) {
		j = 0;
		while (j < game_ptr->current_options.size) {
			if (game_ptr->play_states[game_ptr->undo_array_index].grid[i][j] != 0) {
				draw_block (i, j, game_ptr->play_states[game_ptr->undo_array_index].grid[i][j], grid_offset);
			}
			j++;
		}
		i++;
	}

	if (game_ptr->current_options.undo == 1) {
		offset = 17;
	} else {
		offset = 20;
	}

	WinStrXY (&appW, 106, 17, XR_stringPtr (S_BOTR));
	WinStrXY (&appW, 120, 18 + offset, XR_stringPtr (S_SCORE));
	sprintf (out_str, "%li", game_ptr->play_states[game_ptr->undo_array_index].score);
	WinStr (&appW, out_str);
	WinStrXY (&appW, 120, 18 + 2 * offset, XR_stringPtr (S_BEST));
	if (game_ptr->best_scores[game_ptr->current_options.config_id] != game_ptr->play_states[game_ptr->undo_array_index].score) {
		sprintf (out_str, "%li", game_ptr->best_scores[game_ptr->current_options.config_id]);
	}
	WinStr (&appW, out_str);
	if (game_ptr->current_options.undo == 1) {
		WinStrXY (&appW, 120, 18 + 3 * offset, XR_stringPtr (S_UNDOS));
		sprintf (out_str, "%i", game_ptr->undos_left);
		WinStr (&appW, out_str);
	}

	if (game_ptr->play_states[game_ptr->undo_array_index].game_over == 1) {
		draw_game_over (game_ptr, grid_offset);
	}
}


/*
Update the game window, only the things that have changed are redrawn.
If the undo parameter is UNDO_MOVE the function will take the element above the current element as the old element.
*/

void draw_update_game (game_state *game_ptr, int undo) {
	char out_str[15];
	int current_index;
	int old_index;
	int grid_offset;
	int offset;
	int i;
	int j;

	current_index = game_ptr->undo_array_index;
	if (undo == NORMAL_MOVE) {
		old_index = decrement_undo_index (current_index);
	} else {
		old_index = increment_undo_index (current_index);
	}

	grid_offset = grid_offset_table[game_ptr->current_options.size - 2];

	i = 0;
	while (i < game_ptr->current_options.size) {
		j = 0;
		while (j < game_ptr->current_options.size) {
			if (game_ptr->play_states[old_index].grid[i][j] != game_ptr->play_states[current_index].grid[i][j]) { // Check if tile needs to be redrawn
				draw_block (i, j, game_ptr->play_states[current_index].grid[i][j], grid_offset);
			}
			j++;
		}
		i++;
	}

	if (game_ptr->current_options.undo == 1) {
		offset = 17;
	} else {
		offset = 20;
	}

	sprintf (out_str, "%li  ", game_ptr->play_states[current_index].score);
	WinStrXY (&appW, 162, 18 + offset, out_str);
	if (game_ptr->best_scores[game_ptr->current_options.config_id] != game_ptr->play_states[current_index].score) {
		sprintf (out_str, "%li  ", game_ptr->best_scores[game_ptr->current_options.config_id]);
	}
	WinStrXY (&appW, 162, 18 + 2 * offset, out_str);
	if (game_ptr->current_options.undo == 1 && game_ptr->undos_left_old != game_ptr->undos_left) {
		sprintf (out_str, "%i  ", game_ptr->undos_left);
		WinStrXY (&appW, 162, 18 + 3 * offset, out_str);
	}

	if (game_ptr->current_options.undo == 0 && game_ptr->play_states[current_index].game_over == 1) {
		draw_game_over (game_ptr, grid_offset);
	}
}


/*
Draw best score
*/

void draw_best_score (game_state *game_ptr) {
	char out_str[15];
	int offset;

	if (game_ptr->current_options.undo == 1) {
		offset = 17;
	} else {
		offset = 20;
	}

	sprintf (out_str, "%li  ", game_ptr->best_scores[game_ptr->current_options.config_id]);
	WinStrXY (&appW, 162, 18 + 2 * offset, out_str);
}


/*
Draw a single block
*/

void draw_block (int i, int j, long int value, int grid_offset) {
	grid_offset++;

	switch (value) {
		case -1:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_unicorn, A_REPLACE);
			break;
		case 0:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_0, A_REPLACE);
			break;
		case 2:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_2, A_REPLACE);
			break;
		case 4:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_4, A_REPLACE);
			break;
		case 8:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_8, A_REPLACE);
			break;
		case 16:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_16, A_REPLACE);
			break;
		case 32:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_32, A_REPLACE);
			break;
		case 64:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_64, A_REPLACE);
			break;
		case 128:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_128, A_REPLACE);
			break;
		case 256:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_256, A_REPLACE);
			break;
		case 512:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_512, A_REPLACE);
			break;
		case 1024:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_1024, A_REPLACE);
			break;
		case 2048: // The magical 2048 tile!
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_2048, A_REPLACE);
			break;
		case 4096:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_4096, A_REPLACE);
			break;
		case 8192:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_8192, A_REPLACE);
			break;
		case 16384:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_16384, A_REPLACE);
			break;
		case 32768:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_32768, A_REPLACE);
			break;
		case 65536:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_65536, A_REPLACE);
			break;
		case 131072:
			WinBitmapPut (&appW, i * 19 + grid_offset, j * 19 + grid_offset, (BITMAP *)&block_131072, A_REPLACE);
			break;
	}
}


/*
Draw the game over text in the middle of the grid
*/

void draw_game_over (game_state *game_ptr, int grid_offset) {
	WIN_RECT rectangle;
	int center;
	int x;
	int y;

	center = grid_offset + (game_ptr->current_options.size * 19) / 2;

	WinStrXY (&appW, center - 29, center - 3, XR_stringPtr (S_GAME_OVER_BOX));

	rectangle.x0 = center - 31;
	rectangle.y0 = center - 6;
	rectangle.x1 = center + 31;
	rectangle.y1 = center + 6;

	WinRect (&appW, &rectangle, A_REVERSE | B_NORMAL);

	rectangle.x0++;
	rectangle.y0++;
	rectangle.x1--;
	rectangle.y1--;

	WinRect (&appW, &rectangle, A_NORMAL | B_NORMAL);

	rectangle.x0++;
	rectangle.y0++;
	rectangle.x1--;
	rectangle.y1 = rectangle.y0;

	WinRect (&appW, &rectangle, A_REVERSE | B_NORMAL);
}


/*
Increment the undo index and wrap if needed
*/

int increment_undo_index (int index) {
	if (index == MAX_UNDOS + 1) {
		index = 0;
	} else {
		index++;
	}

	return index;
}


/*
Decrement the undo index and wrap if needed
*/

int decrement_undo_index (int index) {
	if (index == 0) {
		index = MAX_UNDOS + 1;
	} else {
		index--;
	}

	return index;
}


/*
Reset the current game and start a new game with the new game mode
*/

void new_game (game_state *game_ptr) {
	int i;
	int j;

	game_ptr->current_options = game_ptr->new_options;

	game_ptr->undo_array_index = 0;
	game_ptr->undos_left = 0;
	game_ptr->undos_left_old = 0;
	game_ptr->undo_game_over = 0;

	i = 0; // Reset the whole grid
	while (i < game_ptr->current_options.size) {
		j = 0;
		while (j < game_ptr->current_options.size) {
			game_ptr->play_states[game_ptr->undo_array_index].grid[i][j] = 0;
			j++;
		}
		i++;
	}

	game_ptr->play_states[game_ptr->undo_array_index].score = 0;        // Reset the score and
	game_ptr->play_states[game_ptr->undo_array_index].moves = 0;        // the number of moves
	game_ptr->play_states[game_ptr->undo_array_index].keep_playing = 0; // and the gamestate flags
	game_ptr->play_states[game_ptr->undo_array_index].game_over = 0;
	game_ptr->play_states[game_ptr->undo_array_index].won = 0;
	game_ptr->play_states[game_ptr->undo_array_index].empty_cells = game_ptr->current_options.size * game_ptr->current_options.size;

	create_random_tile (game_ptr); // Two new tiles are created
	create_random_tile (game_ptr);

	if (game_ptr->new_options.mode == MODE_UNICORN) {
		create_new_tile (game_ptr, -1);
	}

}


/*
Create a random tile
*/

void create_random_tile (game_state *game_ptr) {
	int rand_val;
	long int new_block_value;

	rand_val = random_range (game_ptr, 1, 100, NULL);
	if (rand_val > 90) { // The chance for a 4-tile is 10%
		new_block_value = 4;
	} else {             // The chance for a 2-tile is 90%
		new_block_value = 2;
	}

	create_new_tile (game_ptr, new_block_value);
}

/*
Create new tile
*/

void create_new_tile (game_state *game_ptr, long int value) {
	block_pos new_block_pos;

	new_block_pos = find_random_empty_cell (game_ptr);
	game_ptr->play_states[game_ptr->undo_array_index].grid[new_block_pos.x][new_block_pos.y] = value;
	game_ptr->play_states[game_ptr->undo_array_index].empty_cells--;
}


/*
Find a random empty cell
*/

block_pos find_random_empty_cell (game_state *game_ptr) {
	block_pos random_position;
	int rand_val;
	int i;
	int j;

	rand_val = random_range (game_ptr, 1, game_ptr->play_states[game_ptr->undo_array_index].empty_cells, NULL);
	j = 0;
	i = 0;
	while (i < game_ptr->current_options.size) {
		j = 0;
		while (j < game_ptr->current_options.size) {
			if (game_ptr->play_states[game_ptr->undo_array_index].grid[i][j] == 0) {
				rand_val--;
				if (rand_val == 0) {
					goto done; // End the loop
				}
			}
			j++;
		}
		i++;
	}

done:
	random_position.x = i;
	random_position.y = j;

	return random_position;
}


/*
Reset the merge info to zero
*/

void set_merge_info_null (game_state *game_ptr) {
	int i;
	int j;

	i = 0;
	while (i < game_ptr->current_options.size) {
		j = 0;
		while (j < game_ptr->current_options.size) {
			game_ptr->grid_merged[i][j] = 0;
			j++;
		}
		i++;
	}
}


/*
Initialize the program's main datastructure with the default values
*/

void set_defaults (game_state *game_ptr) {
	int i;
	int j;

	game_ptr->ready_for_action = 0;

	i = 0;
	while (i < NUM_CONFIG_IDS) {
		game_ptr->best_scores[i] = 0;
		i++;
	}

	game_ptr->current_options.mode = MODE_CLASSIC;
	game_ptr->current_options.size = 4;
	game_ptr->current_options.animations = 0;
	game_ptr->current_options.undo = 0;
	game_ptr->current_options.config_id = ((game_ptr->current_options.mode - 1) * 3) + (game_ptr->current_options.size - 2);
	game_ptr->new_options.mode = MODE_CLASSIC;
	game_ptr->new_options.size = 4;
	game_ptr->new_options.animations = 0;
	game_ptr->new_options.undo = 0;
	game_ptr->new_options.config_id = ((game_ptr->new_options.mode - 1) * 3) + (game_ptr->new_options.size - 2);

	game_ptr->undo_array_index = 0;
	game_ptr->undos_left = 0;
	game_ptr->undos_left_old = 0;
	game_ptr->undo_game_over = 0;

	game_ptr->random_state = 1;

	game_ptr->animation.frame = -1;
	game_ptr->animation.max_frames = MAX_FRAMES;

	game_ptr->play_states[0].moves = 0;
	game_ptr->play_states[0].won = 0;
	game_ptr->play_states[0].game_over = 0;
	game_ptr->play_states[0].keep_playing = 0;
	game_ptr->play_states[0].empty_cells = 0;
	game_ptr->play_states[0].score = 0;

	i = 0;
	while (i < 4) {
		j = 0;
		while (j < 4) {
			game_ptr->play_states[0].grid[i][j] = 0;
			game_ptr->grid_merged[i][j] = 0;
			j++;
		}
		i++;
	}

	i = 1;
	while (i < MAX_UNDOS + 1) {
		game_ptr->play_states[i] = game_ptr->play_states[0];
		i++;
	}

	i = 0;
	while (i < NUM_CONFIG_IDS) {
		j = 0;
		while (j < 5) {
			strcpy(game_ptr->highscore_list[i][j].name, "-");
			game_ptr->highscore_list[i][j].score = 0;
			game_ptr->highscore_list[i][j].year = 0;
			game_ptr->highscore_list[i][j].month = 0;
			game_ptr->highscore_list[i][j].day = 0;
			game_ptr->highscore_list[i][j].hour = 0;
			game_ptr->highscore_list[i][j].minute = 0;
			game_ptr->highscore_list[i][j].second = 0;
			game_ptr->highscore_list[i][j].random = 0;
			j++;
		}
		i++;
	}
}


/*
Display the config dialog
*/

void show_options_dialog (game_state *game_ptr) {
	WORD options_input[3];

	options_input[0] = game_ptr->new_options.mode;
	options_input[1] = game_ptr->new_options.size;
	options_input[2] = game_ptr->new_options.undo + 1;

	if (KB_ENTER == Dialog (&dialog_options, -1, -1, NULL, options_input)) {
		game_ptr->new_options.mode = options_input[0];
		game_ptr->new_options.size = options_input[1];
		game_ptr->new_options.undo = options_input[2] - 1;
		game_ptr->new_options.config_id = (game_ptr->new_options.undo * 6) + ((game_ptr->new_options.mode - 1) * 3) + (game_ptr->new_options.size - 2);
	}
}


/*
Save the highscore to the file blockshs.
If the save file does not exist create it and save the highscore to it.

Save file format:
  int game_version                            - Version of the game which created the file
  highscore highscore_list[NUM_CONFIG_IDS][5] - Array of highscore entries
  long int best_score[NUM_CONFIG_IDS]         - Array of the best scores
*/

void save_highscore (game_state *game_ptr) {
	WORD test;
	FILES save_file;
	HSYM user_input;
	int success;
	char file_name[MAX_SYM_LEN];
	char file_type[] = "HIGH";                                    // max. 4 characters
	BYTE file_type_array[] = {GEN_DATA_TAG, 0};
	char *file_type_array_string[] = {"HIGH", NULL};              // The last NULL array element might not be required

	success = -1;

	user_input = VarNew (file_type_array, (BYTE **)file_type_array_string);
	if (user_input != H_NULL) {
		success = 0;
		if (HSYMtoName (user_input, (BYTE *)file_name) == TRUE) { // Get the file name which was chosen by the user from the hsym
			HSymDel (user_input);                                 // Delete the hsym to open the file with FOpen

			if (FAccess (file_name, (WORD)FM_WRITE_ACCESS, file_type) == FS_OK) {
				if (FOpen (file_name, &save_file, FM_WRITE, file_type) == FS_OK) {
					if (FWrite (&game_version, sizeof (game_version), &save_file) == FS_OK) {
						if (FWrite (game_ptr->highscore_list, sizeof (game_ptr->highscore_list), &save_file) == FS_OK) {
							if (FWrite (game_ptr->best_scores, sizeof (game_ptr->best_scores), &save_file) == FS_OK) {
								success = 1;
							}
						}
					}
					FClose (&save_file);                          // Close file again
				}
			}
		}
	}

	if (success == 1) {
		DlgNotice (XR_stringPtr (S_SAVE_SUCCESS), XR_stringPtr (S_SAVE_SUCCESS_MSG));
	} else if (success == 0) {
		DlgNotice (XR_stringPtr (S_SAVE_FAILURE), XR_stringPtr (S_SAVE_FAILURE_MSG));
	}
}


/*
Restore the highscore from the save file selected by the user
*/

void load_highscore (game_state *game_ptr) {
	FILES save_file;
	HSYM user_input;
	int success;
	int highscore_version;
	char file_name[MAX_SYM_LEN];
	char file_type[] = "HIGH";                       // max. 4 characters
	BYTE file_type_array[] = {GEN_DATA_TAG, 0};
	char *file_type_array_string[] = {"HIGH", NULL}; // As already mentioned above, the last NULL array element might not be required

	success = -1;

	user_input = VarOpen (file_type_array, (BYTE **)file_type_array_string);
	if (user_input != H_NULL) {
		success = 0;
		if (HSYMtoName (user_input, (BYTE *)file_name) == TRUE) {

			if (FAccess (file_name, FM_READ_ACCESS, file_type) == FS_OK) {                                                      // Check if file exists
				if (FOpen (file_name, &save_file, FM_READ, file_type) == FS_OK) {                                               // Open file
					if (FRead (&highscore_version, sizeof (highscore_version), &save_file) == FS_OK) {                          // Read the highscore version
						if (highscore_version <= 17) {                                                                          // If highscore file version is 1.7 or lower
							if (FGetSize (&save_file) == 506) {                                                                 // Check file size
								highscore_old highscore_read_buffer[6][5];
								if (FRead (highscore_read_buffer, sizeof (highscore_read_buffer), &save_file) == FS_OK) {       // Read highscore list into buffer
									long int best_score_read_buffer[6];
									if (FRead (best_score_read_buffer, sizeof (best_score_read_buffer), &save_file) == FS_OK) { // Read best scores list int buffer
										if (check_old_highscore_entries (highscore_read_buffer, 6) == VALID) {
											merge_old_highscore_entries (game_ptr, highscore_read_buffer, 6);
											merge_best_scores (game_ptr, best_score_read_buffer, 6);
											success = 1;
										}
									}
								}
							}
						} else if (highscore_version == 18) {
							if (FGetSize (&save_file) == 1010) {                                                                // Check file size
								highscore_old highscore_read_buffer[12][5];
								if (FRead (highscore_read_buffer, sizeof (highscore_read_buffer), &save_file) == FS_OK) {       // Read highscore list into buffer
									long int best_score_read_buffer[12];
									if (FRead (best_score_read_buffer, sizeof (best_score_read_buffer), &save_file) == FS_OK) { // Read best scores list int buffer
										if (check_old_highscore_entries (highscore_read_buffer, 12) == VALID) {
											merge_old_highscore_entries (game_ptr, highscore_read_buffer, 12);
											merge_best_scores (game_ptr, best_score_read_buffer, 12);
											success = 1;
										}
									}
								}
							}
						} else if (highscore_version == 19) {
							if (FGetSize (&save_file) == 1850) {                                                                // Check file size
								highscore highscore_read_buffer[12][5];
								if (FRead (highscore_read_buffer, sizeof (highscore_read_buffer), &save_file) == FS_OK) {       // Read highscore list into buffer
									long int best_score_read_buffer[12];
									if (FRead (best_score_read_buffer, sizeof (best_score_read_buffer), &save_file) == FS_OK) { // Read best scores list int buffer
										if (check_highscore_entries (highscore_read_buffer) == VALID) {
											merge_highscore_entries (game_ptr, highscore_read_buffer);
											merge_best_scores (game_ptr, best_score_read_buffer, 12);
											success = 1;
										}
									}
								}
							}
						}
					}
					FClose (&save_file); // Close file again
				}
			}
		}
	}

	draw_best_score (game_ptr);

	if (success == 1) {
		DlgNotice (XR_stringPtr (S_LOAD_SUCCESS), XR_stringPtr (S_LOAD_SUCCESS_MSG));
	} else if (success == 0) {
		DlgNotice (XR_stringPtr (S_LOAD_FAILURE), XR_stringPtr (S_LOAD_FAILURE_MSG));
	}
}


/*
Check if the loaded highscore file with an old file version is valid
The function returns VALID or INVALID.
*/

int check_old_highscore_entries (highscore_old highscore_list[][5], int highscore_list_size) {
	int i;
	int j;
	int k;

	i = 0;
	while (i < highscore_list_size) {
		j = 0;
		while (j < 5) {
			k = 0; // k will be the position of the first null byte in the name string
			while (k < 12) {
				if (highscore_list[i][j].name[k] == '\0') { // search for null byte
					break;
				}
				k++;
			}

			if (k > 11) {       // if k is 12 then no null byte was found
				return INVALID; // which makes the file invalid
			}
			j++;
		}
		i++;
	}

	return VALID;
}


/*
Check if the loaded highscore file is valid
The function returns VALID or INVALID.
*/

int check_highscore_entries (highscore highscore_list[12][5]) {
	int i;
	int j;
	int k;

	i = 0;
	while (i < 12) {
		j = 0;
		while (j < 5) {
			k = 0; // k will be the position of the first null byte in the name string
			while (k < 12) {
				if (highscore_list[i][j].name[k] == '\0') { // search for null byte
					break;
				}
				k++;
			}

			if (k > 11) {       // if k is 12 then no null byte was found
				return INVALID; // which makes the file invalid
			}
			j++;
		}
		i++;
	}

	return VALID;
}


/*
Merge the loaded highscore entries from an old version of BotR (version 1.8 or lower)
*/

void merge_old_highscore_entries (game_state *game_ptr, highscore_old highscore_list[][5], int highscore_list_size) {
	highscore new_highscore_entry;
	char *name;
	long int score;
	int duplicates_old;
	int duplicates_new;
	int merge_count;
	int i;
	int j;
	int k;

	// the old highscore entrys without a date will get the current date
	DateAndTime_Get (&new_highscore_entry.year, &new_highscore_entry.month, &new_highscore_entry.day, &new_highscore_entry.hour, &new_highscore_entry.minute, &new_highscore_entry.second);

	i = 0; // very sophisticated merge algorithm
	while (i < highscore_list_size) {
		j = 0;
		while (j < 5) {
			if (can_be_new_highscore_entry (game_ptr, highscore_list[i][j].score, i) == 1) {
				name = highscore_list[i][j].name;
				score = highscore_list[i][j].score;
				duplicates_old = 0;
				duplicates_new = 0;

				k = 0;
				while (k < 5) {
					if ((highscore_list[i][k].score == score) && (strcmp (highscore_list[i][k].name, name) == 0)) {
						duplicates_old++;
					}
					k++;
				}

				k = 0;
				while (k < 5) {
					if ((game_ptr->highscore_list[i][k].score == score) && (strcmp (game_ptr->highscore_list[i][k].name, name) == 0)) {
						duplicates_new++;
					}
					k++;
				}

				merge_count = duplicates_old - duplicates_new;

				if (merge_count > 0) {
					strcpy (new_highscore_entry.name, name);
					new_highscore_entry.score = score;
					while (merge_count > 0) {
						do {
							new_highscore_entry.random = random_range (game_ptr, 1, INT_MAX, NULL);
						} while (check_for_duplicate (game_ptr, &new_highscore_entry, i) == 1); // create new random values until a value is found which was not used before, because every highscore entry should be unique
						add_to_highscore_list (game_ptr, &new_highscore_entry, i);
						merge_count--;
					}
				}
			} else {
				break;
			}
			j++;
		}
		i++;
	}
}


/*
Merge the loaded highscore entries
*/

void merge_highscore_entries (game_state *game_ptr, highscore highscore_list[NUM_CONFIG_IDS][5]) {
	int i;
	int j;

	i = 0;
	while (i < NUM_CONFIG_IDS) {
		j = 0;
		while (j < 5) {
			if (can_be_new_highscore_entry (game_ptr, highscore_list[i][j].score, i) == 1) {
				if (check_for_duplicate (game_ptr, &highscore_list[i][j], i) == 0) {
					add_to_highscore_list (game_ptr, &highscore_list[i][j], i);
				}
			} else {
				break;
			}
			j++;
		}
		i++;
	}
}


/*
Merge the loaded best scores
*/

void merge_best_scores (game_state *game_ptr, long int best_score_list[], int best_score_list_size) {
	int i;

	i = 0;
	while (i < best_score_list_size) {
		if (best_score_list[i] > game_ptr->best_scores[i]) {
			game_ptr->best_scores[i] = best_score_list[i];
		}
		i++;
	}
}


/*
Display the help dialog
*/

void show_help_dialog (void) {
	DlgNotice (XR_stringPtr (S_HELP_TITLE), XR_stringPtr (S_HELP_MESSAGE));
}


/*
Display the credits dialog
*/

void show_about_dialog (void) {
	int major;
	int minor;
	char out_str[150];

	minor = game_version % 10;
	major = (game_version - minor) / 10;

#if (DEVELOPER == 1)
	sprintf (out_str, XR_stringPtr (S_ABOUT_DEVEL), major, minor);
#else
	sprintf (out_str, XR_stringPtr (S_ABOUT_NORMAL), major, minor);
#endif

	DlgNotice (XR_stringPtr (S_ABOUT_TITLE), out_str);
}


/*
This function will be called if the user types in the leet code.
What the function does is currently completely unknown.
*/

void easter_egg (game_state *game_ptr) {
	const long int password[] = {2054797831, 1667735055, 6806019};
	char input[12];
	long int key_tmp[3];
	long int *key;
	long int *seed;
	int num_correct;
	int i;

	if (KB_ENTER == Dialog (&dialog_get_password, -1, -1, input, NULL)) {
		seed = (long int *)input;
		num_correct = 0;
		i = 0;
		while (i < 3) {
			key_tmp[i] = random_raw (game_ptr, &seed[i]);
			random_raw (game_ptr, &seed[i]);

			if (seed[i] == password[i]) {
				num_correct++;
			}

			i++;
		}

		if (num_correct == 3) {
			if (game_ptr->ready_for_action == 0) {
				key = (long int *)game_ptr->key;
				memcpy (key, key_tmp, sizeof(key_tmp));
				game_ptr->ready_for_action = 1;
				easter_egg_show_text (game_ptr, 6);
			} else {
				easter_egg_show_text (game_ptr, 5);
				memset (game_ptr->key, 0, sizeof(game_ptr->key));
				game_ptr->ready_for_action = 0;
			}
		} else {
			DlgNotice (XR_stringPtr (S_DUKE_NUKEM), XR_stringPtr (S_DUKE_INSULT));
		}
	}

	memset (input, '\0', sizeof(input));
	memset (key_tmp, 0, sizeof(key_tmp));
}


/*
Also nobody knows what this function does...
*/

void easter_egg_show_text (game_state *game_ptr, int category) {
	char title[11];
	char text[100];
	char *crypto;
	int text_number;
	int low;
	int high;
	int i;

	if (game_ptr->ready_for_action == 0) {
		return;
	}

	switch (category) {
		case 1:
			low = S_EASTER_EGG_2;
			high = S_EASTER_EGG_5;
			break;
		case 2:
			low = S_EASTER_EGG_6;
			high = S_EASTER_EGG_10;
			break;
		case 3:
			low = S_EASTER_EGG_11;
			high = S_EASTER_EGG_14;
			break;
		case 4:
			low = S_EASTER_EGG_15;
			high = S_EASTER_EGG_17;
			break;
		case 5:
			low = S_EASTER_EGG_18;
			high = S_EASTER_EGG_20;
			break;
		case 6:
			low = S_EASTER_EGG_21;
			high = S_EASTER_EGG_21;
			break;
		default:
			return;
	}

	crypto = XR_stringPtr (S_EASTER_EGG_1);

	i = 0;
	while (i < sizeof(title) - 1 && crypto[i] != '\0') {
		title[i] = game_ptr->key[i % 12] ^ crypto[i];
		i++;
	}
	title[i] = '\0';

	text_number = random_range (game_ptr, low, high, NULL);
	crypto = XR_stringPtr (text_number);

	i = 0;
	while (i < sizeof(text) - 1 && crypto[i] != '\0') {
		text[i] = game_ptr->key[i % 12] ^ crypto[i];
		i++;
	}
	text[i] = '\0';

	DlgNotice (title, text);

	memset (title, 0, sizeof(title));
	memset (text, 0, sizeof(text));
}


/*
The function returns a random integer within a range specified in the parameters
*/

int random_range (game_state *game_ptr, int low, int high, long int *random_state_ptr) {
	long int range;
	long int border;
	long int random_raw_in;
	int random_out;

	range = (long int)high - (long int)low + 1;
	border = 2147483646 - (2147483646 % range);
	do {
		random_raw_in = random_raw (game_ptr, random_state_ptr);
	} while (random_raw_in > border);
	random_out = (int)(low + (random_raw_in % range));

	return random_out;
}


/*
Raw pseudo random number generator
The function is the 32 bit version of the Park-Miller random number generator.
You can download the original article written by Stephen K. Park and Keith W. Miller in 1988 here:
http://www.firstpr.com.au/dsp/rand31/p1192-park.pdf

"The generation of random numbers is too important to be left to chance."
-- Robert R. Coveyou
*/

long int random_raw (game_state *game_ptr, long int *random_state_ptr) {
	long int *tmp_state;
	long int lo;
	long int hi;
	long int test;

	if (random_state_ptr == NULL) {
		tmp_state = &game_ptr->random_state; // use the default random state
	} else {
		tmp_state = random_state_ptr;        // use the custom random state
	}

	lo = *tmp_state % 127773;
	hi = *tmp_state / 127773;

	test = 16807 * lo - 2836 * hi;

	if (test > 0) {
		*tmp_state = test;
	} else {
		*tmp_state = test + 2147483647;
	}

	return *tmp_state;
}


/*
Callback function for dialogs
This function is provided as a callback function when a dialog is opened.
Because we don't need a callback function here, we just leave it empty.
*/

DWORD no_call_back (WORD DlgId, DWORD Value) {
	return TRUE;
}
