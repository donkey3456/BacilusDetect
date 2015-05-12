#pragma once

#include <string>

#include "BacilusDetectDoc.h"
#include "GridppReportEventImpl.h"
#include "GRImport.h"
#include "atlcomtime.h"

// CReport 对话框

class CReport : public CDialogEx
{
	DECLARE_DYNAMIC(CReport)

public:
	CReport(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CReport();

// 对话框数据
	enum { IDD = IDD_DLG_REPORT };
private:
	CRect m_RectLarge;
	CRect m_RectSmall;

// 报表数据
private:
	IGridppReportPtr	m_pGridppReport; 
	IGRDisplayViewerPtr m_pDisplayViewer;

// 控件数据
private:
	CString m_strSlideID;
	CString m_strName;
	int m_nGender;
	int m_nYear;
	CString m_strDepartment;
	CString m_strBedID;
	CString m_strPatientID;
	CString m_strOccupation;
	CString m_strAddress;
	CString m_strSendDoctor;
	CString m_strType;
	CString m_strStayTime;
	CString m_strMethod;
	CString m_strQuantity;
	CString m_strCompareJudge;
	CString m_strCompareDetectResult;
	CString m_strCompareBacilusDensity;
	CString m_strReportDoctor;
	COleDateTime m_dtReportData;
	COleDateTime m_dtDetectData;

private:
	PSLIDEINFO			m_pSlideInfo;
	CBacilusDetectDoc*	m_pDocument;

//内部操作
private:
	void    UpdataSlidInfo2Dlg();
	void    UpdataDlg2SlidInfo();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedPrint();
	afx_msg void OnBnClickedPreview();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnClickedButtonConfirm();
	afx_msg void OnBnClickedChReport();
//	afx_msg void OnBnClickedOk();
//	afx_msg void OnBnClickedCancel();
};
