#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include "SQLCONN.h"
#include "Player.h"
#include <iostream>
#include "Game.h"
#include <codecvt>
#include <locale>
#include <string>




#define SQL_RESULT_LEN 240
#define SQL_RETURN_CODE_LEN 1240
SQLHENV sqlEnv;
SQLHDBC sqlConnection;
SQLHSTMT sqlStatement;

SQLCONN::SQLCONN() {
	sqlEnv = SQL_NULL_HENV;
	sqlConnection = SQL_NULL_HDBC;
	sqlStatement = SQL_NULL_HSTMT;

	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnv);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		std::cerr << "Connection unsuccessful" << std::endl;
		return;
	}
	ret=SQLSetEnvAttr(sqlEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		std::cerr << "Attr failed" << std::endl;
		return;
	}
	ret= SQLAllocHandle(SQL_HANDLE_DBC, sqlEnv, (SQLHANDLE*)&sqlConnection);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		std::cerr << "allocation failed" << std::endl;
		return;
	}
	//std::cout << "connection established" << std::endl;
}

SQLCONN::~SQLCONN() {
	// Free statement handle if it's allocated
	if (sqlStatement != SQL_NULL_HSTMT) {
		SQLFreeHandle(SQL_HANDLE_STMT, sqlStatement);
	}

	// Disconnect and free the connection handle
	if (sqlConnection != SQL_NULL_HDBC) {
		SQLDisconnect(sqlConnection);
		SQLFreeHandle(SQL_HANDLE_DBC, sqlConnection);
	}

	// Free the environment handle
	if (sqlEnv != SQL_NULL_HENV) {
		SQLFreeHandle(SQL_HANDLE_ENV, sqlEnv);
	}
	/*SQLFreeHandle(SQL_HANDLE_DBC, sqlconnectionhandle);
	SQLFreeHandle(SQL_HANDLE_ENV, sqlenvhandle);*/
}

SQLCONN& SQLCONN::createInstance() {
	static SQLCONN instance;
	return instance;
}

bool SQLCONN::connect() {
	SQLWCHAR connStr[] = L"DRIVER={SQL Server};SERVER=localhost\\sqlexpress;DATABASE=master;Integrated Security=True;";
	SQLWCHAR outConnStr[SQL_MAX_MESSAGE_LENGTH];
	SQLSMALLINT outConnStrLen;
	SQLRETURN ret = SQLDriverConnect(sqlConnection, NULL, connStr, SQL_NTS, outConnStr, sizeof(outConnStr), &outConnStrLen, SQL_DRIVER_NOPROMPT);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLWCHAR sqlState[6], message[SQL_RETURN_CODE_LEN];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_DBC, sqlConnection, 1, sqlState, &nativeError, message, SQL_RETURN_CODE_LEN, &length);
		std::wcerr << "Connection failed with error: " << message << std::endl;
		return false;
	}
	std::wcout << L"Connected to the database successfully!" << std::endl;
	return true;
}

