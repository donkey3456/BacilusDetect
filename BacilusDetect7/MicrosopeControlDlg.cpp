// MicrosopeControlDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BacilusDetect.h"
#include "MicrosopeControlDlg.h"
#include "afxdialogex.h"
#include "BacilusDetectView.h"
#include <vector>
#include "Mechanism.h"

// CMicrosopeControlDlg �Ի���
//��΢�����ƶԻ���


// ȫ���ⲿ�豸
extern CMechanism	   g_Mechanism;

// ȫ�ֱ���
extern CString		   g_strDirectory;
extern int			   g_iFOVIndex;

IMPLEMENT_DYNAMIC(CMicrosopeControlDlg, CDialogEx)

CMicrosopeControlDlg::CMicrosopeControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMicrosopeControlDlg::IDD, pParent)
	, m_ZMotorSpeed(0)
	, m_ZMotorPosition(0)
	, m_ZMotorMoveUp(0)
	, m_ZMotorMoveDown(0)
	, m_XMotorSpeed(0)
	, m_XMotorPosition(0)
	, m_XMotorMoveLeft(0)
	, m_XMotorMoveRight(0)
	, m_YMotorSpeed(0)
	, m_YMotorPosition(0)
	, m_YMotorMoveBack(0)
	, m_YMotorMoveFrontad(0)
	, m_expotime_ms(0)
{
	int x = 10, y = 10;
	m_RTImageBmp_Rect.SetRect(x, y, x+CAMERA_IMAGE_WIDTH/2, y+CAMERA_IMAGE_HEIGHT/2 );

	m_ZMotorUnit = DISTANCE_UNIT_UM;	
	m_nTimer = 0;

	m_XMotorUnit = DISTANCE_UNIT_UM;	
	m_YMotorUnit = DISTANCE_UNIT_UM;	

// 	m_Light = 128;	
	m_expotime_ms = 10;

	m_nTimerPeriod = 1000; // 1s
}
// ��ʼ���Ի��������
BOOL CMicrosopeControlDlg::OnInitDialog()
{	
	CDialogEx::OnInitDialog();	

	CCamera* pCam = CCamera::GetInstance();
	CMotorController* pMC = CMotorController::GetInstance();

    // MCUϵͳ	
	CComboBox* pZmotorUnitCombo = (CComboBox*) this->GetDlgItem(IDC_COMBO_Z_UNIT);
	pZmotorUnitCombo->SetCurSel(m_ZMotorUnit);
	m_ZMotorSpeed = pMC->GetZMotorSpeed(m_ZMotorUnit);
	pMC->GetZMotorPosition(m_ZMotorPosition,m_ZMotorUnit);
		
	if ( pCam->isCapturing() )
	{
		m_expotime_us = pCam->GetExposureTime();
	}
	
	m_expotime_ms = m_expotime_us/1000.0;
	CSliderCtrl* pExpotime_SliderBar = (CSliderCtrl*)this->GetDlgItem(IDC_SLIDER_EXPOTIME);
	pExpotime_SliderBar->SetRange(126,66695);
	pExpotime_SliderBar->SetPos((int)(m_expotime_us));

	// X��ϵͳ
	CComboBox* pXmotorUnitCombo = (CComboBox*) this->GetDlgItem(IDC_COMBO_X_UNIT);
	pXmotorUnitCombo->SetCurSel(m_XMotorUnit);
	m_XMotorSpeed = pMC->GetXMotorSpeed(m_XMotorUnit);

	// Y��ϵͳ
	CComboBox* pYmotorUnitCombo = (CComboBox*) this->GetDlgItem(IDC_COMBO_Y_UNIT);
	pYmotorUnitCombo->SetCurSel(m_YMotorUnit);
	m_YMotorSpeed = pMC->GetYMotorSpeed(m_YMotorUnit);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

CMicrosopeControlDlg::~CMicrosopeControlDlg()
{
}

void CMicrosopeControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Text(pDX, IDC_EDIT_LIGHT, m_Light);
	//  DDV_MinMaxUInt(pDX, m_Light, 0, 255);
	DDX_Text(pDX, IDC_EDIT_Z_SPEED, m_ZMotorSpeed);
	DDV_MinMaxFloat(pDX, m_ZMotorSpeed, 0, 2147483648);
	DDX_Text(pDX, IDC_EDIT_Z_POSITION, m_ZMotorPosition);
	//  DDX_Text(pDX, IDC_EDIT_Z_ENCODER, m_ZMotorEncoder);
	DDX_Text(pDX, IDC_EDIT_Z_UP, m_ZMotorMoveUp);
	DDV_MinMaxFloat(pDX, m_ZMotorMoveUp, 0, 2147483648);
	DDX_Text(pDX, IDC_EDIT_Z_DOWN, m_ZMotorMoveDown);
	DDV_MinMaxFloat(pDX, m_ZMotorMoveDown, 0, 2147483648);
	DDX_Text(pDX, IDC_EDIT_X_SPEED, m_XMotorSpeed);
	DDV_MinMaxFloat(pDX, m_XMotorSpeed, 0, 100000);
	DDX_Text(pDX, IDC_EDIT_X_POSITION, m_XMotorPosition);
	DDX_Text(pDX, IDC_EDIT_X_UP, m_XMotorMoveLeft);
	DDV_MinMaxFloat(pDX, m_XMotorMoveLeft, 0, 16777215);
	DDX_Text(pDX, IDC_EDIT_X_DOWN, m_XMotorMoveRight);
	DDV_MinMaxFloat(pDX, m_XMotorMoveRight, 0, 16777215);
	//  DDX_Text(pDX, IDC_EDIT_X_ENCODER, m_XMotorEncoder);
	DDX_Text(pDX, IDC_EDIT_Y_SPEED, m_YMotorSpeed);
	DDV_MinMaxFloat(pDX, m_YMotorSpeed, 0, 100000);
	DDX_Text(pDX, IDC_EDIT_Y_POSITION, m_YMotorPosition);
	//  DDX_Text(pDX, IDC_EDIT_Y_ENCODER, m_YMotorEncoder);
	DDX_Text(pDX, IDC_EDIT_Y_DOWN, m_YMotorMoveBack);
	DDV_MinMaxFloat(pDX, m_YMotorMoveBack, 0, 16777215);
	DDX_Text(pDX, IDC_EDIT_Y_UP, m_YMotorMoveFrontad);
	DDV_MinMaxFloat(pDX, m_YMotorMoveFrontad, 0, 16777215);
	DDX_Text(pDX, IDC_EDIT_EXPOTIME, m_expotime_ms);
	DDV_MinMaxFloat(pDX, m_expotime_ms, 0.126, 66.695);
}

