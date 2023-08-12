#include "Game.h"
#include <iostream>
#include "SQLCONN.h"
#include "LOCALSTORAGE.h"
#include <random>


void Game::fromSQL(bool i) {
	onserver = i;
}
void Game::fromLocal(bool i) {
	onlocal = i;
}
Game::~Game() {
	std::cout << "Game instance deleted" << std::endl;
}

Game& Game::getinstance() {
	static Game instance;
	return instance;
}

Game::Game() {
	/*MainMenu& master = MainMenu::getInstance();
	master.display();*/
	
}

void Game::createPlayer(std::string n) {
	playerN=Player::Player(n);

}
void Game::play() {
	 
}
void Game::loadGame() {
	int choice{ 0 };
	std::string playername;
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
}

bool Game::isSQL()const {
	return onserver;
}
bool Game::isLocal()const {
	return onlocal;
}


//MAIN MENU

MainMenu& MainMenu::getInstance() {
	static MainMenu instance;
	return instance;
}

MainMenu::~MainMenu() {}

MainMenu::MainMenu() {}

void MainMenu::display()const {
	int option = 0;
	std::string name;
	std::cout << "---MAIN MENU---" << std::endl;
	std::cout << "\t 1.) NEW GAME" << std::endl;
	std::cout << "\t 2.) LOAD GAME" << std::endl;
	
	
	std::cout << "\n What would you like to do?: ";
	std::cin >> option;
	switch (option) {

	case 1: {
		std::cout << " What name will you bestow upon your new character?: ";
		std::cin >> name;

		Game& gameInstance=Game::getinstance();
		gameInstance.createPlayer(name);
	}
		  break;
	case 2: {
		Game::getinstance().loadGame();
		
	}
		  break;
	default:
		std::cout << "Please enter valid number" << std::endl;
		break;
	}
}

Map::Map(int n) {
	if (n == SMALL_MAP) {
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
	else if (n == MEDIUM_MAP) {
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
Map::~Map() {}
void Map::add(int s, int d) {
	mapp[s].push_back(d);
	mapp[d].push_back(s);
}
int Map::availableMoves(int a) {
	int movement{ 0 };
	if (pathwayy[a]) {//this gets the .second of the map which is whether there is an enemy
		std::cout << "There is an enemy in the room" << std::endl;
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

void Map::loadPathway(int n) {
	srand(time(NULL));
	for (int i = 1; i <= n; ++i) {
		
		pathwayy.insert(std::pair<int, bool>(i, (rand() % 2)));
	}
	for (const auto& entry : pathwayy) {

		std::cout << entry.first << ", " << entry.second << std::endl;
	}
}

