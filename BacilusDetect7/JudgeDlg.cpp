// JudgeDlg.cpp : ʵ���ļ�
// ������϶Ի���

#include "stdafx.h"
#include "BacilusDetect.h"
#include "JudgeDlg.h"
#include "afxdialogex.h"
#include "BacilusDetectView.h"
#include "Mechanism.h"
#include "MyBitmap.h"

extern CMechanism		g_Mechanism;

// CJudgeDlg �Ի���

// ȫ�ֱ���
CString			g_strDirectory;
int				g_iFOVIndex;

IMPLEMENT_DYNAMIC(CJudgeDlg, CDialogEx)

CJudgeDlg::CJudgeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CJudgeDlg::IDD, pParent)
	, m_nFOVIndex(0)
	, m_nFOVSequenceIndex(0)
	, m_SlideID(_T(""))
	, m_TargetCount(0)
	, m_JudgedTargetCount(0)
	, m_Total_TargetCount(0)
	, m_Total_JudgedTargetCount(0)
	, m_targetProbability(globalConfirmProbability)
{
	m_pView = (CView*)pParent;
	m_matImage = NULL;
}

CJudgeDlg::~CJudgeDlg()
{
}

void CJudgeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_JUDGE_SLIDEID, m_SlideID);
	DDX_Text(pDX, IDC_TARGETCOUNTINFOV, m_TargetCount);
	DDX_Text(pDX, IDC_TOTAL_TARGETCOUNT, m_Total_TargetCount);
	DDX_Text(pDX, IDC_TOTAL_JUDGED_TARGETCOUNT, m_Total_JudgedTargetCount);
	DDX_Control(pDX, IDC_SPIN_JUDGED_COUNT, m_spinJudgedTargetCount);
	DDX_Control(pDX, IDC_COMBO_FOV, m_comboSequenceIndex);
	DDX_Text(pDX, IDC_TARGETPROBABILITY, m_targetProbability);
	DDV_MinMaxDouble(pDX, m_targetProbability, 0.8, 1);
}


BEGIN_MESSAGE_MAP(CJudgeDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_FOV, &CJudgeDlg::OnCbnSelchangeComboFov)
	ON_EN_CHANGE(IDC_JUDGED_TARGETCOUNTINFOV, &CJudgeDlg::OnEnChangeJudgedTargetcountinfov)
	ON_BN_CLICKED(IDC_BTN_JUDGE, &CJudgeDlg::OnBnClickedBtnJudge)
	ON_BN_CLICKED(IDC_BTN_REPORT, &CJudgeDlg::OnBnClickedBtnReport)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

BOOL CJudgeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_spinJudgedTargetCount.SetBuddy(this->GetDlgItem(IDC_JUDGED_TARGETCOUNTINFOV));
	m_spinJudgedTargetCount.SetRange32(0, 1000000000);
	m_spinJudgedTargetCount.SetPos(0);

	m_pDocument = (CBacilusDetectDoc*) ((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument();
	CBacilusDetectView* pView = (CBacilusDetectView*)( ((CFrameWnd*)AfxGetMainWnd())->GetActiveView() );

	InitiateWorkPositionCombo();

	UpdateWorkPosition();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

// �ڲ�����
void	CJudgeDlg::InitiateWorkPositionCombo()
{
	CString strItem;	
	BOOL bScan, bJudge;

	if ( m_pDocument->GetSlideID().IsEmpty() )
	{
		m_SlideID = _T(" ��δ���벣Ƭ��Ϣ��");
		GetDlgItem(IDC_BTN_REPORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_JUDGE)->EnableWindow(FALSE);
	}
	else
	{
		m_pDocument->GetScanJudgeStatus(bScan, bJudge);
		if ( !bScan && !bJudge )
		{
			strItem = _T("��δɨ�衢δ��ϣ�");
			GetDlgItem(IDC_BTN_REPORT)->EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_JUDGE)->EnableWindow(FALSE);
		}
		else if ( !bScan && bJudge ) 
		{
			strItem =_T("��δɨ�衢����ϣ�");
			GetDlgItem(IDC_BTN_REPORT)->EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_JUDGE)->EnableWindow(FALSE);
		}
		else if ( bScan && !bJudge )
		{
			strItem = _T("����ɨ�衢δ��ϣ�");
			GetDlgItem(IDC_BTN_REPORT)->EnableWindow(TRUE);
			GetDlgItem(IDC_BTN_JUDGE)->EnableWindow(TRUE);
		}
		else 
		{
			strItem = _T("����ɨ�衢����ϣ�");
			GetDlgItem(IDC_BTN_REPORT)->EnableWindow(TRUE);
			GetDlgItem(IDC_BTN_JUDGE)->EnableWindow(TRUE);
		}
		m_SlideID = m_pDocument->GetSlideID() + strItem;
	}
	UpdateData(FALSE);
	
}

