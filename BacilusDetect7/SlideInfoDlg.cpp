// SlideInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BacilusDetect.h"
#include "SlideInfoDlg.h"
#include "afxdialogex.h"
#include "BacilusDetectView.h"
#include "Mechanism.h"

extern CMechanism		g_Mechanism;


// CSlideInfoTab1 对话框

IMPLEMENT_DYNAMIC(CSlideInfoTab1, CDialog)

CSlideInfoTab1::CSlideInfoTab1(CWnd* pParent /*=NULL*/)
	: CDialog(CSlideInfoTab1::IDD, pParent)
	, m_SlideID(_T(""))
	, m_DetectData(COleDateTime::GetCurrentTime())
	, m_DetectTime(COleDateTime::GetCurrentTime())
	, m_Type(_T(""))
	, m_Quantity(_T(""))
	, m_Staytime(_T(""))
	, m_Method(_T(""))
	, m_SendDoctor(_T(""))
{

}

CSlideInfoTab1::~CSlideInfoTab1()
{
}

void CSlideInfoTab1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SLIDEID, m_SlideID);
	DDX_DateTimeCtrl(pDX, IDC_DETECTDATE, m_DetectData);
	DDX_DateTimeCtrl(pDX, IDC_DETECTTIME, m_DetectTime);
	DDX_Text(pDX, IDC_EDIT_TYPE, m_Type);
	DDX_Text(pDX, IDC_EDIT_QUANTITY, m_Quantity);
	DDX_Text(pDX, IDC_EDIT_STAYTIME, m_Staytime);
	DDX_Text(pDX, IDC_EDIT_METHOD, m_Method);
	DDX_Text(pDX, IDC_EDIT_SENDDOCTOR, m_SendDoctor);
}


BEGIN_MESSAGE_MAP(CSlideInfoTab1, CDialog)
	ON_COMMAND(IDOK, &CSlideInfoTab1::OnIdok)
	ON_COMMAND(IDCANCEL, &CSlideInfoTab1::OnIdcancel)
END_MESSAGE_MAP()

// CSlideInfoTab1 消息处理程序
void CSlideInfoTab1::OnIdok()
{
	return;
}

void CSlideInfoTab1::OnIdcancel()
{
	return;
}

// CSlideInfoTab2 对话框

IMPLEMENT_DYNAMIC(CSlideInfoTab2, CDialog)

CSlideInfoTab2::CSlideInfoTab2(CWnd* pParent /*=NULL*/)
	: CDialog(CSlideInfoTab2::IDD, pParent)
	, m_Judge(_T(""))
	, m_DetectResult(_T(""))
	, m_BacilusDensity(_T(""))
{

}

CSlideInfoTab2::~CSlideInfoTab2()
{
}

void CSlideInfoTab2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_JUDEG, m_Judge);
	DDX_Text(pDX, IDC_EDIT_DETECTRESELT, m_DetectResult);
	DDX_Text(pDX, IDC_EDIT_BACILUSDENSE, m_BacilusDensity);
}


BEGIN_MESSAGE_MAP(CSlideInfoTab2, CDialog)
	ON_COMMAND(IDOK, &CSlideInfoTab2::OnIdok)
	ON_COMMAND(IDCANCEL, &CSlideInfoTab2::OnIdcancel)
END_MESSAGE_MAP()

// CSlideInfoTab2 消息处理程序
void CSlideInfoTab2::OnIdok()
{
	return;
}

void CSlideInfoTab2::OnIdcancel()
{
	return;
}


// CSlideInfoTab3 对话框

IMPLEMENT_DYNAMIC(CSlideInfoTab3, CDialog)

CSlideInfoTab3::CSlideInfoTab3(CWnd* pParent /*=NULL*/)
	: CDialog(CSlideInfoTab3::IDD, pParent)
	, m_PatientID(_T(""))
	, m_BedID(_T(""))
	, m_Name(_T(""))
	, m_Section(_T(""))
	, m_Year(0)
	, m_Occupation(_T(""))
	, m_Telephone(_T(""))
	, m_Address(_T(""))
	, m_nGender(0)
{
}

CSlideInfoTab3::~CSlideInfoTab3()
{
}

void CSlideInfoTab3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PATIENTID, m_PatientID);
	DDX_Text(pDX, IDC_EDIT_BEDID, m_BedID);
	DDX_Text(pDX, IDC_EDIT_NAME, m_Name);
	DDX_Text(pDX, IDC_EDIT_SECTION, m_Section);
	DDX_Text(pDX, IDC_EDIT_YEAR, m_Year);
	DDV_MinMaxInt(pDX, m_Year, 0, 1000000000);
	DDX_Text(pDX, IDC_EDIT_OCCUPATION, m_Occupation);
	DDX_Text(pDX, IDC_EDIT_TELEPHONE, m_Telephone);
	DDX_Text(pDX, IDC_EDIT_ADDRESS, m_Address);
	DDX_CBIndex(pDX, IDC_COMBO_GENDER, m_nGender);
}


