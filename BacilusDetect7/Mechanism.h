#pragma once

#include "define.h"

#define MOTOR_Z_MOVE_SAFE_POSITION 0.2			//mm
#define MOTOR_Z_MOVE_SPEED_MAX 50/60.0				//mm/s
#define MOTOR_XY_MOVE_SPEED_MAX 2000/60.0					//mm/s
class CMechanism
{
public:
	CMechanism(void);
	~CMechanism(void);

	// �ڲ���������
private:
	float           m_XMaxRange;        // unit: um
	float           m_XMinRange;
	float           m_YMaxRange;
	float           m_YMinRange;
	float           m_ZMaxRange;
	float           m_ZMinRange;


	cv::Point2f		m_ptSlideOrigin;  // 1����λ����ʼλ�ã�unit: um
	cv::Point3f		m_f3DStopPosition;
	float			m_ZSafeUpperPosition;

	SCANROUTE		m_ScanRoute;        // unit: um

public:
	BOOL			isValidPosition(cv::Point2f f2DPoint);		// �ж�XY���������Ƿ��ڹ涨�Ļ�е��Χ��
	BOOL			isValidPosition(cv::Point3f f3DPoint);		// �ж�XYZ���������Ƿ��ڹ涨�Ļ�е��Χ��
	BOOL			isValidPositionX(float fX);
	BOOL			isValidPositionY(float fY);

	cv::Point2f		GetRelativeXY(cv::Point2f f2DPoint);       // ��������������ת��Ϊ��ǰ��λ�ϵĲ�Ƭ����ϵ�ϵ��������

	float			GetZSafeUpperPosition() { return m_ZSafeUpperPosition; };
	cv::Point3f		GetStopPosition() { return m_f3DStopPosition; };

	void			SetScanRoute(const PSCANROUTE pScanRoute);	// ����ɨ��·��
	void			SetScanRoute(int nRow, int nCol, 
									const CArray<cv::Point2f, cv::Point2f>& aTopology, 
									const CArray<MATRIXINDEX, MATRIXINDEX>& aSequence);
	void			GetScanRoute(SCANROUTE& scanroute);			// ���ɨ��·��
	const PSCANROUTE	GetScanRoute();							// ���ɨ��·����ָ��

	cv::Point2f     GenerateDestPointInXY(int nScanSequenceIndex); // ����һ����Ұ�ľ�������, unit: um

	void			GotoOrigin();								// �ص���ǰ��λ����ʼ��

	void			GotoFOV(int nScanSequenceIndex);			// �˶�����ǰ��λ��һ����Ұ��
	void			GotoDestPositionInXY(cv::Point2f f2DPoint);// �˶���ָ����λ�ô�					
};