// �ڲ�����

void  CMicrosopeControlDlg::UpdateImage(void)
{
	this->InvalidateRect(&m_RTImageBmp_Rect, 0);
}

BEGIN_MESSAGE_MAP(CMicrosopeControlDlg, CDialogEx)
	ON_WM_HSCROLL()
// 	ON_BN_CLICKED(IDC_BTN_LIGHT, &CMicrosopeControlDlg::OnBnClickedBtnLight)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO_Z_UNIT, &CMicrosopeControlDlg::OnCbnSelchangeComboZUnit)
	ON_BN_CLICKED(IDC_BTN_Z_ORIGIN, &CMicrosopeControlDlg::OnBnClickedBtnZOrigin)
	ON_BN_CLICKED(IDC_BTN_Z_STOP, &CMicrosopeControlDlg::OnBnClickedBtnZStop)
	ON_BN_CLICKED(IDC_BTN_Z_SPEED, &CMicrosopeControlDlg::OnBnClickedBtnZSpeed)
	ON_BN_CLICKED(IDC_BTN_Z_UP, &CMicrosopeControlDlg::OnBnClickedBtnZUp)
	ON_BN_CLICKED(IDC_BTN_Z_DOWN, &CMicrosopeControlDlg::OnBnClickedBtnZDonwn)
	ON_BN_CLICKED(IDC_BTN_Z_CHECK, &CMicrosopeControlDlg::OnBnClickedBtnZCheck)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_X_UNIT, &CMicrosopeControlDlg::OnCbnSelchangeComboXUnit)
	ON_BN_CLICKED(IDC_BTN_X_SPEED, &CMicrosopeControlDlg::OnBnClickedBtnXSpeed)
	ON_BN_CLICKED(IDC_BTN_X_DOWN, &CMicrosopeControlDlg::OnBnClickedBtnXDown)
	ON_BN_CLICKED(IDC_BTN_X_UP, &CMicrosopeControlDlg::OnBnClickedBtnXUp)
