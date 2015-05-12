#pragma once

// revised by bqsong on 2015.5.3
/* 
	��Opencv1.0�Ľṹ�޸�ΪOpencv2.4.11�Ľṹ
*/

#include "BacilusDetectView.h"
#include "opencv2\opencv.hpp"

class CBacilusDetectView;

// �Զ����⣺ͨ��������������ع�ʱ��������ͼ������
class CAutoAdjustLight
{
public:
	CAutoAdjustLight(void);
	~CAutoAdjustLight(void);

private:
	CBacilusDetectView*		m_pDetectView;
	SCANROUTE*				m_pScanRoute;				// ɨ�����˽ṹ

	const float				m_fBestImageGray;			// ��Ѳο�����ֵ
	const ULONG				m_ulInitExpTime;			// ��ʼ�ع�ʱ��, unit: us
	const float				m_iExpTimeSkew;				// �ع�ʱ��仯��б��
	const float				m_fAdjustLightPositionZ;	// �Զ�����Z��λ��, unit: um
	const float				m_fInitWBPositionZ;         // ��һ���Զ���ƽ��λ��
	const float				m_fLightZeroThreshold;		// �����������

	const int				m_iSubImgRow;				// ��ͼ������
	const int				m_iSubImgCol;				// ��ͼ������
	int						m_iSubImgWidth;				// ��ͼ�Ŀ��
	int						m_iSubImgHeight;			// ��ͼ�ĸ߶�

	const float				m_fLightDifference;         // ��ͬ��ͼ���Ȳ�������ֵ
	const float				m_fWBZeroThreshold;         // ��ƽ��ϵ���������  

	const cv::Point2f		m_2DfStartPoint;			// �Զ������ʼλ��
	const float				m_fRangeInXY;				// ѡ���ڲ�ͬ�ط������Զ�����ķ�Χ
	const float				m_fDeltaInXY;				// �ڲ�ͬ�ط������Զ�����ı仯����

public:
	BOOL  InitAutoAdjustLight(CBacilusDetectView* pDetectView);		// ��ʼ���Զ�������
	BOOL  Work(void);

private:
	BOOL TuneLight(void);    // �Զ�����
	BOOL CheckWhiteBalance(void); // ����Զ�����֮���ƽ���Ƿ�����Ҫ��
};

