#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/random.h>
#include "TictacToe.h"

#define DRIVER_AUTHOR "Abid Niaz"
#define DRIVER_DESC "TicTacToe Driver Game"
#define MODULE_NAME "TicTacToe"
#define MAX_LEN 10
#define MIN_CMD 3

//user input position definitions
#define UVAL 0
#define UCMD 1
#define UPOSX 3
#define UPOSY 5

//move definitions
#define MAX_MOVES 9
#define NO_MOVE 0
#define X_MOVE 1
#define O_MOVE 2

//win conditions
#define NO_GAME 0
#define USER_WIN 1
#define COMP_WIN 2
#define NON_WIN 3
#define NEW_GAME 4

//corresponding to each box on the board, same order as POS
#define BOX_1 0
#define BOX_2 1
#define BOX_3 2
#define BOX_4 3
#define BOX_5 4
#define BOX_6 5
#define BOX_7 6
#define BOX_8 7
#define BOX_9 8

//for board printing
#define POS_00 1
#define POS_10 5
#define POS_20 9
#define POS_01 25
#define POS_11 29
#define POS_21 33
#define POS_02 49
#define POS_12 53
#define POS_22 57
#define BOARD_STR " * | * | * \n0,0|1,0|2,0\n * | * | * \n0,1|1,1|2,1\n * | * | * \n0,2|1,2|2,2\n"
#define BOARD_LEN 72
/*


static dev_t device_num; //holds device number
static struct cdev the_cdev; //pretty sure its needed for read/write
static struct class* gLogic = NULL; //for /sys/class
*/