// 	ON_BN_CLICKED(IDC_BTN_X_STOP, &CMicrosopeControlDlg::OnBnClickedBtnXStop)
// 	ON_BN_CLICKED(IDC_BTN_X_CHECK, &CMicrosopeControlDlg::OnBnClickedBtnXCheck)
	ON_CBN_SELCHANGE(IDC_COMBO_Y_UNIT, &CMicrosopeControlDlg::OnCbnSelchangeComboYUnit)
	ON_BN_CLICKED(IDC_BTN_Y_SPEED, &CMicrosopeControlDlg::OnBnClickedBtnYSpeed)
	ON_BN_CLICKED(IDC_BTN_Y_DOWN, &CMicrosopeControlDlg::OnBnClickedBtnYDown)
	ON_BN_CLICKED(IDC_BTN_Y_UP, &CMicrosopeControlDlg::OnBnClickedBtnYUp)
// 	ON_BN_CLICKED(IDC_BTN_Y_STOP, &CMicrosopeControlDlg::OnBnClickedBtnYStop)
// 	ON_BN_CLICKED(IDC_BTN_Y_CHECK, &CMicrosopeControlDlg::OnBnClickedBtnYCheck)
// 	ON_BN_CLICKED(IDC_BTN_X_ORIGIN, &CMicrosopeControlDlg::OnBnClickedBtnXOrigin)
// 	ON_BN_CLICKED(IDC_BTN_Y_ORIGIN, &CMicrosopeControlDlg::OnBnClickedBtnYOrigin)
	ON_BN_CLICKED(IDC_BTN_EXPOTIME, &CMicrosopeControlDlg::OnBnClickedBtnExpotime)
	ON_BN_CLICKED(IDC_BTN_WHITEBALANCE, &CMicrosopeControlDlg::OnBnClickedBtnWhitebalance)
	ON_BN_CLICKED(IDC_BUTTON_SAVEIMAGE, &CMicrosopeControlDlg::OnBnClickedButtonSaveimage)
// 	ON_BN_CLICKED(IDC_BTN_LIGHT_RECORD, &CMicrosopeControlDlg::OnBnClickedBtnLightRecord)
ON_BN_CLICKED(IDC_BTN_FORCE_STOP, &CMicrosopeControlDlg::OnBnClickedBtnForceStop)
END_MESSAGE_MAP()

// CMicrosopeControlDlg ��Ϣ�������
// ��ʾ�����ͼ�� 
void CMicrosopeControlDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);
	CBitmap * pOldBmp = NULL;

	CCamera* pCam = CCamera::GetInstance();
	CBitmap	 BmpImage;                     //��������������ʾ��ͼ��	
	if(pCam->isCapturing())
	{
		cv::Mat   matImageTemp = pCam->GetImage();
		BmpImage.CreateBitmap(matImageTemp.cols, matImageTemp.rows, 1, 32, NULL);		
		cv::Mat matImage4C = cv::Mat(matImageTemp.rows,matImageTemp.cols,CV_8UC4);
		cv::cvtColor(matImageTemp, matImage4C, CV_BGR2BGRA);
		BmpImage.SetBitmapBits(matImage4C.total() * matImage4C.elemSize(), (const void*)matImage4C.ptr() );  //����Ҫ��ʾ��Ұ��ͼ��
	}
	else
	{			
		BmpImage.LoadBitmapW(IDB_MICROSCOPE_IMAGE);  // ��ʼ���������������ڴ����ͼ��
	}
	pOldBmp = MemDC.SelectObject(&BmpImage);

	if (pOldBmp == NULL)
	{
		return;
	}

	dc.SetStretchBltMode(COLORONCOLOR);
	dc.StretchBlt( m_RTImageBmp_Rect.left, m_RTImageBmp_Rect.top, m_RTImageBmp_Rect.Width(), m_RTImageBmp_Rect.Height(),        // Display Destination
		           &MemDC, 0, 0, CAMERA_IMAGE_WIDTH, CAMERA_IMAGE_HEIGHT, // Source
				   SRCCOPY );
	
	MemDC.SelectObject(pOldBmp);
}

//================ MCUϵͳ ==================//
// ����Z��ľ��뵥λ
void CMicrosopeControlDlg::OnCbnSelchangeComboZUnit()
{
	CComboBox* pZmotorUnitCombo = (CComboBox*) this->GetDlgItem(IDC_COMBO_Z_UNIT);
	CMotorController* pMC = CMotorController::GetInstance();
	int nIndex = pZmotorUnitCombo->GetCurSel();
	if ( nIndex != CB_ERR )
	{
		pMC->GetZMotorPosition( m_ZMotorPosition, DISTANCE_UNIT (nIndex) );
		m_ZMotorSpeed = pMC->GetZMotorSpeed( DISTANCE_UNIT (nIndex) );
		m_ZMotorMoveUp = pMC->ZMilli2Unit(
													   pMC->ZUnit2Milli(m_ZMotorMoveUp, m_ZMotorUnit),
													   DISTANCE_UNIT (nIndex)  );
	    m_ZMotorMoveDown = pMC->ZMilli2Unit(
													   pMC->ZUnit2Milli(m_ZMotorMoveDown, m_ZMotorUnit),
													   DISTANCE_UNIT (nIndex)  );

		m_ZMotorUnit = DISTANCE_UNIT (nIndex);
		UpdateData(FALSE);
	}
}