void SQLCONN::disconnect() {
	if (sqlConnection != SQL_NULL_HDBC) {
		SQLDisconnect(sqlConnection);
	}
}
bool SQLCONN::getBoss(int loc, int dunNum, Boss& b) {

	if (!connect()) {
		return false;
	}


	SQLHSTMT hStmt;
	SQLAllocHandle(SQL_HANDLE_STMT, sqlConnection, &hStmt);

	SQLWCHAR* sqlQuery = (SQLWCHAR*)L"SELECT * FROM Boss WHERE dungeonLocation = ? AND bossLocation=?";
	SQLRETURN ret = SQLPrepare(hStmt, sqlQuery, SQL_NTS);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[6], message[SQL_MAX_MESSAGE_LENGTH]; 
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_RETURN_CODE_LEN, &length);
		std::wcerr << "SQLPrepare failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}

	ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &loc, 0, NULL);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[6], message[SQL_RETURN_CODE_LEN];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_RETURN_CODE_LEN, &length);
		std::wcerr << "SQLBindParameter failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}
	ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &dunNum, 0, NULL);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[6], message[SQL_RETURN_CODE_LEN];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_RETURN_CODE_LEN, &length);
		std::wcerr << "SQLBindParameter failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}

	ret = SQLExecute(hStmt);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[SQL_SQLSTATE_SIZE+1], message[SQL_MAX_MESSAGE_LENGTH];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_MAX_MESSAGE_LENGTH, &length);
		std::wcerr << "SQLExecute failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}
	std::cout << "fetching...." << std::endl;
	while (SQLFetch(hStmt) == SQL_SUCCESS) {
		//std::cout << enemy.location << " :";
		// Assuming columns in order of EnemyID, SpawnLocation, EnemyName, HP, MP, STR, DEF, SPD, DODGE, SKILL1, SKILL2, SKILL3
		SQLINTEGER enemyID, SpawnLocation, HP, MP, Str, Def, Spd, dodge, Skill1, Skill2, Skill3, Skill4, Skill5, Skill6, XPGiven, GoldGiven;
		SQLCHAR enemyName[255], Skill1Desc[255], Skill2Desc[255], Skill3Desc[255], Skill4Desc[255], Skill5Desc[255], Skill6Desc[255]; // Assuming max length of 50 for name
		SQLGetData(hStmt, 1, SQL_C_LONG, &enemyID, 0, NULL);
		SQLGetData(hStmt, 2, SQL_C_LONG, &SpawnLocation, 0, NULL);
		SQLGetData(hStmt, 3, SQL_C_LONG, &enemyID, 0, NULL);
		SQLGetData(hStmt, 4, SQL_C_CHAR, enemyName, sizeof(enemyName), NULL);
		SQLGetData(hStmt, 5, SQL_C_LONG, &HP, 0, NULL);
		SQLGetData(hStmt, 6, SQL_C_LONG, &MP, 0, NULL);
		SQLGetData(hStmt, 7, SQL_C_LONG, &Str, 0, NULL);
		SQLGetData(hStmt, 8, SQL_C_LONG, &Def, 0, NULL);
		SQLGetData(hStmt, 9, SQL_C_LONG, &Spd, 0, NULL);
		SQLGetData(hStmt, 10, SQL_C_LONG, &dodge, 0, NULL);
		SQLGetData(hStmt, 11, SQL_C_LONG, &Skill1, 0, NULL);
		SQLGetData(hStmt, 12, SQL_C_LONG, &Skill2, 0, NULL);
		SQLGetData(hStmt, 13, SQL_C_LONG, &Skill3, 0, NULL);
		SQLGetData(hStmt, 14, SQL_C_LONG, &Skill4, 0, NULL);
		SQLGetData(hStmt, 15, SQL_C_LONG, &Skill5, 0, NULL);
		SQLGetData(hStmt, 16, SQL_C_LONG, &Skill6, 0, NULL);
		SQLGetData(hStmt, 20, SQL_C_CHAR, Skill1Desc, sizeof(Skill1Desc), NULL);
		SQLGetData(hStmt, 21, SQL_C_CHAR, Skill2Desc, sizeof(Skill2Desc), NULL);
		SQLGetData(hStmt, 22, SQL_C_CHAR, Skill3Desc, sizeof(Skill3Desc), NULL);
		SQLGetData(hStmt, 23, SQL_C_CHAR, Skill4Desc, sizeof(Skill4Desc), NULL);
		SQLGetData(hStmt, 24, SQL_C_CHAR, Skill5Desc, sizeof(Skill5Desc), NULL);
		SQLGetData(hStmt, 25, SQL_C_CHAR, Skill6Desc, sizeof(Skill6Desc), NULL);
		SQLGetData(hStmt, 29, SQL_C_LONG, &XPGiven, 0, NULL);
		SQLGetData(hStmt, 30, SQL_C_LONG, &GoldGiven, 0, NULL);

		// Populate Enemy object
		b.location = SpawnLocation;
		b.changeName(reinterpret_cast<char*>(enemyName));
		b.setHP(HP);
		b.setMP(MP);
		b.setStr(Str);
		b.setDef(Def);
		b.setSpd(Spd);
		b.setDodge(dodge);
		std::string convertedSkill1 = reinterpret_cast<char*>(Skill1Desc);
		std::string convertedSkill2 = reinterpret_cast<char*>(Skill2Desc);
		std::string convertedSkill3 = reinterpret_cast<char*>(Skill3Desc);
		std::string convertedSkill4 = reinterpret_cast<char*>(Skill4Desc);
		std::string convertedSkill5 = reinterpret_cast<char*>(Skill5Desc);
		std::string convertedSkill6 = reinterpret_cast<char*>(Skill6Desc);
		b.updateATKList(convertedSkill1, Skill1);
		b.updateATKList(convertedSkill2, Skill2);
		b.updateATKList(convertedSkill3, Skill3);
		b.updateATKList(convertedSkill4, Skill4);
		b.updateATKList(convertedSkill5, Skill5);
		b.updateATKList(convertedSkill6, Skill6);
		b.setGold(GoldGiven);
		b.setXP(XPGiven);
	}


	SQLFreeHandle(SQL_HANDLE_STMT, sqlStatement);
	disconnect();
	return true;
}
void NullTerminateString(SQLCHAR* str, SQLLEN length) {
	if (length > 0 && str[length - 1] != '\0') {
		str[length] = '\0';
	}
}
void SQLCONN::getEnemies(int location, int dungeonNum, std::vector<Enemy>&el ) {
	
	if (!connect()) {
		return;
	}
	

	SQLHSTMT hStmt;
	SQLAllocHandle(SQL_HANDLE_STMT, sqlConnection, &hStmt);

	SQLWCHAR* sqlQuery = (SQLWCHAR*)L"SELECT * FROM Enemy WHERE SpawnLocation = ? AND dungeonNum=?";
	SQLRETURN ret = SQLPrepare(hStmt, sqlQuery, SQL_NTS);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[6], message[SQL_RETURN_CODE_LEN];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_RETURN_CODE_LEN, &length);
		std::wcerr << "SQLPrepare failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return;
	}

	ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &location, 0, NULL);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[6], message[SQL_RETURN_CODE_LEN];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_RETURN_CODE_LEN, &length);
		std::wcerr << "SQLBindParameter failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return;
	}
	ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &dungeonNum, 0, NULL);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[6], message[SQL_RETURN_CODE_LEN];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_RETURN_CODE_LEN, &length);
		std::wcerr << "SQLBindParameter failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return;
	}
	
	ret = SQLExecute(hStmt);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[6], message[SQL_RETURN_CODE_LEN];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_RETURN_CODE_LEN, &length);
		std::wcerr << "SQLExecute failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return;
	}
		std::cout << "fetching...." << std::endl;
		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			Enemy enemy(location);
			//std::cout << enemy.location << " :";
			// Assuming columns in order of EnemyID, SpawnLocation, EnemyName, HP, MP, STR, DEF, SPD, DODGE, SKILL1, SKILL2, SKILL3
			SQLINTEGER enemyID, SpawnLocation,  dungeonNum, HP, MP, Str, Def, Spd, dodge, Skill1, Skill2, Skill3, XPGiven, GoldGiven;
			SQLCHAR enemyName[255], Skill1Desc[255], Skill2Desc[255], Skill3Desc[255]; // Assuming max length of 50 for name
			SQLGetData(hStmt, 1, SQL_C_LONG, &enemyID, 0, NULL);
			SQLGetData(hStmt, 2, SQL_C_LONG, &SpawnLocation, 0, NULL);
			SQLGetData(hStmt, 3, SQL_C_LONG, &dungeonNum, 0, NULL);
			SQLGetData(hStmt, 4, SQL_C_CHAR, enemyName, sizeof(enemyName), NULL);
			SQLGetData(hStmt, 5, SQL_C_LONG, &HP, 0, NULL);
			SQLGetData(hStmt, 6, SQL_C_LONG, &MP, 0, NULL);
			SQLGetData(hStmt, 7, SQL_C_LONG, &Str, 0, NULL);
			SQLGetData(hStmt, 8, SQL_C_LONG, &Def, 0, NULL);
			SQLGetData(hStmt, 9, SQL_C_LONG, &Spd, 0, NULL);
			SQLGetData(hStmt, 10, SQL_C_LONG, &dodge, 0, NULL);
			SQLGetData(hStmt, 11, SQL_C_LONG, &Skill1, 0, NULL);
			SQLGetData(hStmt, 12, SQL_C_LONG, &Skill2, 0, NULL);
			SQLGetData(hStmt, 13, SQL_C_LONG, &Skill3, 0, NULL);
			SQLGetData(hStmt, 20, SQL_C_CHAR, Skill1Desc, sizeof(Skill1Desc), NULL);
			SQLGetData(hStmt, 21, SQL_C_CHAR, Skill2Desc, sizeof(Skill2Desc), NULL);
			SQLGetData(hStmt, 22, SQL_C_CHAR, Skill3Desc, sizeof(Skill3Desc), NULL);
			SQLGetData(hStmt, 29, SQL_C_LONG, &XPGiven, 0, NULL);
			SQLGetData(hStmt, 30, SQL_C_LONG, &GoldGiven, 0, NULL);

			// Populate Enemy object
			enemy.location = SpawnLocation;
			enemy.changeName(reinterpret_cast<char*>(enemyName));
			enemy.setHP(HP);
			enemy.setMP(MP);
			enemy.setStr(Str);
			enemy.setDef(Def);
			enemy.setSpd(Spd);
			enemy.setDodge(dodge);
			std::string convertedSkill1 = reinterpret_cast<char*>(Skill1Desc);
			std::string convertedSkill2 = reinterpret_cast<char*>(Skill2Desc);
			std::string convertedSkill3 = reinterpret_cast<char*>(Skill3Desc);
			enemy.updateATKList(convertedSkill1, Skill1);
			enemy.updateATKList(convertedSkill2, Skill2);
			enemy.updateATKList(convertedSkill3, Skill3);
			enemy.setGold(GoldGiven);
			enemy.setXP(XPGiven);
			//std::cout << enemy.getName() << std::endl;
			el.push_back(enemy);
		}

	
	SQLFreeHandle(SQL_HANDLE_STMT, sqlStatement);
	disconnect();
}
bool SQLCONN::displayNames() {
	/*if (!connect()) {
		return false;
	}*/
	SQLHSTMT sqlStatement;
	SQLAllocHandle(SQL_HANDLE_STMT, sqlConnection, &sqlStatement);
	
	SQLWCHAR* sqlQuery = (SQLWCHAR*)L"SELECT playerName FROM Player";
	//gotta do this annoying imbue since SQLWCHAR is a wchar_t
	//std::wcout.imbue(std::locale());
	SQLRETURN ret=SQLExecDirect(sqlStatement, sqlQuery, SQL_NTS);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLWCHAR errorMsg[SQL_MAX_MESSAGE_LENGTH];
		SQLSMALLINT msgLen;
		SQLGetDiagRec(SQL_HANDLE_STMT, sqlStatement, 1, NULL, NULL, errorMsg, SQL_MAX_MESSAGE_LENGTH, &msgLen);
		std::wcout<< "Error exe query: " << errorMsg << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, sqlStatement);
		return false;
	}
	SQLCHAR PlayerName[255];
	
	SQLLEN playerNameLength;

	int i = 1;
	while (SQLFetch(sqlStatement) == SQL_SUCCESS) {
		//SQLGetData(hStmt, 4, SQL_C_CHAR, enemyName, sizeof(enemyName), NULL);
		SQLGetData(sqlStatement, 1, SQL_C_CHAR, PlayerName, sizeof(PlayerName), &playerNameLength);
		PlayerName[playerNameLength] = '\0';
		std::string playersName(reinterpret_cast<char*>(PlayerName), playerNameLength);
		std::cout <<i<<".) "<< playersName << std::endl;
		++i;
		
		playerList.push_back(playersName);
	}
	ret=SQLFreeStmt(sqlStatement, SQL_CLOSE);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLWCHAR errorMsg[SQL_MAX_MESSAGE_LENGTH];
		SQLSMALLINT msgLen;
		SQLGetDiagRec(SQL_HANDLE_STMT, sqlStatement, 1, NULL, NULL, errorMsg, SQL_MAX_MESSAGE_LENGTH, &msgLen);
		std::wcout << "Error freeing data: " << errorMsg << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, sqlStatement);
		return false;
	}

	SQLFreeHandle(SQL_HANDLE_STMT, sqlStatement);
	return true;
}

