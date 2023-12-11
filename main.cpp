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

INSERT INTO Enemy (SpawnLocation, dungeonNum, EnemyName, HP, MP, Str, Def, Spd, dodge, Skill1, Skill2, Skill3, Skill4, Skill5, Skill6, Skill1Desc, Skill2Desc, Skill3Desc, Skill4Desc, Skill5Desc, Skill6Desc)
VALUES(1, 5, 'Goblin Blackblade', 38, 10, 4,4,3, 5, 2, 5, 2, 6, 2, 2, 'Lurker','Short sword','Sneaky blade','Ebondy blade','Dirty wound','flanking slice');


DUP EX

INSERT INTO TableName (Column1, CustomID, Column3, Column4, Column5)
SELECT Column1, 'NewValue', Column3, Column4, Column5 FROM TableName
WHERE CustomID='OrigValue'

12/6 Possible solution to subsrcipt error-> place all fights in free space mem (dynamic) and clear after each fight. Possible use for smart pointers

12/7 All battle completed and errors seem to have been avoided. Battles brought into dynamic mem. Main issue was with the calling of ran() to get a random enemy from enemyList-> rand() required a "+1" to include the last
number being searched for- this at times would then be +1 more than the given vector holding the enemies since its 0 based. Created a logic statement to validate for size discrepancy. Debugger is truly mans best friend 
Primary walk through working, now need to weave it into gameConsole to trigger the games Main Menu [save data, game store, travel, available dungeons]
*/
/*

SET UP FOR 

Enter Mission-> SQL LOAD (Map#, # of dun, loc #, dun names) -> gives list of missions based on map # and loc #

Inventory-> Linked to player load

Store-> map#, loc#


12/10 can create dynamic maps. Trying to add dynamic creation of displayed missions that then create the map with random names and mix of creatures
   ->Need to load db with more creatures and bosses. Verify parameters are flexible and reachable
   -> do a test run on new char and have lvl and location continuously being updated when needed
*/
int main() {
	
	

	MainMenu& mainMenu = MainMenu::getInstance();

	if (!mainMenu.display()) {
		return -1;//failed
	}
	//////start official game
	Game::getinstance().playerN.displayStats(true);
	GameConsole newGame;
	newGame.display();
	newGame.options();


	/*Game& demo = Game::getinstance();
	std::string name;
	std::cout << "Whats the name of the character?: ";
	std::cin >> name;
	demo.createPlayer(name);

	std::cout << demo.playerN.getName() << std::endl;
	if (demo.PrePlay()) {
		std::cout << "done" << std::endl;
	}
	std::cout << "done but you lost" << std::endl;*/

	return 0;
}