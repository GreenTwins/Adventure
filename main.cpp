//#include "SQLCONN.h"
#include <iostream>
//#include "Character.h"
//#include "Player.h"
//#include "LOCALSTORAGE.h"
//#include <random>
#include "Game.h"







int main() {
	

	/*Game& newGame = Game::getinstance();
	MainMenu& mainMenu = MainMenu::getInstance();
	mainMenu.display();*/

	Map newMap(8);
	newMap.makeMove(1);
	

	std::cout << "Reached the end of the map" << std::endl;
	return 0;
}