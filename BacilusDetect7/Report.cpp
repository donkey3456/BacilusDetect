// Report.cpp : 实现文件
//

#include "stdafx.h"
#include "BacilusDetect.h"
#include "Report.h"
#include "afxdialogex.h"

// CReport 对话框



IMPLEMENT_DYNAMIC(CReport, CDialogEx)

CReport::CReport(CWnd* pParent /*=NULL*/)
	: CDialogEx(CReport::IDD, pParent)
	, m_strSlideID(_T(""))
	, m_strName(_T(""))
	, m_nGender(0)
	, m_nYear(0)
	, m_strDepartment(_T(""))
	, m_strBedID(_T(""))
	, m_strPatientID(_T(""))
	, m_strOccupation(_T(""))
	, m_strAddress(_T(""))
	, m_strSendDoctor(_T(""))
	, m_strType(_T(""))
	, m_strStayTime(_T(""))
	, m_strMethod(_T(""))
	, m_strQuantity(_T(""))
	, m_strCompareJudge(_T(""))
	, m_strCompareDetectResult(_T(""))
	, m_strCompareBacilusDensity(_T(""))
	, m_strReportDoctor(_T(""))
	, m_dtReportData(COleDateTime::GetCurrentTime())
	, m_dtDetectData(COleDateTime::GetCurrentTime())
{
	m_RectLarge = CRect(0, 0, 0, 0);
	m_RectSmall = CRect(0, 0, 0, 0);
}

CReport::~CReport()
{
}

void CReport::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SliderIndex, m_strSlideID);
	DDX_Text(pDX, IDC_EDIT_PNAME, m_strName);
	DDX_CBIndex(pDX, IDC_COMBO_GENDER, m_nGender);
	DDX_Text(pDX, IDC_EDIT_AGE, m_nYear);
	DDX_Text(pDX, IDC_EDIT_SECTION, m_strDepartment);
	DDX_Text(pDX, IDC_EDIT_BED, m_strBedID);
	DDX_Text(pDX, IDC_EDIT_INNO, m_strPatientID);
	DDX_Text(pDX, IDC_EDIT_WORK, m_strOccupation);
	DDX_Text(pDX, IDC_EDIT_ADDR, m_strAddress);
	DDX_Text(pDX, IDC_EDIT_SEND_DOC, m_strSendDoctor);
	DDX_Text(pDX, IDC_EDIT_SORT, m_strType);
	DDX_Text(pDX, IDC_EDIT_S_TIME, m_strStayTime);
	DDX_Text(pDX, IDC_EDIT_METHOD, m_strMethod);
	DDX_Text(pDX, IDC_EDIT_QUATITY, m_strQuantity);
	DDX_Text(pDX, IDC_EDIT_DIAG, m_strCompareJudge);
	DDX_Text(pDX, IDC_EDIT_RESULT, m_strCompareDetectResult);
	DDX_Text(pDX, IDC_EDIT_CHROMA, m_strCompareBacilusDensity);
	DDX_Text(pDX, IDC_EDIT_REP_DOC, m_strReportDoctor);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_REP_DATE, m_dtReportData);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_SEND_DATE, m_dtDetectData);
}

//内部操作
void    CReport::UpdataSlidInfo2Dlg()
{
	m_strSlideID = m_pSlideInfo->strSlideID;	
	m_strMethod = m_pSlideInfo->strMethod;
	m_strQuantity = m_pSlideInfo->strQuantity;
	m_strSendDoctor = m_pSlideInfo->strSendDoctor;
	m_strStayTime = m_pSlideInfo->strStayTime;
	m_strType = m_pSlideInfo->strType;
	m_dtDetectData =  m_pSlideInfo->dtDetectData;
	m_strCompareJudge = m_pSlideInfo->strCompareJudge;
	m_strCompareDetectResult = m_pSlideInfo->strCompareDetectResult;
	m_strCompareBacilusDensity = m_pSlideInfo->strCompareBacilusDensity;
	m_strAddress = m_pSlideInfo->strAddress;
	m_strBedID = m_pSlideInfo->strBedID;
	m_strName = m_pSlideInfo->strName;
	m_nGender = m_pSlideInfo->nGender;
	m_strOccupation = m_pSlideInfo->strOccupation;
	m_strPatientID = m_pSlideInfo->strPatientID;
	m_strDepartment = m_pSlideInfo->strDepartment;
	m_nYear = m_pSlideInfo->nYear;
	m_strReportDoctor = m_pSlideInfo->strReportDoctor;
	
	UpdateData(FALSE);
}