//File operator functions
//reading the dev file, returns the number of bytes read
ssize_t hello2_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	checkForWin();
	printk(KERN_INFO "Game Status:\n");
	if(winCond == USER_WIN)
	{
		printk(KERN_INFO "WIN\n");
		printk(KERN_INFO "You Won! Enter '00 X' or '00 O' to start a new game\n");
	}
	else if(winCond == COMP_WIN)
	{
		printk(KERN_INFO "LOSE\n");
		printk(KERN_INFO "HA HAHAHA HA! YOU FOOL! DID YOU THINK YOU WOULD WIN AGAINST THE MACHINE!? WHAT? TRY AGAIN? HEH, FINE...\n Enter '00 X' or '00 O' to start a new game\n");
	}
	else if(winCond == NON_WIN)
	{
		printk(KERN_INFO "TIE\n");
		printk(KERN_INFO "Tie game. Not bad\n Enter '00 X' or '00 O' to start a new game\n");
	}
	else if(winCond == NEW_GAME)
	{
		printk(KERN_INFO "GAMENOW\n");
		printk(KERN_INFO "The game is currently running\n");
		if(userIsX == true)
		{
			printk(KERN_INFO "You are X\n");
		}
		else
		{
			printk(KERN_INFO "You are O");
		}
	}
	else
	{
		printk(KERN_INFO "NOGAME\n");
		printk(KERN_INFO "No game is ;currently running\nEnter '00 X' or '00 O' to start a new game\n");
	}
	printBoard();
	
	return 0;
}
//writing to dev file, returns the number of bytes written
ssize_t hello2_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	//char* exIn = "hello";
	char userIn [MAX_LEN];
	//int i = 0;
	
	
	printk(KERN_INFO"You are trying to write\n");
	if(length > MAX_LEN)
	{
		printk(KERN_ALERT "ERROR -- INVALID LENGTH -- Write() only supports up to 10 characters\n");
		return -EINVAL;
	}
	if(copy_from_user(userIn, buffer, length) != 0)
	{
		printk(KERN_ALERT "ERROR -- COPY FAIL -- copy_from_user has failed to copy user input\n");
		return -EFAULT;
	}
	
	//First char for validity
	if(userIn[UVAL] != '0')
	{
		printk(KERN_INFO "UNKCMD\n");
		printk(KERN_ALERT "ERROR -- UNKNOWN COMMAND -- Not sure what you're trying to enter,\n valid command format:\n 00 X/O\n01\n02 X Y\n03\n");
		return -EINVAL;
	}
	
	//Second char for command number
	if(userIn[UCMD] == '1') //command to print board
	{
		printBoard();
	}
	if(userIn[UCMD] == '0')//command for new game
	{
		if(length < MIN_CMD)
		{
			printk(KERN_INFO "UNKCMD\n");
		printk(KERN_ALERT "ERROR -- UNKNOWN COMMAND -- Not sure what you're trying to enter,\n valid command format:\n 00 X/O\n01\n02 X Y\n03\n");
		return -EINVAL;
		}
		startTicTacToe(userIn[UPOSX]); //3 since thats the position user choice should be in
	}
	if(userIn[UCMD] == '2')//command for user to make a move
	{
		if(userIn[UPOSX] == '0')
		{
			if(userIn[UPOSY] == '0')
			{
				if(letUserMove(POS_00) < 0)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
			}
			else if(userIn[UPOSY] == '1')
			{
				if(letUserMove(POS_01) < 0)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
			}
			else if(userIn[UPOSY] == '2')
			{
				if(letUserMove(POS_02) < 0)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
			}
			else
			{
					printk(KERN_INFO "OOT\n");
					printk(KERN_ALERT "ERROR -- OUT OF BOUNDS -- Space is not within board bounds\n");
					return -EINVAL;
			}
		}
		else if(userIn[UPOSX] == '1')
		{
			if(userIn[UPOSY] == '0')
			{
				if(letUserMove(POS_10) < 0)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
			}
			else if(userIn[UPOSY] == '1')
			{
				if(letUserMove(POS_11) < 0)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
			}
			else if(userIn[UPOSY] == '2')
			{
				if(letUserMove(POS_12) < 0)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
			}
			else
			{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- OUT OF BOUNDS -- Space is not within board bounds\n");
					return -EINVAL;
			}
		}
		else if(userIn[UPOSX] == '2')
		{
			if(userIn[UPOSY] == '0')
			{
				if(letUserMove(POS_20) < 0)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
			}
			else if(userIn[UPOSY] == '1')
			{
				if(letUserMove(POS_21) < 0)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
			}
			else if(userIn[UPOSY] == '2')
			{
				if(letUserMove(POS_22) < 0)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
			}
			else
			{
					printk(KERN_INFO "OOT\n");
					printk(KERN_ALERT "ERROR -- OUT OF BOUNDS -- Space is not within board bounds\n");
					return -EINVAL;
			}
		}
		else
		{
			printk(KERN_INFO "UNKCMD");
			printk(KERN_ALERT "ERROR -- UNKNOWN COMMAND -- Not sure what you're trying to enter, valid command format: 00 X/O\n01\n02 X Y\n03\n");
		}
	}
	if(userIn[UCMD] == '3')//command to let computer make a move
	{
		printk(KERN_INFO "Computer is making a move\n");
		if(letCompMove() < 0)
		{
			return -EINVAL;
		}	
	}
	return length;
}

//opens the file, returns 0 when there is no error
int hello2_open(struct inode *pinode, struct file *pfile)
{
	printk(KERN_INFO "A Game Has Been Accessed\n");
	return 0;
}

//lets the file leave, return 0 for no error
int hello2_release(struct inode *pinode, struct file *pfile)
{
	printk(KERN_INFO "You have been released\n");
	return 0;
}

static struct file_operations fops = //pointers for file operators
{
	.owner = THIS_MODULE, //Necessary for file operations
	.read = hello2_read, //sets read function
	.write = hello2_write, //sets write function
	.open = hello2_open, //sets open function
	.release = hello2_release, //sets release function
	
};
//Hope that this misc device works
 static struct miscdevice hello2_misc = 
 {
 	.minor = MISC_DYNAMIC_MINOR,
 	.name = MODULE_NAME,
 	.fops = &fops,
 	.mode = 0666, //full read write permissions, not executable
 };


