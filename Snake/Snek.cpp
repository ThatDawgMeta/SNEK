/*
					|||||       ||||    ||		|||||||		||  ||
				   |||			|| ||   ||      ||			|| ||
					||||		||  ||  ||		|||||||		||||
					  |||		||   || ||		||			|| ||
				  ||||||		||    ||||		|||||||		||  ||				*By MetaCerberus & MemoryScan
*/


//used to define movingDirection
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3


#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <chrono>
#include <random>
#include <queue>
#include <fstream>
using namespace std;
using namespace std::chrono;

//Global Variables
int posY = 0;
int posX = 0;
int yMax = 0;
int xMax = 0;
int movingDir = RIGHT; //remember character pressed
bool dead = false;
int buffer[120][30]; //Need to fix later to remove magic numbers.
int score = 0;
const int snakeMaxLength = 100;
queue<COORD> snakeBody;
uint16_t speedDelay = 200;
int highScore = 0;


// Go to a coordinate on screen
void gotoxy(int x, int y) {
	COORD pos = { x, y };
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(output, pos);
}

//Hide blinking Cursor
void hideCursor()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(consoleHandle, &cursorInfo);

	cursorInfo.bVisible = false;

	SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

//Display current Score
void scoreUpdate()
{
	gotoxy(0, 1);
	cout << "SCORE: " << score;
}

//Display Snake Length
void snakeStats()
{
	gotoxy(15, 1);
	cout << "Snake Length: " << snakeBody.size();
}

//Moves snake automatically
void moveSnake(int dir) {
	if (dir == UP && movingDir == DOWN)
	{
		return;
	}
	if (dir == LEFT && movingDir == RIGHT)
	{
		return;
	}
	if (dir == RIGHT && movingDir == LEFT)
	{
		return;
	}
	if (dir == DOWN && movingDir == UP)
	{
		return;
	}
	//HERE TO UPODATE TAIL
	gotoxy(posX, posY);
	cout << '+';

	if (dir == UP)
		posY--;
	else if (dir == DOWN)
		posY++;
	else if (dir == LEFT)
		posX--;
	else if (dir == RIGHT)
		posX++;


	// dead check
	if (posY < 3)
	{
		posY = 3;
		dead = true;
	}
	if (posY > yMax - 1)
	{
		posY = yMax - 1;
		dead = true;
	}
	if (posX < 0)
	{
		posX = 0;
		dead = true;
	}
	if (posX > xMax - 1)
	{
		posX = xMax - 1;
		dead = true;
	}
	if (buffer[posX][posY] == 2)
	{
		dead = true;
	}
	if (!dead)
	{
		snakeStats();
		snakeBody.push(COORD{ (short)posX, (short)posY });
		COORD poppedElement = snakeBody.front();
		gotoxy(posX, posY);
		cout << 'O';

		//Check if snake ate food
		if (buffer[posX][posY] == 1)
		{
			buffer[posX][posY] = 0;
			score += snakeBody.size() + 1;
			scoreUpdate();
			if (speedDelay >= 5)
			{
				speedDelay -= 5;
			}
		}
		else
		{
			snakeBody.pop();
		}
		gotoxy(poppedElement.X, poppedElement.Y);
		cout << ' ';
		buffer[poppedElement.X][poppedElement.Y] = 0;

	}
	else
	{
		gotoxy((xMax / 2) - 4, yMax / 2);
		cout << "YOU DED";
		gotoxy(posX, posY);
	}

	movingDir = dir;
	buffer[posX][posY] = 2;
}

int main()
{
	while (true)
	{
		dead = false;
		speedDelay = 200;
		score = 0;
		snakeBody = {};
		hideCursor();


		//Define miliseconds
		uint64_t msMove = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		uint64_t updateMove = msMove + speedDelay;
		uint64_t secGen = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
		uint64_t updateGen = secGen + 1;


		//Getting Console Size
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		yMax = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		xMax = csbi.dwSize.X;
		//cout << "X Size is " << xMax << endl;  //Display X Size
		//cout << "Y Size is " << yMax << endl;  //Display Y Size
		for (int i = 0; i < xMax; i++)
		{
			for (int j = 0; j < yMax; j++)
			{
				buffer[i][j] = 0;
				gotoxy(i, j);
				cout << ' ';
			}
		}
		//Display Top Border and Scoring
		scoreUpdate();
		gotoxy((xMax / 2) - 2, 0);
		cout << "SNEK";
		gotoxy((xMax / 2) - 14, 1);
		cout << "By MetaCerberus & MemoryScan";
		gotoxy(0, 2);
		for (int i = 0; i < xMax; i++)
		{
			cout << '_';
		}
		const string SCORE_SHEET = "scoresheet.txt";
		ifstream scoreSheet;
		scoreSheet.open(SCORE_SHEET);
		if (scoreSheet.good())
		{
			scoreSheet >> highScore;
		}
		gotoxy(90, 1);
		cout << "High Score: " << highScore;
		scoreSheet.close();

		//Set cursor position on screen
		posX = xMax / 2;
		posY = yMax / 2;
		gotoxy(posX, posY);

		//Set initial character on screen
		cout << 'O';
		gotoxy(posX, posY);
		snakeBody.push(COORD{ (short)posX, (short)posY });


		//Allow for non blocking keyboard input
		while (true) {
			//Placing Food
			uint64_t secGen = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

			//Random Number Generator For Food
			random_device FoodGen;
			uniform_int_distribution<> xFoodPos(0, xMax - 1);
			uniform_int_distribution<> yFoodPos(3, yMax - 1);

			//Place Food
			int xFoodPlace = xFoodPos(FoodGen);
			int yFoodPlace = yFoodPos(FoodGen);
			if (secGen > updateGen)
			{
				updateGen = secGen + 6;
				if (buffer[xFoodPlace][yFoodPlace] != 2)
				{
					gotoxy(xFoodPlace, yFoodPlace);
					cout << 'X';
					gotoxy(posX, posY);
					buffer[xFoodPlace][yFoodPlace] = 1;
				}
			}

			msMove = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
			if (msMove > updateMove)
			{
				updateMove = msMove + speedDelay;
				moveSnake(movingDir);
			}
			if (_kbhit()) { // Check if a key has been pressed
				char key = _getch(); // Read the pressed key
				if (key == 27) { // Check if the Esc key (ASCII 27) was pressed
					break; // Exit the loop if the Esc key is pressed
				}
				//Move character up
				else if (key == 119) {
					moveSnake(UP);
				}
				//move character down
				else if (key == 115) {
					moveSnake(DOWN);
				}
				//move character left
				else if (key == 97) {
					moveSnake(LEFT);
				}
				//move character right
				else if (key == 100) {
					moveSnake(RIGHT);
				}
			}
			if (dead)
			{
				break;
			}
		}
		//recording high score
		if (highScore < score)
		{
			highScore = score;
			ofstream scoreSheet;
			scoreSheet.open(SCORE_SHEET);
			scoreSheet << highScore;
			scoreSheet.close();
		}

		gotoxy(xMax / 2 - 22, yMax / 2);
		cout << "    Press Escape To Exit Or 1 To Restart    ";
		while (true)
		{
			if (_kbhit())
			{
				char key = _getch(); // Read the pressed key
				if (key == 27) { // Check if the Esc key (ASCII 27) was pressed
					exit(0); // Exit the game if the Esc key is pressed
				}
				else if (key == 49) // 1 key
				{
					gotoxy(xMax / 2 - 5, yMax / 2 + 1);
					cout << "Restarting";
					break;
				}

			}
		}












	}
}