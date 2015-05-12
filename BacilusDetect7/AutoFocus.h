#pragma once

// revised by bqsong on 2015.5.3
/* 
	��Opencv1.0�Ľṹ�޸�ΪOpencv2.4.11�Ľṹ
*/

#include "ImageProcess.h"
#include "BacilusDetectView.h"
#include "opencv2\opencv.hpp"

class CBacilusDetectView;

class CAutoFocus
{
public:
	CAutoFocus(void);
	~CAutoFocus(void);

typedef struct tagSearchData
{
	float fSamplePosition;
	float fSampleImageCost;
	float fCostSkew;        // б�ʵķ���
} StructSearchData;

typedef struct tagCostPeakData
{
	float		 fZPosition;        // �󲽳�ʱ����������ɽ���λ��
	float		 fImageCost;
	int			 iSubImgCount;
	float		 fProbability;
	float		 fZBestPosition;   // �󲽳�ʱ���������ɽ���λ��
} StructCostPeakData;

typedef struct tagBestImageData
{
	cv::Mat		matImage;
	cv::Mat     matDstGrayImg;
	float		fZPosition;         // ���ͼ���λ��
	float		fImageCost;
	float*		pfSubImgCost;
	int			iSubImgCount;
} StructBestImageData;

typedef struct tagFocusPlaneRange
{
	float		 fZPosition;
	int			 iPeakCount;
	float		 fRangeProbability;
	float		 fZPositionMean;
} StructFocusPlaneRange;

typedef struct tagFOVCostPeakData
{
	cv::Point2f  				cvFOVPosition;   // ����λ��
	vector <StructCostPeakData>	vCostPeakData;
} StructFOVCostPeakData;

typedef struct tagLargeFocusingData
{
	cv::Point2f 	 cvFOVPosition;        // ��Ҫ���д�Χ�۽���Ұ�ľ���λ��
	float			 fXYRange;			   // ������Ұ����Ҫȷʵ��ƽ����Ұ�ľ��뷶Χ
	float			 fFocusChangeRange;    // ��ƽ����1ƽ��mm�ֲ������ڱ仯��Χ
	float			 fXYChangeSpace;       // ÿ�β��Բ�ͬ��Ұ֮��ľ��룬um
	float			 fDiffPosition;        // ��һ��������Ұ�뵱ǰ��Ұ֮���λ�ò�
	int				 nIndex;			   // ���Բ�ͬ��Ұ�����
	int				 iMaxTest;			   // ������Ұ���������
} StructLargeFocusingData;

//typedef struct tagFOVFocusInfo
//{
//	CvPoint3D32f	cvFOV3DPos;           // ����λ��
//	float			fNearFOV_Z_Average;	  // ����������Ұ��ƽ���ƽ��ֵ
//	int				nScanSequenceIndex;
//	int				iValidSubImgCount;
//} StructFOVFocusInfo;

typedef struct tagInitFOVFocusInfo
{
	cv::Point3f 	cvFOV3DPos;           // ��ʮ�ִ���ʼ��ƽ��ľ���λ��
	float			fZRefPosition;        // Z��ο�λ��
	float			fRefLight;			  // �Զ�����֮�󣬲ο�λ�õĲο�����ֵ
	float			fXYRange;             // ������Ƭ�Ϻ�ʮ�ֵķ�Χ
	float			fDeltaY;              // ����ʱ��Y����ı仯����	
	float			fStopThreshold;       // ����ͼ������ֵ������Ƭ�Ϻ�ʮ�ֵ�λ�ã�ֹͣ������ʮ�ֵ�����
	float			fLightThreshold;      // ͼ�����Ȳ���̫��
	float			fCostThreshold;		  // ͼ�������ȱ�����ڸ�ֵ�����϶�����Ч�ĺ�ʮ�ֽ�ƽ��
	float			fFocusChangeRange;    // ���Ų�Ƭ��ƽ��ı仯��Χ
	float           fMinPostion;          // ���Ų�Ƭ��ƽ��仯���ϼ���
	float			fMaxPostion;          // ���Ų�Ƭ��ƽ��仯���¼���
} StructInitFOVFocusInfo;

enum SEARCH_STOP { NOSTOP, SEARCH_PEAK };

enum SCAN_TYPE { FIRSTSCAN, POSTSCAN };

private:
	float			m_fWorkPositionMechanics[2];

private:
	const float			 m_fFPS;             // �����֡Ƶ��
	const float			 m_fZRefPosition;	// unit: um;  ��òο�ͼ���Z��λ��
	const float			 m_fMaxStep;    // ��Χ������ƽ��Ĳ���
	const float			 m_fMinStep;    // ��ϸ�������ͼ��Ĳ���	
	
