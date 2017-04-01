#include <tiams.h>
#include "Blocks.h"
#include "tr.h"

TOOLBOX app_menu, RC_NO_IDS, 0, 240 {
	S_NEW_GAME,       ACM_NEW
	S_HIGHSCORE_MENU, ACM_HIGHSCORE
	S_OPTIONS_MENU,   ACM_OPTIONS
	S_EXTRAS {
		S_SAVE_HIGHSCORE, ACM_SAVE
		S_LOAD_HIGHSCORE, ACM_LOAD
		S_HELP_MENU,      ACM_HELP
		S_ABOUT_MENU,     ACM_ABOUT
	}
}

DIALOG dialog_options, 0, 0, no_call_back {
	POPUP, {DF_TAB_SPACES, 12, 14}, S_GAME_MODE, popup_game_mode, 0
	POPUP, {DF_TAB_SPACES, 12, 24}, S_GRID_SIZE, popup_grid_size, 1
	POPUP, {DF_TAB_SPACES, 12, 34}, S_UNDO, popup_undo, 2
	HEADER, {0, 0, 0}, S_OPTIONS_TITLE, PDB_OK, PDB_CANCEL
}

POPUP popup_game_mode, RC_NO_IDS, 0 {
	S_CLASSIC, MODE_CLASSIC
	S_UNICORN, MODE_UNICORN
}

POPUP popup_grid_size, RC_NO_IDS, 0 {
	S_2X2, 2
	S_3X3, 3
	S_4X4, 4
}

POPUP popup_undo, RC_NO_IDS, 0 {
	S_DISABLED, 1
	S_ENABLED, 2
}

DIALOG dialog_win_notification, 0, 0, no_call_back {
	POPUP, {DF_TAB_SPACES, 12, 14}, S_CHOOSE, popup_select_keep_playing, 0
	HEADER, {0, 0, 0}, S_YOU_WIN_1, PDB_OK, PDB_CANCEL
}

POPUP popup_select_keep_playing, RC_NO_IDS, 0 {
	S_KEEP_GOING, 1
	S_END_GAME, 2
}

DIALOG dialog_win_and_game_over, 0, 0, no_call_back {
	TEXT, {0, 12, 14}, S_WIN_END_1
	TEXT, {0, 12, 24}, S_WIN_END_2
	HEADER, {0, 0, 0}, S_YOU_WIN_2, PDB_OK, PDB_CANCEL
}

DIALOG dialog_game_over, 0, 0, no_call_back {
	TEXT, {0, 12, 14}, S_GAME_OVER
	HEADER, {0, 0, 0}, S_GAME_OVER_TITLE, PDB_OK, PDB_CANCEL
}

DIALOG dialog_new_highscore, 0, 0, no_call_back {
	EDIT, {DF_TAB_SPACES, 12, 14}, S_NAME, 0, 11, 11
	HEADER, {0, 0, 0}, S_NEW_HIGHSCORE, PDB_OK, PDB_CANCEL
}

DIALOG dialog_undo_game_over, 0, 0, no_call_back {
	POPUP, {DF_TAB_SPACES, 12, 14}, S_CHOOSE_1, popup_undo_end_game, 0
	HEADER, {0, 0, 0}, S_GAME_OVER_UNDO, PDB_OK, PDB_CANCEL
}

DIALOG dialog_undo_win_and_game_over, 0, 0, no_call_back {
	TEXT, {0, 12, 14}, S_WIN_END_UNDO_1
	TEXT, {0, 12, 24}, S_WIN_END_UNDO_2
	POPUP, {DF_TAB_SPACES, 12, 34}, S_CHOOSE_2, popup_undo_end_game, 0
	HEADER, {0, 0, 0}, S_YOU_WIN_3, PDB_OK, PDB_CANCEL
}

POPUP popup_undo_end_game, RC_NO_IDS, 0 {
	S_CHOOSE_UNDO, 1
	S_CHOOSE_END_GAME, 2
}

DIALOG dialog_get_password, 0, 0, no_call_back {	/* 3A5T3R 3GG */
	EDIT, {DF_TAB_SPACES, 12, 14}, S_PASSWORD, 0, 11, 11
	HEADER, {0, 0, 0}, S_READY_FOR_ACTION, PDB_OK, PDB_CANCEL
}