//#include "SQLCONN.h"
#include <iostream>
//#include "Character.h"
//#include "Player.h"
//#include "LOCALSTORAGE.h"
//#include <random>
#include "Game.h"







int main() {
	/*SQLCONN& CM = SQLCONN::createInstance();
	CM.connect();
	std::cout << " ah we're here" << std::endl;
	CM.disconnect();*/

	Game& newGame = Game::getinstance();


	return 0;
}