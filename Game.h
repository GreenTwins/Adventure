#pragma once
#ifndef _GAME_H_
#define _GAME_H_
#include "Player.h"
#include "SQLCONN.h"
#include "LOCALSTORAGE.h"
#include "Enemy.h"
#include "CONSOLECHANGE.h"
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
	std::vector<Enemy>enemyList;
	static Game& getinstance();
	void createPlayer(std::string n);
	void loadPlayer();//will need something in the input
	//enter map-> map holds the dungeons and expects a player and a list of enemies as its input
	bool loadGame();
	bool isSQL()const;
	bool isLocal()const;

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