BEGIN_MESSAGE_MAP(CSlideInfoTab3, CDialog)
	ON_COMMAND(IDOK, &CSlideInfoTab3::OnIdok)
	ON_COMMAND(IDCANCEL, &CSlideInfoTab3::OnIdcancel)
END_MESSAGE_MAP()

// CSlideInfoTab3 消息处理程序
void CSlideInfoTab3::OnIdok()
{
	return;
}

void CSlideInfoTab3::OnIdcancel()
{
	return;
}

// CSlideInfoDlg 对话框

IMPLEMENT_DYNAMIC(CSlideInfoDlg, CDialogEx)

CSlideInfoDlg::CSlideInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSlideInfoDlg::IDD, pParent)
{
	m_pSlideInfo = NULL;
}

CSlideInfoDlg::~CSlideInfoDlg()
{
}

BOOL CSlideInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CBacilusDetectView* pView = (CBacilusDetectView*)( ((CFrameWnd*)AfxGetMainWnd())->GetActiveView() );

	m_pDocument = (CBacilusDetectDoc*) ((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument();
	
	m_TabCtrl_SlideInfo.InsertItem(0, _T("玻片信息"));
	m_TabCtrl_SlideInfo.InsertItem(1, _T("诊断结果"));
	m_TabCtrl_SlideInfo.InsertItem(2, _T("病人信息"));
	
	m_Tab0.Create(IDD_DLG_SLIDEINFO_TAB1, &m_TabCtrl_SlideInfo);
	m_Tab1.Create(IDD_DLG_SLIDEINFO_TAB2, &m_TabCtrl_SlideInfo);
	m_Tab2.Create(IDD_DLG_SLIDEINFO_TAB3, &m_TabCtrl_SlideInfo);
	CRect rc;
	m_TabCtrl_SlideInfo.GetClientRect(rc);
	rc.top += 30;
	rc.bottom -= 10;
	rc.left += 10;
	rc.right -= 10;
	m_Tab0.MoveWindow(&rc);	
	m_Tab1.MoveWindow(&rc);
	m_Tab2.MoveWindow(&rc);

	UpdataSlidInfo2Dlg();
	
	m_Tab0.ShowWindow(SW_SHOW);
	m_Tab1.ShowWindow(SW_HIDE);
	m_Tab2.ShowWindow(SW_HIDE);

	m_pDialog[0] = &m_Tab0;
	m_pDialog[1] = &m_Tab1;
	m_pDialog[2] = &m_Tab2;

	m_CurSelTab = 0;
	m_TabCtrl_SlideInfo.SetCurFocus(m_CurSelTab);
	
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSlideInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_SLIDEINFO, m_TabCtrl_SlideInfo);
	//  DDX_CBIndex(pDX, IDC_COMBO_WORKPOSITION, m_nWorkPosition);
	//  DDV_MinMaxInt(pDX, m_nWorkPosition, 0, 4);
}


BEGIN_MESSAGE_MAP(CSlideInfoDlg, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SLIDEINFO, &CSlideInfoDlg::OnTcnSelchangeTabSlideinfo)
// 	ON_CBN_SELCHANGE(IDC_COMBO_WORKPOSITION, &CSlideInfoDlg::OnCbnSelchangeComboWorkposition)
END_MESSAGE_MAP()

void    CSlideInfoDlg::UpdataSlidInfo2Dlg()
{
	m_pSlideInfo = m_pDocument->GetSlideInfo();  // 获得当前工位上的玻片信息地址，并初始化该对话框
	
	m_Tab0.m_SlideID = m_pSlideInfo->strSlideID;	
	m_Tab0.m_Method = m_pSlideInfo->strMethod;
	m_Tab0.m_Quantity = m_pSlideInfo->strQuantity;
	m_Tab0.m_SendDoctor = m_pSlideInfo->strSendDoctor;
	m_Tab0.m_Staytime = m_pSlideInfo->strStayTime;
	m_Tab0.m_Type = m_pSlideInfo->strType;
	if ( m_Tab0.m_SlideID.IsEmpty() )
	{
		m_Tab0.m_DetectData = COleDateTime::GetCurrentTime();
		m_Tab0.m_DetectTime = COleDateTime::GetCurrentTime();
	}
	else
	{
		m_Tab0.m_DetectData =  m_pSlideInfo->dtDetectData;
		m_Tab0.m_DetectTime =  m_pSlideInfo->dtDetectTime;
	}

	m_Tab1.m_Judge = m_pSlideInfo->strJudge;
	m_Tab1.m_DetectResult = m_pSlideInfo->strDetectResult;
	m_Tab1.m_BacilusDensity = m_pSlideInfo->strBacilusDensity;

	m_Tab2.m_Address = m_pSlideInfo->strAddress;
	m_Tab2.m_BedID = m_pSlideInfo->strBedID;
	m_Tab2.m_Name = m_pSlideInfo->strName;
	m_Tab2.m_nGender = m_pSlideInfo->nGender;
	m_Tab2.m_Occupation = m_pSlideInfo->strOccupation;
	m_Tab2.m_PatientID = m_pSlideInfo->strPatientID;
	m_Tab2.m_Section = m_pSlideInfo->strDepartment;
	m_Tab2.m_Telephone = m_pSlideInfo->strTelephone;
	m_Tab2.m_Year = m_pSlideInfo->nYear;

	m_Tab0.UpdateData(FALSE);
	m_Tab1.UpdateData(FALSE);
	m_Tab2.UpdateData(FALSE);
}

