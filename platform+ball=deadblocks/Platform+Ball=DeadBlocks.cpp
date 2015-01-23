#include <iostream>
#include <vector>
#include <conio.h>
#include <time.h>
#include <string>
#include <fstream>

#include "ConsoleGaming.h"


using namespace std;

HANDLE consoleHandle;
bool escHit = 0;
bool lose = 0;

typedef vector<GameObject>::iterator randomAccess_iterator;
typedef vector<GameObject>::const_iterator const_iterator;

// Window constants
const int WindowWidth = 70;
const int WindowHeight = 30;
// Block variables
const char BlockSymbol = '-';
const char hitBlockSymbol = ' ';
int blockSpeed = 1;

//09.01.2015 02:35 changed ball initial direction
// Ball variables
int ballSpeed = -1;//skorostta po y zadyljitelno e +/-1 kogato nablijava blokovete, za da ne gi preska4a
int ballSpeedX = 1;

// Platform variables
int platformSpeed = 3;
int platformLength = 10;

// Game variables
unsigned long sleepDuration = 50;

GameObject ball(WindowWidth / 2, WindowHeight / 2, '*');
vector<GameObject> blocks;
vector<GameObject> platform;

unsigned int blockSpawnInterval = 10;//taq promenliva nikade ne se polzva; qvno trqbva da q polzwame za ne6to

void Update()
{
	COORD direction = { 0, 0 };
	if (_kbhit())
	{
		char key = _getch();
		switch (key)
		{
		case 'a':
			//direction.X = -ballSpeed;
			direction.X = -1;
			break;
		case 'd':
			//direction.X = ballSpeed;
			direction.X = 1;
			break;
		case 27:
			escHit = 1;
			break;
		};
	}
	//09.01.2015 02:13 ograni4ena e platformata da ne izliza izvan ekrana
	int beginOfPlatform = platform.begin()._Ptr->Coordinates.X, endOfPlatform = beginOfPlatform + platform.size() - 1;
	for (randomAccess_iterator platformBody = platform.begin(); platformBody != platform.end(); ++platformBody)
	{
		if ((beginOfPlatform > 0 || direction.X>0) && (endOfPlatform < 68 || direction.X<0))
		{
			platformBody->Coordinates.X += direction.X*platformSpeed;
			platformBody->Coordinates.Y += direction.Y*platformSpeed;

		}
	}
	ball.Coordinates.Y += ballSpeed;
	ball.Coordinates.X += ballSpeedX;
	if (ball.Coordinates.X <= 0)
	{
		ballSpeedX *= -1;
		cout << '\a';
	}
	else if (ball.Coordinates.X >= 68)
	{
		ballSpeedX *= -1;
		cout << '\a';
	}
	if ((ball.Coordinates.Y >= WindowHeight - 1) &&
		(ball.Coordinates.X < beginOfPlatform || ball.Coordinates.X > endOfPlatform)) lose = 1; //gubi6; sled nego ne trqbwa li da ima i break?
	if (ball.Coordinates.Y >= WindowHeight - 1 || ball.Coordinates.Y <= 0)
	{
		cout << '\a';
		ballSpeed = -ballSpeed;
		if (direction.X > 0) ballSpeedX += 1;
		else if (direction.X < 0) ballSpeedX -= 1;//kogato mardame nalqvo/nadqsno platformata i top4eto da se marda
	}

	//09.01.2015 02:51 dobaveno e otnemane na ucelen blok
	// Loop trough all blocks
	for (randomAccess_iterator block = blocks.begin(); block != blocks.end(); ++block)
	{
		int currX = block._Ptr->Coordinates.X, currY = block._Ptr->Coordinates.Y;
		// Remove any block that is hit by the ball
		// Implement unit collision
		//if (ballSpeedX == 1)
		/*for (int i = 1; i <= 3; i++) tuk trqbwa da se proverqva kogato
		skorostta e >1 dali na nqkoi ot trite reda "po patq" na top4eto ima neucelen blok

		*/{
			if (ball.Coordinates.Y - 1 == currY && ball.Coordinates.X == currX)// && currY >= 0) //bez poslednoto uslovie bugva
			{
				blocks.erase(block);
				cout << '\a';
				ballSpeed = -ballSpeed;
				break;
			}
		}
	}
}

void Draw()
{
	//miganeto na ekrana ako moje da se mahne...?
	ClearScreen(consoleHandle);

	for (const_iterator platformBody = platform.cbegin(); platformBody != platform.cend(); ++platformBody)
	{
		platformBody->Draw(consoleHandle);
	}

	for (const_iterator block = blocks.cbegin(); block != blocks.cend(); ++block)
	{
		block->Draw(consoleHandle);
	}

	ball.Draw(consoleHandle);

}

int main()
{

	PlaySound(TEXT("Miley.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);//da se dobavi i sila na zvuka

	//golemina na ekrana
	system("MODE 71,30");//ne pipai 6iro4inata na ekrana, ako e po-tesen se poqvqva byg
	//menu
	string option = "";

	while (1)
	{
		escHit = 0;
		lose = 0;
		consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		ClearScreen(consoleHandle);
		cout << "Home screen(Esc)" << endl;
		cout << "Select option" << endl << endl;
		cout << "Start(s)" << endl;
		cout << "Instructions(i)" << endl;//not ready
		cout << "High scores(h)" << endl;
		cout << "Change settings(c)" << endl;//not ready
		cout << "Exit(x)" << endl;
		cin >> option;
		if (option == "s")
		{
			consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

			srand(time(NULL));

			int platformY = WindowHeight - 1;
			int platformX = WindowWidth / 2 - platformLength / 2;
			//09.01.2015 02:55 changed platform symbol
			char platformSymbol = '_';

			for (int i = 0; i < platformLength; i++)
			{
				platform.push_back(GameObject(platformX + i, platformY, platformSymbol));
			}


			// Generate blocks
			int blocksPerColumn = 3;
			for (int i = 0; i < WindowWidth - 1; i++)
			{
				for (int j = 0; j < blocksPerColumn; j++)
				{
					blocks.push_back(GameObject(i, j, BlockSymbol));
				}
			}

			while (true)
			{

				if (escHit) break;
				else if (lose)
				{
					consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
					ClearScreen(consoleHandle);
					cout << "LOSER" << endl;
					cout << "Press Esc for home screen" << endl;
					for (int i = 0; i <= 6; i++) cout << '\a';
					//while da ne se maha, kakto i skobite pod nego
					while (_getch() != 27)
					{
					}
					WinExec("platform+Ball=DeadBlocks", SW_SHOW);//ako moje pesenta da ne po4va otna4alo tuk...
					exit(1);

				}
				else
				{
					Update();
					Draw();
					Sleep(sleepDuration);
				}
			}
		}
		else if (option == "h")
		{
			consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			ClearScreen(consoleHandle);
			long score;
			string name;
			ifstream score_file("score.txt");
			if (score_file)   score_file >> name >> score;
			cout << name << " " << score << endl;
			cout << "Press any key for home screen" << endl;
			_getch();
		}
		else if (option == "x")
		{
			return 0;
		}
	}
}