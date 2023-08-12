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

	Map newMap;

	newMap.loadPathway(8);
	//newMap.add(1, 2);
	//newMap.add(1, 4);
	//newMap.add(2, 3);
	//newMap.add(2, 6);
	//newMap.add(3, 4);
	//newMap.add(3, 6);
	//newMap.add(4, 5);
	//newMap.add(5, 7);
	//newMap.add(5, 8);
	//
	////newMap.makeMove(5);
	//newMap.beg = 1;
	//newMap.end = 8;
	//newMap.makeMove(newMap.beg);

	std::cout << "Reached the end of the map" << std::endl;
	return 0;
}