//TicTacToe Functions
//starts the game, resets all needed variables
int startTicTacToe(const char userChoice)
{
	//resetting conditions
	int i = 0;
	winCond = NEW_GAME; 
	totalMoves = NO_MOVE; //default as zero moves, create new game resets this to zero
	
	
	if(userChoice == 'X' || userChoice == 'x')
	{
		userTurn = true;
		userIsX = true;
	}
	else if(userChoice == 'O' || userChoice == 'o')
	{
		userTurn = false;
		userIsX = false;
	}
	else
	{
		printk(KERN_ALERT "ERROR -- PLEASE CHOOSE 'X' OR 'O'\n");
		return -EINVAL;
	}
	
	while(i != MAX_MOVES)
	{
		userMoves[i] = NO_MOVE;
		compMoves[i] = NO_MOVE;
		i++;
	}
	printk(KERN_INFO "OK\n");
	printk(KERN_INFO "NEW GAME!\n");
	printBoard();
	
	return 0;
}

//check for win condition and only win condition
int checkForWin(void)
{
	printk(KERN_INFO "TOTAL MOVES: %d\n", totalMoves);
	if(userIsX == true) //checking all 7 possible user win matches, as X, Comp as O
	{
		if(userMoves[BOX_1] == X_MOVE && userMoves[BOX_2] == X_MOVE && userMoves[BOX_3] == X_MOVE)
		{
			//checking top row
			winCond = USER_WIN;
			userTurn = false;
			return winCond;
		}
		if(userMoves[BOX_1] == X_MOVE && userMoves[BOX_4] == X_MOVE && userMoves[BOX_7] == X_MOVE)
		{
			//checking left down row
			winCond = USER_WIN;
			userTurn = false;
			return winCond;
		}
		if(userMoves[BOX_1] == X_MOVE && userMoves[BOX_5] == X_MOVE && userMoves[BOX_9] == X_MOVE)
		{
			//checking left to right diagonal row
			winCond = USER_WIN;
			userTurn = false;
			return winCond;
		}
		if(userMoves[BOX_2] == X_MOVE && userMoves[BOX_5] == X_MOVE && userMoves[BOX_8] == X_MOVE)
		{
			//checking middle down row
			winCond = USER_WIN;
			userTurn = false;
			return winCond;
		}
		if(userMoves[BOX_3] == X_MOVE && userMoves[BOX_6] == X_MOVE && userMoves[BOX_9] == X_MOVE)
		{
			//checking right down row
			winCond = USER_WIN;
			userTurn = false;
			return winCond;
		}
		if(userMoves[BOX_3] == X_MOVE && userMoves[BOX_5] == X_MOVE && userMoves[BOX_7] == X_MOVE)
		{
			//checking right to left diagonal row
			winCond = USER_WIN;
			userTurn = false;
			return winCond;
		}
		if(userMoves[BOX_4] == X_MOVE && userMoves[BOX_5] == X_MOVE && userMoves[BOX_6] == X_MOVE)
		{
			//checking middle row
			winCond = USER_WIN;
			userTurn = false;
			return winCond;
		}
		if(userMoves[BOX_7] == X_MOVE && userMoves[BOX_8] == X_MOVE && userMoves[BOX_9] == X_MOVE)
		{
			//checking bottom row
			winCond = USER_WIN;
			userTurn = false;
			return winCond;
		}
		
		//checking computer
		if(compMoves[BOX_1] == O_MOVE && compMoves[BOX_2] == O_MOVE && compMoves[BOX_3] == O_MOVE)
		{
			//checking top row
			winCond = COMP_WIN;
			userTurn = false;
			return winCond;
		}
		if(compMoves[BOX_1] == O_MOVE && compMoves[BOX_4] == O_MOVE && compMoves[BOX_7] == O_MOVE)
		{
			//checking left down row
			winCond = COMP_WIN;
			userTurn = false;
			return winCond;
		}
		if(compMoves[BOX_1] == O_MOVE && compMoves[BOX_5] == O_MOVE && compMoves[BOX_9] == O_MOVE)
		{
			//checking left to right diagonal row
			winCond = COMP_WIN;
			userTurn = false;
			return winCond;
		}
		if(compMoves[BOX_2] == O_MOVE && compMoves[BOX_5] == O_MOVE && compMoves[BOX_8] == O_MOVE)
		{
			//checking middle down row
			winCond = COMP_WIN;
			userTurn = false;
			return winCond;
		}
		if(compMoves[BOX_3] == O_MOVE && compMoves[BOX_6] == O_MOVE && compMoves[BOX_9] == O_MOVE)
		{
			//checking right down row
			winCond = COMP_WIN;
			userTurn = false;
			return winCond;
		}
		if(compMoves[BOX_3] == O_MOVE && compMoves[BOX_5] == O_MOVE && compMoves[BOX_7] == O_MOVE)
		{
			//checking right to left diagonal row
			winCond = COMP_WIN;
			userTurn = false;
			return winCond;
		}
		if(compMoves[BOX_4] == O_MOVE && compMoves[BOX_5] == O_MOVE && compMoves[BOX_6] == O_MOVE)
		{
			//checking middle row
			winCond = COMP_WIN;
			userTurn = false;
			return winCond;
		}
		if(compMoves[BOX_7] == O_MOVE && compMoves[BOX_8] == O_MOVE && compMoves[BOX_9] == O_MOVE)
		{
			//checking bottom row
			winCond = COMP_WIN;
			userTurn = false;
			return winCond;
		}
	}
	else //user is not an X
	{
		if(userMoves[BOX_1] == O_MOVE && userMoves[BOX_2] == O_MOVE && userMoves[BOX_3] == O_MOVE)
		{
			//checking top row
			winCond = USER_WIN;
			userTurn = false;
			return winCond;
		}
		if(userMoves[BOX_1] == O_MOVE && userMoves[BOX_4] == O_MOVE && userMoves[BOX_7] == O_MOVE)
		{
			//checking left down row
			winCond = USER_WIN;
			userTurn = false;
			return winCond;
		}
		if(userMoves[BOX_1] == O_MOVE && userMoves[BOX_5] == O_MOVE && userMoves[BOX_9] == O_MOVE)
		{
			//checking left to right diagonal row
			winCond = USER_WIN;
			userTurn = false;
			return winCond;
		}
		if(userMoves[BOX_2] == O_MOVE && userMoves[BOX_5] == O_MOVE && userMoves[BOX_8] == O_MOVE)
		{
			//checking middle down row
			winCond = USER_WIN;
			userTurn = false;
			return winCond;
		}
		if(userMoves[BOX_3] == O_MOVE && userMoves[BOX_6] == O_MOVE && userMoves[BOX_9] == O_MOVE)
		{
			//checking right down row
			winCond = USER_WIN;
			userTurn = false;
			return winCond;
		}
		if(userMoves[BOX_3] == O_MOVE && userMoves[BOX_5] == O_MOVE && userMoves[BOX_7] == O_MOVE)
		{
			//checking right to left diagonal row
			winCond = USER_WIN;
			userTurn = false;
			return winCond;
		}
		if(userMoves[BOX_4] == O_MOVE && userMoves[BOX_5] == O_MOVE && userMoves[BOX_6] == O_MOVE)
		{
			//checking middle row
			winCond = USER_WIN;
			userTurn = false;
			return winCond;
		}
		if(userMoves[BOX_7] == O_MOVE && userMoves[BOX_8] == O_MOVE && userMoves[BOX_9] == O_MOVE)
		{
			//checking bottom row
			winCond = USER_WIN;
			userTurn = false;
			return winCond;
		}
		
		//checking computer
		if(compMoves[BOX_1] == X_MOVE && compMoves[BOX_2] == X_MOVE && compMoves[BOX_3] == X_MOVE)
		{
			//checking top row
			winCond = COMP_WIN;
			userTurn = false;
			return winCond;
		}
		if(compMoves[BOX_1] == X_MOVE && compMoves[BOX_4] == X_MOVE && compMoves[BOX_7] == X_MOVE)
		{
			//checking left down row
			winCond = COMP_WIN;
			userTurn = false;
			return winCond;
		}
		if(compMoves[BOX_1] == X_MOVE && compMoves[BOX_5] == X_MOVE && compMoves[BOX_9] == X_MOVE)
		{
			//checking left to right diagonal row
			winCond = COMP_WIN;
			userTurn = false;
			return winCond;
		}
		if(compMoves[BOX_2] == X_MOVE && compMoves[BOX_5] == X_MOVE && compMoves[BOX_8] == X_MOVE)
		{
			//checking middle down row
			winCond = COMP_WIN;
			userTurn = false;
			return winCond;
		}
		if(compMoves[BOX_3] == X_MOVE && compMoves[BOX_6] == X_MOVE && compMoves[BOX_9] == X_MOVE)
		{
			//checking right down row
			winCond = COMP_WIN;
			userTurn = false;
			return winCond;
		}
		if(compMoves[BOX_3] == X_MOVE && compMoves[BOX_5] == X_MOVE && compMoves[BOX_7] == X_MOVE)
		{
			//checking right to left diagonal row
			winCond = COMP_WIN;
			userTurn = false;
			return winCond;
		}
		if(compMoves[BOX_4] == X_MOVE && compMoves[BOX_5] == X_MOVE && compMoves[BOX_6] == X_MOVE)
		{
			//checking middle row
			winCond = COMP_WIN;
			userTurn = false;
			return winCond;
		}
		if(compMoves[BOX_7] == X_MOVE && compMoves[BOX_8] == X_MOVE && compMoves[BOX_9] == X_MOVE)
		{
			//checking bottom row
			winCond = COMP_WIN;
			userTurn = false;
			return winCond;
		}
	}
	if(totalMoves == MAX_MOVES) //at this point, tie game
	{
		winCond = NON_WIN;
		userTurn = false;
		return winCond;
	}
	return winCond;
}


