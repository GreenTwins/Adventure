#include "Character.h"
#include <iostream>
#include <random>



void Character::changeName(const std::string& n) {
	Character::name = n;
}
Character::Character() {

}
Character::Character(std::string& s)
	:name(s) {
	}
Character::Character(int s) {

}
void Character::increaseStats(char s, int val) {
	switch (s) {
	case 'h':
		Character::hp += val;
		break;
	case 'm':
		Character::mp += val;
		break;
	case 's':
		Character::str += val;
		break;
	case 'd':
		Character::def += val;
		break;
	case 'p':
		Character::spd += val;
		break;
	case 'o':
		Character::dodge += val;
		break;
	default:
		std::cerr << "Incorrect option: " << s << std::endl;
		break;
	}	
}

void Character::updateATKList( std::string& a, int val) {
	Character::attacks.emplace(a, val);
}
void Character::setHP(int v) {
	Character::hp = v;
	//std::cout << Character::hp<<std::endl;
}
void Character::setMP(int v) {
	Character::mp = v;
	//std::cout << Character::mp << std::endl;
}
void Character::setStr(int v) {
	Character::str = v;
	//std::cout << Character::str << std::endl;
}
void Character::setDef(int v) {
	Character::def = v;
	//std::cout << Character::def << std::endl;
}
void Character::setSpd(int v) {
	Character::spd = v;
	//std::cout << Character::spd << std::endl;
}
void Character::setDodge(int v) {
	Character::dodge = v;
	//std::cout << Character::dodge << std::endl;
}