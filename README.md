# GoGoGo

## Building and Running
### Linux
* Run qmake for Qt 5.7. For example: /opt/Qt/5.7/gcc_64/bin/qmake 
* Run make
* ./GoGoGo 
* If you have a HDPI monitor, you may need to run ./gogogo.sh instead to avoid microscopic fonts. If that doesn't work, try fiddling with the options in gogogo.sh.
* If it runs, but you can't play against the computer, make sure GnuGo is installed:
** On the command line type: which gnugo
** If it says something like: /usr/games/gnugo, it is installed.
** Open Edit>Preferences and choose the Engine tab. Enter /usr/games/gnugo into the input labeled Command. Click Ok. Restart GoGoGo.

### Windows (Broken)
If you wish to compile anyway, you can try opening GoGoGo.pro in QtCreator and compiling from there. The last time I tried this, it compiled and ran. However, after I configured GnuGo in Preferences, it crashed on its second communication with GnuGo. I'm not very familiar with Windows, but judging by debugger output, it looks to me like there may be a bug in the guts of the Qt library itself.

I'd appreciate help getting it working.


### Mac 
I have no idea if it works or not. Try the instructions for Linux, or opening GoGoGo.pro in qtcreator.

I'd appreciate help getting it working.

## Goals
* Provide a tool to get past the "I suck!" stage of learning Go.
* Undo/Redo moves (undo done)
* Hints showing strengths of possible moves (done)
* Switch sides
* Network play
* Network chat or video with other player
* Optional highlighting of board to show good moves, dead and dying stones, etc. (hints done, others limited by gnugo)
* Generally use every feature of GNU Go but with a more user friendly interface.
* Look nice, optional dark theme (done for now -- darkish default look)
* C++ for performance and because I prefer it. (done)
* Stability (pretty good but in progress)
* Multiplatform, starting with Linux. (Linux working, Windows compiles but crashes)

## Known Requirements
* Qt 5.7 (untested with older versions )
* GnuGo path currently hard coded to: /usr/local/gnugo/bin/gnugo
* Linux Might compile for other OSs Qt supports too

## Known Limitations 
* It is in a late alpha or early beta state.
* Last I checked, it compiles under Windows 10 but crashes when communicating with GnuGo.
* Autoplay computer against itself broken
* End of game markers are kinda ugly
* No network games
* Load game from file doesn't show move history
* May not be following official Go rules to determine when game ends
* No free placement of handicap stones--uses default placements
* Responses to GnuGo commands typed into command input are no longer reflected in the application. They are printed to the terminal. This is because of low level changes to how GoGoGo communicates with GnuGo.


## Screenshot 2016-07-03
![GoGoGo Screenshot 12 June 2016](/screenshots/GoGoGo_playing.png)

Additional screenshots are on the wiki, or have a look in the screenshots directory.