// Z�Ჽ�����Ѱ�����
void CMicrosopeControlDlg::OnBnClickedBtnZOrigin()
{
	CMotorController* pMC = CMotorController::GetInstance();
	if ( pMC->isConnecting() )
	{
		pMC->Home();
	}
}

// ֹͣZ�Ჽ��������˶�
void CMicrosopeControlDlg::OnBnClickedBtnZStop()
{
	CMotorController* pMC = CMotorController::GetInstance();
	if ( pMC->isConnecting() )
	{
		pMC->Stop();
	}
}

// ����Z�Ჽ��������ٶ�
void CMicrosopeControlDlg::OnBnClickedBtnZSpeed()
{
	UpdateData(TRUE);
	CMotorController* pMC = CMotorController::GetInstance();

	if ( pMC->isConnecting() )
	{
		pMC->SetZMotorSpeed(m_ZMotorSpeed, m_ZMotorUnit);
	}
}

// Z�Ჽ����������˶�
void CMicrosopeControlDlg::OnBnClickedBtnZUp()
{
	UpdateData(TRUE);
	CMotorController* pMC = CMotorController::GetInstance();
	if ( pMC->isConnecting() )
	{
		m_nTimer = SetTimer(1, m_nTimerPeriod, 0);  
		pMC->ZMotorRelativeMove(-m_ZMotorMoveUp, m_ZMotorUnit);
	}
}

// Z�Ჽ����������˶�
void CMicrosopeControlDlg::OnBnClickedBtnZDonwn()
{
	UpdateData(TRUE);	
	CMotorController* pMC = CMotorController::GetInstance();

	if ( pMC->isConnecting() )
	{
		m_nTimer = SetTimer(1, m_nTimerPeriod, 0);  
		pMC->ZMotorRelativeMove(m_ZMotorMoveDown, m_ZMotorUnit);
	}
}

// ��ѯMCUϵͳ״̬
void CMicrosopeControlDlg::OnBnClickedBtnZCheck()
{	
	CMotorController* pMC = CMotorController::GetInstance();

	if ( pMC->isConnecting() )
	{
		CString str;
		pMC->GetZMotorPosition(m_ZMotorPosition, m_ZMotorUnit);
		str.Format(_T("%g"), m_ZMotorPosition);
		this->SetDlgItemTextW(IDC_EDIT_Z_POSITION,str);

		pMC->GetXMotorPosition(m_XMotorPosition, m_XMotorUnit);
		str.Format(_T("%g"), m_XMotorPosition);
		this->SetDlgItemTextW(IDC_EDIT_X_POSITION,str);

		pMC->GetYMotorPosition(m_YMotorPosition, m_YMotorUnit);
		str.Format(_T("%g"), m_YMotorPosition);
		this->SetDlgItemTextW(IDC_EDIT_Y_POSITION,str);

		MOTOR_STATUS nStatus;
		pMC->GetCurrentStatus(nStatus);
		CButton* pBtn = NULL;
		pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_ALARM);
		pBtn->SetCheck(BST_UNCHECKED);
		pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_BUSY);
		pBtn->SetCheck(BST_UNCHECKED);
		pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_IDLE);
		pBtn->SetCheck(BST_UNCHECKED);
		pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_HOLD);
		pBtn->SetCheck(BST_UNCHECKED);
		pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_QUEUE);
		pBtn->SetCheck(BST_UNCHECKED);

		if (!pMC->isNeedReset())
		{
			pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_RESET);
			pBtn->SetCheck(BST_UNCHECKED);
		}
		else
		{
			pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_RESET);
			pBtn->SetCheck(BST_CHECKED);
		}
		switch(nStatus)
		{
		case MOTOR_ALARM:
			pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_ALARM);
			pBtn->SetCheck(BST_CHECKED);
			break;
		case MOTOR_HOLD:
			pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_HOLD);
			pBtn->SetCheck(BST_CHECKED);
			break;
		case MOTOR_RUN:
			pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_BUSY);
			pBtn->SetCheck(BST_CHECKED);
			break;
		case MOTOR_QUEUE:
			pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_QUEUE);
			pBtn->SetCheck(BST_CHECKED);
			break;
		case MOTOR_IDLE:
			pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_IDLE);
			pBtn->SetCheck(BST_CHECKED);
			break;
		default:
			break;
		}	
	}
}
// ------------------------------------------------//



