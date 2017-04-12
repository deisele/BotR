#ifndef LANG_DE_H
#define LANG_DE_H

#include <tiams.h>

/*
Language of this localization file
*/

#define BOTR_LANGUAGE "Deutsch"


/*
Name which the BotR app should have on the [APPS] menu
*/

#define BOTR_APP_NAME "Blocks on the Rocks"


/*
Name which the localizer app should have in the VAR-LINK [F7:APPS] screen
*/

#define LOCALIZER_APP_NAME "German BotR localizer"


/*
Translations of all BotR strings
*/

#define S_NEW_GAME_TR         "Neues Spiel"
#define S_HIGHSCORE_MENU_TR   "Highscore"
#define S_OPTIONS_MENU_TR     "Optionen"
#define S_EXTRAS_TR           "Extras"
#define S_SAVE_HIGHSCORE_TR   "Highscore speichern"
#define S_LOAD_HIGHSCORE_TR   "Highscore laden"
#define S_HELP_MENU_TR        "Hilfe"
#define S_ABOUT_MENU_TR       "Info"

#define S_GAME_MODE_TR        "Spielmodus: "
#define S_GRID_SIZE_TR        "Feldgr" RF_O_UMLAUT RF_LONG_S "e:  "
#define S_UNDO_TR             "R" RF_U_UMLAUT "ckg" RF_A_UMLAUT "ngig: "
#define S_OPTIONS_TITLE_TR    "Optionen"

#define S_CLASSIC_TR          "Klassisch"
#define S_UNICORN_TR          "Einhorn"

#define S_2X2_TR              "2x2"
#define S_3X3_TR              "3x3"
#define S_4X4_TR              "4x4"

#define S_DISABLED_TR         "Aus"
#define S_ENABLED_TR          "An"

#define S_CHOOSE_TR           "Auswahl:    "
#define S_YOU_WIN_1_TR        "Gewonnen"

#define S_KEEP_GOING_TR       "Weiter"
#define S_END_GAME_TR         "Beenden"

#define S_WIN_END_1_TR        "Sie haben gewonnen, aber es"
#define S_WIN_END_2_TR        "sind keine weiteren Z" RF_U_UMLAUT "ge m" RF_O_UMLAUT "glich."
#define S_YOU_WIN_2_TR        "Gewonnen"

#define S_GAME_OVER_TR        "Spiel zu Ende"
#define S_GAME_OVER_TITLE_TR  "Verloren"

#define S_NAME_TR             "Name:       "
#define S_NEW_HIGHSCORE_TR    "Neue Highscore"

#define S_CHOOSE_1_TR         "Auswahl:    "
#define S_GAME_OVER_UNDO_TR   "Verloren"

#define S_WIN_END_UNDO_1_TR   "Sie haben gewonnen, aber es"
#define S_WIN_END_UNDO_2_TR   "sind keine weiteren Z" RF_U_UMLAUT "ge m" RF_O_UMLAUT "glich."
#define S_CHOOSE_2_TR         "Auswahl:    "
#define S_YOU_WIN_3_TR        "Gewonnen"

#define S_CHOOSE_UNDO_TR      "R" RF_U_UMLAUT "ckg" RF_A_UMLAUT "ngig"
#define S_CHOOSE_END_GAME_TR  "Beenden"

#define S_PASSWORD_TR         "Passwort:   "
#define S_READY_FOR_ACTION_TR "Ready for action?"

#define S_NO_NAME_TR          "leer"

#define S_HIGHSCORE_TITLE_TR  "Highscore"

#define S_BOTR_TR             "Blocks on the Rocks"
#define S_SCORE_TR            "Score: "
#define S_BEST_TR             "Best:  "
#define S_UNDOS_TR            "Undos: "

#define S_GAME_OVER_BOX_TR    "Game over!"

#define S_SAVE_SUCCESS_TR     "Gespeichert"
#define S_SAVE_FAILURE_TR     "Fehler"
#define S_SAVE_SUCCESS_MSG_TR "Highscore gespeichert."
#define S_SAVE_FAILURE_MSG_TR "Beim Speichern der Highscore ist ein Fehler aufgetreten."