	float			 m_fMinPostion;			  // ��ƽ�����ڵļ��޷�Χ
	float			 m_fMaxPostion;
	float			 m_fStartSearchPosition;  // ÿ��������ƽ�����ʼ��
	float			 m_fEndSearchPosition;    // ÿ��������ƽ�����ֹ��	

	int				 m_iSubImgWidth;        // ��ͼ�Ŀ��
	int				 m_iSubImgHeight;		// ��ͼ�ĸ߶�
	int				 m_iSubImgRow;          // ��ͼ������
	int				 m_iSubImgCol;			// ��ͼ������

	StructLargeFocusingData			m_LargeFocusingData;  // ��ΧѰ�ҽ�ƽ������е�����

private:
	cv::Mat			 m_matBestImage;		  // �۽���ͼ��Output
	cv::Mat			 m_matRefGrayImg;     // �ο�ͼ��ԭʼ�Ҷ�ͼ����Сһ��
	cv::Mat			 m_matCurDstGrayImg;  // ��ǰԭʼͼ��ת���Ĵ������ĻҶ�ͼ��	

	vector <StructSearchData>		m_vSampleData;        // ��ָ����Χ������ʱ��¼�µ�����
	vector <StructFocusPlaneRange>  m_vFocusPlaneRange;   // ��ƽ���ڲ�ͬ�����ϵĸ���
	vector <StructFOVCostPeakData>  m_vFOVCostPeakData;   // ɨ�������Ѱ�ҽ�ƽ�������
	StructFOVCostPeakData			m_CurFOVCostPeakData; // ��ǰ���ھ۽�����Ұ������
	int								m_nCurFOVIndex;       // ��ǰ��Ұ��ɨ�����˽ṹ�е�����

	vector <StructFOVCostPeakData>  m_vFOVCostPeakDataInLargeRange;   // ɨ������д�ΧѰ�ҽ�ƽ�������

	//CArray<StructFOVFocusInfo, StructFOVFocusInfo>	m_arrayFOVFocusInfo;      // ɨ�����˽ṹ��ɨ��۽�����������
	//vector <StructFOVFocusInfo>     m_vPostProcessFOV;

private:
	StructInitFOVFocusInfo  m_structInitFOVFocusInfo;   // ������Ƭ�Ϻ�ʮ�ֵ�λ�ã������г�ʼ�۽����������Ų�Ƭ��ƽ��ķ�Χ

private:
	CBacilusDetectView*		m_pDetectView;
	SCANROUTE*				m_pScanRoute;        // ɨ�����˽ṹ
	CFile					m_File;
	

private:
	double GetHalfImageLightness(cv::Mat matHalfGrayImg);  // �����ͼ������������ֵ
	void  SetReferenceImage();		 // Z���˶����ο����òο�ͼ�񣬱����� m_matRefGrayImg ��
	void  GenerateCurDstGrayImg();   // ���ɼ����ĵ�ǰԭʼͼ��ת��Ϊ�������ĻҶ�ͼ�񣬱����� m_matCurDstGrayImg ��
	void  RecordSampleData(float fStartPos, float fEndPos, float fStep, SEARCH_STOP Enum_stop);        // ��ָ���Ĳ�����������ָ����Χ�ڽ�����������¼�µ����ݱ����� m_vSampleData ��
	void  MarkPeakInCostCurve(vector <StructCostPeakData> * pvPeakData, vector <StructSearchData> * pvSampleData, float fZeroSkewError); // �������������У��Ӻ���ǰѰ������ɽ���λ�ã���¼��pvPeakData��
	BOOL  AnalyzeImpurity(cv::Mat matBinaryImage); // ������ֵͼ���ǰ�������Ƿ�������
	void  AnalyzeFocusPosProb(float fStartPos, float fEndPos, float fStep);     // �ڸ����ķ�Χ�ڣ����ϵ������������ƽ�ƽ���λ�ü�����ʣ������ݱ����� m_vFOVCostPeakData ��
	BOOL  GenerateEstFocusData(cv::Point2f cvFOVPosition, vector <StructFOVCostPeakData>& vFOVCostPeakData);         // �� vFOVCostPeakData �е����ݽ��з��������۽���Ұ���������ݰ���Z��λ���������򣬲�����m_vFocusPlaneRange����
	BOOL  EstFocusPositionStop();    // ��ΧѰ�ҽ�ƽ������Ƿ���ֹ
	BOOL  CheckTestFOVValid(cv::Point2f cvFOVPosition); 	// �����Ҫ���Ե���Ұλ���Ƿ����Ѳ��Ե���Ұλ�ÿ���̫��
	BOOL  Move2NextTestFOV();        // XY���ƶ�����һ��������Ұ��

	void  EstFocusRange(cv::Point2f cvFOVPosition); // ���ƴ�Χ������ƽ��ʱ���������޷�Χ����������� m_fStartSearchPosition, m_fEndSearchPosition ��
	BOOL  EstFocusPosition(cv::Point2f cvFOVPosition, float fStartPos = 0, float fEndPos = 0, float fStep = 0 ); // ��Χ������ƽ�����ڵķ�Χ��λ�����䣩���ڽ�ƽ�����ڵļ��޷�Χ�ڣ����������ƽ�ƽ���λ��	
	
	int   SearchBestImage(StructBestImageData* pSBestImageData, float fStartPos, float fEndPos, float fStep);    // �ڸ����ľֲ���Χ�ڣ���fStepΪ����������������ͼ�񣬲�����������Ӧ��λ�� 
	BOOL  SearchBestImageStop(vector <StructSearchData> * pvSampleData);							             // �ж��������ͼ��Ĺ����Ƿ���ֹ	
	BOOL  SearchPeakStop(vector <StructSearchData> * pvSampleData); // ��̬�۽������У��ж������Ϸ�Ĺ����Ƿ���ֹ

	BOOL  GetBestImage(float* pfBestFocusPos, float fStep, SCAN_TYPE scantype = FIRSTSCAN);   // ��õ�ǰ��Ұ��ѵ�ͼ���Լ���ƽ���λ�ã���ѵ�ͼ�񱣴��� m_matBestImage �У������������ɵ����ݱ����� m_CurFOVCostPeakData ֮�У�ͬʱ���䱣���� m_vFOVCostPeakData ��
	BOOL  WhetherExtendRangeDown(float fExtendCostThreshold, float fZeroSkewError = 0);   // �ж��Ƿ���Ҫ����������Χ
	BOOL  WhetherExtendRangeUp(float fExtendCostThreshold, float fZeroSkewError = 0);   // �ж��Ƿ���Ҫ����������Χ

	float ComputeBestPeak(vector <StructSearchData>& vSampleData, vector <StructSearchData>::iterator iPeak); // ͨ������������ϵķ���������������Ⱦֲ���ֵ�����λ��

	BOOL  SearchRedCrossPosition(cv::Point2f startPos, int iScanSign);

	void  DrawCostCurve(vector <StructSearchData>::const_iterator iFirst, vector <StructSearchData>::const_iterator iLast);  // For Debuging
	void  DrawCostCurve2(vector <StructSearchData> * pvSearchData, vector <StructCostPeakData> * pvPeakData);  // For Debuging
	void  DrawPeakProbCurve(vector <StructCostPeakData>::const_iterator iFirst, vector <StructCostPeakData>::const_iterator iLast);  // For Debuging
	void  DrawFocusPlaneRangeCurve();  // For Debuging
	

public:
	BOOL  InitAotoFocus( CBacilusDetectView* pDetectView);		// ��ʼ���Զ��۽���
	float GetCurFOVBestImage(cv::Mat * pmatBestImage, CvPoint2D32f cvFOVPosition, int nCurFOVIndex);	        // ��õ�ǰ��Ұ��ѵ�ͼ���Լ���ƽ���λ��
	//int	  AnalyzeScanedFOVs();                                                                                  // ����ɨ�����Ұ�۽�������з���������ɨ�����˽ṹ�ж���Щ��Ұ��Ҫ���¾۽�
	//int	  GetPostProcessFOVBestImage(IplImage** ppcvBestImage, CvPoint3D32f* pcvFOV3DPos, int nIndex);          // ��ú�����Ұ����ѵ�ͼ����Ұλ�á���ƽ��λ���Լ���Ұɨ�����
	BOOL  EstInitFocusPosition();                                                                               // ���ò�Ƭ�ϵĺ�ʮ�ֹ���Ŀ��㽹ƽ��ĳ�ʼλ��
};


bool  lesser_SearchDataImageCost( CAutoFocus::StructSearchData elem1, CAutoFocus::StructSearchData elem2 );
bool  lesser_CostPeakDataZPosition( CAutoFocus::StructCostPeakData elem1, CAutoFocus::StructCostPeakData elem2 );
bool  lesser_FocusPlaneRangeProbability( CAutoFocus::StructFocusPlaneRange elem1, CAutoFocus::StructFocusPlaneRange elem2 );
bool  lesser_FOVPosition( CAutoFocus::StructFOVCostPeakData elem1, CAutoFocus::StructFOVCostPeakData elem2 );