void	CJudgeDlg::UpdateWorkPosition()
{
	m_comboSequenceIndex.ResetContent();

	// ����ͼ���ļ���Ŀ¼
	g_strDirectory = m_pDocument->GetDirectory();

	if ( m_SlideID.IsEmpty() )
	{
		m_TargetCount = 0;
		m_Total_TargetCount = 0;
		m_Total_JudgedTargetCount = 0;
		m_spinJudgedTargetCount.SetPos(0);
		GetDlgItem(IDC_BTN_REPORT)->EnableWindow(FALSE);
	}
	else
	{
		BOOL bScan, bJudge;
		m_pDocument->GetScanJudgeStatus(bScan, bJudge);
		if ( bScan )       // ��ʼ�����ƽ�˸˾���Ұ����ؼ�����
		{
			m_pDocument->SetFOVSequence();       // ��������
			int nCount = m_pDocument->GetFOVCount();
			_ASSERTE(nCount>0);
			CString strItem;
			for(m_nFOVSequenceIndex=0; m_nFOVSequenceIndex<nCount; m_nFOVSequenceIndex++)
			{
				m_nFOVIndex = m_pDocument->GetFOVIndex( m_nFOVSequenceIndex);
				if (m_pDocument->GetFOVCheckedStatus( m_nFOVIndex)) 
				{
					strItem.Format(_T("%3d���Ѽ�飩"), m_nFOVIndex+1);
				}
				else
				{
					strItem.Format(_T("%3d��δ��飩"), m_nFOVIndex+1);
				}
				m_comboSequenceIndex.InsertString(m_nFOVSequenceIndex, strItem);
			}
			m_comboSequenceIndex.SetCurSel(0);

			m_nFOVSequenceIndex = m_comboSequenceIndex.GetCurSel();
			if ( m_nFOVSequenceIndex == CB_ERR )
			{
				return;
			}
			m_nFOVIndex = m_pDocument->GetFOVIndex( m_nFOVSequenceIndex);

			m_TargetCount = m_pDocument->GetTargetCountInFOV( m_nFOVIndex);
			m_Total_TargetCount = m_pDocument->GetTargetCountInSlide();
			m_Total_JudgedTargetCount = m_pDocument->GetJudgedTargetCountInSlide();
			m_JudgedTargetCount = m_pDocument->GetJudgedTargetCountInFOV( m_nFOVIndex);
			m_spinJudgedTargetCount.SetPos(m_JudgedTargetCount);
			GetDlgItem(IDC_BTN_REPORT)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_BTN_REPORT)->EnableWindow(FALSE);
		}
	}

	UpdateData(FALSE);
	this->Invalidate();
}

// ��OpenCVͼ����ʾ���ؼ���
void	CJudgeDlg::DrawPicture2HDC()
{
	m_matImage = LoadFOVImage2Color(m_pDocument->GetDirectory(), m_nFOVIndex);
	if ( !m_matImage.empty() )
	{
		CArray<CvRect, CvRect>* pRect;
		CvRect cvRect;
		CvPoint pt1, pt2;
		int i;
		//pRect = m_pDocument->GetTargetInfoInFOV(m_nWorkPosition, m_nFOVIndex);
		//for ( i = 0; i < pRect->GetSize(); i++ )
		//{				
		//	cvRect = pRect->GetAt(i);
		//	pt1.x = cvRect.x;
		//	pt1.y = cvRect.y;
		//	pt2.x = cvRect.x + cvRect.width;
		//	pt2.y = cvRect.y + cvRect.height;
		//	cvRectangle(m_pcvImage, pt1, pt2, CV_RGB(255,0,0), 2, 8,0);
		//}
				
		pRect = m_pDocument->GetSimpleTargetInfoInFOV(m_nFOVIndex);
		for ( i = 0; i < pRect->GetSize(); i++ )
		{				
			cvRect = pRect->GetAt(i);
			pt1.x = cvRect.x;
			pt1.y = cvRect.y;
			pt2.x = cvRect.x + cvRect.width;
			pt2.y = cvRect.y + cvRect.height;
			cv::rectangle(m_matImage, pt1, pt2, CV_RGB(255,0,0), 2, 8,0);
		}
		pRect = m_pDocument->GetOverlapTargetInfoInFOV( m_nFOVIndex);
		for ( i = 0; i < pRect->GetSize(); i++ )
		{				
			cvRect = pRect->GetAt(i);
			pt1.x = cvRect.x;
			pt1.y = cvRect.y;
			pt2.x = cvRect.x + cvRect.width;
			pt2.y = cvRect.y + cvRect.height;
			cv::rectangle(m_matImage, pt1, pt2, CV_RGB(0,255,0), 2, 8,0);
		}

		UINT ID = IDC_FOV_IMAGE;
		
		CDC *pDC = this->GetDlgItem(ID)->GetDC();
		HDC hDC = pDC->GetSafeHdc();

		CRect	rect;
		this->GetDlgItem(ID)->GetClientRect(&rect);
		
		CMyBitmap bmp;
		bmp.CopyFromMat(m_matImage);
		bmp.Draw(hDC,rect,1,0,0);

		ReleaseDC(pDC);	
	}	
}

