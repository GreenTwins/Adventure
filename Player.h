#pragma once
#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "Character.h"


class Player : public Character {
	int numofAtks = 6;
	int lvl{ 1 };
	bool LoadedInfo = false;
	int MAX_HP = 0;
	
public:
	Player();
	Player(const Player& o) = delete;
	Player(std::string& s);
	virtual ~Player() {}
	int getLvl()const;
	void setlvl(int lvl);
	void refillHP();
	//ATK
	void createATK(int num);
	int attack()override;
	virtual void displayStats(bool Eloop) const override;
	void CreateStats();
	bool can_level_up();
	//EXP
	void setMax_HP(int a);
	int getMaxHP()const;
	void increaseHealth(int lvl);
	void increaseStats();
	std::pair<std::string, int>attack1;
	std::pair<std::string, int>attack2;
	std::pair<std::string, int>attack3;
	std::pair<std::string, int>attack4;
	std::pair<std::string, int>attack5;
	std::pair<std::string, int>attack6;
};


#endif