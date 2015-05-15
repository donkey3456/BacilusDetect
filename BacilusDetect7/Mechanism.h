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

	// 内部操作数据
private:
	float           m_XMaxRange;        // unit: um
	float           m_XMinRange;
	float           m_YMaxRange;
	float           m_YMinRange;
	float           m_ZMaxRange;
	float           m_ZMinRange;


	cv::Point2f		m_ptSlideOrigin;  // 1个工位的起始位置，unit: um
	cv::Point3f		m_f3DStopPosition;
	float			m_ZSafeUpperPosition;

	SCANROUTE		m_ScanRoute;        // unit: um

public:
	BOOL			isValidPosition(cv::Point2f f2DPoint);		// 判断XY绝对坐标是否在规定的机械范围内
	BOOL			isValidPosition(cv::Point3f f3DPoint);		// 判断XYZ绝对坐标是否在规定的机械范围内
	BOOL			isValidPositionX(float fX);
	BOOL			isValidPositionY(float fY);

	cv::Point2f		GetRelativeXY(cv::Point2f f2DPoint);       // 将绝对物理坐标转换为当前工位上的玻片坐标系上的相对坐标

	float			GetZSafeUpperPosition() { return m_ZSafeUpperPosition; };
	cv::Point3f		GetStopPosition() { return m_f3DStopPosition; };

	void			SetScanRoute(const PSCANROUTE pScanRoute);	// 设置扫描路径
	void			SetScanRoute(int nRow, int nCol, 
									const CArray<cv::Point2f, cv::Point2f>& aTopology, 
									const CArray<MATRIXINDEX, MATRIXINDEX>& aSequence);
	void			GetScanRoute(SCANROUTE& scanroute);			// 获得扫描路径
	const PSCANROUTE	GetScanRoute();							// 获得扫描路径的指针

	cv::Point2f     GenerateDestPointInXY(int nScanSequenceIndex); // 生成一个视野的绝对坐标, unit: um

	void			GotoOrigin();								// 回到当前工位的起始点

	void			GotoFOV(int nScanSequenceIndex);			// 运动到当前工位的一个视野处
	void			GotoDestPositionInXY(cv::Point2f f2DPoint);// 运动到指定的位置处					
};