// �����ع�ʱ��
void CMicrosopeControlDlg::OnBnClickedBtnExpotime()
{
	UpdateData(TRUE);
	ULONG temp_us = (ULONG) (m_expotime_ms*1000);  // us
	CSliderCtrl* pExpotime_SliderBar = (CSliderCtrl*)this->GetDlgItem(IDC_SLIDER_EXPOTIME);	
	pExpotime_SliderBar->SetPos(temp_us);

	CCamera* pCam = CCamera::GetInstance();
	pCam->SetExposureTime(temp_us);
}

// �Զ���ƽ��
void CMicrosopeControlDlg::OnBnClickedBtnWhitebalance()
{
	// �����Զ���ƽ������򣺴��׻��ɫ����
	RECT			auxRect;        // �����Զ���ƽ������򣺴��׻��ɫ����
	auxRect.top = 100;
	auxRect.left = 100;
	auxRect.bottom = CAMERA_IMAGE_HEIGHT-100;
	auxRect.left = CAMERA_IMAGE_WIDTH-100;
	CCamera* pCam = CCamera::GetInstance();
	pCam->AutoWhiteBalance(&auxRect);
}



//======================= X��ϵͳ =====================//
// ����X��ľ��뵥λ
void CMicrosopeControlDlg::OnCbnSelchangeComboXUnit()
{
	CComboBox* pXmotorUnitCombo = (CComboBox*) this->GetDlgItem(IDC_COMBO_X_UNIT);
	CMotorController* pMC = CMotorController::GetInstance();
	int nIndex = pXmotorUnitCombo->GetCurSel();
	if ( nIndex != CB_ERR )
	{
		pMC->GetXMotorPosition( m_XMotorPosition, DISTANCE_UNIT (nIndex) );
		m_XMotorSpeed = pMC->GetXMotorSpeed( DISTANCE_UNIT (nIndex) );
		m_XMotorMoveLeft = pMC->XYMilli2Unit(
													   pMC->XYUnit2Milli(m_XMotorMoveLeft, m_XMotorUnit),
													   DISTANCE_UNIT (nIndex)  );
	    m_XMotorMoveRight = pMC->XYMilli2Unit(
													   pMC->XYUnit2Milli(m_XMotorMoveRight, m_XMotorUnit),
													   DISTANCE_UNIT (nIndex)  );

		m_XMotorUnit = DISTANCE_UNIT (nIndex);
		UpdateData(FALSE);
	}
}

// ����X�Ჽ��������ٶ�
void CMicrosopeControlDlg::OnBnClickedBtnXSpeed()
{
	float tempspeed = m_XMotorSpeed;
	UpdateData(TRUE);
	CMotorController* pMC = CMotorController::GetInstance();

	if ( pMC->isConnecting() )
	{
		pMC->SetXMotorSpeed(m_XMotorSpeed, m_XMotorUnit);
	}	
}

// X�Ჽ����������˶�
void CMicrosopeControlDlg::OnBnClickedBtnXDown()
{
	UpdateData(TRUE);
	CMotorController* pMC = CMotorController::GetInstance();

	if ( pMC->isConnecting() )
	{
		float fXPosition;
		pMC->GetXMotorPosition(fXPosition,DISTANCE_UNIT_UM);
		fXPosition += m_XMotorMoveRight;

		if ( g_Mechanism.isValidPositionX( fXPosition ) )
		{
			m_nTimer = SetTimer(1, m_nTimerPeriod, 0);  
			pMC->XMotorRelativeMove(m_XMotorMoveRight, m_XMotorUnit);  //�����˶�Ϊ������
		}
		else
		{
			AfxMessageBox(_T("���õľ��볬����е��Χ"));
		}
	}	
}

// X�Ჽ����������˶�
void CMicrosopeControlDlg::OnBnClickedBtnXUp()
{
	UpdateData(TRUE);
	CMotorController* pMC = CMotorController::GetInstance();

	if ( pMC->isConnecting() )
	{
		m_nTimer = SetTimer(1, m_nTimerPeriod, 0);  
		pMC->XMotorRelativeMove(-m_XMotorMoveLeft, m_XMotorUnit); //�����˶�Ϊ������
	}
}


