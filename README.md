# GoGoGo

## Goals
* Provide a tool to get past the "I suck!" stage of learning Go.
* Undo/Redo moves
* Hints showing strengths of possible moves
* Switch sides
* Network play
* Optional highlighting of board to show good moves, dead and dying stones, etc.
* Generally use every feature of GNU Go but with a more user friendly interface.
* Look nice, optional dark theme
* C++ for performance and because I prefer it.
* Stability 
* Multiplatform, starting with Linux.

## Known Requirements
* Qt 5.6 (untested with older versions )
* GnuGo path currently hard coded to: /usr/local/gnugo/bin/gnugo
* Linux Might compile for other OSs Qt supports too

## Known Limitations 
#### On the off chance someone wants to try it before it is ready for prime time.
* It is in a very pre-alpha state.
* Mouse not implemented yet. GNU Go commands can be typed into the bottom right input widget. Ex. play black b5 and hit enter.
* Only understands the commands "play" and "genmove"
* Genmove will currently autoplay computer vs computer
* The "Resign" button clears the board and starts the GNU Go server.
* The "Pass" button stops the GNU Go server

## Screenshot 12 June 2016
![GoGoGo Screenshot 12 June 2016](/screenshots/GoGoGo_12Jun2016.png)
