//#include "SQLCONN.h"
#include <iostream>
//#include "Character.h"
//#include "Player.h"
//#include "LOCALSTORAGE.h"
//#include <random>
#include "Game.h"







int main() {
	

	Game& newGame = Game::getinstance();
	MainMenu& mainMenu = MainMenu::getInstance();
	mainMenu.display();

	return 0;
}