//======================= Y��ϵͳ =====================//
// ����Y��ľ��뵥λ
void CMicrosopeControlDlg::OnCbnSelchangeComboYUnit()
{
	CComboBox* pYmotorUnitCombo = (CComboBox*) this->GetDlgItem(IDC_COMBO_Y_UNIT);
	CMotorController* pMC = CMotorController::GetInstance();

	int nIndex = pYmotorUnitCombo->GetCurSel();
	if ( nIndex != CB_ERR )
	{
		pMC->GetYMotorPosition( m_YMotorPosition, DISTANCE_UNIT (nIndex) );
		m_YMotorSpeed = pMC->GetYMotorSpeed( DISTANCE_UNIT (nIndex) );
		m_YMotorMoveFrontad = pMC->XYMilli2Unit(
													   pMC->XYUnit2Milli(m_YMotorMoveFrontad, m_YMotorUnit),
													   DISTANCE_UNIT (nIndex)  );
	    m_YMotorMoveBack = pMC->XYMilli2Unit(
													   pMC->XYUnit2Milli(m_YMotorMoveBack, m_YMotorUnit),
													   DISTANCE_UNIT (nIndex)  );

		m_YMotorUnit = DISTANCE_UNIT (nIndex);
		UpdateData(FALSE);
	}
}

// ����Y�Ჽ��������ٶ�
void CMicrosopeControlDlg::OnBnClickedBtnYSpeed()
{
	float tempspeed = m_YMotorSpeed;
	UpdateData(TRUE);
	CMotorController* pMC = CMotorController::GetInstance();

	if ( pMC->isConnecting() )
	{
		pMC->SetYMotorSpeed(m_YMotorSpeed, m_YMotorUnit);
	}
}

// Y�Ჽ���������˶�
void CMicrosopeControlDlg::OnBnClickedBtnYDown()
{
	UpdateData(TRUE);
	CMotorController* pMC = CMotorController::GetInstance();

	if ( pMC->isConnecting() )
	{
		m_nTimer = SetTimer(1, m_nTimerPeriod, 0);  
		pMC->YMotorRelativeMove(-m_YMotorMoveBack, m_YMotorUnit); //��ǰ�˶�Ϊ������
	}
}

// Y�Ჽ�������ǰ�˶�
void CMicrosopeControlDlg::OnBnClickedBtnYUp()
{
	UpdateData(TRUE);
	CMotorController* pMC = CMotorController::GetInstance();

	if ( pMC->isConnecting() )
	{
		float fYMotorPosition;
		pMC->GetYMotorPosition(fYMotorPosition,DISTANCE_UNIT_UM);
		fYMotorPosition += m_YMotorMoveFrontad;

		if ( g_Mechanism.isValidPositionY( fYMotorPosition ) )
		{
			m_nTimer = SetTimer(1, m_nTimerPeriod, 0);  
			pMC->YMotorRelativeMove(m_YMotorMoveFrontad, m_YMotorUnit);//��ǰ�˶�Ϊ������
		}
		else
		{
			AfxMessageBox(_T("���õľ��볬����е��Χ"));
		}
	}
}


void CMicrosopeControlDlg::OnCancel()
{
	KillTimer(m_nTimer);
	CMotorController* pMC = CMotorController::GetInstance();
	if ( pMC->isConnecting() )
	{
		pMC->Stop();
	}
	CDialogEx::OnCancel();
}

