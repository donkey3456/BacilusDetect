#include "StdAfx.h"

#include "AutoFocus.h"
#include <math.h>
#include <algorithm> 
#include <vector>
#include "Camera.h"
#include "MotorController.h"
#include "Mechanism.h"
#include "BacilusDetectDoc.h"

extern CMechanism		g_Mechanism;

cv::Point2f	 g_cvCurFOVPosition;

bool  lesser_SearchDataImageCost( CAutoFocus::StructSearchData elem1, CAutoFocus::StructSearchData elem2 )
{
	if( elem1.fSampleImageCost < elem2.fSampleImageCost )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool  lesser_CostPeakDataZPosition( CAutoFocus::StructCostPeakData elem1, CAutoFocus::StructCostPeakData elem2 )
{
	if( elem1.fZPosition < elem2.fZPosition )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool  lesser_FocusPlaneRangeProbability( CAutoFocus::StructFocusPlaneRange elem1, CAutoFocus::StructFocusPlaneRange elem2 )
{
	if( elem1.fRangeProbability < elem2.fRangeProbability )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool  lesser_FOVPosition( CAutoFocus::StructFOVCostPeakData elem1, CAutoFocus::StructFOVCostPeakData elem2 )
{
	float felem1 = __max( fabs(elem1.cvFOVPosition.x - g_cvCurFOVPosition.x), fabs(elem1.cvFOVPosition.y - g_cvCurFOVPosition.y) );
	float felem2 = __max( fabs(elem2.cvFOVPosition.x - g_cvCurFOVPosition.x), fabs(elem2.cvFOVPosition.y - g_cvCurFOVPosition.y) );
	if( felem1 < felem2 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



CAutoFocus::CAutoFocus(void)
	: m_fFPS( 10 )
	, m_fZRefPosition( 0 )  // um
	, m_fMaxStep( 5 )   // um
	, m_fMinStep( 1 )   // um
{	
	m_fWorkPositionMechanics[0] = m_fZRefPosition;
	m_fWorkPositionMechanics[1] = 600;
	//m_fWorkPositionMechanics[1][0] = 70;       // ֻ��1����λ
	//m_fWorkPositionMechanics[1][1] = 690;
	//m_fWorkPositionMechanics[2][0] = 150;
	//m_fWorkPositionMechanics[2][1] = 730;
	//m_fWorkPositionMechanics[3][0] = 140;
	//m_fWorkPositionMechanics[3][1] = 700;
	//m_fWorkPositionMechanics[4][0] = 150;
	//m_fWorkPositionMechanics[4][1] = 750;

	m_fMinPostion = m_fWorkPositionMechanics[0];
	m_fMaxPostion = m_fWorkPositionMechanics[1]; // um

	m_fStartSearchPosition = m_fMinPostion;
	m_fEndSearchPosition = m_fMaxPostion;  

	CCamera* pCamera = CCamera::GetInstance();
	cv::Mat		matImage = pCamera->GetImage();	// ��������㵽��ͼ��
	cv::Size    ImageSize = pCamera->GetCameraImageSize();
	
	m_matBestImage.create(ImageSize, CV_8UC3);
	m_matRefGrayImg.create(cv::Size(ImageSize.width/4, ImageSize.height/4), CV_8UC1);
	m_matRefGrayImg.setTo(0);;
	m_matCurDstGrayImg.create(m_matRefGrayImg.size(),CV_8UC1);

	m_iSubImgCol = 5;
	m_iSubImgRow = 4;
	m_iSubImgWidth = ImageSize.width/ 4 / m_iSubImgCol;
	m_iSubImgHeight = ImageSize.height/ 4 / m_iSubImgRow;

	m_LargeFocusingData.fXYRange = 1000;        // um, 1mm
	m_LargeFocusingData.fFocusChangeRange = 25; // um
	m_LargeFocusingData.fXYChangeSpace = 400;   // um
	m_LargeFocusingData.fDiffPosition = 100;    // um
	m_LargeFocusingData.nIndex = 0;
	m_LargeFocusingData.iMaxTest = 9;

	m_pDetectView = NULL;
	m_pScanRoute = NULL;
	m_nCurFOVIndex = 0;

	m_structInitFOVFocusInfo.fZRefPosition = 300;  // um
	m_structInitFOVFocusInfo.fXYRange = 5000;      // um
	m_structInitFOVFocusInfo.fDeltaY = 300;        // um
	m_structInitFOVFocusInfo.fStopThreshold = 50;  
	m_structInitFOVFocusInfo.fLightThreshold = 25;
	m_structInitFOVFocusInfo.fCostThreshold = 10;
	m_structInitFOVFocusInfo.fFocusChangeRange = 200; // um
}

CAutoFocus::~CAutoFocus(void)
{
	m_File.Close();
}


double CAutoFocus::GetHalfImageLightness(cv::Mat matHalfGrayImg)
{
	int i,j;
	cv::Scalar temp;
	double fMaxLight = -1;
	for ( j=0; j<m_iSubImgRow; j++ )
	{
		for ( i=0; i<m_iSubImgCol; i++ )
		{
			cv::Rect ROIRect(i*m_iSubImgWidth, j*m_iSubImgHeight, m_iSubImgWidth, m_iSubImgHeight);      //ROI����  
			cv::Mat matImgROI(matHalfGrayImg,ROIRect);                                                   //Image���м䲿��  
			temp = cv::mean( matImgROI );

			if ( fMaxLight < temp[0] )
			{
				 fMaxLight = temp[0];
			}
		}
	}
	return fMaxLight;
}

void  CAutoFocus::SetReferenceImage()
{
	CCamera* pCamera = CCamera::GetInstance();
	CMotorController* pMotorController = CMotorController::GetInstance();     

	// Z���˶����ο���
	pMotorController->ZMotorSmoothMove2AbsolutePos( m_fZRefPosition, DISTANCE_UNIT_UM );

	// ȷ��XY���Ѿ�ֹͣ�˶�
	pMotorController->WaitUntilDone();
	
	// �ȴ����㵽��ͼ��
	::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);
	::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);  //�ȴ��µ���������㵽��ͼ������
	// ��òο�ͼ��	
	cv::Mat   matImage = pCamera->GetImage();
	cv::Mat   matGrayImg;
	cv::cvtColor(matImage, matGrayImg, CV_BGR2GRAY); // ת���ɻҶ�ͼ��
	cv::resize(matGrayImg, m_matRefGrayImg, m_matRefGrayImg.size());
	
	pCamera = NULL;
	pMotorController = NULL;
}

void  CAutoFocus::GenerateCurDstGrayImg()
{
	CCamera* pCamera = CCamera::GetInstance();
	cv::Mat   matImage = pCamera->GetImage(); // ��������㵽��ͼ��
	cv::Mat   matGrayImg(matImage.size(),CV_8UC1);
	cv::Mat	  matHalfGrayImg(m_matRefGrayImg.size(), CV_8UC1);
	
	cv::cvtColor(matImage, matGrayImg, CV_BGR2GRAY);//ת���ɻҶ�ͼ��
	cv::resize(matGrayImg,  matHalfGrayImg,  matHalfGrayImg.size());
	
	// �ο�ͼ���ȥ��ǰͼ��
	//cv::Mat_<uchar> mat1 = m_matRefGrayImg;
	//cv::Mat_<uchar> mat2 = matHalfGrayImg;
	//int src1, src2, dst;
	//int i, j, imin = 255;
	//for ( i = 0; i < m_matRefGrayImg.rows; i++ )
	//{
	//	for ( j = 0; j < m_matRefGrayImg.cols; j++ )
	//	{
	//		src1 = mat1(i,j);
	//		src2 = mat2(i,j);
	//		dst = src1 - src2;
	//		if ( imin > dst ) 
	//		{ 
	//			imin = dst; 
	//		}
	//	}
	//}
	cv::Mat temp(m_matRefGrayImg.size(), CV_16SC1);
	cv::subtract(m_matRefGrayImg, matHalfGrayImg, temp, cv::noArray(), CV_16S);
	double minVal;
	cv::minMaxLoc(temp, &minVal, NULL, NULL, NULL);
	//imin = minVal;
	if ( minVal > 0 )
	{
		minVal = 0;
	}

	//for ( i = 0; i < m_matRefGrayImg.rows; i++ )
	//{
	//	for ( j = 0; j < m_matRefGrayImg.cols; j++ )
	//	{
	//		src1 = mat1(i,j);
	//		src2 = mat2(i,j);
	//		dst = src1 - src2;
	//		mat2(i,j) = dst - imin;
	//	}
	//}
	cv::subtract(m_matRefGrayImg, matHalfGrayImg, temp, cv::noArray(), CV_16S);
	cv::subtract(temp, minVal, matHalfGrayImg , cv::noArray(), CV_8U);

	cv::medianBlur(matHalfGrayImg,m_matCurDstGrayImg,3);

	pCamera = NULL;
}

void  CAutoFocus::RecordSampleData(float fStarPos, float fEndPos, float fStep, SEARCH_STOP Enum_stop)
{
	CCamera* pCamera = CCamera::GetInstance();
	CMotorController* pMotorController = CMotorController::GetInstance(); 

	// Z���˶����������ʼ��
	pMotorController->ZMotorSmoothMove2AbsolutePos( fStarPos, DISTANCE_UNIT_UM );
	
	// ��������������Z�����˶��ٶ�
	pMotorController->SetZMotorSpeed(fStep *  m_fFPS, DISTANCE_UNIT_UM);
	using namespace std;
	m_vSampleData.clear();
	int iDataSize = 2 * ceil( fabs(fEndPos - fStarPos) / fStep );
	m_vSampleData.reserve(iDataSize);

	// ȷ��XY���Ѿ�ֹͣ�˶�
	pMotorController->WaitUntilDone();
	
	::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);		// �µ�ͼ�����ݵ���
	
	// ����̨����ʼ������ֹ�������˶�������ʼ������������¼���˶������е�����������
	pMotorController->ZMotorRelativeMove((fEndPos - fStarPos), DISTANCE_UNIT_UM);  	
	int nIndex;
	int i, j;

	LARGE_INTEGER l1,l2,lf;
	QueryPerformanceFrequency(&lf);
	vector<double> times;
	double time;
	QueryPerformanceCounter(&l1);
	for ( nIndex = 0; nIndex<iDataSize; nIndex++ )
	{	
		
		StructSearchData structSearchData;	
		::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);		// �µ�ͼ�����ݵ���
		QueryPerformanceCounter(&l2);
		time = double(l2.QuadPart - l1.QuadPart)/lf.QuadPart;
		times.push_back(time);
		// ���ɼ����ĵ�ǰԭʼͼ��ת��Ϊ�������ĻҶ�ͼ��
		QueryPerformanceCounter(&l1);
		GenerateCurDstGrayImg(); 
		QueryPerformanceCounter(&l2);
		time = double(l2.QuadPart - l1.QuadPart)/lf.QuadPart;
		times.push_back(time);
		QueryPerformanceCounter(&l1);
		// ���Z�ᵱǰ��λ��, unit: um
		float fZPos;
		pMotorController->GetZMotorPosition(fZPos,DISTANCE_UNIT_UM);
		QueryPerformanceCounter(&l2);
		time = double(l2.QuadPart - l1.QuadPart)/lf.QuadPart;
		times.push_back(time);
		QueryPerformanceCounter(&l1);
		structSearchData.fSamplePosition = fZPos;
				
		// ���㵱ǰ�Ҷ�ͼ���������
		cv::Scalar temp, tmean; 
		float fSum = 0;
		for ( j=0; j<m_iSubImgRow; j++ )
		{
			for ( i=0; i<m_iSubImgCol; i++ )
			{
				cv::Rect ROIRect(i*m_iSubImgWidth, j*m_iSubImgHeight, m_iSubImgWidth, m_iSubImgHeight);      //ROI����  
				cv::Mat matImgROI(m_matCurDstGrayImg,ROIRect);                                               //Image���м䲿��  
				cv::meanStdDev(matImgROI,tmean,temp);
				fSum += temp[0];
			}
		}	
		structSearchData.fSampleImageCost = fSum  / (m_iSubImgRow*m_iSubImgCol);
		
		volatile MOTOR_STATUS	status;
		pMotorController->GetCurrentStatus((MOTOR_STATUS)status);
		if ( status != MOTOR_RUN )						 // ���ֹͣ�˶���������Χ����
		{
			break;
		}
	
		m_vSampleData.push_back(structSearchData);

		BOOL  bStop = FALSE;
		switch (Enum_stop)
		{
		case NOSTOP:
			bStop = FALSE;
			break;

		case SEARCH_PEAK:
			bStop = SearchPeakStop(&m_vSampleData);
			break;

		default:
			break;
		}

		if ( bStop )
		{
			pMotorController->Stop();
			break;
		}
		QueryPerformanceCounter(&l2);
		time = double(l2.QuadPart - l1.QuadPart)/lf.QuadPart;
		times.push_back(time);
		QueryPerformanceCounter(&l1);
	}
	m_vSampleData.shrink_to_fit();
	//DrawCostCurve(m_vSampleData.begin(),m_vSampleData.end());
	pCamera = NULL;
	pMotorController = NULL;
}

void  CAutoFocus::MarkPeakInCostCurve(vector <StructCostPeakData> * pvPeakData, vector <StructSearchData> * pvSampleData, float fZeroSkewError)
{
	pvPeakData->clear();

	if ( pvSampleData->size() <= 2 )
	{
		return;
	}

	if ( pvSampleData->size() == 3 )
	{
		if ( ( ( *(pvSampleData->begin() + 1) ).fSampleImageCost > (*(pvSampleData->begin()  )).fSampleImageCost ) &&
			 ( ( *(pvSampleData->begin() + 1) ).fSampleImageCost > (*(pvSampleData->begin()+2)).fSampleImageCost ) )
		{
			StructCostPeakData CostPeakData;
			CostPeakData.fZPosition = ( *(pvSampleData->begin() + 1) ).fSamplePosition;
			CostPeakData.fImageCost = ( *(pvSampleData->begin() + 1) ).fSampleImageCost;
			pvPeakData->push_back(CostPeakData);
		}
		return;
	}

	// �������������ߵ�б��	
	float fDeltaPosition, fDeltaCost;
	vector <StructSearchData>::iterator Iter, Iter1, Iter2;
	for( Iter = pvSampleData->begin(); Iter != pvSampleData->end(); Iter++ )
	{
		if ( Iter == pvSampleData->begin() )
		{
			fDeltaCost =  (*(Iter+1)).fSampleImageCost - (*Iter).fSampleImageCost;
			fDeltaPosition = 2 * ( (*(Iter+1)).fSamplePosition - (*Iter).fSamplePosition );   // ����һ��������ǰ����һ��λ��
		}
		else if ( Iter == ( pvSampleData->end()-1 ) )
		{
			fDeltaCost =  (*Iter).fSampleImageCost - (*(Iter-1)).fSampleImageCost;
			fDeltaPosition = 2 * ( (*Iter).fSamplePosition - (*(Iter-1)).fSamplePosition ); // �����һ�������������һ��λ��
		}
		else
		{
			fDeltaCost =  (*(Iter+1)).fSampleImageCost - (*(Iter-1)).fSampleImageCost;
			fDeltaPosition = (*(Iter+1)).fSamplePosition - (*(Iter-1)).fSamplePosition; 
		}

		if ( fDeltaPosition == 0 )
		{
			(*Iter).fCostSkew = 0; // 0
			continue;
		}

		// ����: fDeltaPosition > 0		
		if ( fDeltaCost >= fZeroSkewError * fabs(fDeltaPosition) )
		{
			(*Iter).fCostSkew = 1; // +
		}
		else if ( fDeltaCost > -fZeroSkewError * fabs(fDeltaPosition) )
		{
			(*Iter).fCostSkew = 0; // 0
		}
		else
		{
			(*Iter).fCostSkew = -1; // -
		}

		if ( fDeltaPosition < 0 )
		{
			(*Iter).fCostSkew = - (*Iter).fCostSkew;
		}
	}
		
	// �Ӻ���ǰѰ������ɽ���λ��
	Iter = pvSampleData->end() - 1;
	while( Iter > pvSampleData->begin() )
	{
		if ( (Iter > pvSampleData->begin()) && (*Iter).fCostSkew > 0 )  // +
		{
			Iter--;
			continue;
		}
		if ( Iter == pvSampleData->begin() ) { break; }

		while( Iter > pvSampleData->begin() )
		{
			if( (*Iter).fCostSkew == (*(Iter-1)).fCostSkew )
			{
				Iter--;
			}
			else
			{
				Iter2 = Iter;
				break;
			}
		}
		if ( Iter == pvSampleData->begin() ) { break; }

		if ( (*Iter).fCostSkew == 0 )        
		{
			Iter--;
			if( (*Iter).fCostSkew < 0 )		    // -0
			{
				continue;
			}
			else								// +0
			{
				Iter1 = Iter;
			}
		}
		else                                 
		{
			Iter--;
			Iter1 = Iter;
			if ( (*Iter).fCostSkew == 0 )	         // 0-
			{
				if ( Iter > pvSampleData->begin() )
				{
					if ( (*(Iter-1)).fCostSkew > 0 ) // +0-
					{
						Iter--;
						Iter1 = Iter;
					}
				}
			}							              // +-
		}

		// Ѱ��ָ�������������ȵ����ֵ	

		vector <StructSearchData>::iterator iPeak = max_element(Iter1, Iter2+1, lesser_SearchDataImageCost);
		if( iPeak == pvSampleData->begin() )		{ break; }
		if( iPeak == (pvSampleData->end() - 1) )	{ continue; }
		if( ( (*iPeak).fSampleImageCost >= (*(iPeak-1)).fSampleImageCost ) && ( (*iPeak).fSampleImageCost >= (*(iPeak+1)).fSampleImageCost ) )   // ȷ����ɽ��
		{
			StructCostPeakData CostPeakData;
			CostPeakData.fZPosition = (*iPeak).fSamplePosition;
			CostPeakData.fImageCost = (*iPeak).fSampleImageCost;
			CostPeakData.fZBestPosition = ComputeBestPeak(*pvSampleData, iPeak);

			pvPeakData->push_back(CostPeakData);

			if ( Iter >= iPeak )
			{
				Iter = iPeak - 1;
			}
		}
	}	
}

// ͨ������������ϵķ���������������Ⱦֲ���ֵ�����λ��
float  CAutoFocus::ComputeBestPeak(vector <StructSearchData>& vSampleData, vector <StructSearchData>::iterator iPeak)
{
	if ( iPeak == vSampleData.begin() )
	{
		return -1;
	}

	if ( iPeak == ( vSampleData.end() - 1 ) )
	{
		return -1;
	}

	cv::Mat  matX, matY, matBeta;
	matX.create(3, 3, CV_64F);
	matY.create(3, 1, CV_64F);
	matBeta.create(3, 1, CV_64F);
	//CvMat* matX = cvCreateMat( 3, 3, CV_64F );
	//CvMat* matY = cvCreateMat( 3, 1, CV_64F );
	//CvMat* matBeta = cvCreateMat( 3, 1, CV_64F );

	double x0 = (*(iPeak-1)).fSamplePosition;
	double x;
	int i = 0;
	vector <StructSearchData>::iterator Iter;
	for ( Iter = iPeak-1; Iter < iPeak + 2; Iter++, i++ )
	{
		x = (*Iter).fSamplePosition - x0;

		matX.at<double>(i, 0) = x * x;
		matX.at<double>(i, 1) = x;
		matX.at<double>(i, 2) = 1;
		//cvSetReal2D(matX,i,0,	x * x ); 
		//cvSetReal2D(matX,i,1,	x     );
		//cvSetReal2D(matX,i,2,	1     );

		matY.at<double>(i, 0) = (*Iter).fSampleImageCost;
		//cvSetReal2D(matY,i,0, (*Iter).fSampleImageCost );
	}
	
	// Linear least squares method
	solve(matX, matY, matBeta, CV_SVD);
	//cvSolve( matX, matY, matBeta, CV_SVD);

	//double a = cvGetReal2D( matBeta, 0, 0 );    
	//double b = cvGetReal2D( matBeta, 1, 0 );     	
	double a = matBeta.at<double>(0, 0);    
	double b = matBeta.at<double>(1, 0);    

	//cvReleaseMat(&matX);
	//cvReleaseMat(&matY);
	//cvReleaseMat(&matBeta);

	return x0 - b / 2 / a;
}

BOOL  CAutoFocus::AnalyzeImpurity(cv::Mat matBinaryImage)
{
	int iSmallArea = 50;
	int iLargeArea = 300;
	int iImpurityArea = 200;
	float fLenghWidthRatio = 0.25;

	int iArea = CalcTargetArea(matBinaryImage);		
	if ( iArea > iLargeArea )   // ������Ƚϴ󣬲�������
	{
		return FALSE; 
	}

	BOOL bRet = TRUE;

	vector<cv::Vec4i> hierarchy;
	vector<vector<cv::Point> > contour;
	findContours( matBinaryImage,  contour, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
 
 	vector<vector<cv::Point> >::const_iterator cIter;
 	for ( cIter = contour.begin(); cIter < contour.end();  cIter++)
 	{
 		double fArea = fabs( contourArea(*cIter) );
 		cv::RotatedRect Box = minAreaRect(*cIter);
 		if ( fArea > iImpurityArea )    // Ŀ�������Ƚϴ󣬲�������
 		{
 			bRet = FALSE;
 			break;
 		}
 		else if (  fArea > iSmallArea ) // ����Ŀ��ĳ���ȣ��ж�Ŀ���Ƿ�Ϊ����
 		{
 			float fmin, fmax;
 			if ( Box.size.width < Box.size.height )
 			{
 				fmin = Box.size.width;
 				fmax = Box.size.height;
 			}
 			else
 			{
 				fmin = Box.size.height;
 				fmax = Box.size.width;
 			}
 			if ( fmax != 0 )
 			{
 				if ( (fmin / fmax) < fLenghWidthRatio )   // ���Ŀ���ǳ�����״���������ϸ������������
 				{
 					bRet = FALSE;
 					break;
 				}									      // ���Ŀ�����״�ȽϽӽ�Բ�Σ��������ʣ�����Ѱ����һ��Ŀ�����
 			}
 		}
 		else                            // Ŀ��������С�������ʣ�����Ѱ����һ��Ŀ�����
 		{
 		}				
 	}

	return bRet;
}

BOOL  CAutoFocus::CheckTestFOVValid(cv::Point2f cvFOVPosition)
{
	if ( m_vFOVCostPeakDataInLargeRange.empty() )       // û������֪ʶ����ǰ�Ĳ�����Ұλ����Ч
	{
		return TRUE;
	}

	for( vector <StructFOVCostPeakData>::const_iterator Iter = m_vFOVCostPeakDataInLargeRange.begin(); Iter != m_vFOVCostPeakDataInLargeRange.end(); Iter++ )
	{
		float fTemp = __max ( fabs( (*Iter).cvFOVPosition.x - cvFOVPosition.x ),  fabs( (*Iter).cvFOVPosition.y - cvFOVPosition.y ) );
		if ( fTemp  < 1.9 * m_LargeFocusingData.fDiffPosition )   // 190 um����Ҫ���Ե���Ұλ�����Ѳ��Ե���Ұλ�ÿ���̫����λ��ѡ�񲻺���
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL  CAutoFocus::Move2NextTestFOV()
{
	CMotorController* pMotorController = CMotorController::GetInstance();

	if ( m_LargeFocusingData.nIndex >= m_LargeFocusingData.iMaxTest )
	{
		m_LargeFocusingData.nIndex = 0;		
		return FALSE;
	}

	if ( m_LargeFocusingData.nIndex < 0 )
	{
		m_LargeFocusingData.nIndex = 0;
	}		
	
	_ASSERTE( m_pScanRoute != NULL );

	int xSign = 1;
	int ySign = 1;
	if ( m_nCurFOVIndex < ( m_pScanRoute->ScanSequence.GetSize() - 1 ) )
	{
		MATRIXINDEX curMatrixIndex, nextMatrixIndex;
		curMatrixIndex = m_pScanRoute->ScanSequence[m_nCurFOVIndex];
		int tempNextFOVIndex;
		for ( tempNextFOVIndex = m_nCurFOVIndex+1; tempNextFOVIndex < m_pScanRoute->ScanSequence.GetSize(); tempNextFOVIndex++ )
		{
			nextMatrixIndex = m_pScanRoute->ScanSequence[tempNextFOVIndex];
			if ( ( nextMatrixIndex.col != curMatrixIndex.col ) && (nextMatrixIndex.row != curMatrixIndex.row) )
			{
				break;
			}
		}
		if ( nextMatrixIndex.col < curMatrixIndex.col )
		{
			xSign = -1;
		}
		if ( nextMatrixIndex.row < curMatrixIndex.row )
		{
			ySign = -1;
		}
	}
	
	while ( m_LargeFocusingData.nIndex < m_LargeFocusingData.iMaxTest )
	{
		int i, j;
		switch ( m_LargeFocusingData.nIndex )
		{
		case 0:
			i = 0; j = 0;
			break;
		case 1:
			i = 1; j = 0;
			break;
		case 2:
			i = 1; j = 1;
			break;
		case 3:
			i = 0; j = 1;
			break;
		case 4:
			i = 2; j = 0;
			break;
		case 5:
			i = 2; j = 1;
			break;
		case 6:
			i = 2; j = 2;
			break;
		case 7:
			i = 1; j = 2;
			break;		
		case 8:
			i = 0; j = 2;
			break;
		default:
			m_LargeFocusingData.nIndex = 0;
			return FALSE;
			break;
		}		

		cv::Point2f	 cvNextFOVPosition;
		cvNextFOVPosition.x = m_LargeFocusingData.cvFOVPosition.x + (m_LargeFocusingData.fDiffPosition + i * m_LargeFocusingData.fXYChangeSpace) * xSign;
		cvNextFOVPosition.y = m_LargeFocusingData.cvFOVPosition.y + (m_LargeFocusingData.fDiffPosition + j * m_LargeFocusingData.fXYChangeSpace) * ySign;

		m_LargeFocusingData.nIndex++;

		// �����Ҫ���Ե���Ұλ���Ƿ����Ѳ��Ե���Ұλ�ÿ���̫�����������������Ұ����Ҫ����λ�ý��в���
		if ( CheckTestFOVValid(cvNextFOVPosition) )
		{	
			pMotorController->XYMotorSmoothMove2Point(cvNextFOVPosition, DISTANCE_UNIT_UM);
			return TRUE;
		}
	
		ySign = -ySign;
		cvNextFOVPosition.y = m_LargeFocusingData.cvFOVPosition.y + (m_LargeFocusingData.fDiffPosition + j * m_LargeFocusingData.fXYChangeSpace) * ySign;
		if ( CheckTestFOVValid(cvNextFOVPosition) )
		{	
			pMotorController->XYMotorSmoothMove2Point(cvNextFOVPosition, DISTANCE_UNIT_UM);
			return TRUE;
		}

		xSign = -xSign;
		cvNextFOVPosition.x = m_LargeFocusingData.cvFOVPosition.x + (m_LargeFocusingData.fDiffPosition + i * m_LargeFocusingData.fXYChangeSpace) * xSign;
		if ( CheckTestFOVValid(cvNextFOVPosition) )
		{	
			pMotorController->XYMotorSmoothMove2Point(cvNextFOVPosition, DISTANCE_UNIT_UM);
			return TRUE;
		}

		ySign = -ySign;
		cvNextFOVPosition.y = m_LargeFocusingData.cvFOVPosition.y + (m_LargeFocusingData.fDiffPosition + j * m_LargeFocusingData.fXYChangeSpace) * ySign;
		if ( CheckTestFOVValid(cvNextFOVPosition) )
		{	
			pMotorController->XYMotorSmoothMove2Point(cvNextFOVPosition, DISTANCE_UNIT_UM);
			return TRUE;
		}

		xSign = -xSign;
	}

	m_LargeFocusingData.nIndex = 0;		

	pMotorController = NULL;

	return FALSE;
}

void  CAutoFocus::AnalyzeFocusPosProb(float fStarPos, float fEndPos, float fStep)
{
	CMotorController* pMotorController = CMotorController::GetInstance();     

	float fZeroSkewError = 0.01; // ����������0б���������
	float fCostThreshold = 1;     // ���Խ��ж�ֵ����������������ֵ�����ڸ�ֵ������������Ŀ����Ҫ����

	if ( fStarPos > fEndPos )
	{
		float temp = fEndPos;
		fEndPos = fStarPos;
		fStarPos = temp;
	}
	
	// �ڸ����ķ�Χ�ڣ����ϵ���������������¼�µ����ݱ����� m_vSampleData ��
	RecordSampleData(fStarPos, fEndPos, fStep, NOSTOP);	
	if ( m_vSampleData.empty() )
	{
		return ;
	}
		
	// �Ӻ���ǰѰ������ɽ���λ��
	vector <StructCostPeakData> vPeakData;
	MarkPeakInCostCurve(&vPeakData, &m_vSampleData, fZeroSkewError);
	//==============================================================//
	//DrawCostCurve2(&m_vSampleData, &vPeakData);  
	//==============================================================//
	if ( vPeakData.empty() )
	{
		return;
	}
	
	// ����ÿһ����ֵ���������ͼ�񣬲����з���
	vector <StructCostPeakData>::iterator PeakIter;
	for ( PeakIter=vPeakData.begin(); PeakIter!=vPeakData.end(); PeakIter++)
	{
		// ����ÿһ����ֵ���������ͼ��		
		StructBestImageData SBestImageData;
		SBestImageData.matImage.create(m_matBestImage.size(), CV_8UC3);
		SBestImageData.matDstGrayImg.create(m_matCurDstGrayImg.size(), CV_8UC1);
		SBestImageData.pfSubImgCost = new float[m_iSubImgRow*m_iSubImgCol];				
			
		float fStarPos = (*PeakIter).fZPosition - fStep;
		float fEndPos = fStarPos - 4*fStep;
		SearchBestImage(&SBestImageData, fStarPos, fEndPos, m_fMinStep);

		// �ֱ�������ͼ���ÿһ����ͼ��������Ч��ͼ������
		cv::Mat matBinaryImg = cv::Mat::zeros(SBestImageData.matDstGrayImg.size(), CV_8UC1);
		
		int iSubImgCount = 0;
		float*	pfSubImgCost = SBestImageData.pfSubImgCost;

		int* piSubImgInfo = new int[m_iSubImgRow*m_iSubImgCol];	
		int* ptemp = piSubImgInfo; 
		cv::Mat matBinaryImgTemp = matBinaryImg.clone();
		int i,j;
		for ( j=0; j<m_iSubImgRow; j++ )
		{
			for ( i=0; i<m_iSubImgCol; i++ )
			{
				*ptemp = 0;

				cv::Rect ROIRect(i*m_iSubImgWidth, j*m_iSubImgHeight, m_iSubImgWidth, m_iSubImgHeight);      //ROI���� 

				cv::Mat matDstGrayImg_ROI(SBestImageData.matDstGrayImg, ROIRect);
				//cvSetImageROI( SBestImageData.pcvDstGrayImg, cvRect(i*m_iSubImgWidth, j*m_iSubImgHeight, m_iSubImgWidth, m_iSubImgHeight) );
				if ( *pfSubImgCost> fCostThreshold )
				{
					// ͼ���ֵ��
					cv::Mat matBinaryImg_ROI(matBinaryImg, ROIRect);
					//cvSetImageROI( pcvBinaryImg , cvRect(i*m_iSubImgWidth, j*m_iSubImgHeight, m_iSubImgWidth, m_iSubImgHeight) );
					int thresholdValue = otsu (matDstGrayImg_ROI);		
					cv::threshold(matDstGrayImg_ROI, matBinaryImg_ROI, thresholdValue, 255, CV_THRESH_BINARY);
					//cvThreshold(SBestImageData.pcvDstGrayImg, pcvBinaryImg, thresholdValue, 255, CV_THRESH_BINARY);

					cv::Mat matBinaryImgTemp_ROI(matBinaryImgTemp, ROIRect);
					matBinaryImg.copyTo(matBinaryImgTemp_ROI);
					//cvSetImageROI( pcvBinaryImgTemp, cvRect(i*m_iSubImgWidth, j*m_iSubImgHeight, m_iSubImgWidth, m_iSubImgHeight) );
					//cvCopy(  pcvBinaryImg, pcvBinaryImgTemp );

					// ��������ͼ�Ƿ�����Ч��ǰ��
					if ( ! AnalyzeImpurity(matBinaryImg_ROI) )
					{
						*ptemp = 1;
						iSubImgCount++;
					}					
				}
				
				pfSubImgCost++;
				ptemp++;
			}
		}	

		(*PeakIter).iSubImgCount = iSubImgCount;

		delete [] piSubImgInfo;
		////==============================================================//
	
		delete [] SBestImageData.pfSubImgCost;
	}
	
	// �������ͼ��λ�ڽ�ƽ���ϵĸ���
	int iTotalSubImgCount = 0;
	for ( PeakIter=vPeakData.begin(); PeakIter!=vPeakData.end(); PeakIter++)
	{
		iTotalSubImgCount += (*PeakIter).iSubImgCount;
	}
	if ( iTotalSubImgCount > 0 )
	{
		StructFOVCostPeakData	SCurFOVCostPeakData; // ��ǰ���ھ۽�����Ұ������
		cv::Point2f ptPos;
		pMotorController->GetXYMotorPosition(ptPos,DISTANCE_UNIT_UM);
		SCurFOVCostPeakData.cvFOVPosition = ptPos;      // ��¼��ǰ��Ұ��λ��
		for ( PeakIter=vPeakData.begin(); PeakIter!=vPeakData.end(); PeakIter++)
		{
			int iSubImgCount = (*PeakIter).iSubImgCount;
			if ( iSubImgCount > 0 )
			{
				(*PeakIter).fProbability = (float)(iSubImgCount * iSubImgCount)  / (float) ( (m_iSubImgRow * m_iSubImgCol) * iTotalSubImgCount );
				SCurFOVCostPeakData.vCostPeakData.push_back( (*PeakIter) );
			}
		}
		SCurFOVCostPeakData.vCostPeakData.shrink_to_fit();

		m_vFOVCostPeakDataInLargeRange.push_back(SCurFOVCostPeakData);	// ��¼�´�Χ�۽�������
		m_vFOVCostPeakData.push_back(SCurFOVCostPeakData);  // ��¼�µ�ǰ���ھ۽�����Ұ������
	}

	pMotorController = NULL;
}

// �� vFOVCostPeakData �е����ݽ��з��������۽���Ұ���������ݰ���Z��λ���������򣬲�����m_vFocusPlaneRange����
BOOL  CAutoFocus::GenerateEstFocusData(cv::Point2f cvFOVPosition, vector <StructFOVCostPeakData>& vFOVCostPeakData)
{
	m_vFocusPlaneRange.clear();

	if ( vFOVCostPeakData.empty() )
	{
		return FALSE;
	}

	using namespace std;
	vector <StructCostPeakData>		vCostPeakData;  
	for( vector <StructFOVCostPeakData>::const_iterator Iter = vFOVCostPeakData.begin(); Iter != vFOVCostPeakData.end(); Iter++ )
	{
		float fTemp = __max ( fabs( (*Iter).cvFOVPosition.x - cvFOVPosition.x ),  fabs( (*Iter).cvFOVPosition.y - cvFOVPosition.y ) );
		if ( fTemp  <= m_LargeFocusingData.fXYRange )
		{
			vCostPeakData.insert(vCostPeakData.end(), (*Iter).vCostPeakData.begin(), (*Iter).vCostPeakData.end() );
		}
	}

	if ( vCostPeakData.empty() )
	{
		return FALSE;
	}

	sort( vCostPeakData.begin(), vCostPeakData.end(), lesser_CostPeakDataZPosition );

	//DrawPeakProbCurve( vCostPeakData.begin(), vCostPeakData.end() );

	float fRange = m_LargeFocusingData.fFocusChangeRange;	
	m_vFocusPlaneRange.reserve( vCostPeakData.size() );
	StructFocusPlaneRange temp;
	vector <StructCostPeakData>::const_iterator	Iter, Iter1, Iter2, IterTemp;
	for ( Iter = vCostPeakData.begin(); Iter != vCostPeakData.end(); Iter++ )
	{
		temp.fZPosition = (*Iter).fZPosition;
		Iter1 = Iter2 = Iter;

		IterTemp = Iter;
		while( IterTemp > vCostPeakData.begin() )			 
		{
			IterTemp--;
			if( fabs( (*IterTemp).fZPosition - (*Iter).fZPosition ) < fRange )
			{
				Iter1 = IterTemp;
			}
			else
			{
				break;
			}
		}

		IterTemp = Iter;
		while( IterTemp < (vCostPeakData.end()-1) )			 
		{
			IterTemp++;
			if( fabs( (*IterTemp).fZPosition - (*Iter).fZPosition ) < fRange )
			{
				Iter2 = IterTemp;
			}
			else
			{
				break;
			}
		}

		temp.iPeakCount = Iter2 - Iter1 + 1;
		float fRangeProbability = 0;
		float fZPositionSum = 0;
		for ( IterTemp = Iter1; IterTemp <= Iter2; IterTemp++ )
		{
			fRangeProbability += (*IterTemp).fProbability;
			fZPositionSum += (*IterTemp).fZPosition;
		}
		temp.fRangeProbability = fRangeProbability;
		temp.fZPositionMean = fZPositionSum / temp.iPeakCount;
		m_vFocusPlaneRange.push_back(temp);
	}
	if ( m_vFocusPlaneRange.empty() )
	{
		return FALSE;
	}

	//DrawFocusPlaneRangeCurve();

	return TRUE;
}

BOOL  CAutoFocus::EstFocusPositionStop()
{
	if ( ! GenerateEstFocusData(m_LargeFocusingData.cvFOVPosition, m_vFOVCostPeakDataInLargeRange) )
	{
		return FALSE;
	}	

	vector <StructFocusPlaneRange>::const_iterator MaxProbIter = max_element(m_vFocusPlaneRange.begin(), m_vFocusPlaneRange.end(), lesser_FocusPlaneRangeProbability);

	if ( (*MaxProbIter).fRangeProbability >= 1 )
	{
		m_fStartSearchPosition = (*MaxProbIter).fZPositionMean - m_LargeFocusingData.fFocusChangeRange;
		m_fEndSearchPosition = (*MaxProbIter).fZPositionMean + m_LargeFocusingData.fFocusChangeRange;  
		return TRUE;
	}
	
	return FALSE;
}


int   CAutoFocus::SearchBestImage(StructBestImageData* pSBestImageData /*Output*/, float fStartPos, float fEndPos, float fStep)
{
	CCamera* pCamera = CCamera::GetInstance();
	CMotorController* pMotorController = CMotorController::GetInstance();

	// Z���˶�����ʼ��
	pMotorController->ZMotorSmoothMove2AbsolutePos(fStartPos, DISTANCE_UNIT_UM);
	pMotorController->WaitUntilDone();
	// ��������������Z�����˶��ٶ�	
	pMotorController->SetZMotorSpeed(fStep * m_fFPS, DISTANCE_UNIT_UM);  

	using namespace std;
	vector <StructSearchData> vSampleData;
	int iDataSize = 2 * ceil( fabs(fEndPos - fStartPos) / fStep );
	vSampleData.reserve(iDataSize);

	::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);		// �µ�ͼ�����ݵ���
	
	// ����̨��ʼ���ϵ��½��������˶�������ʼ������������¼���˶������е�����������
	pMotorController->ZMotorRelativeMove((fEndPos - fStartPos), DISTANCE_UNIT_UM);
	//pMotorController->ZMotorStartRelativeMove((fEndPos - fStartPos), DISTANCE_UNIT_UM);   	
	int nIndex;	 
	float fBestDefCost = -1;
	int i, j;
	float*	pfSubImgCost = new float[m_iSubImgRow*m_iSubImgCol];
	
	//debug
	LARGE_INTEGER l1,l2,lf;
	QueryPerformanceFrequency(&lf);
	vector<double> times;
	for ( nIndex = 0; nIndex<iDataSize; nIndex++ )
	{	
		StructSearchData structSearchData;
		//���µ�ǰ״̬
		pMotorController->QueryCurrentStatus();

		// ���ֹͣ�˶���������Χ����
		DWORD dwRes = WaitForSingleObject(pMotorController->GetIdleHandle(),0);
		if (dwRes == WAIT_OBJECT_0)
		{
			break;
		}
		//���������˶����Ȼ�õ�ǰλ��
		structSearchData.fSamplePosition = pMotorController->GetZMotorPosition();
		
		//�ȴ���һ����Ƭ
		::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);		// �µ�ͼ�����ݵ���
		// ���ɼ����ĵ�ǰԭʼͼ��ת��Ϊ�������ĻҶ�ͼ��
		GenerateCurDstGrayImg();
		
		// ���Z�ᵱǰ��λ��, unit: um
		// ���㵱ǰ�Ҷ�ͼ���������	
		cv::Scalar mean, temp; 	
		float fSum = 0;
		float*	pfSubImgCostTemp = pfSubImgCost;
		for ( j=0; j<m_iSubImgRow; j++ )
		{
			for ( i=0; i<m_iSubImgCol; i++ )
			{
				cv::Rect ROIRect(i*m_iSubImgWidth, j*m_iSubImgHeight, m_iSubImgWidth, m_iSubImgHeight);      //ROI����
				cv::Mat  m_matCurDstGrayImg_ROI(m_matCurDstGrayImg, ROIRect);
				cv::meanStdDev(m_matCurDstGrayImg_ROI, mean, temp);
				//cvSetImageROI( m_pcvCurDstGrayImg, cvRect(i*m_iSubImgWidth, j*m_iSubImgHeight, m_iSubImgWidth, m_iSubImgHeight) );
				//cvAvgSdv(m_pcvCurDstGrayImg, NULL, &temp);	// �Ҷ�ͼ���׼����	
			
				*pfSubImgCostTemp++ = temp[0];
				fSum += temp[0];
			}
		}	
		structSearchData.fSampleImageCost = fSum / (m_iSubImgRow*m_iSubImgCol);

		if ( fBestDefCost < structSearchData.fSampleImageCost )
		{
			fBestDefCost = structSearchData.fSampleImageCost;

			cv::Mat matImage = pCamera->GetImage(); // ��������㵽��ͼ��
			matImage.copyTo(pSBestImageData->matImage);
			m_matCurDstGrayImg.copyTo(pSBestImageData->matDstGrayImg);// ����������ĻҶ�ͼ��
		
			pSBestImageData->fZPosition = structSearchData.fSamplePosition;
			pSBestImageData->fImageCost = structSearchData.fSampleImageCost;	
			float*	pfTemp = pSBestImageData->pfSubImgCost;
			pfSubImgCostTemp = pfSubImgCost;
			for ( i = 0; i<m_iSubImgRow*m_iSubImgCol; i++ )
			{
				*pfTemp++ = *pfSubImgCostTemp++;
			}
		}	
		
		vSampleData.push_back(structSearchData);

		if ( SearchBestImageStop(&vSampleData) )
		{
			pMotorController->Stop();
			break;
		}	
	}
	vSampleData.shrink_to_fit();
	delete [] pfSubImgCost;
	
	pMotorController = NULL;
	
	pCamera = NULL;
	//==============================================================//
	//DrawCostCurve(vSampleData.begin(), vSampleData.end());	
	//==============================================================//


	// Debug
	float fTempL = vSampleData.size();
	if ( fTempL <= 0 )
	{
		return -1;
	}
	m_File.Write(&fTempL, sizeof(float));
	vector <StructSearchData>::iterator IterTemp;
	for ( IterTemp = vSampleData.begin(); IterTemp != vSampleData.end(); IterTemp++ )
	{
		m_File.Write(&((*IterTemp).fSamplePosition), sizeof(float));
		m_File.Write(&((*IterTemp).fSampleImageCost), sizeof(float));
	}

	if ( (*(vSampleData.begin())).fSampleImageCost >= fBestDefCost ) 
	{
		return 1;
	}

	if ( (*(vSampleData.end()-1)).fSampleImageCost >= fBestDefCost ) 
	{
		return -1;
	}

	return 0;
}

BOOL  CAutoFocus::SearchBestImageStop(vector <StructSearchData> * pvSampleData)
{
	if ( pvSampleData->size() < 3 )       // ����̫�٣��޷�׼ȷ�ж�
	{
		return FALSE;
	}

	vector <StructSearchData>::const_iterator iPeak = max_element(pvSampleData->begin(), pvSampleData->end(), lesser_SearchDataImageCost);
	vector <StructSearchData>::const_iterator iLast = pvSampleData->end() - 1;
	if( iPeak == iLast )			       // ��ǰֵΪ���ֵ����ȷ���Ƿ��Ƿ嶥
	{
		return FALSE;
	}
	if( iPeak == pvSampleData->begin() )   // ��һ��ֵΪ���ֵ����ȷ���Ƿ��Ƿ嶥
	{
		return FALSE;
	}

	float fChangeRatioError = 0.1;  // ��������Ч�仯���������; 10%
	float fmaxCost = (*iPeak).fSampleImageCost;
	float fcurCost = (*iLast).fSampleImageCost;
	if ( (fmaxCost - fcurCost) / fmaxCost > fChangeRatioError )     // ��ǰ����������嶥�Ѿ���������Ч�仯
	{
		return TRUE;
	}

	float fChangPosition = 5;   // 5um
	if ( fabs((*iLast).fSamplePosition -(*iPeak).fSamplePosition) > fChangPosition )  // ��ǰλ���Ѿ��뿪�嶥5umԶ�ˣ�������Ϊ�Ѿ���������ֵ��
	{
		return TRUE;
	}	

	return FALSE;
}

BOOL  CAutoFocus::SearchPeakStop(vector <StructSearchData> * pvSampleData)
{
	if ( pvSampleData->size() < 6 )					   // ����̫�٣��޷�׼ȷ�ж�
	{
		return FALSE;
	}

	vector <StructSearchData>::const_iterator iLast = pvSampleData->end() - 1;
	float fcurCost = (*iLast).fSampleImageCost;
	if ( fcurCost > (*(iLast-1)).fSampleImageCost )   // ��ǰֵ������������������ƣ�����ȷ���Ѿ��ҵ����е�ɽ��
	{
		return FALSE;
	}

	float fZero = 0.1;
	vector <StructSearchData>::const_iterator iPeak = max_element(pvSampleData->begin()+1, pvSampleData->end(), lesser_SearchDataImageCost);	// Ѱ��ָ�������������ȵ����ֵ
	if ( (*iPeak).fSampleImageCost < fZero )      // ���ֵ̫С����ȷ����ɽ��
	{
		return FALSE;
	}

	if( iPeak == iLast )						  // ��ǰֵΪ���ֵ����ȷ���Ƿ��Ƿ嶥
	{
		return FALSE;
	}
	if( iPeak == pvSampleData->begin() )          // ��һ��ֵΪ���ֵ����ȷ���Ƿ��Ƿ嶥
	{
		return FALSE;
	}

	float fChangeRatioError = 0.15;  // ��������Ч�仯���������; 15%
	float fmaxCost = (*iPeak).fSampleImageCost;
	if ( (fmaxCost - fcurCost) / fmaxCost > fChangeRatioError )     // ��ǰ����������嶥�Ѿ���������Ч�仯
	{
		return TRUE;
	}
		
	return FALSE;
}

BOOL  CAutoFocus::WhetherExtendRangeDown(float fImageCostThreshold, float fZeroSkewError)
{
	if (m_vSampleData.size() < 3)
	{
		return TRUE;
	}

	vector <StructSearchData>::iterator Iter;
	Iter = m_vSampleData.end() - 1;
	float fDeltaCost = (*Iter).fSampleImageCost - (*(Iter-1)).fSampleImageCost;
	float fDeltaPosition = (*Iter).fSamplePosition - (*(Iter-1)).fSamplePosition;
	if (	(	fDeltaCost > fabs( fZeroSkewError * fDeltaPosition )	)
		&&	(	(*Iter).fSampleImageCost > fImageCostThreshold	)
	   )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL  CAutoFocus::WhetherExtendRangeUp(float fImageCostThreshold, float fZeroSkewError)
{
	if (m_vSampleData.size() < 3)
	{
		return TRUE;
	}

	vector <StructSearchData>::iterator Iter;
	Iter = m_vSampleData.begin() + 1;
	if  (   (*Iter).fSampleImageCost > (*(m_vSampleData.begin())).fSampleImageCost )
	{
		return FALSE;
	}

	float fDeltaCost = (*Iter).fSampleImageCost - (*(Iter+1)).fSampleImageCost;
	float fDeltaPosition = (*(Iter+1)).fSamplePosition - (*Iter).fSamplePosition;
	if (	(	fDeltaCost > fabs( fZeroSkewError * fDeltaPosition )	)
		&&	(	(*Iter).fSampleImageCost > fImageCostThreshold	)
	   )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL  CAutoFocus::GetBestImage(float* pfBestFocusPos, float fStep, SCAN_TYPE scantype)
{
	m_CurFOVCostPeakData.vCostPeakData.clear();	

	float fCostThreshold = 1;     // ���Խ��ж�ֵ����������������ֵ�����ڸ�ֵ������������Ŀ����Ҫ����

	float	fStarPos = m_fStartSearchPosition;
	float	fEndPos = m_fEndSearchPosition;
	if ( fStarPos > fEndPos )
	{
		float temp = fEndPos;
		fEndPos = fStarPos;
		fStarPos = temp;
	}

	// �ڸ����ķ�Χ�ڣ����ϵ���������������¼�µ����ݱ����� m_vSampleData ��
	RecordSampleData(fStarPos, fEndPos, fStep, NOSTOP);	
	if ( m_vSampleData.empty() )
	{
		return FALSE;
	}
	float fExtendCostThreshold = 2;
	BOOL bExtendRange = FALSE;
	if ( WhetherExtendRangeUp(fExtendCostThreshold ) )
	{
		bExtendRange = TRUE;
		fStarPos -= m_LargeFocusingData.fFocusChangeRange;
	}
	if ( WhetherExtendRangeDown(fExtendCostThreshold ) )
	{
		bExtendRange = TRUE;
		fEndPos += m_LargeFocusingData.fFocusChangeRange;
	}
	if ( bExtendRange )
	{
		//DrawCostCurve(m_vSampleData.begin(), m_vSampleData.end());

		RecordSampleData(fStarPos, fEndPos, fStep, NOSTOP);	
		if ( m_vSampleData.empty() )
		{
			return FALSE;
		}
	}

	// Debug
	float fTempL = m_vSampleData.size();
	m_File.Write(&fTempL, sizeof(float));
	vector <StructSearchData>::iterator IterTemp;
	for ( IterTemp = m_vSampleData.begin(); IterTemp != m_vSampleData.end(); IterTemp++ )
	{
		m_File.Write(&((*IterTemp).fSamplePosition), sizeof(float));
		m_File.Write(&((*IterTemp).fSampleImageCost), sizeof(float));
	}

	// �Ӻ���ǰѰ������ɽ���λ��
	vector <StructCostPeakData> vPeakData;
	float fZeroSkewError = 0.01;
	MarkPeakInCostCurve(&vPeakData, &m_vSampleData, fZeroSkewError);

	//==============================================================//
	//2(&m_vSampleData, &vPeakData);  

	//if ( scantype == POSTSCAN )
	//{
	//	DrawCostCurve2(&m_vSampleData, &vPeakData);  
	//}
	//==============================================================//
	if ( vPeakData.empty() )
	{
		return FALSE;
	}
		
	// ����ÿһ����ֵ���������ͼ�񣬲����з���
	vector <StructBestImageData> vBestImageData;
	vBestImageData.reserve(vPeakData.size());
	vector <StructCostPeakData>::iterator PeakIter;
	for ( PeakIter=vPeakData.begin(); PeakIter!=vPeakData.end(); PeakIter++)
	{
		// ����ÿһ����ֵ���������ͼ��
		// ����ÿһ����ֵ���������ͼ��		
		StructBestImageData SBestImageData;
		SBestImageData.matImage.create(m_matBestImage.size(), CV_8UC3);
		SBestImageData.matDstGrayImg.create(m_matCurDstGrayImg.size(), CV_8UC1);
		SBestImageData.pfSubImgCost = new float[m_iSubImgRow*m_iSubImgCol];

		float fSearchRange = 10;  // um
		float fStarPos, fEndPos;
		if (	(	(*PeakIter).fZBestPosition < ( (*PeakIter).fZPosition + fSearchRange )	)
			&&	(	(*PeakIter).fZBestPosition > ( (*PeakIter).fZPosition - fSearchRange )	)	)
		{
			fStarPos = (*PeakIter).fZBestPosition - 2 * fStep + 1;
		}
		else
		{
			fStarPos = (*PeakIter).fZPosition - 2 * fStep + 1;
		}			
		fEndPos = fStarPos - fSearchRange;	
		
		int	  nTry = SearchBestImage(&SBestImageData, fStarPos, fEndPos, m_fMinStep);
		
		if ( nTry == 1 )	// ������ʼ��Χ
		{
			fEndPos = fStarPos - 2;
			fStarPos = (*PeakIter).fZPosition;
		}
		
		if ( nTry == -1 )	// ���ӽ�����Χ
		{
			fStarPos = fEndPos + 2;
			fEndPos = fStarPos - fSearchRange;	
		}

		// �ֱ�������ͼ���ÿһ����ͼ��������Ч��ͼ������
		cv::Mat	  matBinaryImg = cv::Mat::zeros(SBestImageData.matDstGrayImg.size(), CV_8UC1);
		int iSubImgCount = 0;
		float*	pfSubImgCost = SBestImageData.pfSubImgCost;
		int i, j;
		for ( j=0; j<m_iSubImgRow; j++ )
		{
			for ( i=0; i<m_iSubImgCol; i++ )
			{
				cv::Rect ROIRect(i*m_iSubImgWidth, j*m_iSubImgHeight, m_iSubImgWidth, m_iSubImgHeight);      //ROI����
				cv::Mat matDstGrayImg_ROI(SBestImageData.matDstGrayImg, ROIRect);
				//cvSetImageROI( SBestImageData.pcvDstGrayImg, cvRect(i*m_iSubImgWidth, j*m_iSubImgHeight, m_iSubImgWidth, m_iSubImgHeight) );		
				if ( (*pfSubImgCost++) > fCostThreshold )
				{
					// ͼ���ֵ��
					cv::Mat matBinaryImg_ROI(matBinaryImg, ROIRect);
					//cvSetImageROI( pcvBinaryImg , cvRect(i*m_iSubImgWidth, j*m_iSubImgHeight, m_iSubImgWidth, m_iSubImgHeight) );
					int thresholdValue = otsu (matDstGrayImg_ROI);				
					cv::threshold(matDstGrayImg_ROI, matBinaryImg_ROI, thresholdValue, 255, CV_THRESH_BINARY);
					//cvThreshold(SBestImageData.pcvDstGrayImg, pcvBinaryImg, thresholdValue, 255, CV_THRESH_BINARY);
				
					// ��������ͼ�Ƿ�����Ч��ǰ��
					if ( ! AnalyzeImpurity(matBinaryImg_ROI) )
					{
						iSubImgCount++;
					}
				}
			}
		}	
		(*PeakIter).iSubImgCount = iSubImgCount;
		SBestImageData.iSubImgCount = iSubImgCount;

		delete [] SBestImageData.pfSubImgCost;

		SBestImageData.pfSubImgCost = NULL;

		vBestImageData.push_back(SBestImageData);
	}

	// ������ѵ�ͼ��
	vector <StructBestImageData>::iterator BestImageIter, BestImageIterTmp;
	int iSubImgCountMax = -1;
	float fImageCost = -1;
	for ( BestImageIter = vBestImageData.begin(); BestImageIter != vBestImageData.end(); BestImageIter++ )
	{
		if ( (*BestImageIter).iSubImgCount > iSubImgCountMax )
		{
			iSubImgCountMax = (*BestImageIter).iSubImgCount;
			fImageCost = (*BestImageIter).fImageCost;
			BestImageIterTmp = BestImageIter;
			continue;
		}

		if ( (*BestImageIter).iSubImgCount == iSubImgCountMax )
		{
			if ( (*BestImageIter).fImageCost > fImageCost )
			{
				fImageCost = (*BestImageIter).fImageCost;
				BestImageIterTmp = BestImageIter;
			}
		}
	}

	(*BestImageIterTmp).matImage.copyTo(m_matBestImage);
	//cvCopy((*BestImageIterTmp).pcvImage, m_pcvBestImage);  // Output
	*pfBestFocusPos = (*BestImageIterTmp).fZPosition;      // Output	
	
	//for ( BestImageIter = vBestImageData.begin(); BestImageIter != vBestImageData.end(); BestImageIter++ )
	//{
	//	cvReleaseImage(&((*BestImageIter).pcvImage));
	//}

	if ( scantype == FIRSTSCAN )
	{		
		// �������ͼ��λ�ڽ�ƽ���ϵĸ���
		int iTotalSubImgCount = 0;
		for ( PeakIter=vPeakData.begin(); PeakIter!=vPeakData.end(); PeakIter++)
		{
			iTotalSubImgCount += (*PeakIter).iSubImgCount;
		}
		if ( iTotalSubImgCount > 0 )
		{
			for ( PeakIter=vPeakData.begin(); PeakIter!=vPeakData.end(); PeakIter++)
			{
				int iSubImgCount = (*PeakIter).iSubImgCount;
				if ( iSubImgCount > 0 )
				{
					(*PeakIter).fProbability = (float)(iSubImgCount * iSubImgCount)  / (float) ( (m_iSubImgRow * m_iSubImgCol) * iTotalSubImgCount );
					m_CurFOVCostPeakData.vCostPeakData.push_back((*PeakIter));
				}
			}
			m_CurFOVCostPeakData.vCostPeakData.shrink_to_fit();
			m_vFOVCostPeakData.push_back(m_CurFOVCostPeakData);  // ��¼�µ�ǰ���ھ۽�����Ұ������
		}		
	}
	
	return TRUE;
}

// ���ƴ�Χ������ƽ��ʱ���������޷�Χ����������� m_fStartSearchPosition, m_fEndSearchPosition ��
void  CAutoFocus::EstFocusRange(cv::Point2f cvFOVPosition)
{
	float fDelta = __max( fabs( m_structInitFOVFocusInfo.cvFOV3DPos.x - cvFOVPosition.x ), fabs( m_structInitFOVFocusInfo.cvFOV3DPos.y - cvFOVPosition.y ) );
	float fChange = m_LargeFocusingData.fFocusChangeRange;
	if ( fDelta > m_LargeFocusingData.fXYRange  )
	{
		fChange = m_LargeFocusingData.fFocusChangeRange * fDelta / m_LargeFocusingData.fXYRange;
	}
	m_fStartSearchPosition = __max(m_fMinPostion, m_structInitFOVFocusInfo.cvFOV3DPos.z - fChange);
	m_fEndSearchPosition = __min(m_fMaxPostion, m_structInitFOVFocusInfo.cvFOV3DPos.z + fChange);
	_ASSERT( m_fStartSearchPosition < m_fEndSearchPosition );
}

// ��Χ������ƽ�����ڵķ�Χ��λ�����䣩���ڽ�ƽ�����ڵļ��޷�Χ�ڣ����������ƽ�ƽ���λ�ã���������� m_fStartSearchPosition, m_fEndSearchPosition ��
BOOL  CAutoFocus::EstFocusPosition(cv::Point2f cvFOVPosition, float fStarPos, float fEndPos, float fStep )
{
	if ( fStarPos <= 0 )
	{
		fStarPos = m_fMinPostion;
	}
	if ( fEndPos <= 0 )
	{
		fEndPos = m_fMaxPostion;
	}
	if ( fStep <= 0 )
	{
		fStep = m_fMaxStep;
	}

	m_LargeFocusingData.cvFOVPosition = cvFOVPosition;      // ��¼��ǰ��Ұ��λ��
	BOOL  b = FALSE;
	m_LargeFocusingData.nIndex = 0;
	while ( 1 )
	{
		WhetherStopWorkThread(m_pDetectView);			// ֹͣ���鰴ť�����£�ɨ����鹤���̱߳���ֹ

		// XY���ƶ�����һ��������Ұ��
		if ( !Move2NextTestFOV() )   // �����������������Ұ����Ȼû��100%��ȷ����ƽ�����ڵ�����
		{
			b = FALSE;  
			break;
		}		
				
		AnalyzeFocusPosProb(fStarPos, fEndPos, fStep);

		if ( EstFocusPositionStop() )
		{
			b = TRUE;
			break;
		}
	}

	return b;
}


BOOL  CAutoFocus::InitAotoFocus( CBacilusDetectView* pDetectView) 
{ 
	m_pScanRoute = g_Mechanism.GetScanRoute(); 
	if ( m_pScanRoute == NULL )
	{
		return FALSE;
	}
		
	m_vFOVCostPeakData.reserve( m_pScanRoute->ScanSequence.GetSize() * m_LargeFocusingData.iMaxTest );

	m_pDetectView = pDetectView;

	m_vSampleData.clear();
	m_vFocusPlaneRange.clear();
	m_vFOVCostPeakData.clear();
	m_vFOVCostPeakDataInLargeRange.clear();

	m_fMinPostion = m_fWorkPositionMechanics[0];
	m_fMaxPostion = m_fWorkPositionMechanics[1]; // um
	m_fStartSearchPosition = m_fMinPostion;
	m_fEndSearchPosition = m_fMaxPostion;  

	//m_arrayFOVFocusInfo.SetSize( m_pScanRoute->ScanSequence.GetSize() );

	// Z���˶����ο����òο�ͼ��
	SetReferenceImage();

	m_structInitFOVFocusInfo.cvFOV3DPos.z = ( m_fMinPostion + m_fMaxPostion ) / 2;
	m_structInitFOVFocusInfo.fMinPostion = m_fMinPostion;
	m_structInitFOVFocusInfo.fMaxPostion = m_fMaxPostion;


	// Debug
	CBacilusDetectDoc* m_pDocument =(CBacilusDetectDoc*) pDetectView->GetDocument();	// Document��ַ
	CString strDirectory = m_pDocument->GetDirectory();  
	CString fileName = strDirectory + _T("\\FocusData.data");
	if ( !m_File.Open(fileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary) )
	{
		return FALSE;
	}
	
	return TRUE;
} 


float CAutoFocus::GetCurFOVBestImage(cv::Mat& matBestImage /*Output*/, CvPoint2D32f cvFOVPosition, int nCurFOVIndex)
{	
	CMotorController* pMotorController = CMotorController::GetInstance();  

	float fBestFocusPos;  // Output

	_ASSERT( nCurFOVIndex < m_pScanRoute->ScanSequence.GetSize() );

	m_CurFOVCostPeakData.cvFOVPosition = cvFOVPosition;
	m_nCurFOVIndex = nCurFOVIndex;	

	// Debue
	float fTempIndex = -m_nCurFOVIndex;
	m_File.Write(&fTempIndex, sizeof(float));

	// Ԥ�����Ұ��ƽ��λ�����ڵ����䷶Χ
	BOOL bFocusEst = FALSE;
	if ( GenerateEstFocusData(cvFOVPosition, m_vFOVCostPeakData) )
	{
		vector <StructFocusPlaneRange>::const_iterator MaxProbIter = max_element(m_vFocusPlaneRange.begin(), m_vFocusPlaneRange.end(), lesser_FocusPlaneRangeProbability);
		if ( (*MaxProbIter).fRangeProbability >= 1 )
		{
			m_fStartSearchPosition = (*MaxProbIter).fZPositionMean - m_LargeFocusingData.fFocusChangeRange;
			m_fEndSearchPosition = (*MaxProbIter).fZPositionMean + m_LargeFocusingData.fFocusChangeRange; 		
			bFocusEst = TRUE;
		}
	}	
	if ( !bFocusEst )      // ������Ϣ���ܹ����Ƴ�Ŀ����λ�ã������½��д�Χ��ʼ���۽�����
	{
		EstFocusRange(cvFOVPosition);  // ���ƴ�Χ������ƽ��ʱ���������޷�Χ����������� m_fStartSearchPosition, m_fEndSearchPosition ��
	
		bFocusEst = EstFocusPosition(cvFOVPosition, m_fStartSearchPosition, m_fEndSearchPosition);	// ��Χ������ƽ�����ڵķ�Χ��λ�����䣩���ڽ�ƽ�����ڵļ��޷�Χ�ڣ����������ƽ�ƽ���λ�ã���������� m_fStartSearchPosition, m_fEndSearchPosition ��

		// XYƽ̨�ص�ԭ����λ��
		pMotorController->XYMotorSmoothMove2Point(cvFOVPosition, DISTANCE_UNIT_UM);		
		pMotorController->WaitUntilDone();
	}

	BOOL  bBestImage = FALSE;
	if ( bFocusEst )   // �ڹ��Ƶľ۽��������������ͼ��
	{		
		float fStep = m_fMaxStep;
		bBestImage = GetBestImage(&fBestFocusPos, fStep);	        // �����������ɵ����ݱ����� m_CurFOVCostPeakData ֮�У�ͬʱ���䱣���� m_vFOVCostPeakData ��
		if ( !bBestImage )          
		{
			fStep /= 2;
			bBestImage = GetBestImage(&fBestFocusPos, fStep);       // ���û���ҵ�ɽ�壬����һ��Ĳ���������һ��
		}
	}
	
	// û�й��Ƴ���ǰ��Ұ�Ľ�ƽ�棬������һ���ķ�Χ��û�����������ͼ�񣬷�������Ұ��ֻ�Ǽ򵥵�����һ�����ͼ��
	if ( !bBestImage )
	{
		StructBestImageData SBestImageData;
		SBestImageData.matImage.create(m_matBestImage.size(), CV_8UC3);
		SBestImageData.matDstGrayImg.create(m_matCurDstGrayImg.size(), CV_8UC1);
		SBestImageData.pfSubImgCost = new float[m_iSubImgRow*m_iSubImgCol];
				
		// ������һ�ξ۽��������ĸ��������ͼ��
		float fStartPos, fEndPos;
		fStartPos = ( m_fStartSearchPosition + m_fEndSearchPosition ) /2 - 10;
		fEndPos = fStartPos + 20;
		SearchBestImage(&SBestImageData, fStartPos, fEndPos, m_fMinStep);

		SBestImageData.matImage.copyTo(m_matBestImage);		// Output
		fBestFocusPos = SBestImageData.fZPosition;        // Output	

		delete [] SBestImageData.pfSubImgCost;
	}

	//MATRIXINDEX curFOVMatrix = m_pScanRoute->ScanSequence[m_nCurFOVIndex];
	//m_arrayFOVFocusInfo[ m_pScanRoute->ColNum * curFOVMatrix.row + curFOVMatrix.col ].cvFOV3DPos = cvPoint3D32f( cvFOVPosition.x, cvFOVPosition.y, fBestFocusPos );
	//m_arrayFOVFocusInfo[ m_pScanRoute->ColNum * curFOVMatrix.row + curFOVMatrix.col ].nScanSequenceIndex = m_nCurFOVIndex;
	//m_arrayFOVFocusInfo[ m_pScanRoute->ColNum * curFOVMatrix.row + curFOVMatrix.col ].iValidSubImgCount = m_iValidSubImgCount;

	// Output
	matBestImage = m_matBestImage.clone(); 
	pMotorController = NULL;
	return fBestFocusPos;
}


//int  CAutoFocus::AnalyzeScanedFOVs()
//{
//	float fDeltaFocusPos = 10;   // 10 um
//
//	m_vPostProcessFOV.clear();
//
//	if ( m_pScanRoute->ColNum < 3 )
//	{
//		return 0;
//	}
//
//	MATRIXINDEX curFOVMatrix;
//	CvPoint3D32f cvCurFOV3DPos, cvLastFOV3DPos, cvNextFOV3DPos;
//	float fTemp;
//	int nIndex = m_pScanRoute->ScanSequence.GetSize()  - 1;
//	for ( ; nIndex >= 0; nIndex-- )
//	{
//		curFOVMatrix = m_pScanRoute->ScanSequence[nIndex];
//		cvCurFOV3DPos = m_arrayFOVFocusInfo[ m_pScanRoute->ColNum * curFOVMatrix.row + curFOVMatrix.col ].cvFOV3DPos;
//
//		BOOL bEdge = FALSE;
//		if ( curFOVMatrix.col == 0 )                                     // ��ǰ����ɨ�����˽ṹͼ�е��ұ߽���
//		{
//			if ( curFOVMatrix.row == 0 )									// ��ǰ����ɨ�����˽ṹͼ�е����½���
//			{
//				if ( m_pScanRoute->RowNum < 2 )
//				{
//					continue;
//				}
//				cvLastFOV3DPos = m_arrayFOVFocusInfo[ m_pScanRoute->ColNum * curFOVMatrix.row        + curFOVMatrix.col + 1 ].cvFOV3DPos;
//				cvNextFOV3DPos = m_arrayFOVFocusInfo[ m_pScanRoute->ColNum * (curFOVMatrix.row + 1)  + curFOVMatrix.col     ].cvFOV3DPos;
//			}
//			else if ( curFOVMatrix.row == (m_pScanRoute->RowNum - 1) )	   // ��ǰ����ɨ�����˽ṹͼ�е����Ͻ���
//			{
//				if ( m_pScanRoute->RowNum < 2 )
//				{
//					continue;
//				}
//				cvLastFOV3DPos = m_arrayFOVFocusInfo[ m_pScanRoute->ColNum * curFOVMatrix.row        + curFOVMatrix.col + 1 ].cvFOV3DPos;
//				cvNextFOV3DPos = m_arrayFOVFocusInfo[ m_pScanRoute->ColNum * (curFOVMatrix.row - 1)  + curFOVMatrix.col     ].cvFOV3DPos;
//			}
//			else
//			{
//				bEdge = TRUE;
//			}
//		}
//		else if ( curFOVMatrix.col == (m_pScanRoute->ColNum - 1) )     // ��ǰ����ɨ�����˽ṹͼ�е���߽���
//		{
//			if ( curFOVMatrix.row == 0 )									// ��ǰ����ɨ�����˽ṹͼ�е����½���
//			{
//				if ( m_pScanRoute->RowNum < 2 )
//				{
//					continue;
//				}
//				cvLastFOV3DPos = m_arrayFOVFocusInfo[ m_pScanRoute->ColNum * curFOVMatrix.row        + curFOVMatrix.col - 1 ].cvFOV3DPos;
//				cvNextFOV3DPos = m_arrayFOVFocusInfo[ m_pScanRoute->ColNum * (curFOVMatrix.row + 1)  + curFOVMatrix.col     ].cvFOV3DPos;
//			}
//			else if ( curFOVMatrix.row == (m_pScanRoute->RowNum - 1) )	   // ��ǰ����ɨ�����˽ṹͼ�е����Ͻ���
//			{
//				if ( m_pScanRoute->RowNum < 2 )
//				{
//					continue;
//				}
//				cvLastFOV3DPos = m_arrayFOVFocusInfo[ m_pScanRoute->ColNum * curFOVMatrix.row        + curFOVMatrix.col - 1 ].cvFOV3DPos;
//				cvNextFOV3DPos = m_arrayFOVFocusInfo[ m_pScanRoute->ColNum * (curFOVMatrix.row - 1)  + curFOVMatrix.col     ].cvFOV3DPos;
//			}
//			else
//			{
//				bEdge = TRUE;
//			}
//		}
//		else						 // ��ǰ����ɨ�����˽ṹͼ�е��ڲ�
//		{
//			cvLastFOV3DPos = m_arrayFOVFocusInfo[ m_pScanRoute->ColNum * curFOVMatrix.row + curFOVMatrix.col - 1 ].cvFOV3DPos;
//			cvNextFOV3DPos = m_arrayFOVFocusInfo[ m_pScanRoute->ColNum * curFOVMatrix.row + curFOVMatrix.col + 1 ].cvFOV3DPos;
//		}
//
//		if ( bEdge )
//		{
//			if ( m_pScanRoute->RowNum < 3 )
//			{
//				continue;
//			}
//			cvLastFOV3DPos = m_arrayFOVFocusInfo[ m_pScanRoute->ColNum * (curFOVMatrix.row - 1) + curFOVMatrix.col ].cvFOV3DPos;
//			cvNextFOV3DPos = m_arrayFOVFocusInfo[ m_pScanRoute->ColNum * (curFOVMatrix.row + 1) + curFOVMatrix.col ].cvFOV3DPos;
//		}
//
//		fTemp = __max ( fabs( cvCurFOV3DPos.x - cvLastFOV3DPos.x ),  fabs( cvCurFOV3DPos.y - cvLastFOV3DPos.y ) );
//		if ( fTemp > m_LargeFocusingData.fXYRange )
//		{
//			continue;
//		}
//		fTemp = __max ( fabs( cvCurFOV3DPos.x - cvNextFOV3DPos.x ),  fabs( cvCurFOV3DPos.y - cvNextFOV3DPos.y ) );
//		if ( fTemp > m_LargeFocusingData.fXYRange )
//		{
//			continue;
//		}
//
//		if ( ( cvCurFOV3DPos.z < cvLastFOV3DPos.z ) && ( cvCurFOV3DPos.z < cvNextFOV3DPos.z ) )
//		{
//			float fAverage = ( cvLastFOV3DPos.z + cvNextFOV3DPos.z ) / 2;
//			if ( ( fAverage - cvCurFOV3DPos.z ) > fDeltaFocusPos )
//			{
//				StructFOVFocusInfo structFOVFocusInfo;
//				structFOVFocusInfo.cvFOV3DPos = cvCurFOV3DPos;
//				structFOVFocusInfo.fNearFOV_Z_Average = fAverage;
//				structFOVFocusInfo.nScanSequenceIndex = nIndex;
//				m_vPostProcessFOV.push_back(structFOVFocusInfo);
//			}
//		}
//	}
//
//	return m_vPostProcessFOV.size();
//}
//
//
//int	 CAutoFocus::GetPostProcessFOVBestImage(IplImage** ppcvBestImage, CvPoint3D32f* pcvFOV3DPos, int nIndex)
//{
//	int   nScanSequenceIndex =  m_vPostProcessFOV.at(nIndex).nScanSequenceIndex; // Output
//	float fBestFocusPos;      // Output
//
//	CvPoint3D32f  cvCurFOV3DPos = m_vPostProcessFOV.at(nIndex).cvFOV3DPos;
//	CvPoint2D32f  cvFOVPosition;
//	cvFOVPosition.x = cvCurFOV3DPos.x;
//	cvFOVPosition.y = cvCurFOV3DPos.y;
//
//	m_fStartSearchPosition = cvCurFOV3DPos.z + m_fMaxStep;
//	m_fEndSearchPosition = m_vPostProcessFOV.at(nIndex).fNearFOV_Z_Average + m_LargeFocusingData.fFocusChangeRange;
//
//	pCamera->EnableNotifyMsg(FALSE);
//
//	// XYƽ̨�ص�ԭ����λ��
//	pMotorController->XYMotorSmoothMove2Point(cvFOVPosition, DISTANCE_UNIT_UM);
//	// ȷ��XY���Ѿ�ֹͣ�˶�
//	while (  (pMotorController->GetXMotorStatus() == PCI_MOTOR_RUNNING) 
//		  || (pMotorController->GetYMotorStatus() == PCI_MOTOR_RUNNING)  )
//	{
//		Sleep(10);
//	}
//	
//	pCamera->EnableNotifyMsg(TRUE);
//	
//	// �ڹ��Ƶľ۽�����������һ�����ͼ��
//	BOOL  bBestImage = FALSE;
//	float fStep = m_fMaxStep / 2;
//	bBestImage = GetBestImage(&fBestFocusPos, fStep, POSTSCAN);	     
//	
//	// û���Խϴ�Ĳ���û����������ƽ�棬���������Ұ��ֻ�Ǽ򵥵�����һ�����ͼ��
//	if ( !bBestImage )          
//	{
//		StructBestImageData SBestImageData;
//		SBestImageData.pcvImage = cvCreateImage(cvGetSize(m_pcvBestImage), IPL_DEPTH_8U, 3);
//		SBestImageData.pcvDstGrayImg = cvCreateImage(cvGetSize(m_pcvCurDstGrayImg), IPL_DEPTH_8U, 1);
//		SBestImageData.pfSubImgCost = new float[m_iSubImgRow*m_iSubImgCol];
//				
//		// ������һ�ξ۽��������ĸ��������ͼ��
//		float fStartPos, fEndPos;
//		fStartPos = ( m_fStartSearchPosition + m_fEndSearchPosition ) /2 - 10;
//		fEndPos = fStartPos + 20;
//		SearchBestImage(&SBestImageData, fStartPos, fEndPos, m_fMinStep);
//
//		cvCopy(SBestImageData.pcvImage, m_pcvBestImage);  // Output
//		fBestFocusPos = SBestImageData.fZPosition;        // Output	
//
//		cvReleaseImage(&SBestImageData.pcvImage);
//		cvReleaseImage(&SBestImageData.pcvDstGrayImg );
//		delete [] SBestImageData.pfSubImgCost;
//	}
//	
//	// Output
//	*ppcvBestImage = m_pcvBestImage;  
//	pcvFOV3DPos->x = cvFOVPosition.x;
//	pcvFOV3DPos->y = cvFOVPosition.y;
//	pcvFOV3DPos->z = fBestFocusPos;
//	return nScanSequenceIndex;
//}


BOOL   CAutoFocus::EstInitFocusPosition()
{
	CCamera* pCamera = CCamera::GetInstance();
	CMotorController* pMotorController = CMotorController::GetInstance();  

	// ���Զ����⴦��Z���˶����ο�λ�ã���òο�����ֵ
	pMotorController->ZMotorSmoothMove2AbsolutePos(m_structInitFOVFocusInfo.fZRefPosition, DISTANCE_UNIT_UM );  
	::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);		// �µ�ͼ�����ݵ���
	cv::Mat matImage = pCamera->GetImage();                         // ��������㵽��ͼ��
	m_structInitFOVFocusInfo.fRefLight = CalImageAverageGray(matImage);  // ����ͼ������

	// ��������ʼ��			
	cv::Point2f startpos = g_Mechanism.GenerateDestPointInXY( 0 );
	startpos.x -= m_structInitFOVFocusInfo.fXYRange/2;
	startpos.y -= m_structInitFOVFocusInfo.fXYRange/2;    
	pMotorController->XYMotorSmoothMove2Point(startpos, DISTANCE_UNIT_UM);   
	pMotorController->WaitUntilDone();

	// ��ʼ��Χ������ʮ��
	float fSpeed = 4000; // um/s
	cv::Point2f curtpos, destpos, bestpos;
	float fY;
	int iSign = 1, iScanSign;     // ɨ���еķ���
	float fMaxLight = -1, fMinLight = 256;
	BOOL  bStop = FALSE;
	for ( fY = 0; fY < m_structInitFOVFocusInfo.fXYRange; fY += m_structInitFOVFocusInfo.fDeltaY )
	{
		WhetherStopWorkThread(m_pDetectView);			// ֹͣ���鰴ť�����£�ɨ����鹤���̱߳���ֹ

		// XYƽ̨�˶���ɨ���е���ʼ��
		pMotorController->GetXYMotorPosition(curtpos, DISTANCE_UNIT_UM);
		destpos = curtpos;
		destpos.y = startpos.y + fY;
		pMotorController->XYMotorSmoothMove2Point(destpos, DISTANCE_UNIT_UM);   
		pMotorController->WaitUntilDone();
		curtpos = destpos;
		
		// ��ʼһɨ���е���������
		pMotorController->SetXMotorSpeed(fSpeed,DISTANCE_UNIT_UM);  
		destpos.x = curtpos.x + iSign * m_structInitFOVFocusInfo.fXYRange;
		destpos.y = curtpos.y;
		pMotorController->XYMove2(destpos, DISTANCE_UNIT_UM);
		MOTOR_STATUS motor_status;
		pMotorController->GetCurrentStatus(motor_status);
		while (  motor_status == MOTOR_RUN 	)
		{
			::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);		// �µ�ͼ�����ݵ���
						
			// ��õ�ǰFOV��XY����
			cv::Point2f curFOVPosition ;
			pMotorController->GetXYMotorPosition(curFOVPosition, DISTANCE_UNIT_UM);
	
			// ����ͼ������
			float fLight = CalImageAverageGray(matImage);   
			
			if ( fMaxLight < fLight )
			{
				fMaxLight = fLight;
			}
			if ( fMinLight > fLight )
			{
				fMinLight = fLight;
				bestpos = curFOVPosition;
				iScanSign = iSign;
			}

			// �ж��Ƿ��Ѿ�����ֹͣ��������
			if (   ( (fMaxLight - fMinLight) > m_structInitFOVFocusInfo.fStopThreshold )
				|| ( (m_structInitFOVFocusInfo.fRefLight - fMinLight) > m_structInitFOVFocusInfo.fStopThreshold ) )
			{
				if ( fLight > fMinLight )      // �Ѿ�Խ����ѵ����򣬿���ֹͣ����������
				{
					pMotorController->Stop();
					
					bStop = SearchRedCrossPosition(bestpos,iScanSign);  // ����������ʮ��
				}
			}

			pMotorController->GetCurrentStatus(motor_status);
		}
				
		if ( bStop )
		{
			break;			
		}

		// ֮����ɨ������������һ������еķ����෴����������
		iSign = -iSign;
	}

	if ( !bStop )
	{
		// û���ҵ���ʮ�֣�������Сֵ������Ҫ��ʧ��
		return FALSE;
	}
	
	// ��¼�º�ʮ�ֽ�ƽ���λ���Լ����Ų�Ƭ��ƽ��ı仯��Χ
	m_structInitFOVFocusInfo.fMinPostion = __max(m_fZRefPosition, m_structInitFOVFocusInfo.cvFOV3DPos.z - m_structInitFOVFocusInfo.fFocusChangeRange);
	m_structInitFOVFocusInfo.fMaxPostion = m_structInitFOVFocusInfo.cvFOV3DPos.z + m_structInitFOVFocusInfo.fFocusChangeRange;

	m_fMinPostion = m_structInitFOVFocusInfo.fMinPostion;
	m_fMaxPostion = m_structInitFOVFocusInfo.fMaxPostion;
	m_fStartSearchPosition = m_fMinPostion;
	m_fEndSearchPosition = m_fMaxPostion;  

	StructFOVCostPeakData	SCurFOVCostPeakData; // ��ǰ���ھ۽�����Ұ������
	SCurFOVCostPeakData.cvFOVPosition.x = m_structInitFOVFocusInfo.cvFOV3DPos.x;      // ��¼��ǰ��Ұ��λ��
	SCurFOVCostPeakData.cvFOVPosition.y = m_structInitFOVFocusInfo.cvFOV3DPos.y; 
	StructCostPeakData SCurCostPeakData;
	SCurCostPeakData.fImageCost = 100;
	SCurCostPeakData.fProbability = 10;
	SCurCostPeakData.fZPosition = m_structInitFOVFocusInfo.cvFOV3DPos.z;
	SCurCostPeakData.iSubImgCount = 20;
	SCurFOVCostPeakData.vCostPeakData.push_back(SCurCostPeakData);
	SCurFOVCostPeakData.vCostPeakData.shrink_to_fit();

	m_vFOVCostPeakDataInLargeRange.push_back(SCurFOVCostPeakData);	// ��¼�´�Χ�۽�������
	m_vFOVCostPeakData.push_back(SCurFOVCostPeakData);				// ��¼�µ�ǰ���ھ۽�����Ұ������

	pCamera = NULL;
	pMotorController = NULL;

	return TRUE;
}

BOOL  CAutoFocus::SearchRedCrossPosition(cv::Point2f startPos, int iScanSign)
{
	CCamera* pCamera = CCamera::GetInstance();
	CMotorController* pMotorController = CMotorController::GetInstance();  

	cv::Size  fCameraFOVSize = pCamera->GetCameraFOVSize();
	
	cv::Mat   matImage = pCamera->GetImage();                       // ��������㵽��ͼ��
	
	// ��ϸ������ʮ�����λ��
	int i = 0;
	int iSearchFOVNum = 24;
	CvPoint2D32f destpos, minLightPos, bestPos;
	destpos.y = startPos.y;
	float fMinLight = 256;
	for ( i = 0; i < iSearchFOVNum; i++ )
	{
		destpos.x = startPos.x - iScanSign * i * fCameraFOVSize.width * 0.25;	
		pMotorController->XYMotorSmoothMove2Point(destpos, DISTANCE_UNIT_UM);   
		pMotorController->WaitUntilDone();
		
		::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);		// �µ�ͼ�����ݵ���

		 matImage = pCamera->GetImage();                       // ��������㵽��ͼ��

		// ��õ�ǰFOV��XY����
		cv::Point2f curFOVPosition;
		pMotorController->GetXYMotorPosition(curFOVPosition, DISTANCE_UNIT_UM);

		// ����ͼ������
		float fLight = CalImageAverageGray(matImage);  

		if ( fLight < fMinLight )  
		{
			fMinLight = fLight;				
			minLightPos = curFOVPosition;
		}		
	}
	
	// ȡ����ֵ�Ƚ�С����Ұ��Ϊ��ʮ�ֵ�λ�ã��������̫������۽�ʱ��������Ұ�ľ۽�λ�ò��̫�󣬴�ʱѡȡ����λ�ã�
	if ( fMinLight < m_structInitFOVFocusInfo.fLightThreshold )
	{
		BOOL bStop = FALSE;
		
		destpos = minLightPos;
		iSearchFOVNum = 12;
		for ( i = 1; i < iSearchFOVNum; i++ )
		{
			destpos.x = minLightPos.x + i * fCameraFOVSize.width * 0.25;	
			pMotorController->XYMotorSmoothMove2Point(destpos, DISTANCE_UNIT_UM);   
			pMotorController->WaitUntilDone();

			::WaitForSingleObject(pCamera->CapturedEvent(),INFINITE);		// �µ�ͼ�����ݵ���

			 matImage = pCamera->GetImage();                       // ��������㵽��ͼ��

			// ��õ�ǰFOV��XY����
			cv::Point2f curFOVPosition;
			pMotorController->GetXYMotorPosition(curFOVPosition, DISTANCE_UNIT_UM);

			// ����ͼ������
			float fLight = CalImageAverageGray(matImage);  

			if ( ( (m_structInitFOVFocusInfo.fRefLight - fLight) > m_structInitFOVFocusInfo.fStopThreshold )
				&& ( fLight > m_structInitFOVFocusInfo.fLightThreshold )  )
			{
				bestPos = curFOVPosition;
				bStop = TRUE;
				break;
			}		
		}
		
		if ( !bStop )
		{
			return FALSE;
		}
	}
	else
	{
		bestPos = minLightPos;
	}

	m_structInitFOVFocusInfo.cvFOV3DPos.x = bestPos.x;
	m_structInitFOVFocusInfo.cvFOV3DPos.y = bestPos.y;
	
	// XYƽ̨�˶�����������ʮ�ֵ�λ�ô�
	pMotorController->XYMotorSmoothMove2Point(bestPos, DISTANCE_UNIT_UM);   
	pMotorController->WaitUntilDone();
	
	// �ڼ��޷�Χ�ڣ�Z����ϵ���������ʮ�ֵĽ�ƽ�棬������¼�µ����ݱ����� m_vSampleData ��
	RecordSampleData(m_fMinPostion, m_fMaxPostion, m_fMaxStep, NOSTOP);	
	if ( m_vSampleData.empty() )
	{
		return FALSE;
	}
	float fZeroSkewError = 0.01; // ����������0б���������
	vector <StructCostPeakData> vPeakData;
	MarkPeakInCostCurve(&vPeakData, &m_vSampleData, fZeroSkewError);
	//==============================================================//
	//DrawCostCurve2(&m_vSampleData, &vPeakData);  
	//==============================================================//
	if ( vPeakData.empty() )
	{
		return FALSE;
	}

	// ��¼�º�ʮ�ֽ�ƽ���λ��
	float fMaxImageCost = -1;
	vector <StructCostPeakData>::iterator vIter;
	for ( vIter = vPeakData.begin(); vIter != vPeakData.end(); vIter++ )
	{
		if ( fMaxImageCost < (*vIter).fImageCost )
		{
			fMaxImageCost = (*vIter).fImageCost;
			m_structInitFOVFocusInfo.cvFOV3DPos.z = (*vIter).fZPosition - m_fMaxStep;
		}
	}
	if ( fMaxImageCost < m_structInitFOVFocusInfo.fCostThreshold )
	{
		return FALSE;
	}

	pCamera = NULL;
	pMotorController = NULL;

	return TRUE;
}


void  CAutoFocus::DrawCostCurve(vector <StructSearchData>::const_iterator iFirst, vector <StructSearchData>::const_iterator iLast)
{
	const char* name = "CostCurve";
	cv::namedWindow(name, CV_WINDOW_AUTOSIZE );
	cv::Mat	matImg;
	matImg.create(cv::Size(840, 600), CV_8UC3);
	vector <StructSearchData>::const_iterator Iter = max_element(iFirst, iLast, lesser_SearchDataImageCost);
	StructCoordinateFunction scf = DrawCoordinate(matImg, (*iFirst).fSamplePosition, (*(iLast-1)).fSamplePosition, 0, (*Iter).fSampleImageCost);
	for ( Iter=iFirst; Iter!=iLast; Iter++ )
	{
		PlotPointInCoordinate(matImg, scf, (*Iter).fSamplePosition, (*Iter).fSampleImageCost, CV_RGB(255,0,0), 1);
	}	
	cv::imshow(name, matImg);
	cv::waitKey(0);
	cv::destroyWindow(name);
}

void  CAutoFocus::DrawCostCurve2(vector <StructSearchData> * pvSearchData, vector <StructCostPeakData> * pvPeakData)
{
	if ( pvSearchData->empty() )
	{
		return;
	}
	vector <StructSearchData>::const_iterator iFirst = pvSearchData->begin();
	vector <StructSearchData>::const_iterator iLast = pvSearchData->end();

	const char* name = "Cost&Peak";
	cv::namedWindow(name, CV_WINDOW_AUTOSIZE );
	cv::Mat  matImg;
	matImg.create(cv::Size(840, 700), CV_8UC3);

	vector <StructSearchData>::const_iterator Iter = max_element(iFirst, iLast, lesser_SearchDataImageCost);
	StructCoordinateFunction scf = DrawCoordinate(matImg, (*iFirst).fSamplePosition, (*(iLast-1)).fSamplePosition, /*-1*/0, /*__max( (*Iter).fSampleImageCost, 1 )*/ (*Iter).fSampleImageCost);

	for ( Iter=iFirst; Iter!=iLast; Iter++ )
	{
		PlotPointInCoordinate( matImg, scf, (*Iter).fSamplePosition, (*Iter).fSampleImageCost, CV_RGB(255,0,0), 1);
		//PlotPointInCoordinate( pcvImg, scf, (*Iter).fSamplePosition, (*Iter).fCostSkew, CV_RGB(0,255,0), 1);
	}	

	if ( !pvPeakData->empty() )
	{
		vector <StructCostPeakData>::const_iterator PeakIter;
		for ( PeakIter = pvPeakData->begin(); PeakIter  != pvPeakData->end(); PeakIter++ )
		{
			PlotPointInCoordinate( matImg, scf, (*PeakIter).fZPosition, (*PeakIter).fImageCost, CV_RGB(0,0,255), 1);
		}
	}

	cv::imshow(name, matImg);
	cv::waitKey(0);
	cv::destroyWindow(name);
}

void  CAutoFocus::DrawPeakProbCurve(vector <StructCostPeakData>::const_iterator iFirst, vector <StructCostPeakData>::const_iterator iLast)
{
	const char* name = "Peak Prob";
	cv::namedWindow(name, CV_WINDOW_AUTOSIZE );
	cv::Mat  matImg;
	matImg.create(cv::Size(840, 600), CV_8UC3);
	StructCoordinateFunction scf = DrawCoordinate(matImg, (*iFirst).fZPosition, (*(iLast-1)).fZPosition, 0, 1);
	vector <StructCostPeakData>::const_iterator Iter;
	for ( Iter=iFirst; Iter!=iLast; Iter++ )
	{
		PlotPointInCoordinate( matImg, scf, (*Iter).fZPosition, (*Iter).fProbability, CV_RGB(255,0,0), 1);
	}	

	cv::imshow(name, matImg);
	cv::waitKey(0);
	cv::destroyWindow(name);
}

void  CAutoFocus::DrawFocusPlaneRangeCurve()
{
	const char* name = "FocusPlaneRangeProb";
	cv::namedWindow(name, CV_WINDOW_AUTOSIZE );
	cv::Mat  matImg;
	matImg.create(cv::Size(840, 600), CV_8UC3);
	StructCoordinateFunction scf;
	if ( !m_vFocusPlaneRange.empty() )
	{
		vector <StructFocusPlaneRange>::const_iterator MaxProbIter = max_element(m_vFocusPlaneRange.begin(), m_vFocusPlaneRange.end(), lesser_FocusPlaneRangeProbability);
	    scf = DrawCoordinate(matImg, (*m_vFocusPlaneRange.begin()).fZPosition, (*(m_vFocusPlaneRange.end()-1)).fZPosition, 0, (*MaxProbIter).fRangeProbability);
		vector <StructFocusPlaneRange>::const_iterator Iter;
		for ( Iter=m_vFocusPlaneRange.begin(); Iter!=m_vFocusPlaneRange.end(); Iter++ )
		{
			PlotPointInCoordinate( matImg, scf, (*Iter).fZPosition, (*Iter).fRangeProbability, CV_RGB(255,0,0), 1);
		}
	}
	
	cv::imshow(name, matImg);
	cv::waitKey(0);
	cv::destroyWindow(name);
}

