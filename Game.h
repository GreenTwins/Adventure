
#include "Player.h"
#include "Enemy.h"
#include "CONSOLECHANGE.h"
#include <map>
#include <list>
#include <vector>

#pragma once
#ifndef _MAP_H_
#define _MAP_H_

/******************************************************************************************************
MAP CLASS

Holds the overview of the concept of "Map", containing various map sizes, map creation, start and end map data, 
character movements, enemy placement


*******************************************************************************************************/
class Map {

public:
	const int SMALL_MAP = 8;
	const int MEDIUM_MAP = 10;
	const int LARGE_MAP = 12;
	int data;
	int beg, end;

	bool isBossPresent = false;
	std::map<int, std::list<int>>mapp;//provides the moves
	//std::vector<Room>pathway;
	std::map<int, bool>pathwayy;//provides enemy location
	/*std::map<int, int>locationData;
	std::vector<std::map<int, int>>locations;*/
	std::map<int, std::list<int>>locationData;

	Map();
	~Map();
	void createPaths(int Location);
	void loadPathway(int n);
	void add(int s, int d);
	int availableMoves(int a);
	void makeMove(int currLocation);
	void loadMapData();
	bool bossBattle(int loc, int dunNum, Player& p1);
	bool DungeonBattle(Player& pl, Enemy en);
};

#endif

/******************************************************************************************************
GAME CLASS

Holds the overview of the concept of "Game", loaded data location (server/local) map locations, holds 
list of enemies based on location given, player interactions (load, fight, create) as well as game playability


*******************************************************************************************************/
#pragma once
#ifndef _GAME_H_
#define _GAME_H_
class Game {
	

	Game(); //
	~Game();
	Game(Game& o) = delete;
	Game& operator=(const Game&) = delete;
	bool onserver = false;
	bool onlocal = false;
public:
	Player playerN;
	std::vector<std::map<int, int>>locations;//first int has mapLocation and 2nd is the num of dungeons at that location. The location also determines the size of the map
	std::vector<Enemy>enemyList;//this gets loaded based on the location
	bool GameInit = false;
	static Game& getinstance();
	void createPlayer(std::string n);
	void loadPlayer();//will need something in the input
	//enter map-> map holds the dungeons and expects a player and a list of enemies as its input
	bool loadGame();
	void loadEnemies(int loc, int dunNum, std::vector<Enemy>&enemyList);
	bool PrePlay();
	bool play(Map& currentMap);
	void getLocationName(int loc);
	bool isSQL()const;
	void fromSQL(bool i);
	void fromLocal(bool i);
	bool isLocal()const;
};


#endif


/******************************************************************************************************
MAIN MENU CLASS

Holds the overview of the concept of "Main Menu", containing all overarching game functions such as 
New game instance, load game instance and save game instance


*******************************************************************************************************/
#pragma once
#ifndef _MAIN_MENU_H_
#define _MAIN_MENU_H_
class MainMenu {
	MainMenu();
	~MainMenu();
	MainMenu& operator=(const MainMenu&) = delete;
	MainMenu(MainMenu& o) = delete;
public:
	static MainMenu& getInstance();
	void Save();
	bool display()const;
};


#endif


#pragma once
#ifndef _GAME_CONSOLE_H_
#define _GAME_CONSOLE_H_

/******************************************************************************************************
GAME CONSOLE CLASS

Holds the overview of the concept of "Game Console", containing all options available to the player such as
travelling, battling, inventory, store, display stats and going back to the main menu. This class also verifies
missions completed and fulfilled requirements to unlock additional locations

*******************************************************************************************************/
class GameConsole {
public:
	GameConsole(Player p );
	~GameConsole();
	void display()const;
	void options();
	int LocationsforTravel()const;
	void EnterStore();
	int EnterMission();
	void checkInventory();
	void backtoMain();
	void showStats()const;
};

#endif
