
# Blocks on the Rocks for TI-92 Plus and TI Voyage 200

Blocks on the Rocks is a clone of the game 2048
(http://gabrielecirulli.github.io/2048/) for 68k calculators. The game has
different game modes and a highscore list for each mode. It is created as a
FlashApp so you can start it directly from the apps screen and switch between
apps while playing.

Tested with AMS 2.09 and higher.

## Installation
To install the game send blocks.9xK to your calculator, then you can start it
directly from the apps menu or the apps screen.

## Features
* cool iconset
* different game modes
* unicorn mode
* one highscore list for each mode
* easter egg
* you can switch to another app while playing and continue the game later

## Authors
* Daniel Eisele (daniel_eisele@gmx.de)
* Janek Schoffit

## Website
http://daniel.ibme.de

## Build
To build Blocks on the Rocks you will need TI Flash Studio (Windows XP only).
Unfortunately TI does not provide the download anymore, so you need to find
other sources. Hint: search for "FlashStudio1.1.43.exe". Please let me know if
you can't find the file.

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

For generating the C hex array of an image I recommend Image Studio. You can
download it from here:
http://www.ticalc.org/archives/files/fileinfo/307/30736.html

Feel free to send me an email (daniel_eisele@gmx.de) if you have any questions.
