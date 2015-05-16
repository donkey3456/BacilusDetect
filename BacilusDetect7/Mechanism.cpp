
#include "StdAfx.h"

#include "Mechanism.h"
#include "MotorController.h"

CMechanism::CMechanism(void)
{
	// 机械参数
	m_XMaxRange = 53000; // um
	m_XMinRange = 0;
	m_YMaxRange = 0;
	m_YMinRange = -38000;
	m_ZMaxRange = 0;
	m_ZMinRange = -14000;
	m_ZSafeUpperPosition = MOTOR_Z_MOVE_SAFE_POSITION * 1000;
	m_ptSlideOrigin.x = -4183;
	m_ptSlideOrigin.y = -28500;
	

	m_f3DStopPosition.x = 95000;  
	m_f3DStopPosition.y = 0;
	m_f3DStopPosition.z = 20000;

}


CMechanism::~CMechanism(void)
{
	m_ScanRoute.ScanTopology.RemoveAll(); 
	m_ScanRoute.ScanTopology.FreeExtra();
	m_ScanRoute.ScanSequence.RemoveAll();
	m_ScanRoute.ScanSequence.FreeExtra();
}


BOOL CMechanism::isValidPosition(cv::Point2f f2DPoint)
{
	if (   ( f2DPoint.x > m_XMaxRange) || ( f2DPoint.x < m_XMinRange)
		|| ( f2DPoint.y > m_YMaxRange) || ( f2DPoint.y < m_YMinRange) )
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CMechanism::isValidPosition(cv::Point3f f3DPoint)
{
	if (   ( f3DPoint.x > m_XMaxRange) || ( f3DPoint.x < m_XMinRange)
		|| ( f3DPoint.y > m_YMaxRange) || ( f3DPoint.y < m_YMinRange) 
		|| ( f3DPoint.z > m_ZMaxRange) || ( f3DPoint.z < m_ZMinRange) )
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CMechanism::isValidPositionX(float fX)
{
	if ( ( fX > m_XMaxRange) || ( fX < m_XMinRange) )
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CMechanism::isValidPositionY(float fY)
{
	if ( ( fY > m_YMaxRange) || ( fY < m_YMinRange) )
	{
		return FALSE;
	}
	
	return TRUE;
}


cv::Point2f CMechanism::GetRelativeXY(cv::Point2f f2DPoint)
{
	// 计算当前物理坐标在当前工位上的玻片坐标系上的相对坐标
	cv::Point2f slidepos;   
	slidepos.x = f2DPoint.x - m_ptSlideOrigin.x;
	slidepos.y = f2DPoint.y - m_ptSlideOrigin.y;

	return slidepos;
}

void CMechanism::SetScanRoute(const PSCANROUTE pScanRoute)
{
	m_ScanRoute.RowNum = pScanRoute->RowNum;
	m_ScanRoute.ColNum = pScanRoute->ColNum;

	m_ScanRoute.ScanTopology.SetSize(pScanRoute->ScanTopology.GetSize());
	m_ScanRoute.ScanTopology.Copy(pScanRoute->ScanTopology);

	m_ScanRoute.ScanSequence.SetSize(pScanRoute->ScanSequence.GetSize());
	m_ScanRoute.ScanSequence.Copy(pScanRoute->ScanSequence);
}

void CMechanism::SetScanRoute(int nRow, int nCol, 
							const CArray<cv::Point2f, cv::Point2f>& aTopology, 
							const CArray<MATRIXINDEX, MATRIXINDEX>& aSequence)
{
	m_ScanRoute.RowNum = nRow;
	m_ScanRoute.ColNum = nCol;

	m_ScanRoute.ScanTopology.SetSize(aTopology.GetSize());
	m_ScanRoute.ScanTopology.Copy(aTopology);

	m_ScanRoute.ScanSequence.SetSize(aSequence.GetSize());
	m_ScanRoute.ScanSequence.Copy(aSequence);
}

void CMechanism::GetScanRoute(SCANROUTE& scanroute)
{
	scanroute.RowNum = m_ScanRoute.RowNum;
	scanroute.ColNum = m_ScanRoute.ColNum;

	scanroute.ScanTopology.SetSize(m_ScanRoute.ScanTopology.GetSize());
	scanroute.ScanTopology.Copy(m_ScanRoute.ScanTopology);

	scanroute.ScanSequence.SetSize(m_ScanRoute.ScanSequence.GetSize());
	scanroute.ScanSequence.Copy(m_ScanRoute.ScanSequence);
}

const PSCANROUTE CMechanism::GetScanRoute()
{
	return &m_ScanRoute;
}


cv::Point2f    CMechanism::GenerateDestPointInXY(int nScanSequenceIndex)
{
	cv::Point2f point, slideorigin;
	MATRIXINDEX matrixindex;
	if ( nScanSequenceIndex >= m_ScanRoute.ScanSequence.GetSize() )
	{
		nScanSequenceIndex = m_ScanRoute.ScanSequence.GetSize() - 1;
	}
	if (nScanSequenceIndex < 0)
	{
		nScanSequenceIndex = 0;
	}

	matrixindex = m_ScanRoute.ScanSequence[nScanSequenceIndex];
	point = m_ScanRoute.ScanTopology[matrixindex.row * m_ScanRoute.ColNum + matrixindex.col];   // um
	point.x += m_ptSlideOrigin.x;
	point.y += m_ptSlideOrigin.y;
	if ( point.x > m_XMaxRange)
	{
		point.x = m_XMaxRange;
	}
	if ( point.x < m_XMinRange)
	{
		point.x = m_XMinRange;
	}
	if ( point.y > m_YMaxRange)
	{
		point.y = m_YMaxRange;
	}
	if ( point.y < m_YMinRange)
	{
		point.y = m_YMinRange;
	}

	return point;
}

void CMechanism::GotoOrigin()
{
	CMotorController* pMC = CMotorController::GetInstance();

	//如果太高，先快速降低
	pMC->SetZMotorSpeed(MOTOR_Z_MOVE_SPEED_MAX);
	pMC->ZMove2(MOTOR_Z_MOVE_SAFE_POSITION);

	cv::Point2f point = GenerateDestPointInXY( 0 );
	pMC->XYMove2(point, DISTANCE_UNIT_UM);
	pMC->WaitUntilDone();
	
	//移动到safteUpperPosition
	pMC->ZMove2(m_ZSafeUpperPosition,DISTANCE_UNIT_UM);
}

void CMechanism::GotoFOV(int nScanSequenceIndex)
{
	CMotorController* pMC = CMotorController::GetInstance();

	cv::Point2f destpos = GenerateDestPointInXY( nScanSequenceIndex ); // 生成下当前视野在XY平台上的绝对坐标, unit: un 
	pMC->SetXMotorSpeed(MOTOR_XY_MOVE_SPEED_MAX);
	pMC->SetYMotorSpeed(MOTOR_XY_MOVE_SPEED_MAX);
	pMC->XYMove2(destpos, DISTANCE_UNIT_UM);		         // 运动到该视野处
	
	pMC->WaitUntilDone();
}

void CMechanism::GotoDestPositionInXY(cv::Point2f f2DPoint)
{
	CMotorController* pMC = CMotorController::GetInstance();
	pMC->SetZMotorSpeed(MOTOR_Z_MOVE_SPEED_MAX);
	pMC->ZMove2(m_ZSafeUpperPosition,DISTANCE_UNIT_UM);
	
	pMC->SetXMotorSpeed(MOTOR_XY_MOVE_SPEED_MAX);
	pMC->SetYMotorSpeed(MOTOR_XY_MOVE_SPEED_MAX);
	pMC->XYMove2(f2DPoint, DISTANCE_UNIT_UM);		         // 运动到该视野处

	pMC->WaitUntilDone();
}