//Prints the board
int printBoard()
{
	int i = 0;
	char temp_Board [BOARD_LEN];
	strncpy(temp_Board, BOARD_STR, BOARD_LEN);
	checkForWin();
	if(winCond == NO_GAME)
	{
		printk(KERN_INFO "%s", temp_Board);
		return 0;
	}
	if(userIsX == true)
	{
		while(i != BOARD_LEN) //changing - to X/O as appropriate
		{
			if(i == POS_00 && userMoves[BOX_1] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			if(i == POS_00 && compMoves[BOX_1] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			
			if(i == POS_10 && userMoves[BOX_2] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			if(i == POS_10 && compMoves[BOX_2] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			
			if(i == POS_20 && userMoves[BOX_3] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			if(i == POS_20 && compMoves[BOX_3] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			
			if(i == POS_01 && userMoves[BOX_4] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			if(i == POS_01 && compMoves[BOX_4] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			
			if(i == POS_11 && userMoves[BOX_5] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			if(i == POS_11 && compMoves[BOX_5] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			
			if(i == POS_21 && userMoves[BOX_6] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			if(i == POS_21 && compMoves[BOX_6] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			
			if(i == POS_02 && userMoves[BOX_7] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			if(i == POS_02 && compMoves[BOX_7] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			
			if(i == POS_12 && userMoves[BOX_8] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			if(i == POS_12 && compMoves[BOX_8] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			
			if(i == POS_22 && userMoves[BOX_9] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			if(i == POS_22 && compMoves[BOX_9] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			i++;
		}
	}
	else
	{
		while(i != BOARD_LEN) //changing - to X/O as appropriate
		{
			if(i == POS_00 && userMoves[0] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			if(i == POS_00 && compMoves[0] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			
			if(i == POS_10 && userMoves[1] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			if(i == POS_10 && compMoves[1] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			
			if(i == POS_20 && userMoves[2] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			if(i == POS_20 && compMoves[2] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			
			if(i == POS_01 && userMoves[3] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			if(i == POS_01 && compMoves[3] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			
			if(i == POS_11 && userMoves[4] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			if(i == POS_11 && compMoves[4] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			
			if(i == POS_21 && userMoves[5] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			if(i == POS_21 && compMoves[5] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			
			if(i == POS_02 && userMoves[6] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			if(i == POS_02 && compMoves[6] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			
			if(i == POS_12 && userMoves[7] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			if(i == POS_12 && compMoves[7] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			
			if(i == POS_22 && userMoves[8] == O_MOVE)
			{
				temp_Board[i] = 'O';
			}
			if(i == POS_22 && compMoves[8] == X_MOVE)
			{
				temp_Board[i] = 'X';
			}
			i++;
		}	
	}
	printk(KERN_INFO "%s", temp_Board);
	return 0;
}

//Tries to let the user make a move at a desired position
int letUserMove(const int pos)
{
	//int i = 0;
	checkForWin();
	if(winCond == USER_WIN)
	{
		printk(KERN_INFO "WIN\n");
		printk(KERN_INFO "You Won! Enter '00 X' or '00 O' to start a new game\n");
		return USER_WIN;
	}
	else if(winCond == COMP_WIN)
	{
		printk(KERN_INFO "LOSE\n");
		printk(KERN_INFO "HA HAHAHA HA! YOU FOOL! DID YOU THINK YOU WOULD WIN AGAINST THE MACHINE!? WHAT? TRY AGAIN? HEH, FINE| '00 X' or '00 O' to start a new game\n");
		return COMP_WIN;
	}
	else if(winCond == NON_WIN)
	{
		printk(KERN_INFO "TIE\n");
		printk(KERN_INFO "Tie game. Not bad\n Enter '00 X' or '00 O' to start a new game\n");
		return NON_WIN;
	}
	else //now to actually make a move
	{
		if(userTurn == false)
		{
			printk(KERN_INFO "OOT");
			printk(KERN_ALERT "ERROR -- OUT OF TURN -- You are attempting to move out of turn, enter '03' to let the computer take its turn\n");
			return 0;
		}
		if(pos == POS_00)
		{
			if(userMoves[BOX_1] == NO_MOVE)
			{
				if(compMoves[BOX_1] != NO_MOVE)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
				else //empty box
				{
					if(userIsX == true)
					{
						userMoves[BOX_1] = X_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
					else
					{
						userMoves[BOX_1] = O_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
				}
			}
			else
			{
				printk(KERN_INFO "ILLMOVE\n");
				printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
				return -EINVAL;
			}
		}
		if(pos == POS_10)
		{
			if(userMoves[BOX_2] == NO_MOVE)
			{
				if(compMoves[BOX_2] != NO_MOVE)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
				else //empty box
				{
					if(userIsX == true)
					{
						userMoves[BOX_2] = X_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
					else
					{
						userMoves[BOX_2] = O_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
				}
			}
			else
			{
				printk(KERN_INFO "ILLMOVE\n");
				printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
				return -EINVAL;
			}
		}
		if(pos == POS_20)
		{
			if(userMoves[BOX_3] == NO_MOVE)
			{
				if(compMoves[BOX_3] != NO_MOVE)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
				else //empty box
				{
					if(userIsX == true)
					{
						userMoves[BOX_3] = X_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
					else
					{
						userMoves[BOX_3] = O_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
				}
			}
			else
			{
				printk(KERN_INFO "ILLMOVE\n");
				printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
				return -EINVAL;
			}
		}
		if(pos == POS_01)
		{
			if(userMoves[BOX_4] == NO_MOVE)
			{
				if(compMoves[BOX_4] != NO_MOVE)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
				else //empty box
				{
					if(userIsX == true)
					{
						userMoves[BOX_4] = X_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
					else
					{
						userMoves[BOX_4] = O_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
				}
			}
			else
			{
				printk(KERN_INFO "ILLMOVE\n");
				printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
				return -EINVAL;
			}
		}
		if(pos == POS_11)
		{
			if(userMoves[BOX_5] == NO_MOVE)
			{
				if(compMoves[BOX_5] != NO_MOVE)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
				else //empty box
				{
					if(userIsX == true)
					{
						userMoves[BOX_5] = X_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
					else
					{
						userMoves[BOX_5] = O_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
				}
			}
			else
			{
				printk(KERN_INFO "ILLMOVE\n");
				printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
				return -EINVAL;
			}
		}
		if(pos == POS_21)
		{
			if(userMoves[BOX_6] == NO_MOVE)
			{
				if(compMoves[BOX_6] != NO_MOVE)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
				else //empty box
				{
					if(userIsX == true)
					{
						userMoves[BOX_6] = X_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
					else
					{
						userMoves[BOX_6] = O_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
				}
			}
			else
			{
				printk(KERN_INFO "ILLMOVE\n");
				printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
				return -EINVAL;
			}
		}
		if(pos == POS_02)
		{
			if(userMoves[BOX_7] == NO_MOVE)
			{
				if(compMoves[BOX_7] != NO_MOVE)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
				else //empty box
				{
					if(userIsX == true)
					{
						userMoves[BOX_7] = X_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
					else
					{
						userMoves[BOX_7] = O_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
				}
			}
			else
			{
				printk(KERN_INFO "ILLMOVE\n");
				printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
				return -EINVAL;
			}
		}
		if(pos == POS_12)
		{
			if(userMoves[BOX_8] == NO_MOVE)
			{
				if(compMoves[BOX_8] != NO_MOVE)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
				else //empty box
				{
					if(userIsX == true)
					{
						userMoves[BOX_8] = X_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
					else
					{
						userMoves[BOX_8] = O_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
				}
			}
			else
			{
				printk(KERN_INFO "ILLMOVE\n");
				printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
				return -EINVAL;
			}
		}
		if(pos == POS_22)
		{
			if(userMoves[BOX_9] == NO_MOVE)
			{
				if(compMoves[BOX_9] != NO_MOVE)
				{
					printk(KERN_INFO "ILLMOVE\n");
					printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
					return -EINVAL;
				}
				else //empty box
				{
					if(userIsX == true)
					{
						userMoves[BOX_9] = X_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
					else
					{
						userMoves[BOX_9] = O_MOVE;
						userTurn = false;
						totalMoves++;
						return 0;
					}
				}
			}
			else
			{
				printk(KERN_INFO "ILLMOVE\n");
				printk(KERN_ALERT "ERROR -- ILLEGAL MOVE -- Space has already been taken\n");
				return -EINVAL;
			}
		}
		return 0;
	}
}

//the computer now makes its move
int letCompMove(void)
{
	int i = 0;
	int p = 0;
	int goodPos; //desired box to win
	checkForWin();
	if(winCond == USER_WIN)
	{
		printk(KERN_INFO "WIN\n");
		printk(KERN_INFO "You Won! Enter '00 X' or '00 O' to start a new game\n");
		return USER_WIN;
	}
	else if(winCond == COMP_WIN)
	{
		printk(KERN_INFO "LOSE\n");
		printk(KERN_INFO "HA HAHAHA HA! YOU FOOL! DID YOU THINK YOU WOULD WIN AGAINST THE MACHINE!? WHAT? TRY AGAIN? HEH, FINE| '00 X' or '00 O' to start a new game\n");
		return COMP_WIN;
	}
	else if(winCond == NON_WIN)
	{
		printk(KERN_INFO "TIE\n");
		printk(KERN_INFO "Tie game. Not bad\n Enter '00 X' or '00 O' to start a new game\n");
		return NON_WIN;
	}
	else //now to actually make a move
	{
		if(userTurn == true)
		{
			printk(KERN_INFO "OOT");
			printk(KERN_ALERT "ERROR -- OUT OF TURN -- You are attempting to make the computer move out of turn, to make your turn use:\n 02 X Y\n 'X' and 'Y' are the respective coordinates\n");
			return 0;
		}
		if(userIsX == true) //check for user x moves and comp o moves
		{
			while( p != (2*MAX_MOVES)) //only checks 18 times, anymore is not worth it
			{
				get_random_bytes(&i, sizeof(i));
				goodPos = i % MAX_MOVES;
				if(goodPos < 0)
				{
					goodPos = goodPos * (-1);
				}
				if( userMoves[goodPos] != X_MOVE && compMoves[goodPos] != O_MOVE)
				{
					//printk(KERN_INFO "%d\n", goodPos);
					compMoves[goodPos] = O_MOVE;
					userTurn = true;
					totalMoves++;
					return 0;
				}
				p++;
			}
			i = 0;
			while(i != MAX_MOVES) //if our luck is really bad and we have to run through the list manually
			{
					if(userMoves[i] != X_MOVE && compMoves[i] != O_MOVE)
					{
						//printk(KERN_INFO "COMP MOVE: %d\n", i);
						compMoves[i] = O_MOVE;
						userTurn = true;
						totalMoves++;
						return 0;
					}
					i++;
			}
			//at this point check for win condition, throw an error
			checkForWin();
			return -EINVAL;
		}
		else //check for user o moves and comp x moves
		{
			while( p != (2*MAX_MOVES)) //only checks 18 times, anymore is not worth it
			{
				get_random_bytes(&i, sizeof(i));
				goodPos = i % MAX_MOVES;
				if(goodPos < 0)
				{
					goodPos = goodPos * (-1);
				}
				if( userMoves[goodPos] != O_MOVE && compMoves[goodPos] != X_MOVE)
				{
					//printk(KERN_INFO "COMP MOVE: %d\n", i);
					compMoves[goodPos] = X_MOVE;
					userTurn = true;
					totalMoves++;
					return 0;
				}
				p++;
			}
			i = 0;
			while(i != MAX_MOVES) //if our luck is really bad and we have to run through the list manually
			{
					if(userMoves[i] != O_MOVE && compMoves[i] != X_MOVE)
					{
						printk(KERN_INFO "COMP MOVE: %d\n", i);
						compMoves[i] = X_MOVE;
						userTurn = true;
						totalMoves++;
						return 0;
					}
					i++;
			}
			//at this point check for win condition, throw an error
			checkForWin();
			return -EINVAL;
		}
	}
}



int err; //Holds error number in case of anything

static int __init hello_init(void)
{

	printk(KERN_INFO "Welcome, to TicTacToe\n"); //Welcome info
	/*
	err = alloc_chrdev_region(&device_num, 0, 1, "hello2_proc"); //number is in /proc/device
	if ( err < 0) //gets a major number for the device and makes a proc file
	{
		printk(KERN_ALERT "Device Number Allocation Failed");
		return -1;
	}
	
	gLogic = class_create(THIS_MODULE, "hello2_sys");
	if(gLogic == NULL)
	{
		printk(KERN_ALERT "Device Class Creation Failed");
		unregister_chrdev_region(device_num, 1);
		return -1;
	}
	//Now we can make a device file
	if( device_create(gLogic, NULL, device_num, NULL, "tictactoe") == NULL) //makes a /dev/'file' using the class and device number, with name 'whatever filename you want'
	{
		printk(KERN_ALERT "Device Creation Failed");
		class_destroy(gLogic);
		unregister_chrdev_region(device_num, 1);
		return -1;
	}
	 //Setup is complete, now to actually make the character device
	 cdev_init(&the_cdev, &fops);
	if (cdev_add(&the_cdev, device_num, 1) < 0)
	{
		printk(KERN_ALERT "Character Device Creation Failed");
		device_destroy(gLogic, device_num); //destroys the device from /dev/'file'
		class_destroy(gLogic); //destroys the class form /sys/class
		unregister_chrdev_region(device_num, 1); //unregisters the device from /proc/device
		return -1;
	}
	*/
	err = misc_register(&hello2_misc);
	if (err < 0)
	{
		printk(KERN_ALERT "Miscellaneous Device Creation Failed");
		return -1;
	}
	
	return 0;
}

static void __exit hello_exit(void)
{
	/*
	device_destroy(gLogic, device_num); //destroys the device from /dev/'file'
	class_destroy(gLogic); //destroys the class form /sys/class
	unregister_chrdev_region(device_num, 1);
	*/
	misc_deregister(&hello2_misc);
	printk(KERN_INFO "Thank you so much for playing my game!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
MODULE_SUPPORTED_DEVICE("TicTacToe");