void    CReport::UpdataDlg2SlidInfo()
{
	UpdateData(TRUE);

	m_pSlideInfo->strSlideID = m_strSlideID;	
	m_pSlideInfo->strMethod = m_strMethod;
	m_pSlideInfo->strQuantity = m_strQuantity;
	m_pSlideInfo->strSendDoctor = m_strSendDoctor;
	m_pSlideInfo->strStayTime = m_strStayTime;
	m_pSlideInfo->strType = m_strType;
	m_pSlideInfo->dtDetectData = m_dtDetectData;
	m_pSlideInfo->strCompareJudge = m_strCompareJudge;
	m_pSlideInfo->strCompareDetectResult = m_strCompareDetectResult;
	m_pSlideInfo->strCompareBacilusDensity = m_strCompareBacilusDensity;
	m_pSlideInfo->strAddress = m_strAddress;
	m_pSlideInfo->strBedID = m_strBedID;
	m_pSlideInfo->strName = m_strName;
	m_pSlideInfo->nGender = (GENDER)m_nGender;
	m_pSlideInfo->strOccupation = m_strOccupation;
	m_pSlideInfo->strPatientID = m_strPatientID;
	m_pSlideInfo->strDepartment = m_strDepartment;
	m_pSlideInfo->nYear = m_nYear;
	m_pSlideInfo->strReportDoctor = m_strReportDoctor;	
	m_pSlideInfo->dtReportData = m_dtReportData;
}

BEGIN_MESSAGE_MAP(CReport, CDialogEx)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_PRINT, &CReport::OnBnClickedPrint)
	ON_BN_CLICKED(IDC_BTN_PREVIEW, &CReport::OnBnClickedPreview)
	ON_BN_CLICKED(IDC_BUTTON_CONFIRM, &CReport::OnBnClickedButtonConfirm)
	ON_BN_CLICKED(IDC_CH_REPORT, &CReport::OnBnClickedChReport)
//	ON_BN_CLICKED(IDOK, &CReport::OnBnClickedOk)
//	ON_BN_CLICKED(IDCANCEL, &CReport::OnBnClickedCancel)
END_MESSAGE_MAP()

