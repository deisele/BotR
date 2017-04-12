
# Blocks on the Rocks for TI-92 Plus and TI Voyage 200

Blocks on the Rocks is a clone of the game 2048
(https://gabrielecirulli.github.io/2048/) for 68k calculators. The game has
different game modes and a highscore list for each mode. It is created as a
FlashApp so you can start it directly from the apps screen and switch between
apps while playing.

Tested with AMS 2.09 and higher.

## Installation
To install the game send blocks.9xK to your calculator, then you can start it
directly from the apps menu or the apps screen.

### Translation
If you want to use a translated version of the game, you have to choose if you
want to have the standalone version (translation included in the app) or if
you want to use the base app (blocks.9xK) with an additional localizer app.

#### Standalone
Just send the right app from the standalone directory to your calculator

#### Localizer app
Send the base app (blocks.9xK) and one or more localizer apps from the localizer
directory to your calculator. The localization system will automatically use the
language which you have currently selected in the "MODE" menu. To use a specific
language you must have the official TI language pack of that language installed
(otherwise you would not be able to select that language in the "MODE" menu of
your calculator).

## Features
* cool iconset
* different game modes
* unicorn mode
* one highscore list for each mode
* easter egg
* you can switch to another app while playing and continue the game later
* language localization

## Authors
* Daniel Eisele (daniel_eisele@gmx.de)
* Janek Schoffit

## Website
https://daniel.ibme.de/de/projekte/blocks.html

## Build
To build Blocks on the Rocks you will need TI Flash Studio (Windows XP only)
which can be downloaded here(look for fsinst.exe in the attachments section):
https://epsstore.ti.com/OA_HTML/csksxvm.jsp?nSetId=80279
There is also a newer version available from unofficial sources (hint: search
for "FlashStudio1.1.43.exe"), which I personally use. I don't think that the new
version has any relevant benefits, so it shouldn't matter which one you use.

To run TI Flash Studio you will also need the Microsoft Java Virtual Machine
which is not supported by Microsoft anymore, so you can only get it from
unofficial sources.

As soon as you get TI Flash Studio running, open Blocks.FSP and press the
Build button. Blocks on the Rocks will now be compiled and you can play it in
the simulator of TI Flash Studio.

You may also want to add the directory
"C:\\\<programs\>\\\<shared files\>\\TI Shared\\TI Flash Studio" to your PATH
variable to get some debugger features working. Replace \<programs\> and
\<Shared Files\> with the actual folder names.

To send the FlashApp to a real calculator, it must be signed with the
cryptographic key of TI. As all TI keys are cracked, you can sign the FlashApp
yourself. You can get the keys here: http://brandonw.net/calculators/keys/

The default key of TI Flash Studio is stored in the file sdk-92p.key which is
located in the program folder of TI Flash Studio.
The first line in the key file is the key id.
The second line is value n of the key (bytes in reverse order) with a 40 in
front of it.
The third line is value d of the key (bytes in reverse order) with a 40 in
front of it.
For the TI-92 Plus Flash application signing key the file looks like this:

    0101
    406104CDFAD955D41F1ECCB9B622007FE8BC75E8B28DA178334755FEF27C564D47B04FD82498C163B762991C68CF64E29236BC41A4C1BCB9793B6EE965407C74BC
    40E17C4AA4C942CC57282E9B911E7B39352A2D8D4EC15398366C971FF7D2D92FB18D35D60A78C086CCE0F08F0F4CC36F676A4604E529DD72C95C11AED2F48D6D37

If you get strange errors related to file names or see file names with some part
of the ending cut off, then you have to reduce the file name length. This can be
done by using the 8.3 file/directory names, which is the default when you open
the .fsp project file directly from the explorer. More info about 8.3 file names
here: https://en.wikipedia.org/wiki/8.3_filename

For generating the C hex array of an image I recommend Image Studio. You can
download it from here:
http://www.ticalc.org/archives/files/fileinfo/307/30736.html

Feel free to send me an email (daniel_eisele@gmx.de) if you have any questions.

## SDK Documentation
The documentation for the 68k calculator SDK is split up into two files:
* sdk8992pguide.pdf (developer guide for TIOS)
* sdk8992pcompiler.pdf (compiler manual)

You can download them here (look into the attachments section):
https://epsstore.ti.com/OA_HTML/csksxvm.jsp?nSetId=80279

## Translation
All strings in the application are stored in the app frame, which makes it
possible to translate BotR just by exchanging the app frame. Another way to use
a translation is to create a separate localizer application that hooks the
translated frame ahead of the default BotR app frame.

The translation files are located in the "translations" folder. If you want to
create a new translation or edit an existing one, just make a copy of one of the
existing files and edit the the strings in it.

To use the translation files (lang_en.h, lang_de.h, lang_leet.h or you own) as
the default language, you only have to update the #include statement in line 5
of blocks.c with the new file name and rebuild the application.

If you want to use a translation as a localizer app, it basically works in the
same way. The difference here is that to make building multiple localizer apps
easier there is a separate project file for each translation file. The name of
the translation header file is specified with a define in the compiler command
line parameters in the project file. To create a new localizer app, copy one of
the existing project files (botr_en.fsp or botr_de.fsp) and update the name of
the translation file in both COMPILEOPTIONS lines and change the localizer app
name in the FSNAME line to prevent name conflicts.

The localizer app works by monitoring the selected language on the calculator
(you can select the language in the third page of the "MODE" menu). If the
localizer app detects that the selected language matches the language of the
translation file, it hooks the translation frame ahead of the normal BotR app
frame.

One problem with the localizer app is that the user has to install the official
language pack from TI to make a specific language available in the language
selection menu of the calculator. This means that you can't use the localizer
app without the TI language pack. The leetspeak translation doesn't have a TI
language pack so it can't be used in a localizer app currently.
