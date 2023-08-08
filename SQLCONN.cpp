#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include "SQLCONN.h"
#include <iostream>




#define SQL_RESULT_LEN 240
#define SQL_RETURN_CODE_LEN 1000


SQLCONN::SQLCONN() {
	SQLRETURN ret=SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, (SQLHANDLE*)&sqlenvhandle);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		std::cerr << "Connection unsuccessful" << std::endl;
		return;
	}
	ret=SQLSetEnvAttr(sqlenvhandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		std::cerr << "Attr failed" << std::endl;
		return;
	}
	ret= SQLAllocHandle(SQL_HANDLE_DBC, sqlenvhandle, (SQLHANDLE*)&sqlconnectionhandle);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		std::cerr << "allocation failed" << std::endl;
		return;
	}
	std::cout << "default established" << std::endl;
}

SQLCONN::~SQLCONN() {
	SQLFreeHandle(SQL_HANDLE_DBC, sqlconnectionhandle);
	SQLFreeHandle(SQL_HANDLE_ENV, sqlenvhandle);
}

SQLCONN& SQLCONN::createInstance() {
	static SQLCONN instance;
	return instance;
}

bool SQLCONN::connect() {
	SQLWCHAR retconstring[SQL_RETURN_CODE_LEN];
	SQLWCHAR* connectionStr = (SQLWCHAR*)L"DSN=AdventureServ; Trusted_Connection=yes;";//"Data Source=localhost\\sqlexpress;Integrated Security=True";
	switch (SQLDriverConnect(sqlconnectionhandle, NULL, connectionStr, SQL_NTS, retconstring, 1024, NULL, SQL_DRIVER_NOPROMPT)) {
	case SQL_SUCCESS_WITH_INFO:
		std::cout << "Connection established. " << std::endl;
		return true;
	case SQL_INVALID_HANDLE:
	case SQL_ERROR:
		std::cout << "Connection failed" << std::endl;
		return false;
	default:
		return false;
	}
}

void SQLCONN::disconnect() {
	SQLDisconnect(sqlconnectionhandle);
}