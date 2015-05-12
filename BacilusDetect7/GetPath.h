#pragma once

#include <string>

using namespace std;

inline string GetInstallPath(void)
{
	TCHAR FileName[64];
	GetModuleFileName(NULL, FileName, 64);
	::CharLower( FileName );
	string strFileName( FileName );
	//size_t Index = strFileName.find_first_of("samples");
	size_t Index = strFileName.find(string("samples"));
	string Path = strFileName.substr(0, Index);
	return Path;
};


inline string GetReportTemplatePath(void)
{
	return GetInstallPath() + string("Samples\\reports\\");
}

inline string GetReportDataPath(void)
{
	return GetInstallPath() + string("Samples\\Data");
}

inline string GetReportDataPathFile(void)
{
	return GetInstallPath() + string("Samples\\Data\\NorthWind.mdb");
}

inline string GetDatabaseConnectionString(void)
{
	//char Buf[128];
    //sprintf(Buf, "Provider=Microsoft.Jet.OLEDB.4.0;User ID=Admin;Data Source=%s;", GetReportDataPathFile().c_str());
	//return string(Buf);
	return string("Provider=Microsoft.Jet.OLEDB.4.0;User ID=Admin;Data Source=") + GetReportDataPathFile() + ";";
}