#pragma once
#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "Character.h"


class Player : public Character {
	int numofAtks = 6;
	int lvl{ 1 };
	bool LoadedInfo = false;
public:
	Player();
	Player(const Player& o) = delete;
	Player(std::string& s);
	virtual ~Player() {}

	//ATK
	void createATK(int num);
	int attack()override;
	virtual void displayStats(bool Eloop) const override;
	virtual void CreateStats()override;
};


#endif