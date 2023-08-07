#include "Game.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>


Game::~Game() {
	std::cout << "Game instance deleted" << std::endl;
}

Game& Game::getinstance() {
	static Game instance;
	return instance;
}

Game::Game() {
	//thread 1->load up main
	//thread 2-> check folder location for data, if none: (a)set isSQL to true temp (b) 
	MainMenu& master=MainMenu::getInstance();
}

void Game::mainMenu() {
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
		createPlayer(name);
	}
		  break;
	case 2: {
		std::cout << "coming soon" << std::endl;
	}
		  break;
	default:
		std::cout << "Please enter valid number" << std::endl;
		break;
	}
}

void Game::createPlayer(std::string n) {
	playerN=Player::Player(n);

}
bool Game::loadGame() {
	return 0;
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
	if ((Game::getinstance().isSQL())|| (Game::getinstance().isLocal())) {
		std::cout << "\t 2.) LOAD GAME" << std::endl;
	}
	
	std::cout << "\n What would you like to do?: ";
	std::cin >> option;
}