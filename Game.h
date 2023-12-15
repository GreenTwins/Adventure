
#include "Player.h"
#include "Enemy.h"
#include "CONSOLECHANGE.h"
#include <map>
#include <list>
#include <vector>
#include <memory>
#include <queue>

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
	int currentDungeonLvl = 0;
	int currentDungeonNum = 0;
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
	std::unique_ptr<bool> DungeonBattle(Player& pl, std::unique_ptr<Enemy>& en);

	//SPOILS FROM BATTLE
	int totalGold = 0;
	int totalXP = 0;
	
	//Players Map/Move tracker
	std::map<int, std::list<int>>MoveTracker;
	std::queue<int>q;
	std::map<int, int>visited;
	int currentPlacement, nextPlacement;
	void addEdge(int u, int v);
	bool checkVisited(int n);
	void upDateTracker();
	void displayTracker();
	int inGameInputs();
};

#endif
/******************************************************************************************************
ITEM CLASS

Holds the items for the stores in the game


*******************************************************************************************************/

#pragma once
#ifndef ITEM_H_
#define ITEM_H_

class Item {
	int itemID;
	bool isEquipped;
	std::string itemName;
	int strBuff;
	int defBuff;
	int HPBuff;
	int price;
	int Requirement;
	std::string itemType; //H= helmet, W=weapon, P= Potion, A=Armor, B=Boots
public:
	Item();
	~Item();
	void set_EquipON();
	void set_EquipOff();
	bool getIsEquip()const;
	std::string getItemName()const;
	void setName(std::string n);
	void setItemStats(int str, int def, int hp);
	std::pair<std::string, int> getStats()const;
	void setPrice(int p);
	int getPrice()const;
	void setID(int iD);
	int getitemID()const;
	int getRequirement()const;
	void setRequirement(int req);
	void setType(std::string t);
	std::string getType()const;
	bool operator==(const Item& other)const;
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
	int currentDunNum = 0;
	int currentDunLvl = 0;
	bool newChar = false;
	Player playerN;
	std::vector<Map>maps;
	std::map<int, int>nextPathTracker;
	std::vector<std::map<int, std::string>>AllMissions;
	std::string ActiveMissionName;
	std::vector<Item>StoreWeapons;
	std::vector<Item>StoreWearables;
	std::vector<Item>StorePotions;
	std::map<int, std::string>locationforMissions; //holds dunNum grabbed and name
	std::vector<std::map<int, int>>locations;//first int has mapLocation and 2nd is the num of dungeons at that location. The location also determines the size of the map
	std::vector<Enemy>enemyList;//this gets loaded based on the location
	bool GameInit = false;
	static Game& getinstance();
	void createPlayer(std::string n);
	//enter map-> map holds the dungeons and expects a player and a list of enemies as its input


	bool loadGame();
	bool startMission();
	void loadAllMissions();
	void displayMapsAvailable();
	void loadEnemies(int loc, int dunNum, std::vector<Enemy>&enemyList);
	bool PrePlay();
	bool play(Map& currentMap);
	void getLocationName(int loc);
	bool isSQL()const;
	void fromSQL(bool i);
	void fromLocal(bool i);
	bool isLocal()const;

	//STORE
	void displayStore(int dunLvl);
	void loadStore();
	


	//PLAYERS INVENTORY
	std::vector<Item>Inventory;
	void displayInventory();
	void equipItem(Item& i);
	void unequipItem(Item& i);
	void remove_item(Item i);
	std::map<std::string, bool>activeItems;

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
	GameConsole();
	~GameConsole();
	void display()const;
	void options();
	int LocationsforTravel()const;
	void EnterStore();
	int EnterMission();
	void checkInventory();
	void backtoMain();
	void showStats()const;
	void saveState();
};

#endif
