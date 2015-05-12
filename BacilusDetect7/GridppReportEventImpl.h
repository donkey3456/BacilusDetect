#pragma once

#include "GRImport.h"

/////////////////////////////////////////////////////////////////////////////
// CGridppReportEventImpl
const int  IDC_GRIDPPREPORT = -987654 ;

#define wMajor 4
#define wMinor 5

class CGridppReportEventImpl 
	:public CComObjectRoot
	,public IDispEventImpl<IDC_GRIDPPREPORT, CGridppReportEventImpl, &DIID__IGridppReportEvents, &LIBID_grproLib, wMajor, wMinor>
{
public:
	BEGIN_COM_MAP(CGridppReportEventImpl)
	END_COM_MAP()

	BEGIN_SINK_MAP(CGridppReportEventImpl)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_INITIALIZE, OnInitialize, NULL)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_FETCHRECORD, OnFetchRecord, &OnFetchRecordInfo)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_BEFOREPOSTRECORD, OnBeforePostRecord, NULL)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_BEFORESORT, OnBeforeSort, &OnBeforeSortInfo)

		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_PROCESSBEGIN, OnProcessBegin, NULL)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_PROCESSEND, OnProcessEnd, NULL)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_GROUPBEGIN, OnGroupBegin, &OnGroupBeginInfo)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_GROUPEND, OnGroupEnd, &OnGroupEndInfo)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_PROCESSRECORD, OnProcessRecord, NULL)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_PAGEPROCESSRECORD, OnPageProcessRecord, NULL)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_PAGESTART, OnPageStart, NULL)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_PAGEEND, OnPageEnd, NULL)

		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_SECTIONFORMAT, OnSectionFormat, &OnSectionFormatInfo)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_FIELDGETDIAPLYTEXT, OnFieldGetDisplayText, &OnFieldGetDisplayTextInfo)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_TEXTBOXGETDIAPLYTEXT, OnTextBoxGetDisplayText, &OnTextBoxGetDisplayTextInfo)

		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_PRINTBEGIN, OnPrintBegin, NULL)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_PRINTEND, OnPrintEnd, NULL)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_PRINTPAGE, OnPrintPage, &OnPrintPageInfo)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_PRINTABORTED, OnPrintAborted, NULL)

		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_EXPORTBEGIN, OnExportBegin, &OnExportBeginInfo)
		SINK_ENTRY_INFO(IDC_GRIDPPREPORT, DIID__IGridppReportEvents, DISPID_EXPORTEND, OnExportEnd, &OnExportEndInfo)
	END_SINK_MAP()

	void __stdcall OnInitialize(void)
	{
		Initialize();
	}

	void __stdcall OnFetchRecord(VARIANT_BOOL *pEof)
	{
		FetchRecord(pEof);
	}

	void __stdcall OnBeforePostRecord(void)
	{
		BeforePostRecord();
	}
	
	void __stdcall OnBeforeSort(BSTR SortFields)
	{
		BeforeSort(SortFields);
	}

	void __stdcall OnFieldGetDisplayText(IGRField *pField)
	{
		FieldGetDisplayText(pField);
	}

	void __stdcall OnTextBoxGetDisplayText(IGRTextBox *pTextBox)
	{
		TextBoxGetDisplayText(pTextBox);
	}
		
	void __stdcall OnSectionFormat(IGRSection *pSection)
	{
		SectionFormat(pSection);
	}

	void __stdcall OnProcessBegin(void)
	{
		ProcessBegin();
	}

	void __stdcall OnProcessEnd(void)
	{
		ProcessEnd();
	}

	void __stdcall OnGroupBegin(IGRGroup *pGroup)
	{
		GroupBegin(pGroup);
	}

	void __stdcall OnGroupEnd(IGRGroup *pGroup)
	{
		GroupEnd(pGroup);
	}

	void __stdcall OnProcessRecord(void)
	{
		ProcessRecord();
	}

	void __stdcall OnPageProcessRecord(void)
	{
		PageProcessRecord();
	}

	void __stdcall OnPageStart(void)
	{
		PageStart();
	}

	void __stdcall OnPageEnd(void)
	{
		PageEnd();
	}

	void __stdcall OnPrintBegin(void)
	{
		PrintBegin();
	}
	void __stdcall OnPrintEnd(void)
	{
		PrintEnd();
	}
	void __stdcall OnPrintPage(LONG PageNo)
	{
		PrintPage(PageNo);
	}
	void __stdcall OnPrintAborted(void)
	{
		PrintAborted();
	}

	void __stdcall OnExportBegin(IGRExportOption *pOptionObject)
	{
		ExportBegin(pOptionObject);
	}

	void __stdcall OnExportEnd(IGRExportOption *pOptionObject)
	{
		ExportEnd(pOptionObject);
	}

    static _ATL_FUNC_INFO OnBeforeSortInfo;
    static _ATL_FUNC_INFO OnFetchRecordInfo;
    static _ATL_FUNC_INFO OnFieldGetDisplayTextInfo;
    static _ATL_FUNC_INFO OnTextBoxGetDisplayTextInfo;
    static _ATL_FUNC_INFO OnSectionFormatInfo;
    static _ATL_FUNC_INFO OnGroupBeginInfo;
    static _ATL_FUNC_INFO OnGroupEndInfo;
	static _ATL_FUNC_INFO OnPrintPageInfo;
    static _ATL_FUNC_INFO OnExportBeginInfo;
	static _ATL_FUNC_INFO OnExportEndInfo;

private:
	//重写以下方法
	virtual void Initialize(void) {};
	virtual void FetchRecord(VARIANT_BOOL *pEof) {};
	virtual void BeforePostRecord(void) {};
	virtual void BeforeSort(BSTR SortFields) {};
	virtual void FieldGetDisplayText(IGRField *pField) {};
	virtual void TextBoxGetDisplayText(IGRTextBox *pTextBox) {};
	virtual void SectionFormat(IGRSection *pSection) {};
	virtual void ProcessBegin(void) {};
	virtual void ProcessEnd(void) {};
	virtual void GroupBegin(IGRGroup *pGroup) {};
	virtual void GroupEnd(IGRGroup *pGroup) {};
	virtual void ProcessRecord(void) {};
	virtual void PageProcessRecord(void) {};
	virtual void PageStart(void) {};
	virtual void PageEnd(void) {};
	virtual void PrintBegin(void) {};
	virtual void PrintEnd(void) {};
	virtual void PrintPage(LONG PageNo) {};
	virtual void PrintAborted(void) {};
	virtual void ExportBegin(IGRExportOption *pOptionObject) {};
	virtual void ExportEnd(IGRExportOption *pOptionObject) {};
};
