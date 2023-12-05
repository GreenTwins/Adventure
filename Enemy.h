#pragma once
#ifndef _ENEMY_H_
#define _ENEMY_H_
#include "Character.h"

class Enemy : public Character {
public:
	int numofATK{0};
	Enemy();
	Enemy(int lvl);
	virtual ~Enemy() {}
	int attack()override; 
	virtual void displayStats(bool Eloop) const override;
	

};

#endif