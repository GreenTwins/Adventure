#include "Game.h"
#include <iostream>
#include "SQLCONN.h"
#include "LOCALSTORAGE.h"
#include <random>
#include <string>

/******************************************************************************************************
GAME CLASS init,cleaner, getters and setters

*******************************************************************************************************/

Game::Game() {
	/*MainMenu& master = MainMenu::getInstance();
	master.display();*/

}
Game::~Game() {
	std::cout << "Game instance deleted" << std::endl;
}
void Game::fromSQL(bool i) {
	onserver = i;
}
void Game::fromLocal(bool i) {
	onlocal = i;
}


Game& Game::getinstance() {
	static Game instance;
	return instance;
}


bool Game::isSQL()const {
	return onserver;
}
bool Game::isLocal()const {
	return onlocal;
}
void Game::getLocationName(int loc) {

	switch (loc) {
	case 1:
		std::cout << "You've entered Rom Island" << std::endl;
		break;
	case 2:
		break;
	default:
		std::cerr << "Location is not available" << std::endl;
		break;
	}

}

void Game::loadEnemies(int loc, std::vector<Enemy>&e) {
	try {
		SQLCONN &enemyGrab=SQLCONN::createInstance();
		enemyGrab.getEnemies(loc, e);
		//enemyGrab.disconnect();
		}
	catch (bool result) {
		std::cout << "Database failed to connect" << std::endl;
	}
}
/******************************************************************************************************
GAME CLASS player association

*******************************************************************************************************/
void Game::createPlayer(std::string n) {
	playerN=Player::Player(n);

}
/******************************************************************************************************
GAME CLASS game loading and instances

*******************************************************************************************************/

bool Game::loadGame() {
	int choice{ 0 };
	std::string playername;
	bool conn_success = false;
	std::cout << "Are you loading from a server or local storage?: ";
	std::cout << "1.) Server" << "\t" << "2.) Local Storage" << std::endl;
	std::cin >> choice;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');//clear anything remaining in the stream so we can get playername data
	Player newplayer;
	switch (choice) {
	case 1: 
	{
		//check SQL server
		SQLCONN& connection = SQLCONN::createInstance();
		if (connection.connect()) {
			//gather avail chars in dB & display
			if (connection.displayNames()) {
				fromSQL(true);
				std::cout << "\n Type the name of the character you want to load: ";
				std::getline(std::cin, playername);
				std::cout << "cool" << std::endl;
				//let user choose char
				//loadPlayer();
				conn_success = true;
			}
			else {
				std::cout << "There is no data to load" << std::endl;
			}
			
			connection.disconnect();
		}
		
	
		//load data into Player class
		
		break;
	}
	case 2: {
		//check local storage
		break;
	}
	default: 
		std::cout << "Please choose a correct option" << std::endl;
		break;
	}

	return conn_success;
}


bool Game::PrePlay() {
	bool tryAgain = true;
	bool success = false;
	char option;
	
	while (tryAgain) {
		getLocationName(1);//starting new
		Map newMap;
		newMap.createPaths(1);
		Game::getinstance().loadEnemies(1, Game::enemyList);
		//std::cout << Game::getinstance().enemyList.size();
		if (play(newMap)) {
			//go back to island
			success = true;
			tryAgain = false;
		}
		//got back to main menu or try again
		else {
			std::cout << "You have died. Would you like to try again? Type C to continue or Q to quit: ";
			std::cin >> option;
			if (option == 'Q' || option == 'q') {
				tryAgain = false;
			}
		}
	}
	return success;
	}
	

bool Game::play(Map& currentMap) {
	currentMap.makeMove(1);
	std::cout << "reached the end of the map" << std::endl;
	if (playerN.getHP() > 0) {
		return true;
	}
	return false;
}


//MAIN MENU

/******************************************************************************************************
MAIN MENU CLASS init,cleaner, getters and setters

*******************************************************************************************************/
MainMenu::MainMenu() {}
MainMenu::~MainMenu() {}

MainMenu& MainMenu::getInstance() {
	static MainMenu instance;
	return instance;
}


/******************************************************************************************************
MAIN MENU CLASS only visible function: display() shows the new game, load game and save game options

Each option chosen will determine how the user interacts with the framework

The end result should be to determine if you go to the game console 
*******************************************************************************************************/
bool MainMenu::display()const {
	int option = 0;
	bool GoToConsole = true;
	std::string name;
	std::cout << "---MAIN MENU---" << std::endl;
	std::cout << "\t 1.) NEW GAME" << std::endl;
	std::cout << "\t 2.) LOAD GAME" << std::endl;
	
	
	std::cout << "\n What would you like to do?: ";
	std::cin >> option;
	Game& gameInstance = Game::getinstance();
	switch (option) {

	case 1: {
		std::cout << " What name will you bestow upon your new character?: ";
		std::cin >> name;

		
		gameInstance.createPlayer(name);
		if (!gameInstance.PrePlay()) {
			GoToConsole = false;
		}
	}
		  break;
	case 2: {
		if (!gameInstance.loadGame()) {
			GoToConsole = false;
		}
	}
		  break;
	default:
		std::cout << "Please enter valid number" << std::endl;
		break;
	}

	return GoToConsole;
}
//map()->loadMapData list of all maps with avail bosses and levels// location is chosen and paths are created and filled

/******************************************************************************************************
MAP CLASS init,cleaner, getters and setters

*******************************************************************************************************/
Map::Map() {
	loadMapData();
}
Map::~Map() {}