// ʵʱ����XYZϵͳ״̬�ڽ����ϵ���ʾ
void CMicrosopeControlDlg::OnTimer(UINT_PTR nIDEvent)
{
	//����߳�
	CMotorController* pMC = CMotorController::GetInstance();

	if ( pMC->isConnecting() )
	{
		
		// Z��ϵͳ
		CString str;
		pMC->GetZMotorPosition(m_ZMotorPosition, m_ZMotorUnit);
		str.Format(_T("%g"), m_ZMotorPosition);
		this->SetDlgItemTextW(IDC_EDIT_Z_POSITION,str);
			
		pMC->GetXMotorPosition(m_XMotorPosition, m_XMotorUnit);
		str.Format(_T("%g"), m_XMotorPosition);
		this->SetDlgItemTextW(IDC_EDIT_X_POSITION,str);

		pMC->GetYMotorPosition(m_YMotorPosition, m_YMotorUnit);
		str.Format(_T("%g"), m_YMotorPosition);
		this->SetDlgItemTextW(IDC_EDIT_Y_POSITION,str);

		MOTOR_STATUS nStatus;
		pMC->GetCurrentStatus(nStatus);
		CButton* pBtn = NULL;
		pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_ALARM);
		pBtn->SetCheck(BST_UNCHECKED);
		pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_BUSY);
		pBtn->SetCheck(BST_UNCHECKED);
		pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_IDLE);
		pBtn->SetCheck(BST_UNCHECKED);
		pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_HOLD);
		pBtn->SetCheck(BST_UNCHECKED);
		pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_QUEUE);
		pBtn->SetCheck(BST_UNCHECKED);

		if (!pMC->isNeedReset())
		{
			pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_RESET);
			pBtn->SetCheck(BST_UNCHECKED);
		}
		else
		{
			pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_RESET);
			pBtn->SetCheck(BST_CHECKED);
		}
		switch(nStatus)
		{
		case MOTOR_ALARM:
			pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_ALARM);
			pBtn->SetCheck(BST_CHECKED);
			break;
		case MOTOR_HOLD:
			pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_HOLD);
			pBtn->SetCheck(BST_CHECKED);
			break;
		case MOTOR_RUN:
			pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_BUSY);
			pBtn->SetCheck(BST_CHECKED);
			break;
		case MOTOR_QUEUE:
			pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_QUEUE);
			pBtn->SetCheck(BST_CHECKED);
			break;
		case MOTOR_IDLE:
			pBtn = (CButton*) this->GetDlgItem(IDC_CHECK_IDLE);
			pBtn->SetCheck(BST_CHECKED);
			break;
		default:
			break;
		}

		if (nStatus == MOTOR_ALARM || nStatus == MOTOR_IDLE)
		{
			KillTimer(m_nTimer);
			if ( pMC->isConnecting() )
			{
				if (pMC->isNeedReset())
				{
					pMC->Reset();
				}
				
				if (pMC->isNeedKillAarm())
				{
					pMC->KillAlarm();
				}
			}	
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

// �ü��̿���XYZƽ̨���˶�
BOOL CMicrosopeControlDlg::PreTranslateMessage(MSG* pMsg)
{
// 	if ( pMsg->message == WM_KEYDOWN )
// 	{
// 		UINT PreTranslateMessage = (1<<30);
// 		switch ( pMsg->wParam )               
// 		{
// 		case 0x4F:                                            
// 			if ( (pMsg->lParam & PreTranslateMessage) == 0 )   // ������һ�ΰ���ʱ��Z�Ჽ�������ʼ���������˶�  
// 			{
// 				if ( g_MCUControl.isConnecting() )
// 				{
// 					m_nTimer = SetTimer(1, m_nTimerPeriod, 0); 
// 					//g_MCUControl.ZMotorConitueMoveUp();	    
// 					// û�����Ƶ������˶�̫Σ�գ����Կ��Ǿ����˶���������ķ�ʽ�����������˶�
// 					g_MCUControl.ZMotorStartAbsoluteMove(0); 
// 				}
// 			}
// 			return TRUE;			
// 			break;
// 		case 0x4C:
// 			if ( (pMsg->lParam & PreTranslateMessage) == 0 )  // ������һ�ΰ���ʱ��Z�Ჽ�������ʼ���������˶�  
// 			{
// 				if ( g_MCUControl.isConnecting() )
// 				{
// 					m_nTimer = SetTimer(1, m_nTimerPeriod, 0); 
// 					//g_MCUControl.ZMotorConitueMoveDown();
// 					// û�����Ƶ������˶�̫Σ�գ����Կ��ǲ���5mm��Χ���˶������������˶�
// 					g_MCUControl.ZMotorStartRelativeMove(5,DISTANCE_UNIT_MM);  
// 				}
// 			}
// 			return TRUE;
// 			break;
// 
// 		case 0x41:                                            
// 			if ( (pMsg->lParam & PreTranslateMessage) == 0 )   // ������һ�ΰ���ʱ��X�Ჽ�������ʼ���������˶�  
// 			{
// 				if ( g_PCIControl.isConnecting() )
// 				{
// 					m_nTimer = SetTimer(1, m_nTimerPeriod, 0);  
// 					g_PCIControl.XMotorConitueMoveLeft();
// 				}
// 			}
// 			return TRUE;			
// 			break;
// 		case 0x44:                                            
// 			if ( (pMsg->lParam & PreTranslateMessage) == 0 )   // ������һ�ΰ���ʱ��X�Ჽ�������ʼ���������˶�  
// 			{
// 				if ( g_PCIControl.isConnecting() )
// 				{
// 					cv::Point2f fXYMotorPosition = g_PCIControl.GetXYMotorPosition(DISTANCE_UNIT_UM);
// 					fXYMotorPosition.x += g_PCIControl.GetXMotorSpeed(DISTANCE_UNIT_UM) * m_nTimerPeriod / 500;
// 
// 					if ( g_Mechanism.isValidPosition( fXYMotorPosition ) )
// 					{
// 						m_nTimer = SetTimer(1, m_nTimerPeriod, 0);  
// 						g_PCIControl.XMotorConitueMoveRight();
// 					}
// 				}
// 			}
// 			return TRUE;			
// 			break;
// 
// 		case 0x53:                                            
// 			if ( (pMsg->lParam & PreTranslateMessage) == 0 )   // ������һ�ΰ���ʱ��Y�Ჽ�������ʼ������ǰ�˶�  
// 			{
// 				if ( g_PCIControl.isConnecting() )
// 				{
// 					cv::Point2f fXYMotorPosition = g_PCIControl.GetXYMotorPosition(DISTANCE_UNIT_UM);
// 					fXYMotorPosition.y += g_PCIControl.GetYMotorSpeed(DISTANCE_UNIT_UM) * m_nTimerPeriod / 500;
// 
// 					if ( g_Mechanism.isValidPosition( fXYMotorPosition ) )
// 					{
// 						m_nTimer = SetTimer(1, m_nTimerPeriod, 0);  
// 						g_PCIControl.YMotorConitueMoveFrontad();
// 					}
// 				}
// 			}
// 			return TRUE;			
// 			break;
// 
// 		case 0x57:                                            
// 			if ( (pMsg->lParam & PreTranslateMessage) == 0 )   // ������һ�ΰ���ʱ��Y�Ჽ�������ʼ��������˶�  
// 			{
// 				if ( g_PCIControl.isConnecting() )
// 				{
// 					m_nTimer = SetTimer(1, m_nTimerPeriod, 0);  
// 					g_PCIControl.YMotorConitueMoveBack();
// 				}
// 			}
// 			return TRUE;			
// 			break;
// 
// 		default:
// 			break;
// 		}		
// 	}
// 
// 	if ( pMsg->message == WM_KEYUP)         
// 	{
// 		switch ( pMsg->wParam  )
// 		{
// 		case 0x4F:                          // ��������ʱ��Z�Ჽ�����ֹͣ�����˶� 
// 		case 0x4C:  
// 			if ( g_MCUControl.isConnecting() )
// 			{
// 				g_MCUControl.ZMotorStop();
// 			}
// 			return TRUE;
// 			break;	
// 
// 		case 0x41:                              // ��������ʱ��X�Ჽ�����ֹͣ�����˶�                                     
//         case 0x44:
// 			if ( g_PCIControl.isConnecting() )
// 			{
// 				g_PCIControl.XMotorStop();
// 			}
// 			return TRUE;			
// 			break;
// 
// 		case 0x53:								// ��������ʱ��X�Ჽ�����ֹͣ�����˶�  
// 		case 0x57: 
// 			if ( g_PCIControl.isConnecting() )
// 			{
// 				g_PCIControl.YMotorStop();
// 			}
// 			return TRUE;			
// 			break;
// 
// 		default:
// 			break;
// 		}		
// 	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CMicrosopeControlDlg::OnBnClickedButtonSaveimage()
{
	if ( g_strDirectory.IsEmpty() )
	{
		g_strDirectory = _T("D:\\BacilusDetectTemp");
	}
	CString strFile;
	strFile.Format(_T("%dE"), g_iFOVIndex);

	CFileDialog dlgFile(FALSE);
	dlgFile.GetOFN().lpstrFilter = _T("JPEG File (*.JPG)|*.JPG|");
	dlgFile.GetOFN().lpstrInitialDir = g_strDirectory;  
	dlgFile.GetOFN().lpstrFile = strFile.GetBuffer(1024);
	dlgFile.GetOFN().lpstrDefExt = _T("JPG");
	if(dlgFile.DoModal() == IDOK)
	{
		CString strFileName = dlgFile.GetPathName();		
		int sizeOfString = (strFileName.GetLength() + 1);
		char* filename = new char[sizeOfString];
		int i = 0;
		for ( i=0; i<strFileName.GetLength(); i++ )
		{
			filename[i] = strFileName.GetAt(i);
		}
		filename[i] = '\0';
		CCamera* pCam = CCamera::GetInstance();
		
		cv::imwrite(filename,pCam->GetImage());
		delete [] filename;
	}	

	strFile.ReleaseBuffer();
}

void CMicrosopeControlDlg::OnBnClickedBtnForceStop()
{
	// TODO: �ڴ˼������헸�֪̎��ʽ��ʽ�a
	CMotorController* pMC = CMotorController::GetInstance();
	if ( pMC->isConnecting() )
	{
		pMC->ForceAbort();
	}
}
