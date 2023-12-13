#pragma once
#ifndef _SQLCONN_H_
#define _SQLCONN_H


//struct SQLHANDLE__;
////typedef SQLHANDLE__* SQLHANDLE;
//struct SQLHENV__;
////typedef SQLHENV__* SQLHENV;
//struct SQLHDBC__;
//typedef SQLHDBC__* SQLHDBC;


#include <vector>
#include "Enemy.h"





class SQLCONN {
	SQLCONN();
	~SQLCONN();
	
public:
	SQLCONN(const SQLCONN& o) = delete;
	SQLCONN& operator=(const SQLCONN&) = delete;
	static SQLCONN& createInstance();
	bool connect();
	void disconnect();
	//display all chars
	bool displayNames();
	//gather all enemies from sql
	void getEnemies(int loc, int dunNum, std::vector<Enemy>&e);
	bool getBoss(int loc, int dunNum, Boss& b);
	bool sqlSave();
	std::vector<std::string>playerList;
	bool loadPlayerData(const std::string& a);
	bool grabStoreData(int lvl);

};

#endif