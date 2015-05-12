
#include "StdAfx.h"
#include <windows.h>
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>
#include <stdio.h>
#include <odbcinst.h>
#include <sqlucode.h>
#include <sqlncli.h>
#include <msdasql.h>
#include <msdadc.h>
#include <ole2.h>
#include "ODBCErrors.h"
#include "BacilusDetectDoc.h"


ODBCErrors::ODBCErrors(void)
{
	m_iLine  = -1;
	m_type   = 0;
	m_handle = SQL_NULL_HANDLE;
}

  /// <summary>
    ///Constructor for the ODBCErrors class
    /// </summary>
    /// <param name="iLine">
    /// This parameter is the source code line
    /// at which the error occurred.
    ///</param>
    /// <param name="type">
    /// This parameter is the type of ODBC handle passed in
    /// the next parameter.
    ///</param>
    /// <param name="handle">
    /// This parameter is the handle on which the error occurred.
    ///</param>

ODBCErrors::~ODBCErrors(void)
{
}


ODBCErrors::ODBCErrors(int iLine, SQLSMALLINT type, SQLHANDLE handle)
{
	m_iLine  = iLine;
    m_type   = type;
    m_handle = handle;
}


void ODBCErrors::Print()
{
	SQLSMALLINT i = 0, len = 0;
    SQLINTEGER  native;
    SQLTCHAR    state[9], text[256];
    SQLRETURN   sqlReturn = SQL_SUCCESS;

    if ( m_handle == SQL_NULL_HANDLE )
    {
        wprintf_s(TEXT("The error handle is not a valid handle.\n"), m_iLine);
        return;
    }

    wprintf_s(TEXT("Error Line(%d)\n"), m_iLine);

    while( sqlReturn == SQL_SUCCESS )
    {
        len = 0;

        sqlReturn = SQLGetDiagRec(
            m_type,
            m_handle,
            ++i,
            state,
            &native,
            text,
            sizeof(text)/sizeof(SQLTCHAR),
            &len);

        if ( SQL_SUCCEEDED(sqlReturn) )
            wprintf_s(TEXT("Error(%d, %ld, %s) : %s\n"), i, native, state, text);
    }
}
