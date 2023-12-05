#pragma once
#ifndef _CHARACTER_H_
#define _CHARACTER_H_
#include <string>
#include <map>


class Character {
	std::string name;
	int hp{ 0 };
	int mp{ 0 };
	int str{ 0 };
	int def{ 0 };
	int spd{ 0 };
	int dodge{ 0 };
	
public:
	Character();
	Character(int n);
	Character(std::string& s);
	virtual ~Character() {}
	int location{ 0 };
	std::map<std::string, int>attacks;
	//get stats
	std::string getName()const { return name; }
	int getStr()const { return str; }
	int getDef()const { return def; }
	int getSpd()const { return spd; }
	int getDodge()const { return dodge; }
	int getMP()const { return mp; }
	int getHP()const { return hp; }
	
	//INIT
	void setHP(int v);
	void setMP(int v);
	void setStr(int v);
	void setDef(int v);
	void setSpd(int v);
	void setDodge(int v);
	


	//change stats
	void changeName(const std::string& nN);
	void increaseStats(char s, int val);
	void updateATKList( std::string& a, int v);

	//PLAY
	virtual int attack()=0;
	virtual void displayStats(bool Eloop)const = 0;
	//virtual void CreateStats() = 0;
};

#endif 


