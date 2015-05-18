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
	cv::Size ImageSize = pCamera->GetImage().size();				// 摄像机拍摄到的图像的大小	  

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
				pCamera->EnableNotifyMsg(FALSE);  // 禁摄像机止输出图像

				g_Mechanism.GotoDestPositionInXY(destpos);		// 运动到进行自动调光的位置
			
				// 自动调光
				if ( !TuneLight() )
				{
					return FALSE;
				}

				// 检查自动调光后的图像是否满足要求
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

	// 设置初始最佳曝光时间
	pCamera->SetExposureTime(m_ulInitExpTime);

	cv::Mat   matImage = pCamera->GetImage(); // 摄像机拍摄到的图像
		
	// 自动白平衡初始设置
	pMotorController->ZMotorSmoothMove2AbsolutePos(m_fAdjustLightPositionZ, DISTANCE_UNIT_UM);
	Sleep(200);	
	RECT  auxRect;        // 设置自动白平衡的区域：纯白或灰色区域

	pCamera->AutoWhiteBalance();

	// 计算图像亮度值	
	pCamera->EnableNotifyMsg(TRUE);  // 允许摄像机输出图像
//	Sleep(1000);
 	::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);
 	::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);
	::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);
	::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);  //等待摄像机拍摄的新图像数据
	matImage = pCamera->GetImage();
	float average_gray = CalImageAverageGray(matImage);  // wait for revising

	// 自动调整亮度

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

		WhetherStopWorkThread(m_pDetectView);			     // 停止检验按钮被按下，扫描检验工作线程被终止

		newExp = oldExp + fFactor;
		if ( ( newExp >= 60000 ) || ( newExp <= 0 ) )    // 摄像机曝光时间不能过长（60ms)，也不能过短（0ms)
		{
			continue;
		}
		pCamera->SetExposureTime(newExp);                  // 设置曝光时间
//		Sleep(1000);
 		::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);
 		::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);
		::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);
		::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);  //等待摄像机拍摄的新图像数据
		matImage = pCamera->GetImage();
		average_gray = CalImageAverageGray(matImage);  // wait for revising
		newError = fabs( m_fBestImageGray - average_gray );
	}

	// 再做一次白平衡
	pCamera->AutoWhiteBalance();  

	pCamera = NULL;
	pMotorController = NULL;

	return TRUE;
}


// 检查自动调光之后白平衡是否满足要求
BOOL CAutoAdjustLight::CheckWhiteBalance(void)
{
	CCamera* pCamera = CCamera::GetInstance();

	cv::Mat   matImage = pCamera->GetImage(); // 摄像机拍摄到的图像
	double fMaxLight = -1, fMinLight = 256;
	double fLight;
	int i,j;
	for ( j=0; j<m_iSubImgRow; j++ )
	{
		for ( i=0; i<m_iSubImgCol; i++ )
		{
			cv::Rect ROIRect(i*m_iSubImgWidth, j*m_iSubImgHeight, m_iSubImgWidth, m_iSubImgHeight);//ROI矩形  
			cv::Mat matImgROI(matImage,ROIRect);                                                   //Image的中间部分  
			cv::Scalar temp = cv::mean( matImgROI );
			
			double R = temp[2], G = temp[1], B = temp[0];
			if ( R == 0 )
			{
				return FALSE;
			}
			double fWBError = __max( fabs(1 - B / R), fabs(1 - G / R ) );
			if ( fWBError > m_fWBZeroThreshold )                 // R:G:B != 1:1:1, 白平衡之后图像不是灰色的
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

			if ( (fMaxLight - fMinLight) > m_fLightDifference )   // 图像存在亮暗不均匀的现象
			{
				return FALSE;
			}
		}
	}

	pCamera = NULL;

	return TRUE;
}
