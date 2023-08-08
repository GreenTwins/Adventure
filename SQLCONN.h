#pragma once
#ifndef _SQLCONN_H_
#define _SQLCONN_H


//struct SQLHANDLE__;
////typedef SQLHANDLE__* SQLHANDLE;
//struct SQLHENV__;
////typedef SQLHENV__* SQLHENV;
//struct SQLHDBC__;
//typedef SQLHDBC__* SQLHDBC;

struct sqlenvhandle;
struct sqlconnectionhandle;




class SQLCONN {
	sqlenvhandle* sqlenvhandle;
	sqlconnectionhandle* sqlconnectionhandle;
	SQLCONN();
	~SQLCONN();
	SQLCONN(const SQLCONN& o) = delete;
	SQLCONN& operator=(const SQLCONN&) = delete;
public:
	static SQLCONN& createInstance();
	bool connect();
	void disconnect();
	//display all chars
	bool displayNames();
	//gather all enemies from sql
};

#endif