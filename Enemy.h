#pragma once
#ifndef _ENEMY_H_
#define _ENEMY_H_
#include "Character.h"

class Enemy : public Character {
public:
	int numofATK{0};
	Enemy(int lvl);
	virtual ~Enemy() {}
	virtual int attack()override;

};

#endif