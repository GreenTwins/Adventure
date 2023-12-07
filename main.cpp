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
battle function -> issue resolved. Error was in the SQLCHAR variables used to fetch the data, they were too small (50 char) and was extended to 255. VARCHAR can hold 255 so this is a reasonable grab
NOTE: Nullterminating function that was used to add trailing \0 due to compiler warning that varialbes might not be zero terminating causes reading failure
when printing data. Removed. Added enemies to the map-> need to check spawn rate with countThree variable-> other than that its up and running. Now need
to create the boss room and a boss fight sequence

12/6 running into random vector subsript errors when traversing map. Possible stack overflow issue- already checked that mapp variable is loaded correctly. Possible issue regarding layout of code
theres a recursive call which is tied to a while loop for a turn by turn battle-> this might be the subscript cause. Look into iterative solutions. This issue isnt consistent.
  -> reformed the turn battle code, excluding having 2 while loops based on speed and narrowed down to 1 loop with 2 function calls (this might also be an additional call for subsript error)

  //copy boss
  INSERT INTO Boss (dungeonLocation, BossName, HP, MP, Str, Def, Spd, dodge, Skill1, Skill2, Skill3, Skill4, Skill5, Skill6, Skill1Desc, Skill2Desc, Skill3Desc, Skill4Desc, Skill5Desc, Skill6Desc)
VALUES(1, 'placeHolder', 50, 30, 5,4,4, 2, 5, 10, 4, 6, 2, 3, 'atk1','atk2','atk3','atk4','atk5','atk6');


12/6 Possible solution to subsrcipt error-> place all fights in free space mem (dynamic) and clear after each fight. Possible use for smart pointers
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