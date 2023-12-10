#include "Game.h"
#include <iostream>
#include "SQLCONN.h"
#include "LOCALSTORAGE.h"
#include <random>
#include <string>
#include <memory>

/******************************************************************************************************
GAME CLASS init,cleaner, getters and setters

*******************************************************************************************************/

Game::Game() {
	/*MainMenu& master = MainMenu::getInstance();
	master.display();*/
	enemyList.clear();
	locations.clear();
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

void Game::loadEnemies(int loc, int dunNum, std::vector<Enemy>&e) {
	try {
		SQLCONN &enemyGrab=SQLCONN::createInstance();
		enemyGrab.getEnemies(loc, dunNum, e);
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
	newChar = true;
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
	switch (choice) {
	case 1: 
	{
		//check SQL server
		SQLCONN& connection = SQLCONN::createInstance();
		if (connection.connect()) {
			//gather avail chars in dB & display
			if (connection.displayNames()) {
				fromSQL(true);
				int choice;
				std::cout << "\n Which character would you like to load?:  ";
				std::cin >> choice;
				if (choice <= connection.playerList.size()) {
					std::cout << connection.playerList[choice - 1] << std::endl;
					playerN.changeName(connection.playerList[choice - 1]);
					conn_success = true;
				}
				else {
					std::cout << "You didnt choose a given number" << std::endl;
				}
				//let user choose char
				//loadPlayer();
				
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
	
	currentDunLvl = 1;
	currentDunNum = 1;
	while (tryAgain) {
		getLocationName(1);//starting new
		Map newMap;
		newMap.createPaths(currentDunLvl);
		Game::getinstance().loadEnemies(1, 1,Game::enemyList);
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
			Game::getinstance().playerN.refillHP();
		}
	}
	return success;
	}
	

bool Game::play(Map& currentMap) {
	getinstance().currentDunLvl = currentMap.currentDungeonLvl;
	getinstance().currentDunNum = currentMap.currentDungeonNum;
	currentMap.makeMove(1);
	
	if (playerN.getHP() > 0) {
		std::cout << "You've entered the Boss room" << std::endl;
		if (currentMap.bossBattle(1, 1, playerN)) {
			return true;
		}
		return false;
	}
	std::cout << "You've died" << std::endl;
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
		
		if (!SQLCONN::createInstance().loadPlayerData(gameInstance.playerN.getName())) {
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
void EnemyAttacks(Character& p1, Character& en) {
	int enemyatk = en.attack();
	//player attempts to dodge
	int battleroll = rand() % 6 + 1;
	int damageDone = 0;
	if (p1.getDodge() != battleroll) {
		damageDone = (enemyatk - p1.getDef());
		p1.setHP(p1.getHP() - damageDone);
		std::cout << "You were too slow. You've received " << damageDone << " damage." << std::endl;
	}
	else {
		std::cout << "You've dodged the attack" << std::endl;
	}

}

void PlayerAttacks(Character& p1, Character& en) {
	int playeratk = p1.attack();
	//player attempts to dodge
	int battleroll = rand() % 6 + 1;
	int damageDone = 0;
	//enemy attempts to dodge
	battleroll = rand() % 6 + 1;
	if (en.getDodge() != battleroll) {
		damageDone = (playeratk - en.getDef());
		en.setHP(en.getHP() - damageDone);
		std::cout << "You're attack strikes true. " << damageDone << " damage dealt. " << std::endl;
	}
	else {
		std::cout << "You're attack was dodged" << std::endl;
	}
}

void BossAttacks(Character& p1, Boss& boss) {
	//this will later hold special attacks
	int bossatk = boss.attack();
	//player attempts to dodge
	int battleroll = rand() % 6 + 1;
	int damageDone = 0;
	if (p1.getDodge() != battleroll) {
		damageDone = (bossatk - p1.getDef());
		p1.setHP(p1.getHP() - damageDone);
		std::cout << "You were too slow. You've received " << damageDone << " damage." << std::endl;
	}
	else {
		std::cout << "You've dodged the attack" << std::endl;
	}
}
void DisplayRoundData(Character& p1, Character& en, int roundNum) {
	//DISPLAY CURRENT ROUND DATA
	std::cout << "***** ROUND: " << roundNum << "*****" << std::endl;
	std::cout << p1.getName() << " current HP: " << p1.getHP() << std::endl;
	std::cout << en.getName() << " current HP: " << en.getHP() << std::endl;
	std::cout << "********************************" << std::endl;
}
bool Map::bossBattle(int loc,int dunNum, Player& p1) {

	Boss newBoss(loc);
	SQLCONN& bossload = SQLCONN::createInstance();
	if (!bossload.getBoss(loc, dunNum, newBoss)) {
		std::cout << "Failed to load boss" << std::endl;
		return false;
	}
	int currentRound = 1;
	srand(time(NULL));

	std::cout << (p1.getSpd() <= newBoss.getSpd() ? newBoss.getName() + " is faster!" : "Your speed is greater") << std::endl;

	while ((p1.getHP() > 0) && (newBoss.getHP() > 0)) {
		if (p1.getSpd() <= newBoss.getSpd()) {
			// Enemy attacks first
			BossAttacks(p1, newBoss);
			PlayerAttacks(p1, newBoss);
		}
		else {
			// Player attacks first
			PlayerAttacks(p1, newBoss);
			BossAttacks(p1, newBoss);
		}

		DisplayRoundData(p1, newBoss, currentRound);
		currentRound++;
	}
	if (p1.getHP() > 0) {
		std::cout << "You've triumphed over " << newBoss.getName() << std::endl;
		return true;
	}
	std::cout << "You've fallen in battle" << std::endl;
	return false;
}
std::unique_ptr<bool>Map::DungeonBattle(Player& pl, std::unique_ptr<Enemy>& en) {

	int currentRound = 1;
	srand(time(NULL));

	std::cout << (pl.getSpd() <= en->getSpd() ? en->getName() + " is faster!" : "Your speed is greater") << std::endl;

	while ((pl.getHP() > 0) && (en->getHP() > 0)) {
		if (pl.getSpd() <= en->getSpd()) {
			// Enemy attacks first
			EnemyAttacks(pl, *en);
			PlayerAttacks(pl, *en);
		}
		else {
			// Player attacks first
			PlayerAttacks(pl, *en);
			EnemyAttacks(pl, *en);
		}

		DisplayRoundData(pl, *en, currentRound);
		currentRound++;
	}

	if (pl.getHP() > 0) {
		std::cout << "You've triumphed over " << en->getName() << std::endl;
		return std::make_unique<bool>(true);
	}
	std::cout << "You've fallen in battle" << std::endl;
	return std::make_unique<bool>(false);
}
int Map::availableMoves(int a) {
	int movement{ 0 };
	srand(time(NULL));
	if (pathwayy[a]) {//this gets the .second of the map which is whether there is an enemy
		//std::cout << "There is an enemy in the room" << std::endl;
		int listsize = Game::getinstance().enemyList.size();
		int randomEnemy = rand() % listsize+1;
		
		Enemy newEnemy(currentDungeonNum);
		if (randomEnemy >=listsize) {
			newEnemy = Game::getinstance().enemyList[randomEnemy - 1];
		}
		else {
			newEnemy = Game::getinstance().enemyList[randomEnemy];
		}
		
		

		//std::unique_ptr<Player> playerPtr = std::make_unique<Player>(Game::getinstance().playerN);
		std::unique_ptr<Enemy> newEnemyPtr = std::make_unique<Enemy>(newEnemy);
		std::unique_ptr<bool> battleResultPtr = DungeonBattle(Game::getinstance().playerN, newEnemyPtr);

		//std::cout << newEnemy.getName()<<" has appeared for battle" << std::endl;
		
		if (!(*battleResultPtr)) {
			return -1;
		}
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
	if (currLocation == -1) {
		return;
	}
	makeMove(availableMoves(currLocation));

}

void Map::loadPathway(int n) {//a list of which path has an enemy present
	srand(time(NULL));
	int reachThreeCount = 1;
	for (int i = 1; i <= n; ++i) {
		bool hasEnemy = rand() % 2;
		if (reachThreeCount % 3==0) {
			hasEnemy = true;
		}
		pathwayy.insert(std::pair<int, bool>(i, hasEnemy));
		if (!hasEnemy) {
			reachThreeCount++;
		}
		
	}
	/*for (const auto& entry : pathwayy) {

		std::cout << entry.first << ", " << entry.second << std::endl;
	}*/
}

/******************************************************************************************************
GAME CONSOLE CLASS init,cleaner, getters and setters

*******************************************************************************************************/

GameConsole::GameConsole() {

}
GameConsole::~GameConsole() {}

/******************************************************************************************************
GAME CONSOLE deterministic functions: travel, go to store, back to main, display stats, display curr data, 
inventory

*******************************************************************************************************/
void GameConsole::display()const {
	std::cout << "\n Name: " << Game::getinstance().playerN.getName() << "\t";
	std::cout << "Level: " << Game::getinstance().playerN.getLvl() << std::endl;
	std::cout << "Health: " << Game::getinstance().playerN.getHP() << "\t";
	std::cout << "Current Location: " << Game::getinstance().playerN.location<<std::endl;
	std::cout << "XP: " << "\t";

}
void GameConsole::options() {
	int option;
	bool displayOptions = true;

	while (displayOptions) {
		std::cout << "1.) Travel" << std::endl;
		std::cout << "2.) Enter Mission" << std::endl;
		std::cout << "3.) Go to Inventory" << std::endl;
		std::cout << "4.) Go to Store" << std::endl;
		std::cout << "5.) Display stats" << std::endl;
		std::cout << "6.) Save Game" << std::endl;
		std::cout << "7.) Back to Main Menu" << std::endl;
		std::cout << "8.) Quit" << std::endl;

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
			saveState();
			break;
		case 7:
			backtoMain();
			break;
		case 8: 
			return;
		default:
			std::cerr << "Please choose an appropriate option from above" << std::endl;
			break;
		}
	}
}
void GameConsole::backtoMain() {
	//call main menu instance
	MainMenu::getInstance().display();
}

void GameConsole::saveState() {
	int choice = 0;
	std::cout << "How would you like to save your data?: " << std::endl;
	std::cout << " 1.) Save to database " << std::endl;
	std::cout << " 2.) Save locally " << std::endl;
	std::cin >> choice;
	switch (choice) {
	case 1:
		if (!SQLCONN::createInstance().sqlSave()) {
			std::cout << "failed to save data" << std::endl;
		}
		break;
	case 2:
		savedData::getInstance().uploadData(Game::getinstance().playerN);
		break;
	default:
		std::cerr << "Please choose an appropriate option from above" << std::endl;
		break;
	}
}