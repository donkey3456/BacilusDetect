// Report.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BacilusDetect.h"
#include "Report.h"
#include "afxdialogex.h"

// CReport �Ի���



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

//�ڲ�����
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
	m_pSlideInfo = m_pDocument->GetSlideInfo();  // ��õ�ǰ��λ�ϵĲ�Ƭ��Ϣ��ַ������ʼ���öԻ���
	UpdataSlidInfo2Dlg();

	//��������������
	m_pGridppReport.CreateInstance( __uuidof(GridppReport) );
	ATLASSERT(m_pGridppReport != NULL);

	//ȡ�ò�ѯ��ʾ���ؼ��Ľӿ�ָ��
	CWnd *pDispalyViewerWnd = GetDlgItem(IDC_GRDISPLAYVIEWER1);
	LPUNKNOWN spUnk = pDispalyViewerWnd->GetControlUnknown();
	spUnk->QueryInterface(__uuidof(IGRDisplayViewer), (void**)(&m_pDisplayViewer));
	ATLASSERT(m_pDisplayViewer != NULL);

	//���ļ������뱨��ģ�����ݵ�����������
	std::string FileName =  "report.grf";
	m_pGridppReport->LoadFromFile( FileName.c_str() );

	//��ѯ��ʾ���ؼ���������������
	m_pDisplayViewer->Report = m_pGridppReport;

	//������ѯ��ʾ��������
	m_pDisplayViewer->Start();

	// ���±���
	OnBnClickedButtonConfirm();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

// CReport ��Ϣ�������

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
	GetDlgItem(IDC_CH_REPORT)->SetWindowText(_T("���Ĳ�����>>"));
	GetDlgItem(IDC_BTN_PREVIEW)->SetWindowText(_T("��ӡԤ��"));
	
	if (IDOK != AfxMessageBox(_T("Ԥ��������?"), MB_OKCANCEL|MB_ICONQUESTION ))
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
	//m_pDocument->SaveSlideData(m_nWorkPosition);//�������ݿ�Ľӿ�
	CDialogEx::OnOK();
}

void CReport::OnCancel()
{
	CDialogEx::OnCancel();
}

// ���±���
void CReport::OnBnClickedButtonConfirm()
{
	UpdataDlg2SlidInfo();
	
	CString Str;

	m_pDisplayViewer->Stop();
	
	//����
	m_pGridppReport->ControlByName("MemoBox_PName")->AsMemoBox->Text = (_bstr_t)m_strName;

	//�Ա�
	GetDlgItem(IDC_COMBO_GENDER)->GetWindowText(Str);
	m_pGridppReport->ControlByName("MemoBox_Gender")->AsMemoBox->Text = (_bstr_t)Str;

	//����
	CString str_age;
	if ( m_nYear>0 )
	{
		str_age.Format(_T("%d"),m_nYear);
	}
	m_pGridppReport->ControlByName("MemoBox_Age")->AsMemoBox->Text = (_bstr_t)str_age;


	//����
	m_pGridppReport->ControlByName("MemoBox_RoomNum")->AsMemoBox->Text = (_bstr_t)m_strDepartment;

	//����
	m_pGridppReport->ControlByName("MemoBox_SickbedNum")->AsMemoBox->Text = (_bstr_t)m_strBedID;

	//סԺ��
	m_pGridppReport->ControlByName("MemoBox_PatientNum")->AsMemoBox->Text = (_bstr_t)m_strPatientID;

    //ְҵ
	m_pGridppReport->ControlByName("MemoBox_Work")->AsMemoBox->Text = (_bstr_t)m_strOccupation;

	//סַ
	m_pGridppReport->ControlByName("MemoBox_Add")->AsMemoBox->Text = (_bstr_t)m_strAddress;

	//�걾���
	m_pGridppReport->ControlByName("MemoBox_SliderIndex")->AsMemoBox->Text = (_bstr_t)m_strSlideID;

	//�ͼ�ҽ��
	m_pGridppReport->ControlByName("MemoBox_S_Doctor")->AsMemoBox->Text = (_bstr_t)m_strSendDoctor;

	//�ͼ�����
	GetDlgItem(IDC_DATETIMEPICKER_SEND_DATE)->GetWindowText(Str);
	m_pGridppReport->ControlByName("MemoBox_S_Date")->AsMemoBox->Text = (_bstr_t)Str;

	//�걾���
	m_pGridppReport->ControlByName("MemoBox_Type")->AsMemoBox->Text = (_bstr_t)m_strType;

	//��̵ʱ��
	m_pGridppReport->ControlByName("MemoBox_Time")->AsMemoBox->Text = (_bstr_t)m_strStayTime;

	//Ⱦɫ����
	m_pGridppReport->ControlByName("MemoBox_Method")->AsMemoBox->Text = (_bstr_t)m_strMethod;

	//̵�걾��
	m_pGridppReport->ControlByName("MemoBox_Volume")->AsMemoBox->Text = (_bstr_t)m_strQuantity;

	//�ٴ����
	m_pGridppReport->ControlByName("MemoBox_PathologyResult")->AsMemoBox->Text = (_bstr_t)m_strCompareJudge;

	//������
	m_pGridppReport->ControlByName("MemoBox_LesionLevel")->AsMemoBox->Text = (_bstr_t)m_strCompareDetectResult;

	//ϸ��Ũ��ֵ
	m_pGridppReport->ControlByName("MemoBox_Concentration")->AsMemoBox->Text = (_bstr_t)m_strCompareBacilusDensity;

	//����ҽʦ
	m_pGridppReport->ControlByName("MemoBox_R_Doc")->AsMemoBox->Text = (_bstr_t)m_strReportDoctor;

	//��������
	GetDlgItem(IDC_DATETIMEPICKER_REP_DATE)->GetWindowText(Str);
	m_pGridppReport->ControlByName("MemoBox_R_Date")->AsMemoBox->Text = (_bstr_t)Str;

	//����ͼ��
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
	GetDlgItem(IDC_CH_REPORT)->SetWindowText(_T("���Ĳ�����"));
	GetDlgItem(IDC_BTN_PREVIEW)->SetWindowText(_T("��ӡԤ��<<"));
	
	
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
