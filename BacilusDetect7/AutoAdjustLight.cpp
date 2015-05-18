#include "StdAfx.h"

#include "AutoAdjustLight.h"
#include "Mechanism.h"
#include "MotorController.h"

extern CMechanism		g_Mechanism;

CAutoAdjustLight::CAutoAdjustLight(void)
	: m_fBestImageGray(181.65)
	, m_ulInitExpTime(30963)        // us
	, m_iExpTimeSkew(230) 
	, m_fAdjustLightPositionZ(40)   // um
	, m_fInitWBPositionZ(0)         // um
	, m_fLightZeroThreshold(3)
	, m_iSubImgCol(5)
	, m_iSubImgRow(4)
	, m_fLightDifference(15)
	, m_fWBZeroThreshold(0.1)
	, m_2DfStartPoint( cv::Point2f(-5000, -5000) ) // um
	, m_fRangeInXY(10000)							// um
	, m_fDeltaInXY(2000)							// um
{
	m_pDetectView = NULL;
	m_pScanRoute = NULL; 

	CCamera* pCamera = CCamera::GetInstance();
	cv::Size ImageSize = pCamera->GetImage().size();				// ��������㵽��ͼ��Ĵ�С	  

	m_iSubImgWidth = ImageSize.width / m_iSubImgCol;
	m_iSubImgHeight = ImageSize.height / m_iSubImgRow;

	pCamera = NULL;
}


CAutoAdjustLight::~CAutoAdjustLight(void)
{
}


BOOL  CAutoAdjustLight::InitAutoAdjustLight(CBacilusDetectView* pDetectView)
{
	m_pScanRoute = g_Mechanism.GetScanRoute(); 
	if ( m_pScanRoute == NULL )
	{
		return FALSE;
	}
	m_pDetectView = pDetectView;

	return TRUE;
}

BOOL CAutoAdjustLight::Work(void)
{
	CCamera* pCamera = CCamera::GetInstance();

	cv::Point2f startpos = g_Mechanism.GenerateDestPointInXY( 0 );
		
	cv::Point2f destpos;
	float x, y;
	for ( y = m_2DfStartPoint.y; y < m_fRangeInXY; y += m_fDeltaInXY )
	{
		for ( x = m_2DfStartPoint.x; x < m_fRangeInXY; x += m_fDeltaInXY )
		{
			destpos.x = startpos.x + x;
			destpos.y = startpos.y + y;	
			
			if ( g_Mechanism.isValidPosition(destpos) )
			{
				pCamera->EnableNotifyMsg(FALSE);  // �������ֹ���ͼ��

				g_Mechanism.GotoDestPositionInXY(destpos);		// �˶��������Զ������λ��
			
				// �Զ�����
				if ( !TuneLight() )
				{
					return FALSE;
				}

				// ����Զ�������ͼ���Ƿ�����Ҫ��
				if ( CheckWhiteBalance() )
				{
					return TRUE;
				}
			}
		}
	}
	
	pCamera = NULL;

	return FALSE;
}


BOOL CAutoAdjustLight::TuneLight(void)
{
	CCamera* pCamera = CCamera::GetInstance();
	CMotorController* pMotorController = CMotorController::GetInstance();     

	// ���ó�ʼ����ع�ʱ��
	pCamera->SetExposureTime(m_ulInitExpTime);

	cv::Mat   matImage = pCamera->GetImage(); // ��������㵽��ͼ��
		
	// �Զ���ƽ���ʼ����
	pMotorController->ZMotorSmoothMove2AbsolutePos(m_fAdjustLightPositionZ, DISTANCE_UNIT_UM);
	Sleep(200);	
	RECT  auxRect;        // �����Զ���ƽ������򣺴��׻��ɫ����

	pCamera->AutoWhiteBalance();

	// ����ͼ������ֵ	
	pCamera->EnableNotifyMsg(TRUE);  // ������������ͼ��
//	Sleep(1000);
 	::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);
 	::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);
	::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);
	::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);  //�ȴ�������������ͼ������
	matImage = pCamera->GetImage();
	float average_gray = CalImageAverageGray(matImage);  // wait for revising

	// �Զ���������

	float oldExp = m_ulInitExpTime; 
	float newExp = oldExp;
	
	float newError = fabs( m_fBestImageGray - average_gray );
	float lastError = fabs( m_fBestImageGray - average_gray ) + 1;
	float fFactor = 1;
	while ( newError > m_fLightZeroThreshold )
	{
		if (newError < lastError)
		{
			oldExp = newExp;
			lastError = newError;
			fFactor = ( m_fBestImageGray - average_gray ) * m_iExpTimeSkew;
		}
		else
		{
			fFactor /= 2;			
		}

		WhetherStopWorkThread(m_pDetectView);			     // ֹͣ���鰴ť�����£�ɨ����鹤���̱߳���ֹ

		newExp = oldExp + fFactor;
		if ( ( newExp >= 60000 ) || ( newExp <= 0 ) )    // ������ع�ʱ�䲻�ܹ�����60ms)��Ҳ���ܹ��̣�0ms)
		{
			continue;
		}
		pCamera->SetExposureTime(newExp);                  // �����ع�ʱ��
//		Sleep(1000);
 		::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);
 		::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);
		::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);
		::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);  //�ȴ�������������ͼ������
		matImage = pCamera->GetImage();
		average_gray = CalImageAverageGray(matImage);  // wait for revising
		newError = fabs( m_fBestImageGray - average_gray );
	}

	// ����һ�ΰ�ƽ��
	pCamera->AutoWhiteBalance();  

	pCamera = NULL;
	pMotorController = NULL;

	return TRUE;
}


// ����Զ�����֮���ƽ���Ƿ�����Ҫ��
BOOL CAutoAdjustLight::CheckWhiteBalance(void)
{
	CCamera* pCamera = CCamera::GetInstance();

	cv::Mat   matImage = pCamera->GetImage(); // ��������㵽��ͼ��
	double fMaxLight = -1, fMinLight = 256;
	double fLight;
	int i,j;
	for ( j=0; j<m_iSubImgRow; j++ )
	{
		for ( i=0; i<m_iSubImgCol; i++ )
		{
			cv::Rect ROIRect(i*m_iSubImgWidth, j*m_iSubImgHeight, m_iSubImgWidth, m_iSubImgHeight);//ROI����  
			cv::Mat matImgROI(matImage,ROIRect);                                                   //Image���м䲿��  
			cv::Scalar temp = cv::mean( matImgROI );
			
			double R = temp[2], G = temp[1], B = temp[0];
			if ( R == 0 )
			{
				return FALSE;
			}
			double fWBError = __max( fabs(1 - B / R), fabs(1 - G / R ) );
			if ( fWBError > m_fWBZeroThreshold )                 // R:G:B != 1:1:1, ��ƽ��֮��ͼ���ǻ�ɫ��
			{			
				return FALSE;
			}
			
			fLight = 0.299*R + 0.587*G + 0.114*B;
			if ( fMaxLight < fLight )
			{
				fMaxLight = fLight;
			}
			if ( fMinLight > fLight )
			{
				fMinLight = fLight;
			}

			if ( (fMaxLight - fMinLight) > m_fLightDifference )   // ͼ��������������ȵ�����
			{
				return FALSE;
			}
		}
	}

	pCamera = NULL;

	return TRUE;
}
