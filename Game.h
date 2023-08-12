#pragma once
#ifndef _GAME_H_
#define _GAME_H_
#include "Player.h"
#include "Enemy.h"
#include "CONSOLECHANGE.h"
#include <map>
#include <list>
#include <vector>



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
	static Game& getinstance();
	void createPlayer(std::string n);
	void loadPlayer();//will need something in the input
	//enter map-> map holds the dungeons and expects a player and a list of enemies as its input
	void loadGame();
	bool isSQL()const;
	void fromSQL(bool i);
	void fromLocal(bool i);
	bool isLocal()const;
	void play();
};


#endif

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
	void display()const;
};


#endif

#pragma once
#ifndef _MAP_H_
#define _MAP_H_

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

	Map(int numPaths);
	~Map();
	void loadPathway(int n);
	void add(int s, int d);
	int availableMoves(int a);
	void makeMove(int currLocation);
};

#endif

