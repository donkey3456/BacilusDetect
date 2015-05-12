#pragma once

// revised by bqsong on 2015.5.3
/* 
	将Opencv1.0的结构修改为Opencv2.4.11的结构
*/

#include "BacilusDetectView.h"
#include "opencv2\opencv.hpp"

class CBacilusDetectView;

// 自动调光：通过调节摄像机的曝光时间来调节图像亮度
class CAutoAdjustLight
{
public:
	CAutoAdjustLight(void);
	~CAutoAdjustLight(void);

private:
	CBacilusDetectView*		m_pDetectView;
	SCANROUTE*				m_pScanRoute;				// 扫描拓扑结构

	const float				m_fBestImageGray;			// 最佳参考亮度值
	const ULONG				m_ulInitExpTime;			// 初始曝光时间, unit: us
	const float				m_iExpTimeSkew;				// 曝光时间变化的斜率
	const float				m_fAdjustLightPositionZ;	// 自动调光Z轴位置, unit: um
	const float				m_fInitWBPositionZ;         // 第一次自动白平衡位置
	const float				m_fLightZeroThreshold;		// 亮度误差容限

	const int				m_iSubImgRow;				// 子图的行数
	const int				m_iSubImgCol;				// 子图的列数
	int						m_iSubImgWidth;				// 子图的宽度
	int						m_iSubImgHeight;			// 子图的高度

	const float				m_fLightDifference;         // 不同子图亮度差异门限值
	const float				m_fWBZeroThreshold;         // 白平衡系数误差容限  

	const cv::Point2f		m_2DfStartPoint;			// 自动调光初始位置
	const float				m_fRangeInXY;				// 选择在不同地方进行自动调光的范围
	const float				m_fDeltaInXY;				// 在不同地方进行自动调光的变化距离

public:
	BOOL  InitAutoAdjustLight(CBacilusDetectView* pDetectView);		// 初始化自动调光类
	BOOL  Work(void);

private:
	BOOL TuneLight(void);    // 自动调光
	BOOL CheckWhiteBalance(void); // 检查自动调光之后白平衡是否满足要求
};

