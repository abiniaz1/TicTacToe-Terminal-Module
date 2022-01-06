# TicTacToe-Terminal-Module
This is a Linux module that can play TicTacToe on the terminal

Author: Abid Niaz
Latest Release Date: 4/14/2020
The official version is named tictactoe.c and tictactoe.ko
The hello2.ko is already compiled and ready for insertion
This ^^^ version is made for debugging since it,
includes extra debugging messages.
Tictactoe is fully playable although the AI works at random.
Neither version edits the user buffer when using read()/write()
This is due to safety purposes and complete fear of breaking something
Both versions, when inserted, will appear in /dev/TicTacToe.
Using read() will print the game status and the board.
After 9 turns the game will stop since this is the maximum,
number of moves possible in a 3x3 board.
The game board does not operate using a x,y coordinate board.
That would require making a 2D array and that is not fun,
using kernel code.
This game does not support varying board lengths for the above reason.
It also means that win conditions are not coordinate based.
This module has been tested somewhat intensively, it shouldn't
break but in the always likely chance it does please send bug reports to:
abiniaz1@umbc.edu

Board that will print:

[ * | * | * ]
 
0,0|1,0|2,0

[ * | * | * ]
 
0,1|1,1|2,1

[ * | * | * ]
 
0,2|1,2|2,2

THIS GAME PLAYS IN THE TERMINAL

How to play:
1. Insert the module: 'sudo insmod ./tictactoe.ko'
2. Enter one of the following menu options using:
'echo "<menu option here>" > /dev/TicTacToe'
  - '00 X' or '00 O' to start a new game as either X or O respectively
  - '01' to show the current board
  - '02 X Y' to mark a place on the board,
  'X' and 'Y' being the board coordinates respectively
  - '03' to let the computer make a move. If you start as X,
  you must make your move then tell the computer to make a move.
  If you start as O, you must tell the computer to make a move,
  then make your move.
3. If you want to see the current game status,
  enter: 'cat /dev/TicTacToe'
4. To see any changes to the game enter 'dmesg'
5. Remove the module when done: 'sudo rmmod ./tictactoe.ko'

If you would like to use hello2.ko, simply replace 'tictactoe' with 'hello2' in the module steps
If you like, you can only copy tictactoe.c and TictacToe.h and Makefile to anywhere you like and then compile
BE SURE TO CHECK THE CODE AND MAKE SURE YOU HAVE ALL THE HEADER FILES.

Enjoy playing TicTacToe!