#define S_LOAD_SUCCESS_TR     "Geladen"
#define S_LOAD_FAILURE_TR     "Fehler"
#define S_LOAD_SUCCESS_MSG_TR "Geladene Highscore mit der aktuellen Highscore zusammengeführt."
#define S_LOAD_FAILURE_MSG_TR "Beim Laden der Highscore ist ein Fehler aufgetreten."

#define S_HELP_TITLE_TR       "Hilfe"

#define S_HELP_MESSAGE_TR     "Die Bl" RF_O_UMLAUT "cke werden mit den Pfeiltasten verschoben. " \
                              "Werden zwei Bl" RF_O_UMLAUT "cke zusammengeschoben verdoppelt sich ihr Wert. " \
                              "Ziel ist es, den 2048-Block zu bilden.\n" \
                              "Ist \"R" RF_U_UMLAUT "ckg" RF_A_UMLAUT "ngig\" aktiviert, so k" RF_O_UMLAUT "nnen mit der R" RF_U_UMLAUT "cktaste (" RF_ARROW_LEFT ") bis zu 20 Z" RF_U_UMLAUT "ge r" RF_U_UMLAUT "ckg" RF_A_UMLAUT "ngig gemacht werden."

#define S_ABOUT_TITLE_TR      "Info"
#define S_ABOUT_NORMAL_TR     "Blocks on the Rocks V%i.%i\n\nAutoren:\n  Daniel Eisele\n  (daniel_eisele@gmx.de)\n  Janek Schoffit\n\nWebseite:\n  https://daniel.ibme.de"
#define S_ABOUT_DEVEL_TR      "BotR V%i.%i Devel\n\nAutoren:\n  Daniel Eisele\n  (daniel_eisele@gmx.de)\n  Janek Schoffit\n\nWebseite:\n  https://daniel.ibme.de"

#define S_DUKE_NUKEM_TR       "Duke Nukem"
#define S_DUKE_INSULT_TR      "Damn, you're ugly!"

