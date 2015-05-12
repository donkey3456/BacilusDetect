#pragma once

// revised by bqsong on 2015.5.3
/* 
	将Opencv1.0的结构修改为Opencv2.4.11的结构
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
	float fCostSkew;        // 斜率的符号
} StructSearchData;

typedef struct tagCostPeakData
{
	float		 fZPosition;        // 大步长时，搜索到的山峰的位置
	float		 fImageCost;
	int			 iSubImgCount;
	float		 fProbability;
	float		 fZBestPosition;   // 大步长时，计算出的山峰的位置
} StructCostPeakData;

typedef struct tagBestImageData
{
	cv::Mat		matImage;
	cv::Mat     matDstGrayImg;
	float		fZPosition;         // 最佳图像的位置
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
	cv::Point2f  				cvFOVPosition;   // 绝对位置
	vector <StructCostPeakData>	vCostPeakData;
} StructFOVCostPeakData;

typedef struct tagLargeFocusingData
{
	cv::Point2f 	 cvFOVPosition;        // 需要进行大范围聚焦视野的绝对位置
	float			 fXYRange;			   // 测试视野离需要确实焦平面视野的距离范围
	float			 fFocusChangeRange;    // 焦平面在1平方mm局部区域内变化范围
	float			 fXYChangeSpace;       // 每次测试不同视野之间的距离，um
	float			 fDiffPosition;        // 第一个测试视野与当前视野之间的位置差
	int				 nIndex;			   // 测试不同视野的序号
	int				 iMaxTest;			   // 测试视野的最大数量
} StructLargeFocusingData;

//typedef struct tagFOVFocusInfo
//{
//	CvPoint3D32f	cvFOV3DPos;           // 绝对位置
//	float			fNearFOV_Z_Average;	  // 相邻两个视野焦平面的平均值
//	int				nScanSequenceIndex;
//	int				iValidSubImgCount;
//} StructFOVFocusInfo;

typedef struct tagInitFOVFocusInfo
{
	cv::Point3f 	cvFOV3DPos;           // 红十字处初始焦平面的绝对位置
	float			fZRefPosition;        // Z轴参考位置
	float			fRefLight;			  // 自动调光之后，参考位置的参考亮度值
	float			fXYRange;             // 搜索玻片上红十字的范围
	float			fDeltaY;              // 搜索时，Y方向的变化步长	
	float			fStopThreshold;       // 根据图像亮度值搜索玻片上红十字的位置，停止搜索红十字的门限
	float			fLightThreshold;      // 图像亮度不能太暗
	float			fCostThreshold;		  // 图像清晰度必须大于该值才能认定是有效的红十字焦平面
	float			fFocusChangeRange;    // 整张玻片焦平面的变化范围
	float           fMinPostion;          // 整张玻片焦平面变化的上极限
	float			fMaxPostion;          // 整张玻片焦平面变化的下极限
} StructInitFOVFocusInfo;

enum SEARCH_STOP { NOSTOP, SEARCH_PEAK };

enum SCAN_TYPE { FIRSTSCAN, POSTSCAN };

private:
	float			m_fWorkPositionMechanics[2];

private:
	const float			 m_fFPS;             // 摄像机帧频率
	const float			 m_fZRefPosition;	// unit: um;  获得参考图像的Z轴位置
	const float			 m_fMaxStep;    // 大范围搜索焦平面的步长
	const float			 m_fMinStep;    // 精细搜索最佳图像的步长	
	
	float			 m_fMinPostion;			  // 焦平面所在的极限范围
	float			 m_fMaxPostion;
	float			 m_fStartSearchPosition;  // 每次搜索焦平面的起始点
	float			 m_fEndSearchPosition;    // 每次搜索焦平面的终止点	

	int				 m_iSubImgWidth;        // 子图的宽度
	int				 m_iSubImgHeight;		// 子图的高度
	int				 m_iSubImgRow;          // 子图的行数
	int				 m_iSubImgCol;			// 子图的列数

	StructLargeFocusingData			m_LargeFocusingData;  // 大范围寻找焦平面过程中的数据

private:
	cv::Mat			 m_matBestImage;		  // 聚焦的图像：Output
	cv::Mat			 m_matRefGrayImg;     // 参考图像：原始灰度图像缩小一半
	cv::Mat			 m_matCurDstGrayImg;  // 当前原始图像转换的待评估的灰度图像	

	vector <StructSearchData>		m_vSampleData;        // 在指定范围内搜索时记录下的数据
	vector <StructFocusPlaneRange>  m_vFocusPlaneRange;   // 焦平面在不同区间上的概率
	vector <StructFOVCostPeakData>  m_vFOVCostPeakData;   // 扫描过程中寻找焦平面的数据
	StructFOVCostPeakData			m_CurFOVCostPeakData; // 当前正在聚焦的视野的数据
	int								m_nCurFOVIndex;       // 当前视野在扫描拓扑结构中的索引

	vector <StructFOVCostPeakData>  m_vFOVCostPeakDataInLargeRange;   // 扫描过程中大范围寻找焦平面的数据

	//CArray<StructFOVFocusInfo, StructFOVFocusInfo>	m_arrayFOVFocusInfo;      // 扫描拓扑结构在扫描聚焦过程中数据
	//vector <StructFOVFocusInfo>     m_vPostProcessFOV;

private:
	StructInitFOVFocusInfo  m_structInitFOVFocusInfo;   // 搜索玻片上红十字的位置，并进行初始聚焦，估计整张玻片焦平面的范围

private:
	CBacilusDetectView*		m_pDetectView;
	SCANROUTE*				m_pScanRoute;        // 扫描拓扑结构
	CFile					m_File;
	

private:
	double GetHalfImageLightness(cv::Mat matHalfGrayImg);  // 获得子图像中最大的亮度值
	void  SetReferenceImage();		 // Z轴运动到参考点获得参考图像，保存在 m_matRefGrayImg 中
	void  GenerateCurDstGrayImg();   // 将采集到的当前原始图像转换为待评估的灰度图像，保存在 m_matCurDstGrayImg 中
	void  RecordSampleData(float fStartPos, float fEndPos, float fStep, SEARCH_STOP Enum_stop);        // 以指定的采样步长，在指定范围内进行搜索，记录下的数据保存在 m_vSampleData 中
	void  MarkPeakInCostCurve(vector <StructCostPeakData> * pvPeakData, vector <StructSearchData> * pvSampleData, float fZeroSkewError); // 在清晰度曲线中，从后向前寻找所有山峰的位置，记录到pvPeakData中
	BOOL  AnalyzeImpurity(cv::Mat matBinaryImage); // 分析二值图像的前景对象是否都是杂质
	void  AnalyzeFocusPosProb(float fStartPos, float fEndPos, float fStep);     // 在给定的范围内，从上到下搜索并估计焦平面的位置及其概率，将数据保存在 m_vFOVCostPeakData 中
	BOOL  GenerateEstFocusData(cv::Point2f cvFOVPosition, vector <StructFOVCostPeakData>& vFOVCostPeakData);         // 对 vFOVCostPeakData 中的数据进行分析，将聚焦视野附近的数据按照Z轴位置升序排序，并生成m_vFocusPlaneRange数据
	BOOL  EstFocusPositionStop();    // 大范围寻找焦平面过程是否终止
	BOOL  CheckTestFOVValid(cv::Point2f cvFOVPosition); 	// 检查需要测试的视野位置是否与已测试的视野位置靠的太近
	BOOL  Move2NextTestFOV();        // XY轴移动到下一个测试视野处

	void  EstFocusRange(cv::Point2f cvFOVPosition); // 估计大范围搜索焦平面时的搜索极限范围，结果保存在 m_fStartSearchPosition, m_fEndSearchPosition 中
	BOOL  EstFocusPosition(cv::Point2f cvFOVPosition, float fStartPos = 0, float fEndPos = 0, float fStep = 0 ); // 大范围搜索焦平面所在的范围（位置区间）：在焦平面所在的极限范围内，搜索并估计焦平面的位置	
	
	int   SearchBestImage(StructBestImageData* pSBestImageData, float fStartPos, float fEndPos, float fStep);    // 在给定的局部范围内，以fStep为步长搜索最清晰的图像，并返回其所对应的位置 
	BOOL  SearchBestImageStop(vector <StructSearchData> * pvSampleData);							             // 判断搜索最佳图像的过程是否终止	
	BOOL  SearchPeakStop(vector <StructSearchData> * pvSampleData); // 动态聚焦过程中，判断搜索上峰的过程是否终止

	BOOL  GetBestImage(float* pfBestFocusPos, float fStep, SCAN_TYPE scantype = FIRSTSCAN);   // 获得当前视野最佳的图像以及焦平面的位置，最佳的图像保存在 m_matBestImage 中，搜索过程生成的数据保存在 m_CurFOVCostPeakData 之中，同时将其保存在 m_vFOVCostPeakData 中
	BOOL  WhetherExtendRangeDown(float fExtendCostThreshold, float fZeroSkewError = 0);   // 判断是否需要增加搜索范围
	BOOL  WhetherExtendRangeUp(float fExtendCostThreshold, float fZeroSkewError = 0);   // 判断是否需要增加搜索范围

	float ComputeBestPeak(vector <StructSearchData>& vSampleData, vector <StructSearchData>::iterator iPeak); // 通过二次曲线拟合的方法，计算出清晰度局部峰值的最佳位置

	BOOL  SearchRedCrossPosition(cv::Point2f startPos, int iScanSign);

	void  DrawCostCurve(vector <StructSearchData>::const_iterator iFirst, vector <StructSearchData>::const_iterator iLast);  // For Debuging
	void  DrawCostCurve2(vector <StructSearchData> * pvSearchData, vector <StructCostPeakData> * pvPeakData);  // For Debuging
	void  DrawPeakProbCurve(vector <StructCostPeakData>::const_iterator iFirst, vector <StructCostPeakData>::const_iterator iLast);  // For Debuging
	void  DrawFocusPlaneRangeCurve();  // For Debuging
	

public:
	BOOL  InitAotoFocus( CBacilusDetectView* pDetectView);		// 初始化自动聚焦类
	float GetCurFOVBestImage(cv::Mat * pmatBestImage, CvPoint2D32f cvFOVPosition, int nCurFOVIndex);	        // 获得当前视野最佳的图像以及焦平面的位置
	//int	  AnalyzeScanedFOVs();                                                                                  // 对已扫描的视野聚焦情况进行分析，根据扫描拓扑结构判断哪些视野需要重新聚焦
	//int	  GetPostProcessFOVBestImage(IplImage** ppcvBestImage, CvPoint3D32f* pcvFOV3DPos, int nIndex);          // 获得后处理视野的最佳的图像、视野位置、焦平面位置以及视野扫描序号
	BOOL  EstInitFocusPosition();                                                                               // 利用玻片上的红十字估计目标层焦平面的初始位置
};


bool  lesser_SearchDataImageCost( CAutoFocus::StructSearchData elem1, CAutoFocus::StructSearchData elem2 );
bool  lesser_CostPeakDataZPosition( CAutoFocus::StructCostPeakData elem1, CAutoFocus::StructCostPeakData elem2 );
bool  lesser_FocusPlaneRangeProbability( CAutoFocus::StructFocusPlaneRange elem1, CAutoFocus::StructFocusPlaneRange elem2 );
bool  lesser_FOVPosition( CAutoFocus::StructFOVCostPeakData elem1, CAutoFocus::StructFOVCostPeakData elem2 );