BOOL CReport::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (m_RectLarge.IsRectNull())
	{
		CRect rectSeparater;
		GetWindowRect(&m_RectLarge);
		GetDlgItem(IDC_SEPERATOR)->GetWindowRect(&rectSeparater);
		
		m_RectSmall.left = m_RectLarge.left;
		m_RectSmall.top = m_RectLarge.top;
		m_RectSmall.right = rectSeparater.right;
		m_RectSmall.bottom = m_RectLarge.bottom;
	}
	
	SetWindowPos(NULL,100,100,m_RectSmall.Width(),m_RectSmall.Height(),SWP_NOMOVE|SWP_NOZORDER);
	
	RECT DTWndRt;
	CWnd* DtWnd = GetDesktopWindow();
	DtWnd->GetClientRect(&DTWndRt);
	
	RECT ClientRect;
	GetClientRect( &ClientRect );
	
	RECT NewRt;
	NewRt.left = (DTWndRt.right-DTWndRt.left)/2 - (ClientRect.right-ClientRect.left)/2;
	NewRt.top = (DTWndRt.bottom - DTWndRt.top)/2 - (ClientRect.bottom - ClientRect.top)/2;
	NewRt.right = NewRt.left + m_RectSmall.Width();
	NewRt.bottom = NewRt.top + m_RectSmall.Height();
	
	MoveWindow(&NewRt, TRUE);

	m_pDocument = (CBacilusDetectDoc*) ((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument();
	m_pSlideInfo = m_pDocument->GetSlideInfo();  // 获得当前工位上的玻片信息地址，并初始化该对话框
	UpdataSlidInfo2Dlg();

	//创建报表主对象
	m_pGridppReport.CreateInstance( __uuidof(GridppReport) );
	ATLASSERT(m_pGridppReport != NULL);

	//取得查询显示器控件的接口指针
	CWnd *pDispalyViewerWnd = GetDlgItem(IDC_GRDISPLAYVIEWER1);
	LPUNKNOWN spUnk = pDispalyViewerWnd->GetControlUnknown();
	spUnk->QueryInterface(__uuidof(IGRDisplayViewer), (void**)(&m_pDisplayViewer));
	ATLASSERT(m_pDisplayViewer != NULL);

	//从文件中载入报表模板数据到报表主对象
	std::string FileName =  "report.grf";
	m_pGridppReport->LoadFromFile( FileName.c_str() );

	//查询显示器控件关联报表主对象
	m_pDisplayViewer->Report = m_pGridppReport;

	//启动查询显示器的运行
	m_pDisplayViewer->Start();

	// 更新报表
	OnBnClickedButtonConfirm();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// CReport 消息处理程序

void CReport::OnDestroy()
{
	CDialogEx::OnDestroy();

	m_pDisplayViewer.Release();
	m_pGridppReport.Release();
}


void CReport::OnBnClickedPrint()
{
	m_pGridppReport->Print(TRUE);
}


void CReport::OnBnClickedPreview()
{
	GetDlgItem(IDC_CH_REPORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_CH_REPORT)->SetWindowText(_T("更改病理报告>>"));
	GetDlgItem(IDC_BTN_PREVIEW)->SetWindowText(_T("打印预览"));
	
	if (IDOK != AfxMessageBox(_T("预览病理报告?"), MB_OKCANCEL|MB_ICONQUESTION ))
		return;
	
	SetWindowPos(NULL,0,0,m_RectSmall.Width(),m_RectSmall.Height(),SWP_NOMOVE|SWP_NOZORDER);
	
	
	RECT DTWndRt;
	CWnd* DtWnd = GetDesktopWindow();
	DtWnd->GetClientRect(&DTWndRt);
	
	RECT ClientRect;
	GetClientRect( &ClientRect );
	
	RECT NewRt;
	NewRt.left = (DTWndRt.right-DTWndRt.left)/2 - (ClientRect.right-ClientRect.left)/2;
	NewRt.top = (DTWndRt.bottom - DTWndRt.top)/2 - (ClientRect.bottom - ClientRect.top)/2;
	NewRt.right = NewRt.left + m_RectSmall.Width();
	NewRt.bottom = NewRt.top + m_RectSmall.Height();
	
	MoveWindow(&NewRt, TRUE);
	
	m_pGridppReport->PrintPreview(TRUE);
}


void CReport::OnOK()
{
	m_pDocument->SetSlideJudgeStatus( TRUE);
	//m_pDocument->SaveSlideData(m_nWorkPosition);//存入数据库的接口
	CDialogEx::OnOK();
}

void CReport::OnCancel()
{
	CDialogEx::OnCancel();
}

// 更新报表
void CReport::OnBnClickedButtonConfirm()
{
	UpdataDlg2SlidInfo();
	
	CString Str;

	m_pDisplayViewer->Stop();
	
	//姓名
	m_pGridppReport->ControlByName("MemoBox_PName")->AsMemoBox->Text = (_bstr_t)m_strName;

	//性别
	GetDlgItem(IDC_COMBO_GENDER)->GetWindowText(Str);
	m_pGridppReport->ControlByName("MemoBox_Gender")->AsMemoBox->Text = (_bstr_t)Str;

	//年龄
	CString str_age;
	if ( m_nYear>0 )
	{
		str_age.Format(_T("%d"),m_nYear);
	}
	m_pGridppReport->ControlByName("MemoBox_Age")->AsMemoBox->Text = (_bstr_t)str_age;


	//科室
	m_pGridppReport->ControlByName("MemoBox_RoomNum")->AsMemoBox->Text = (_bstr_t)m_strDepartment;

	//床号
	m_pGridppReport->ControlByName("MemoBox_SickbedNum")->AsMemoBox->Text = (_bstr_t)m_strBedID;

	//住院号
	m_pGridppReport->ControlByName("MemoBox_PatientNum")->AsMemoBox->Text = (_bstr_t)m_strPatientID;

    //职业
	m_pGridppReport->ControlByName("MemoBox_Work")->AsMemoBox->Text = (_bstr_t)m_strOccupation;

	//住址
	m_pGridppReport->ControlByName("MemoBox_Add")->AsMemoBox->Text = (_bstr_t)m_strAddress;

	//标本编号
	m_pGridppReport->ControlByName("MemoBox_SliderIndex")->AsMemoBox->Text = (_bstr_t)m_strSlideID;

	//送检医生
	m_pGridppReport->ControlByName("MemoBox_S_Doctor")->AsMemoBox->Text = (_bstr_t)m_strSendDoctor;

	//送检日期
	GetDlgItem(IDC_DATETIMEPICKER_SEND_DATE)->GetWindowText(Str);
	m_pGridppReport->ControlByName("MemoBox_S_Date")->AsMemoBox->Text = (_bstr_t)Str;

	//标本类别
	m_pGridppReport->ControlByName("MemoBox_Type")->AsMemoBox->Text = (_bstr_t)m_strType;

	//留痰时间
	m_pGridppReport->ControlByName("MemoBox_Time")->AsMemoBox->Text = (_bstr_t)m_strStayTime;

	//染色方法
	m_pGridppReport->ControlByName("MemoBox_Method")->AsMemoBox->Text = (_bstr_t)m_strMethod;

	//痰标本量
	m_pGridppReport->ControlByName("MemoBox_Volume")->AsMemoBox->Text = (_bstr_t)m_strQuantity;

	//临床诊断
	m_pGridppReport->ControlByName("MemoBox_PathologyResult")->AsMemoBox->Text = (_bstr_t)m_strCompareJudge;

	//检验结果
	m_pGridppReport->ControlByName("MemoBox_LesionLevel")->AsMemoBox->Text = (_bstr_t)m_strCompareDetectResult;

	//细菌浓度值
	m_pGridppReport->ControlByName("MemoBox_Concentration")->AsMemoBox->Text = (_bstr_t)m_strCompareBacilusDensity;

	//报告医师
	m_pGridppReport->ControlByName("MemoBox_R_Doc")->AsMemoBox->Text = (_bstr_t)m_strReportDoctor;

	//报告日期
	GetDlgItem(IDC_DATETIMEPICKER_REP_DATE)->GetWindowText(Str);
	m_pGridppReport->ControlByName("MemoBox_R_Date")->AsMemoBox->Text = (_bstr_t)Str;

	//加载图像
	CFileFind find;
	CString strFileName = m_pDocument->GetDirectory();
	int nFOVIndex  = 0;
	if ( m_pDocument->GetFOVCount() > 0 )
	{
		nFOVIndex = m_pDocument->GetFOVIndex( 0);
		Str.Format(_T("\\%d.JPG"), nFOVIndex);
		strFileName += Str;
		if ( find.FindFile(strFileName) )
		{
			m_pGridppReport->ControlByName("FilePictureTitleBox1")->AsPictureBox->LoadFromFile( (LPCTSTR)strFileName );
		}
	}
		
	strFileName = m_pDocument->GetDirectory();
	if ( m_pDocument->GetFOVCount() > 1 )
	{
		nFOVIndex = m_pDocument->GetFOVIndex( 1);
		Str.Format(_T("\\%d.JPG"), nFOVIndex);
		strFileName += Str;
		if ( find.FindFile(strFileName) )
		{
			m_pGridppReport->ControlByName("FilePictureTitleBox2")->AsPictureBox->LoadFromFile( (LPCTSTR)strFileName );
		}
	}

	m_pDisplayViewer->Refresh();
}

void CReport::OnBnClickedChReport()
{
	GetDlgItem(IDC_CH_REPORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_CH_REPORT)->SetWindowText(_T("更改病理报告"));
	GetDlgItem(IDC_BTN_PREVIEW)->SetWindowText(_T("打印预览<<"));
	
	
	SetWindowPos(NULL,0,0,m_RectLarge.Width(),m_RectLarge.Height(),SWP_NOMOVE|SWP_NOZORDER);
	
	RECT DTWndRt;
	CWnd* DtWnd = GetDesktopWindow();
	DtWnd->GetClientRect(&DTWndRt);
	
	RECT ClientRect;
	GetClientRect( &ClientRect );
	
	RECT NewRt;
	NewRt.left = (DTWndRt.right-DTWndRt.left)/2 - (ClientRect.right-ClientRect.left)/2;
	NewRt.top = (DTWndRt.bottom - DTWndRt.top)/2 - (ClientRect.bottom - ClientRect.top)/2;
	NewRt.right = NewRt.left + m_RectLarge.Width();
	NewRt.bottom = NewRt.top + m_RectLarge.Height();
	
	MoveWindow(&NewRt, TRUE);
}


//void CReport::OnBnClickedOk()
//{
//	m_pDocument->SaveSlideData(m_nWorkPosition);
//	CDialogEx::OnOK();
//}


//void CReport::OnBnClickedCancel()
//{
//	CDialogEx::OnCancel();
//}