/******************************************************************************************************
MAP CLASS deterministic functions: creates maps based on certain flags and data inherited from player (lvl,
currentLocation)

*******************************************************************************************************/
void Map::createPaths(int Location) {
	if (Location < 4) {
		add(1, 2);
		add(1, 4);
		add(2, 3);
		add(2, 6);
		add(3, 4);
		add(3, 6);
		add(4, 5);
		add(5, 7);
		add(5, 8);
		loadPathway(SMALL_MAP);
		end = SMALL_MAP;
	}
	else if (Location > 4 && Location < 6) {
		add(1, 2);
		add(1, 3);
		add(2, 4);
		add(2, 6);
		add(3, 8);
		add(4, 5);
		add(5, 6);
		add(5, 7);
		add(6, 8);
		add(7, 10);
		add(8, 9);
		loadPathway(MEDIUM_MAP);
		end = MEDIUM_MAP;
	}
	else {
		add(1, 2);
		add(1, 3);
		add(2, 4);
		add(2, 6);
		add(3, 8);
		add(3, 10);
		add(4, 5);
		add(5, 6);
		add(6, 7);
		add(8, 10);
		add(8, 9);
		add(9, 11);
		add(11, 12);
		loadPathway(LARGE_MAP);
		end = LARGE_MAP;
	}
}
void Map::loadMapData() {
	//locationData.insert(std::pair<int, std::list<int,int,int>(1, 3)); //first island has 3 "dungeons"
	//this loads up all the maps
	locationData[1].push_back(0);
	locationData[1].push_back(0);
	locationData[1].push_back(1);

	locationData[2].push_back(0);
	locationData[2].push_back(1);
	locationData[2].push_back(1);
	
	locationData[3].push_back(0);
	locationData[3].push_back(0);
	locationData[3].push_back(1);

	locationData[4].push_back(0);
	locationData[4].push_back(0);
	locationData[4].push_back(1);

	locationData[5].push_back(0);
	locationData[5].push_back(0);
	locationData[5].push_back(1);

	locationData[6].push_back(0);
	locationData[6].push_back(0);
	locationData[6].push_back(1);

	locationData[7].push_back(0);
	locationData[7].push_back(0);
	locationData[7].push_back(1);

	locationData[8].push_back(0);
	locationData[8].push_back(0);
	locationData[8].push_back(1);

	locationData[9].push_back(0);
	locationData[9].push_back(0);
	locationData[9].push_back(1);
	//locations.push_back(locationData);
}
void Map::add(int s, int d) {
	mapp[s].push_back(d);
	mapp[d].push_back(s);
}
int Map::availableMoves(int a) {
	int movement{ 0 };
	if (pathwayy[a]) {//this gets the .second of the map which is whether there is an enemy
		std::cout << "There is an enemy in the room" << std::endl;
		/*int listsize = Game::getinstance().enemyList.size();
		int randomEnemy = rand() % listsize+1;
		Enemy newEnemy = Game::getinstance().enemyList[randomEnemy];
		std::cout << newEnemy.getName() << std::endl;*/
	}
	std::cout << "The following pathways are available from here: ";
		for (auto neighbor : mapp[a]) {
			std::cout << "[ " << neighbor << " ";
		}
		std::cout << "]";
		std::cout<<"\n Where would you like to go?: ";
		std::cin >> movement;

		return movement;
}
void Map::makeMove(int currLocation) {
	if (currLocation == end) {
		return;
	}
	makeMove(availableMoves(currLocation));

}

void Map::loadPathway(int n) {//a list of which path has an enemy present
	srand(time(NULL));
	for (int i = 1; i <= n; ++i) {
		
		pathwayy.insert(std::pair<int, bool>(i, (rand() % 2)));
	}
	/*for (const auto& entry : pathwayy) {

		std::cout << entry.first << ", " << entry.second << std::endl;
	}*/
}

/******************************************************************************************************
GAME CONSOLE CLASS init,cleaner, getters and setters

*******************************************************************************************************/

GameConsole::GameConsole(Player p) {

}
GameConsole::~GameConsole() {}

/******************************************************************************************************
GAME CONSOLE deterministic functions: travel, go to store, back to main, display stats, display curr data, 
inventory

*******************************************************************************************************/
void GameConsole::display()const {
	std::cout << "Name: " << Game::getinstance().playerN.getName() << "\t";
	std::cout << "Level: " << Game::getinstance().playerN.getLvl() << std::endl;
	std::cout << "Health: " << Game::getinstance().playerN.getHP() << "\t";
	std::cout << "Current Location: " << std::endl;//get location from game
	std::cout << "XP: " << "\t";
	std::cout << "Current Requirement: " << std::endl;


}
void GameConsole::options() {
	int option;
	std::cout << "1.) Travel" << std::endl;
	std::cout << "2.) Enter Mission" << std::endl;
	std::cout << "3.) Go to Inventory" << std::endl;
	std::cout << "4.) Go to Store" << std::endl;
	std::cout << "5.) Display stats" << std::endl;
	std::cout << "6.) Back to Main Menu" << std::endl;

	std::cout << "Which event do you choose to complete?: ";
	std::cin >> option;

	switch (option) {
	case 1:
		//shows available islands
		break;
	case 2:
		//shows islands quests
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		display();
		break;
	case 6:
		backtoMain();
		break;
	default:
		std::cerr << "Please choose an appropriate option from above" << std::endl;
		break;
	}
}
void GameConsole::backtoMain() {
	//call main menu instance
	MainMenu::getInstance();
}