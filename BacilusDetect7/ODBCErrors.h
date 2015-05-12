#pragma once
#include "BacilusDetectDoc.h"
class ODBCErrors
{
private:
    int         m_iLine;    //Source code line on which the error occurred
    SQLSMALLINT m_type;     //Type of handle on which the error occurred
    SQLHANDLE   m_handle;   //ODBC handle on which the error occurred

public:
	ODBCErrors(void);
	~ODBCErrors(void);
	ODBCErrors(int iLine, SQLSMALLINT type, SQLHANDLE handle);
	void Print();
};

