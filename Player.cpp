#include "Player.h"
#include "CONSOLECHANGE.h"
#include <iostream>
#include <random>
#include <iomanip>

Player::Player() {
}
Player::Player(std::string& s) :Character{ s } {
	MAX_HP = 100;
	CreateStats();
	createATK(6);
	displayStats(1);
	setXP(0);
}
int Player::getLvl()const {
	return lvl;
}
void Player::setlvl(int lvl) {
	lvl = lvl;//looks wild huh?
}
void Player::setMax_HP(int maxhp) {
	MAX_HP = maxhp;
}
int Player::getMaxHP()const {
	return MAX_HP;
}
void Player::refillHP() {
	setHP(getMaxHP());
}
void Player::increaseHealth(int lvl) {
	if (lvl == 1) {
		setMax_HP(150);
		setHP(getMaxHP());
	}
	displayStats(true);
}
void Player::increaseStats() {
	srand(time(NULL));
	int strDice = rand() % 6 + 1;
	setStr(getStr() + strDice);
	int defDice = rand() % 6 + 1;
	setDef(getDef() + defDice);
	int spdDice = rand() % 6 + 1;
	setSpd(getSpd() + spdDice);

}
bool Player::can_level_up() {
	if (getXP() > 150) {
		increaseStats();
		increaseHealth(getLvl());
		return true;
	}
	return false;
}
void Player::createATK(int num) {
	std::string atk;
	int val;
	system(CLEAR_SCREEN);
	std::cout << "\n";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	for (int i = 0; i < num; ++i) {
		std::cout << "Attack " << i + 1 << " name: " <<'\n';
		
		std::getline(std::cin, atk);
		val = rand() % 6 + 1;
		
		Player::updateATKList(atk, val);
	}
}
int Player::attack() {
	int roll_Value = rand() % (Player::numofAtks)+1;
	if (roll_Value > 0 && roll_Value <= Character::attacks.size()) {
		auto it = Character::attacks.begin();
		std::advance(it, roll_Value - 1);
		std::cout << Player::getName() << " uses: " << it->first << std::endl;
		return (Player::getStr() + it->second);
	}
	else {
		std::cout << "Access error" << std::endl;
	}
	return 0;
}
void Player::displayStats(bool Eloop)const {
	int i = 1;
	if (Eloop) {
		for (auto it : Player::attacks) {
			std::cout << "Attack " << i<<" : " << it.first << "\t";
			std::cout << " DmG: " << it.second << std::endl;
			++i;
		}
		std::cout << std::endl;
		std::cout << std::endl;
	}
	std::cout << Player::getName()<<" STATS" << std::endl;
	std::cout.width(5);
	std::cout<< "Strength: " << Player::getStr() << std::endl;
	std::cout << "Defense:  " << Player::getDef() << std::endl;
	std::cout << "Speed     " << Player::getSpd()<< std::endl;
	std::cout << "Dodge     " << Player::getDodge() << std::endl;
	std::cout << "Health:   " << Character::getHP() << std::endl;
	std::cout << "Mana:     " << Player::getMP() << std::endl;
	std::cout << "---------------------------------------------" << std::endl;
}
void Player::CreateStats() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int>HealthStats(1, 100);
	std::uniform_int_distribution<int>ATTStats(1, 6);

	for (int i = 0; i < 2; ++i) {
		int diceRoll = HealthStats(gen);
		if (i == 0) {
			Player::setHP(100);
		}
		else {
			Player::setMP(diceRoll);
		}
	}

	for (int i = 0; i < 4; ++i) {
		int diceRoll = ATTStats(gen);
		if (i == 0) {
			Player::setStr(diceRoll);
		}
		else if (i == 1) {
			Player::setDef(diceRoll);
		}
		else if (i == 2) {
			Player::setSpd(diceRoll);
		}
		else {
			Player::setDodge(diceRoll);
		}

	}
}
