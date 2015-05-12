#pragma once
#include "afxcmn.h"
#include "atlcomtime.h"
#include "define.h"
#include "BacilusDetectDoc.h"

// CSlideInfoTab1 对话框
class CSlideInfoTab1 : public CDialog
{
	DECLARE_DYNAMIC(CSlideInfoTab1)

public:
	CSlideInfoTab1(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSlideInfoTab1();

// 对话框数据
	enum { IDD = IDD_DLG_SLIDEINFO_TAB1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString			m_SlideID;
	COleDateTime	m_DetectData;
	COleDateTime	m_DetectTime;
	CString			m_Type;
	CString			m_Quantity;
	CString			m_Staytime;
	CString			m_Method;
	CString			m_SendDoctor;
	afx_msg void OnIdok();
	afx_msg void OnIdcancel();
};

// CSlideInfoTab2 对话框
class CSlideInfoTab2 : public CDialog
{
	DECLARE_DYNAMIC(CSlideInfoTab2)

public:
	CSlideInfoTab2(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSlideInfoTab2();

// 对话框数据
	enum { IDD = IDD_DLG_SLIDEINFO_TAB2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString		m_Judge;
	CString		m_DetectResult;
	CString		m_BacilusDensity;
	afx_msg void OnIdok();
	afx_msg void OnIdcancel();
};

// CSlideInfoTab3 对话框
class CSlideInfoTab3 : public CDialog
{
	DECLARE_DYNAMIC(CSlideInfoTab3)

public:
	CSlideInfoTab3(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSlideInfoTab3();

// 对话框数据
	enum { IDD = IDD_DLG_SLIDEINFO_TAB3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_PatientID;
	CString m_BedID;
	CString m_Name;
	CString m_Section;
	int m_Year;
	CString m_Occupation;
	CString m_Telephone;
	CString m_Address;
	int m_nGender;
	afx_msg void OnIdok();
	afx_msg void OnIdcancel();	
};

// CSlideInfoDlg 对话框
class CSlideInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSlideInfoDlg)

public:
	CSlideInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSlideInfoDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SLIDEINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

// 控件数据
private:
	CTabCtrl		m_TabCtrl_SlideInfo;
	int				m_CurSelTab;
	CSlideInfoTab1  m_Tab0;
	CSlideInfoTab2  m_Tab1;
	CSlideInfoTab3  m_Tab2;
	CDialog*        m_pDialog[3];

// 玻片信息
private:
	PSLIDEINFO		m_pSlideInfo;
	CBacilusDetectDoc* m_pDocument;

//内部操作
private:
	void    UpdataSlidInfo2Dlg();
	void    UpdataDlg2SlidInfo();

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeTabSlideinfo(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnOK();
	virtual void OnCancel();
// 	afx_msg void OnCbnSelchangeComboWorkposition();
};

