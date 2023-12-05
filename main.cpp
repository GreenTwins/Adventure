#include "SQLCONN.h"
#include <iostream>
//#include "Character.h"
//#include "Player.h"
//#include "LOCALSTORAGE.h"
//#include <random>
#include "Game.h"

/*
11/20 : Updated the enemy get function//Need to update enempy.cpp for displayStats and createStats() then run to see if names are visible or that the code 
doesnt error out. If no error see if the names looped through the sql sticks -< make a display function strictly for names. Then load full enemy data

12/5: sql db connection back working but sqlQuery seems to pick up and display garbage data WITH proper amount of table data. Once confirmed w/ valid data will load enemies into dungeon and create 
battle function -> issue resolved. Error was in the SQLCHAR variables used to fetch the data, they were too small (50 char) and was extended to 255. 
NOTE: Nullterminating function that was used to add trailing \0 due to compiler warning that varialbes might not be zero terminating causes reading failure
when printing data. Removed. Added enemies to the map-> need to check spawn rate with countThree variable-> other than that its up and running. Now need
to create the boss room and a boss fight sequence


*/
int main() {
	
	

	//Game& newGame = Game::getinstance();
	MainMenu& mainMenu = MainMenu::getInstance();
	//SQLCONN& newConnection = SQLCONN::createInstance();
	if (!mainMenu.display()) {
		return -1;//failed
	}
	//all relevant data loaded/ completed. Can go to console
	std::cout << "We are now inside the full game" << std::endl;
	return 0;
}