// CJudgeDlg ��Ϣ�������
//void CJudgeDlg::OnCbnSelchangeComboWorkposition()
//{
//	UpdateWorkPosition();	
//}

void CJudgeDlg::OnCbnSelchangeComboFov()
{
	m_nFOVSequenceIndex = m_comboSequenceIndex.GetCurSel();
	if ( m_nFOVSequenceIndex == CB_ERR )
	{
		return;
	}
	m_nFOVIndex = m_pDocument->GetFOVIndex( m_nFOVSequenceIndex);
	m_targetProbability = globalConfirmProbability; // ���Ŷȴ��ڵ���0.9��Ŀ��
	
	m_TargetCount = m_pDocument->GetTargetCountInFOV( m_nFOVIndex);
	m_JudgedTargetCount = m_pDocument->GetJudgedTargetCountInFOV( m_nFOVIndex);
	m_Total_TargetCount = m_pDocument->GetTargetCountInSlide();
	m_Total_JudgedTargetCount = m_pDocument->GetJudgedTargetCountInSlide(); 
	m_spinJudgedTargetCount.SetPos(m_JudgedTargetCount);
		
	CString strItem;
	strItem.Format(_T("%3d���Ѽ�飩"),m_nFOVIndex+1);
	m_comboSequenceIndex.DeleteString(m_nFOVSequenceIndex);
	m_comboSequenceIndex.InsertString(m_nFOVSequenceIndex, strItem);
	m_comboSequenceIndex.SetCurSel(m_nFOVSequenceIndex);

	m_pDocument->SetFOVCheckedStatus( m_nFOVIndex, TRUE);
	
	DrawPicture2HDC();
	UpdateData(FALSE);

	// ���ڹ۲��ͼ�����
	g_iFOVIndex = m_nFOVIndex; 
}


void CJudgeDlg::OnEnChangeJudgedTargetcountinfov()
{
	if ( m_SlideID.IsEmpty() || ( m_comboSequenceIndex.GetCount() == 0 ) )
	{
		m_spinJudgedTargetCount.SetPos(0);
		return;
	}
	
	BOOL bScan, bJudge;
	m_pDocument->GetScanJudgeStatus(bScan, bJudge);
	if ( bScan )
	{
		m_nFOVSequenceIndex = m_comboSequenceIndex.GetCurSel();
		if ( m_nFOVSequenceIndex == CB_ERR )
		{
			return;
		}
		m_nFOVIndex = m_pDocument->GetFOVIndex( m_nFOVSequenceIndex);

		m_JudgedTargetCount = m_spinJudgedTargetCount.GetPos();
		m_pDocument->SetJudgedTargetCountInFOV( m_nFOVIndex, m_JudgedTargetCount);
		m_Total_JudgedTargetCount = m_pDocument->GetJudgedTargetCountInSlide();
	}	

	UpdateData(FALSE);
}

// �۲쵱ǰ��Ұ
void CJudgeDlg::OnBnClickedBtnJudge()
{
	if ( m_SlideID.IsEmpty() )
	{
		return;
	}
	BOOL bScan, bJudge;
	m_pDocument->GetScanJudgeStatus(bScan, bJudge);
	if ( bScan )
	{
		m_nFOVSequenceIndex = m_comboSequenceIndex.GetCurSel();
		if ( m_nFOVSequenceIndex == CB_ERR )
		{
			return;
		}
		m_nFOVIndex = m_pDocument->GetFOVIndex( m_nFOVSequenceIndex);
		m_pView->PostMessageW(MSG_JUDGEDLGOBSERVE_NOTIFY, 0, m_nFOVIndex);
	}
}

void CJudgeDlg::OnOK()
{
	OnCancel();
}

void CJudgeDlg::OnCancel()
{
	this->DestroyWindow();
	m_pView->PostMessageW(MSG_JUDGEDLGRETURN_NOTIFY);
}


void CJudgeDlg::OnBnClickedBtnReport()
{
	UpdateData(TRUE);
	
	m_Report.DoModal();
	InitiateWorkPositionCombo();
	UpdateWorkPosition();
}


void CJudgeDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	DrawPicture2HDC();
}


void CJudgeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	double temp = m_targetProbability;
	if( UpdateData(TRUE) )
	{
		if ( m_targetProbability != temp )
		{
			DrawPicture2HDC();
		}
	}
	else
	{
		m_targetProbability = temp;
		UpdateData(FALSE);
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}