void    CSlideInfoDlg::UpdataDlg2SlidInfo()
{
	m_Tab0.UpdateData(TRUE);
	m_Tab1.UpdateData(TRUE);
	m_Tab2.UpdateData(TRUE);

	m_pSlideInfo = m_pDocument->GetSlideInfo();  // 获得当前工位上的玻片信息地址，并初始化该对话框

	m_pSlideInfo->strSlideID = m_Tab0.m_SlideID;
	m_pSlideInfo->strMethod = m_Tab0.m_Method;
	m_pSlideInfo->strQuantity = m_Tab0.m_Quantity;
	m_pSlideInfo->strSendDoctor = m_Tab0.m_SendDoctor;
	m_pSlideInfo->strStayTime = m_Tab0.m_Staytime;
	m_pSlideInfo->strType = m_Tab0.m_Type;
	m_pSlideInfo->dtDetectData = m_Tab0.m_DetectData;
	m_pSlideInfo->dtDetectTime = m_Tab0.m_DetectTime;
	
	m_pSlideInfo->strJudge = m_Tab1.m_Judge;
	m_pSlideInfo->strDetectResult = m_Tab1.m_DetectResult;
	m_pSlideInfo->strBacilusDensity = m_Tab1.m_BacilusDensity;
	
	m_pSlideInfo->strAddress = 	m_Tab2.m_Address;
	m_pSlideInfo->strBedID = m_Tab2.m_BedID;
	m_pSlideInfo->strName = m_Tab2.m_Name;
	m_pSlideInfo->nGender = (GENDER)m_Tab2.m_nGender;
	m_pSlideInfo->strOccupation = m_Tab2.m_Occupation;
	m_pSlideInfo->strPatientID = m_Tab2.m_PatientID;
	m_pSlideInfo->strDepartment = m_Tab2.m_Section;
	m_pSlideInfo->strTelephone = m_Tab2.m_Telephone;
	m_pSlideInfo->nYear = m_Tab2.m_Year;	
}

// CSlideInfoDlg 消息处理程序

void CSlideInfoDlg::OnTcnSelchangeTabSlideinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_pDialog[m_CurSelTab]->ShowWindow(SW_HIDE);
	m_CurSelTab = m_TabCtrl_SlideInfo.GetCurSel();
	m_pDialog[m_CurSelTab]->ShowWindow(SW_SHOW);

	*pResult = 0;
}

void CSlideInfoDlg::OnOK()
{
	UpdateData(TRUE);
	int i = 0;
	int count = m_TabCtrl_SlideInfo.GetItemCount();

	CSlideInfoTab1* p = (CSlideInfoTab1*)m_pDialog[0];            // 设置相应工位上玻片的相关信息
	CEdit* pEdit = (CEdit*) ( p->GetDlgItem(IDC_EDIT_SLIDEID) );
	if ( pEdit->LineLength() <= 0 )      // 玻片编号不为空
	{
		AfxMessageBox(_T("如果工位上的玻片编号为空，则在自动扫描检验过程中则不会对该工位进行任何操作!"));		
	}

	UpdataDlg2SlidInfo();
}

void CSlideInfoDlg::OnCancel()
{
	int i = 0;
	int count = m_TabCtrl_SlideInfo.GetItemCount();
    for ( i=0; i<count; i++ )
	{
		m_pDialog[i]->UpdateData(FALSE);
	}
	
	CString str1,str2,str;
	if (m_pDocument->GetSlideID().IsEmpty())
	{
		str = _T("玻片编号为空！\n\n如果工位上的玻片编号为空，则在自动扫描检验过程中则不会对该工位进行任何操作!\n\n是否仍然退出该对话框？");

		if ( AfxMessageBox(str, MB_OKCANCEL|MB_ICONQUESTION ) == IDCANCEL )
		{
			return;
		}
	}

	CDialogEx::OnCancel();
}
