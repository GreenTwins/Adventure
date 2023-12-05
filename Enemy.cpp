#include "Enemy.h"
#include <iostream>

Enemy::Enemy() {}
Enemy::Enemy(int lvl) :Character{lvl} {
	if (lvl <= 3) {
		numofATK = 3;
	}
	else if (lvl > 3 && lvl <= 6) {
		numofATK = 6;
	}
	else {
		numofATK = 9;
	}
}
	//based on lvl that shows how many atks they have avail
	//lvl 1-3 (1) 4-6 (2) lvl 7-9 (3)
int Enemy::attack() {
	int roll_Value = rand() % (numofATK)+1;
	if (roll_Value > 0 && roll_Value <= numofATK) {
		auto it = Character::attacks.begin();
		std::advance(it, roll_Value - 1);
		return (Enemy::getStr() + it->second);
	}
	else {
		std::cout << "Access error" << std::endl;
	}
	return 0;
}
void Enemy::displayStats(bool Eloop)const {
	std::cout <<"ENEMY: "<< getName() << " STATS" << std::endl;
	std::cout.width(5);
	std::cout << "Strength: " << getStr() << std::endl;
	std::cout << "Defense:  " << getDef() << std::endl;
	std::cout << "Speed     " << getSpd() << std::endl;
	std::cout << "Dodge     " << getDodge() << std::endl;
	std::cout << "Health:   " << getHP() << std::endl;
	std::cout << "Mana:     " << getMP() << std::endl;
	std::cout << "---------------------------------------------" << std::endl;
}