#define S_EASTER_EGG_1_TR     "\x5C\xD9\xB8\xC5\x77\x77\x65\x1E\x6B\x2E"
#define S_EASTER_EGG_2_TR     "\x51\xD8\xF4\xD3\x77\x4D\x79\x18\x6B\x63\xCD\x91\x38\xC7\xBA\xC3\x3C\x19\x71\x06\x7D\x63\xD8\x90\x7C\x8C\xB0\xC8\x32\x4E\x30\x17\x7B\x21\xDB\x92\x7D\x8C\xB4\xD5\x3A\x17\x3E\x5B\x2E\x22\xD7\x9A\x38\xE5\xF4\xCD\x77\x58\x7C\x19\x2E\x2C\xCC\x8A\x6C\xCD\xF3\xC7\x22\x54\x3E"
#define S_EASTER_EGG_3_TR     "\x54\xC9\xA7\x87\x24\x19\x62\x1A\x6D\x28\x98"
#define S_EASTER_EGG_4_TR     "\x4C\xC4\xB6\xD2\x32\x1E\x63\x55\x61\x2D\xD5\x87\x38\xD8\xA4\xCF\x77\x4E\x71\x0C\x7D\x63\xCD\x96\x71\xDF\xF3\xC3\x36\x57\x30\x10\x60\x27\x95\xDE\x79\xC2\xB7\x80\x3E\x57\x30\x17\x61\x37\xD1\xDE\x77\xCA\xF3\xD4\x3F\x5C\x7D\x59\x2E\x3A\xD6\x8B\x38\xC8\xBA\xC5\x76"
#define S_EASTER_EGG_5_TR     "\x5F\xC9\xA7\x80\x35\x58\x73\x1E\x2E\x37\xD6\xDE\x6F\xC3\xA1\xCB\x7B\x19\x69\x1A\x7B\x63\xCA\x92\x79\xCF\xB8\xC5\x25\x17"
#define S_EASTER_EGG_6_TR     "\x50\xC3\xBF\xD9\x77\x4A\x78\x1C\x7A\x62"
#define S_EASTER_EGG_7_TR     "\x4C\xC4\xBA\xD3\x77\x4B\x75\x14\x62\x2F\xC0\xDE\x68\xC5\xA0\xD3\x32\x4A\x30\x18\x6B\x63\xD6\x98\x7E\x8D"
#define S_EASTER_EGG_8_TR     "\x5C\xCD\xBE\xCE\x77\x50\x64\x5B"
#define S_EASTER_EGG_9_TR     "\x4B\xC4\xBA\xD4\x77\x51\x71\x05\x7E\x26\xD7\x8D\x36"
#define S_EASTER_EGG_10_TR    "\x51\x8B\xBE\x80\x39\x56\x64\x55\x6D\x31\xC0\x97\x76\xCB\xF3\xCF\x21\x5C\x62\x55\x7A\x2B\xD0\x8D\x39"
#define S_EASTER_EGG_11_TR    "\x5F\xDE\xBC\xCF\x21\x40\x31"
#define S_EASTER_EGG_12_TR    "\x51\x8C\xB0\xCF\x22\x55\x74\x55\x6A\x2C\x99\x8A\x70\xC5\xA0\x80\x36\x55\x7C\x55\x6A\x22\xC0\xDF"
#define S_EASTER_EGG_13_TR    "\x51\x8C\xB4\xCF\x77\x4E\x78\x10\x7C\x26\x99\xB7\x38\xDC\xBF\xC5\x36\x4A\x75\x59\x2E\x22\xD7\x9A\x38\xE5\xF3\xD0\x3B\x5C\x71\x06\x6B\x63\xCE\x96\x7D\xDE\xB6\x80\x1E\x19\x77\x1A\x20"
#define S_EASTER_EGG_14_TR    "\x50\xCD\xBA\xCC\x77\x4D\x7F\x55\x7A\x2B\xDC\xDE\x73\xC5\xBD\xC7\x7B\x19\x72\x14\x6C\x3A\x98"
#define S_EASTER_EGG_15_TR    "\x59\xC4\xFF\x80\x3D\x4C\x63\x01\x2E\x2A\xD7\xDE\x6C\xC5\xBE\xC5\x76"
#define S_EASTER_EGG_16_TR    "\x5D\xC2\xB7\x80\x38\x5F\x30\x01\x66\x26\x99\x92\x71\xC2\xB6\x81"
#define S_EASTER_EGG_17_TR    "\x54\xCD\xA0\xD4\x77\x4A\x64\x1A\x7E\x6F\x99\x8A\x77\xD8\xB2\xCC\x77\x5D\x75\x06\x7A\x31\xCC\x9D\x6C\xC5\xBC\xCE\x76"
#define S_EASTER_EGG_18_TR    "\x5D\xCD\xA7\x80\x24\x51\x79\x01\x2E\x22\xD7\x9A\x38\xC8\xBA\xC5\x76"
#define S_EASTER_EGG_19_TR    "\x5C\xCD\xBE\xCE\x7B\x19\x69\x1A\x7B\x64\xCB\x9B\x38\xD9\xB4\xCC\x2E\x18"
#define S_EASTER_EGG_20_TR    "\x41\xC3\xA6\x87\x25\x5C\x30\x14\x60\x63\xD0\x90\x6B\xDC\xBA\xD2\x36\x4D\x79\x1A\x60\x63\xDF\x91\x6A\x8C\xB1\xC9\x25\x4D\x78\x55\x6D\x2C\xD7\x8A\x6A\xC3\xBF\x81"
#define S_EASTER_EGG_21_TR    "\x5A\xCD\xB1\xC5\x24\x15\x30\x17\x7B\x2F\xD5\x9B\x6C\xDF\xFF\x80\x35\x56\x7D\x17\x7D\x6D\x99\xBA\x79\xC1\xBD\x8C\x77\x70\x30\x19\x61\x35\xDC\xDE\x6C\xC4\xBA\xD3\x77\x53\x7F\x17\x2F"

#endif
