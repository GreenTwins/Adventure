#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include "SQLCONN.h"
#include <iostream>
#include "Game.h"




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
		SQLINTEGER enemyID, SpawnLocation, HP, MP, Str, Def, Spd, dodge, Skill1, Skill2, Skill3, Skill4, Skill5, Skill6;
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
			SQLINTEGER enemyID, SpawnLocation,  HP, MP, Str, Def, Spd, dodge, Skill1, Skill2, Skill3;
			SQLCHAR enemyName[255], Skill1Desc[255], Skill2Desc[255], Skill3Desc[255]; // Assuming max length of 50 for name
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
			SQLGetData(hStmt, 20, SQL_C_CHAR, Skill1Desc, sizeof(Skill1Desc), NULL);
			SQLGetData(hStmt, 21, SQL_C_CHAR, Skill2Desc, sizeof(Skill2Desc), NULL);
			SQLGetData(hStmt, 22, SQL_C_CHAR, Skill3Desc, sizeof(Skill3Desc), NULL);

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

			//std::cout << enemy.getName() << std::endl;
			el.push_back(enemy);
		}

	
	SQLFreeHandle(SQL_HANDLE_STMT, sqlStatement);
	disconnect();
}
bool SQLCONN::displayNames() {
	SQLHSTMT sqlStatement;
	SQLAllocHandle(SQL_HANDLE_STMT, sqlConnection, &sqlStatement);
	
	std::string query = "SELECT PlayerName FROM Player";
	//gotta do this annoying imbue since SQLWCHAR is a wchar_t
	std::wcout.imbue(std::locale());
	SQLRETURN ret=SQLExecDirect(sqlStatement, (SQLWCHAR*)query.c_str(), SQL_NTS);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLWCHAR errorMsg[SQL_MAX_MESSAGE_LENGTH];
		SQLSMALLINT msgLen;
		SQLGetDiagRec(SQL_HANDLE_STMT, sqlStatement, 1, NULL, NULL, errorMsg, SQL_MAX_MESSAGE_LENGTH, &msgLen);
		std::wcout<< "Error exe query: " << errorMsg << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, sqlStatement);
		return false;
	}
	SQLWCHAR PlayerName[SQL_RESULT_LEN];
	
	SQLLEN playerNameLength;

	while (SQLFetch(sqlStatement) == SQL_SUCCESS) {
		SQLGetData(sqlStatement, 1, SQL_C_CHAR, PlayerName, SQL_RESULT_LEN, &playerNameLength);
		std::wcout << PlayerName << std::endl;

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

bool SQLCONN::sqlSave() {
	if (!connect()) {
		return;
	}

	//check and see if theres already data available for the given name and....something else
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

	/*ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &location, 0, NULL);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[6], message[SQL_RETURN_CODE_LEN];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_RETURN_CODE_LEN, &length);
		std::wcerr << "SQLBindParameter failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return;
	}*/
	/*ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &dungeonNum, 0, NULL);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		SQLCHAR sqlState[6], message[SQL_RETURN_CODE_LEN];
		SQLINTEGER nativeError;
		SQLSMALLINT length;
		SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, SQL_RETURN_CODE_LEN, &length);
		std::wcerr << "SQLBindParameter failed with error: " << message << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return;
	}*/

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
	//while (SQLFetch(hStmt) == SQL_SUCCESS) {
	//	Enemy enemy(location);
	//	//std::cout << enemy.location << " :";
	//	// Assuming columns in order of EnemyID, SpawnLocation, EnemyName, HP, MP, STR, DEF, SPD, DODGE, SKILL1, SKILL2, SKILL3
	//	SQLINTEGER enemyID, SpawnLocation, HP, MP, Str, Def, Spd, dodge, Skill1, Skill2, Skill3;
	//	SQLCHAR enemyName[255], Skill1Desc[255], Skill2Desc[255], Skill3Desc[255]; // Assuming max length of 50 for name
	//	SQLGetData(hStmt, 1, SQL_C_LONG, &enemyID, 0, NULL);
	//	SQLGetData(hStmt, 2, SQL_C_LONG, &SpawnLocation, 0, NULL);
	//	SQLGetData(hStmt, 3, SQL_C_LONG, &enemyID, 0, NULL);
	//	SQLGetData(hStmt, 4, SQL_C_CHAR, enemyName, sizeof(enemyName), NULL);
	//	SQLGetData(hStmt, 5, SQL_C_LONG, &HP, 0, NULL);
	//	SQLGetData(hStmt, 6, SQL_C_LONG, &MP, 0, NULL);
	//	SQLGetData(hStmt, 7, SQL_C_LONG, &Str, 0, NULL);
	//	SQLGetData(hStmt, 8, SQL_C_LONG, &Def, 0, NULL);
	//	SQLGetData(hStmt, 9, SQL_C_LONG, &Spd, 0, NULL);
	//	SQLGetData(hStmt, 10, SQL_C_LONG, &dodge, 0, NULL);
	//	SQLGetData(hStmt, 11, SQL_C_LONG, &Skill1, 0, NULL);
	//	SQLGetData(hStmt, 12, SQL_C_LONG, &Skill2, 0, NULL);
	//	SQLGetData(hStmt, 13, SQL_C_LONG, &Skill3, 0, NULL);
	//	SQLGetData(hStmt, 20, SQL_C_CHAR, Skill1Desc, sizeof(Skill1Desc), NULL);
	//	SQLGetData(hStmt, 21, SQL_C_CHAR, Skill2Desc, sizeof(Skill2Desc), NULL);
	//	SQLGetData(hStmt, 22, SQL_C_CHAR, Skill3Desc, sizeof(Skill3Desc), NULL);

	//	// Populate Enemy object
	//	enemy.location = SpawnLocation;
	//	enemy.changeName(reinterpret_cast<char*>(enemyName));
	//	enemy.setHP(HP);
	//	enemy.setMP(MP);
	//	enemy.setStr(Str);
	//	enemy.setDef(Def);
	//	enemy.setSpd(Spd);
	//	enemy.setDodge(dodge);
	//	std::string convertedSkill1 = reinterpret_cast<char*>(Skill1Desc);
	//	std::string convertedSkill2 = reinterpret_cast<char*>(Skill2Desc);
	//	std::string convertedSkill3 = reinterpret_cast<char*>(Skill3Desc);
	//	enemy.updateATKList(convertedSkill1, Skill1);
	//	enemy.updateATKList(convertedSkill2, Skill2);
	//	enemy.updateATKList(convertedSkill3, Skill3);

	//	//std::cout << enemy.getName() << std::endl;
	//	el.push_back(enemy);
	//}


	SQLFreeHandle(SQL_HANDLE_STMT, sqlStatement);
	disconnect();
}