void loadATKfromPlayer() {
	std::map<std::string, int>holder = Game::getinstance().playerN.attacks;
	std::map<std::string, int>::iterator it;
	int i = 0;
	for (it = holder.begin(); it != holder.end(); it++) {
		if (i == 0) {
			Game::getinstance().playerN.attack1.first= it->first;
			Game::getinstance().playerN.attack1.second = it->second;
		}
		else if (i == 1) {
			Game::getinstance().playerN.attack2.first = it->first;
			Game::getinstance().playerN.attack2.second = it->second;
		}
		else if (i == 2) {
			Game::getinstance().playerN.attack3.first = it->first;
			Game::getinstance().playerN.attack3.second = it->second;
		}
		else if (i == 3) {
			Game::getinstance().playerN.attack4.first = it->first;
			Game::getinstance().playerN.attack4.second = it->second;
		}
		else if (i == 4) {
			Game::getinstance().playerN.attack5.first = it->first;
			Game::getinstance().playerN.attack5.second = it->second;
		}
		else {
			Game::getinstance().playerN.attack6.first = it->first;
			Game::getinstance().playerN.attack6.second = it->second;
		}
		++i;
	}
	
}
bool SQLCONN::sqlSave() {
	if (!connect()) {
		return false;
	}
	//load data into vars
	std::string playerName, Skill1Desc, Skill2Desc, Skill3Desc, Skill4Desc, Skill5Desc, Skill6Desc;
	int HP, MP, Str, Def, Spd, dodge, level, maxHP, gold, xp; 
	int currentLocation, Skill1, Skill2, Skill3, Skill4, Skill5, Skill6;
	playerName = Game::getinstance().playerN.getName();
	level = Game::getinstance().playerN.getLvl();
	HP= Game::getinstance().playerN.getHP();
	MP = Game::getinstance().playerN.getMP();
	Str = Game::getinstance().playerN.getStr();
	Def = Game::getinstance().playerN.getDef();
	Spd = Game::getinstance().playerN.getSpd();
	dodge = Game::getinstance().playerN.getDodge();
	maxHP = Game::getinstance().playerN.getMaxHP();
	gold = Game::getinstance().playerN.getGold();
	xp = Game::getinstance().playerN.getXP();
	//loadAttacks(Skill1, Skill2, Skill3, Skill4, Skill5, Skill6, Skill1Desc, Skill2Desc, Skill3Desc, Skill4Desc, Skill5Desc, Skill6Desc);
	loadATKfromPlayer();
	
	Skill1Desc = Game::getinstance().playerN.attack1.first;
	Skill1 = Game::getinstance().playerN.attack1.second;
	Skill2Desc = Game::getinstance().playerN.attack2.first;
	Skill2 = Game::getinstance().playerN.attack2.second;
	Skill3Desc = Game::getinstance().playerN.attack3.first;
	Skill3 = Game::getinstance().playerN.attack3.second;
	Skill4Desc = Game::getinstance().playerN.attack4.first;
	Skill4 = Game::getinstance().playerN.attack4.second;
	Skill5Desc = Game::getinstance().playerN.attack5.first;
	Skill5 = Game::getinstance().playerN.attack5.second;
	Skill6Desc = Game::getinstance().playerN.attack6.first;
	Skill6 = Game::getinstance().playerN.attack6.second;
	currentLocation = Game::getinstance().playerN.location;
	
	//convert for 
	//std::cout << currentLocation << std::endl;

	SQLHSTMT hStmt;
	SQLAllocHandle(SQL_HANDLE_STMT, sqlConnection, &hStmt);
	SQLWCHAR* sqlQuery = nullptr;
	SQLRETURN ret;
	//check and see if theres already data available for the given name and....something else
	if (Game::getinstance().newChar) {
		//save to sql
		std::cout << "Saving new data..." << std::endl;
		sqlQuery = (SQLWCHAR*)L"INSERT INTO Player (currentLocation, playerName, HP, MP, Str, Def, Spd, dodge, Skill1,Skill2,Skill3,Skill4,Skill5,Skill6, Skill1Desc, Skill2Desc, Skill3Desc, Skill4Desc, Skill5Desc, Skill6Desc, level, maxHP,XP,GoldAmt)" 
			L"VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

		 ret= SQLPrepare(hStmt, sqlQuery, SQL_NTS);
		if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
			SQLCHAR sqlState[6], message[SQL_RETURN_CODE_LEN];
			SQLINTEGER nativeError;
			SQLSMALLINT length;
			SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_RETURN_CODE_LEN, &length);
			std::wcerr << "SQLPrepare failed with error: " << message << std::endl;
			SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			return false;
		}
		Game::getinstance().newChar = false;
		SQLLEN stringLength = SQL_NTS;
		ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &currentLocation, 0, NULL);
		ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)playerName.c_str(), 0, &stringLength);
		ret = SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &HP, 0, NULL);
		ret = SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &MP, 0, NULL);
		ret = SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Str, 0, NULL);
		ret = SQLBindParameter(hStmt, 6, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Def, 0, NULL);
		ret = SQLBindParameter(hStmt, 7, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Spd, 0, NULL);
		ret = SQLBindParameter(hStmt, 8, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &dodge, 0, NULL);
		ret = SQLBindParameter(hStmt, 9, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Skill1, 0, NULL);
		ret = SQLBindParameter(hStmt, 10, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Skill2, 0, NULL);
		ret = SQLBindParameter(hStmt, 11, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Skill3, 0, NULL);
		ret = SQLBindParameter(hStmt, 12, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Skill4, 0, NULL);
		ret = SQLBindParameter(hStmt, 13, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Skill5, 0, NULL);
		ret = SQLBindParameter(hStmt, 14, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Skill6, 0, NULL);
		ret = SQLBindParameter(hStmt, 15, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)Skill1Desc.c_str(), 0, &stringLength);
		ret = SQLBindParameter(hStmt, 16, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)Skill2Desc.c_str(), 0, &stringLength);
		ret = SQLBindParameter(hStmt, 17, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)Skill3Desc.c_str(), 0, &stringLength);
		ret = SQLBindParameter(hStmt, 18, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)Skill4Desc.c_str(), 0, &stringLength);
		ret = SQLBindParameter(hStmt, 19, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)Skill5Desc.c_str(), 0, &stringLength);
		ret = SQLBindParameter(hStmt, 20, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)Skill6Desc.c_str(), 0, &stringLength);
		ret = SQLBindParameter(hStmt, 21, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &level, 0, NULL);
		ret = SQLBindParameter(hStmt, 22, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &maxHP, 0, NULL);
		ret = SQLBindParameter(hStmt, 23, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &xp, 0, NULL);
		ret = SQLBindParameter(hStmt, 24, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &gold, 0, NULL);
	}
	else {
		//use update
		sqlQuery = (SQLWCHAR*)L"UPDATE Player SET currentLocation=?, HP=?, MP=?, Str=?, Def=?, Spd=?, dodge=?, Skill1=?, Skill2=?, Skill3=?, Skill4=?, Skill5=?, Skill6=?, Skill1Desc=?, Skill2Desc=?, Skill3Desc=?, Skill4Desc=?, Skill5Desc=?, Skill6Desc=?, level=?, MaxHP=?, XP=?, GoldAmt=? WHERE playerName=?";

		ret = SQLPrepare(hStmt, sqlQuery, SQL_NTS);
		if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
			SQLCHAR sqlState[6], message[SQL_RETURN_CODE_LEN];
			SQLINTEGER nativeError;
			SQLSMALLINT length;
			SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_RETURN_CODE_LEN, &length);
			std::wcerr << "SQLPrepare failed with error: " << message << std::endl;
			SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			return false;
		}
		SQLLEN stringLength = SQL_NTS;
		ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &currentLocation, 0, NULL);
		ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &HP, 0, NULL);
		ret = SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &MP, 0, NULL);
		ret = SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Str, 0, NULL);
		ret = SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Def, 0, NULL);
		ret = SQLBindParameter(hStmt, 6, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Spd, 0, NULL);
		ret = SQLBindParameter(hStmt, 7, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &dodge, 0, NULL);
		ret = SQLBindParameter(hStmt, 8, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Skill1, 0, NULL);
		ret = SQLBindParameter(hStmt, 9, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Skill2, 0, NULL);
		ret = SQLBindParameter(hStmt, 10, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Skill3, 0, NULL);
		ret = SQLBindParameter(hStmt, 11, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Skill4, 0, NULL);
		ret = SQLBindParameter(hStmt, 12, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Skill5, 0, NULL);
		ret = SQLBindParameter(hStmt, 13, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &Skill6, 0, NULL);
		ret = SQLBindParameter(hStmt, 14, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)Skill1Desc.c_str(), 0, &stringLength);
		ret = SQLBindParameter(hStmt, 15, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)Skill2Desc.c_str(), 0, &stringLength);
		ret = SQLBindParameter(hStmt, 16, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)Skill3Desc.c_str(), 0, &stringLength);
		ret = SQLBindParameter(hStmt, 17, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)Skill4Desc.c_str(), 0, &stringLength);
		ret = SQLBindParameter(hStmt, 18, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)Skill5Desc.c_str(), 0, &stringLength);
		ret = SQLBindParameter(hStmt, 19, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)Skill6Desc.c_str(), 0, &stringLength);
		ret = SQLBindParameter(hStmt, 20, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &level, 0, NULL);
		ret = SQLBindParameter(hStmt, 21, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &maxHP, 0, NULL);
		ret = SQLBindParameter(hStmt, 22, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &xp, 0, NULL);
		ret = SQLBindParameter(hStmt, 23, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &gold, 0, NULL);
		ret = SQLBindParameter(hStmt, 24, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)playerName.c_str(), 0, &stringLength);
	}
	
	

	ret = SQLExecute(hStmt);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[SQL_SQLSTATE_SIZE+1], message[SQL_MAX_MESSAGE_LENGTH];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_RETURN_CODE_LEN, &length);
		std::wcerr << "SQLExecute failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		disconnect();
		return false;
	}
	std::cout << "Player data save!" << std::endl;

	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	disconnect();
	return true;
}
bool SQLCONN::loadPlayerData(const std::string& a) {
	if (!connect()) {
		return false;
	}
	SQLHSTMT hStmt;
	
	SQLAllocHandle(SQL_HANDLE_STMT, sqlConnection, &hStmt);

	SQLWCHAR* sqlQuery = (SQLWCHAR*)L"SELECT * FROM Player WHERE playerName = ?";
	SQLRETURN ret = SQLPrepare(hStmt, sqlQuery, SQL_NTS);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[6], message[SQL_MAX_MESSAGE_LENGTH];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_RETURN_CODE_LEN, &length);
		std::wcerr << "SQLPrepare failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}

	ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 255, 0, (SQLPOINTER*)a.c_str(), a.size(), NULL);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[6], message[SQL_RETURN_CODE_LEN];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_RETURN_CODE_LEN, &length);
		std::wcerr << "SQLBindParameter failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}
	ret = SQLExecute(hStmt);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[SQL_SQLSTATE_SIZE + 1], message[SQL_MAX_MESSAGE_LENGTH];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_MAX_MESSAGE_LENGTH, &length);
		std::wcerr << "SQLExecute failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}
	std::cout << "fetching...." << std::endl;
	while (SQLFetch(hStmt) == SQL_SUCCESS) {
		// Assuming columns in order of EnemyID, SpawnLocation, EnemyName, HP, MP, STR, DEF, SPD, DODGE, SKILL1, SKILL2, SKILL3
		SQLINTEGER currentLocation, HP, MP, Str, Def, Spd, dodge, Skill1, Skill2, Skill3, Skill4, Skill5, Skill6, level, MaxHP, GoldAmt, XP;
		SQLCHAR playerName[255], Skill1Desc[255], Skill2Desc[255], Skill3Desc[255], Skill4Desc[255], Skill5Desc[255], Skill6Desc[255]; // Assuming max length of 50 for name
		
		SQLGetData(hStmt, 2, SQL_C_LONG, &currentLocation, 0, NULL);
		SQLGetData(hStmt, 4, SQL_C_LONG, &HP, 0, NULL);
		SQLGetData(hStmt, 5, SQL_C_LONG, &MP, 0, NULL);
		SQLGetData(hStmt, 6, SQL_C_LONG, &Str, 0, NULL);
		SQLGetData(hStmt, 7, SQL_C_LONG, &Def, 0, NULL);
		SQLGetData(hStmt, 8, SQL_C_LONG, &Spd, 0, NULL);
		SQLGetData(hStmt, 9, SQL_C_LONG, &dodge, 0, NULL);
		SQLGetData(hStmt, 10, SQL_C_LONG, &Skill1, 0, NULL);
		SQLGetData(hStmt, 11, SQL_C_LONG, &Skill2, 0, NULL);
		SQLGetData(hStmt, 12, SQL_C_LONG, &Skill3, 0, NULL);
		SQLGetData(hStmt, 13, SQL_C_LONG, &Skill4, 0, NULL);
		SQLGetData(hStmt, 14, SQL_C_LONG, &Skill5, 0, NULL);
		SQLGetData(hStmt, 15, SQL_C_LONG, &Skill6, 0, NULL);
		SQLGetData(hStmt, 19, SQL_C_CHAR, &Skill1Desc, sizeof(Skill1Desc), NULL);
		SQLGetData(hStmt, 20, SQL_C_CHAR, &Skill2Desc, sizeof(Skill2Desc), NULL);
		SQLGetData(hStmt, 21, SQL_C_CHAR, &Skill3Desc, sizeof(Skill3Desc), NULL);
		SQLGetData(hStmt, 22, SQL_C_CHAR, &Skill4Desc, sizeof(Skill4Desc), NULL);
		SQLGetData(hStmt, 23, SQL_C_CHAR, &Skill5Desc, sizeof(Skill5Desc), NULL);
		SQLGetData(hStmt, 24, SQL_C_CHAR, &Skill6Desc, sizeof(Skill6Desc), NULL);
		SQLGetData(hStmt, 28, SQL_C_LONG, &level, 0, NULL);
		SQLGetData(hStmt, 29, SQL_C_LONG, &MaxHP, 0, NULL);
		SQLGetData(hStmt, 30, SQL_C_LONG, &GoldAmt, 0, NULL);
		SQLGetData(hStmt, 31, SQL_C_LONG, &XP, 0, NULL);

		// Populate 
		std::string name = a;
		Player p1(name);
		p1.LoadedInfo = true;
		p1.location = currentLocation;
		p1.init();
		p1.setHP(HP);
		p1.setMP(MP);
		p1.setStr(Str);
		p1.setDef(Def);
		p1.setSpd(Spd);
		p1.setDodge(dodge);

		//Game::getinstance().playerN.location = currentLocation;
		////Game::getinstance().playerN.changeName(reinterpret_cast<char*>(playerName));
		//Game::getinstance().playerN.setHP(HP);
		//Game::getinstance().playerN.setMP(MP);
		//Game::getinstance().playerN.setStr(Str);
		//Game::getinstance().playerN.setDef(Def);
		//Game::getinstance().playerN.setSpd(Spd);
		//Game::getinstance().playerN.setDodge(dodge);
		std::string convertedSkill1 = reinterpret_cast<char*>(Skill1Desc);
		std::string convertedSkill2 = reinterpret_cast<char*>(Skill2Desc);
		std::string convertedSkill3 = reinterpret_cast<char*>(Skill3Desc);
		std::string convertedSkill4 = reinterpret_cast<char*>(Skill4Desc);
		std::string convertedSkill5 = reinterpret_cast<char*>(Skill5Desc);
		std::string convertedSkill6 = reinterpret_cast<char*>(Skill6Desc);
		p1.updateATKList(convertedSkill1, Skill1);
		p1.updateATKList(convertedSkill2, Skill2);
		p1.updateATKList(convertedSkill3, Skill3);
		p1.updateATKList(convertedSkill4, Skill4);
		p1.updateATKList(convertedSkill5, Skill5);
		p1.updateATKList(convertedSkill6, Skill6);
		p1.setlvl(level);
		p1.setMax_HP(MaxHP);
		p1.setGold(GoldAmt);
		p1.setXP(XP);
		//Game::getinstance().playerN.updateATKList(convertedSkill1, Skill1);
		//Game::getinstance().playerN.updateATKList(convertedSkill2, Skill2);
		//Game::getinstance().playerN.updateATKList(convertedSkill3, Skill3);
		//Game::getinstance().playerN.updateATKList(convertedSkill4, Skill4);
		//Game::getinstance().playerN.updateATKList(convertedSkill5, Skill5);
		//Game::getinstance().playerN.updateATKList(convertedSkill6, Skill6);
		/*Game::getinstance().playerN.setlvl(level);
		Game::getinstance().playerN.setMax_HP(MaxHP);
		Game::getinstance().playerN.setGold(GoldAmt);
		Game::getinstance().playerN.setXP(XP);*/
		Game::getinstance().playerN = p1;
	}
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	disconnect();
	return true;

}
bool SQLCONN::grabStoreData(int lvl) {
	if (!connect()) {
		return false;
	}
	SQLHSTMT hStmt;

	//std::string currentLevel = std::to_string(lvl); //data in DB is a varchar for future flexibility in item requirement
	SQLAllocHandle(SQL_HANDLE_STMT, sqlConnection, &hStmt);

	SQLWCHAR* sqlQuery = (SQLWCHAR*)L"SELECT * FROM StoreItems WHERE Requirement <= ?";
	SQLRETURN ret = SQLPrepare(hStmt, sqlQuery, SQL_NTS);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[6], message[SQL_MAX_MESSAGE_LENGTH];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_RETURN_CODE_LEN, &length);
		std::wcerr << "SQLPrepare failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}

	ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_C_LONG, 255, 0, &lvl, 0, NULL);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[6], message[SQL_RETURN_CODE_LEN];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_RETURN_CODE_LEN, &length);
		std::wcerr << "SQLBindParameter failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}
	ret = SQLExecute(hStmt);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[SQL_SQLSTATE_SIZE + 1], message[SQL_MAX_MESSAGE_LENGTH];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_MAX_MESSAGE_LENGTH, &length);
		std::wcerr << "SQLExecute failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}
	std::cout << "fetching...." << std::endl;
	while (SQLFetch(hStmt) == SQL_SUCCESS) {
		// Assuming columns in order of EnemyID, SpawnLocation, EnemyName, HP, MP, STR, DEF, SPD, DODGE, SKILL1, SKILL2, SKILL3
		SQLINTEGER itemID, itemPrice, itemStrBuff, itemDefBuff, itemHPBuff, Requirement;
		SQLCHAR itemName[255], itemType[255]; // Assuming max length of 50 for name

		SQLLEN itemNameLength;
		SQLLEN itemTypeLength;


		SQLGetData(hStmt, 1, SQL_C_LONG, &itemID, 0, NULL);
		SQLGetData(hStmt, 2, SQL_C_CHAR, &itemName, sizeof(itemName), &itemNameLength);
		SQLGetData(hStmt, 3, SQL_C_LONG, &itemPrice, 0, NULL);
		SQLGetData(hStmt, 4, SQL_C_LONG, &itemStrBuff, 0, NULL);
		SQLGetData(hStmt, 5, SQL_C_LONG, &itemDefBuff, 0, NULL);
		SQLGetData(hStmt, 6, SQL_C_CHAR, &itemType, sizeof(itemType), &itemTypeLength);
		SQLGetData(hStmt, 7, SQL_C_LONG, &itemHPBuff, 0, NULL);
		SQLGetData(hStmt, 8, SQL_C_LONG, &Requirement, 0, NULL);
		
		
		
	
	
		

		// Populate Enemy object
		Item newItem;
		newItem.setID(itemID);
		std::string itName = reinterpret_cast<char*>(itemName);
		newItem.setName(itName);
		newItem.setPrice(itemPrice);
		newItem.setItemStats(itemStrBuff, itemDefBuff, itemHPBuff);
		newItem.setRequirement(Requirement);
		std::string itType = reinterpret_cast<char*>(itemType);
		newItem.setType(itType);
		
		if ((newItem.getType() == "Helmet")||(newItem.getType() == "Boots")||(newItem.getType() == "Armor")) {
			Game::getinstance().StoreWearables.push_back(newItem);
		}
		else if (newItem.getType() == "Potion") {
			Game::getinstance().StorePotions.push_back(newItem);
		}
		else {
			Game::getinstance().StoreWeapons.push_back(newItem);
		}
	}
	
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	disconnect();
	return true;

}