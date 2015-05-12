#pragma once

#include "define.h"
#include "afxwin.h"
#include "MotorController.h"
#include "Camera.h"

// CMicrosopeControlDlg 对话框
//显微镜控制对话框

class CMicrosopeControlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMicrosopeControlDlg)

public:
	CMicrosopeControlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMicrosopeControlDlg();
	virtual BOOL OnInitDialog();

// 对话框数据
	enum { IDD = IDD_DLG_MICROSCOPE_CONTOL };

// 控件数据
private:  
	CRect			m_RTImageBmp_Rect;   // 显示的图像在界面中的位置
          
	DISTANCE_UNIT   m_ZMotorUnit;		// MCU系统、Z轴系统的数据
	float			m_ZMotorSpeed;
	float			m_ZMotorPosition;
//	float			m_ZMotorEncoder;
	float			m_ZMotorMoveUp;
	float			m_ZMotorMoveDown;

//	int				m_Light;
	// 显微镜光源、摄像机曝光时间
	float			m_expotime_ms;
	ULONG			m_expotime_us;

	DISTANCE_UNIT   m_XMotorUnit;        // PCI控制卡、X轴系统的数据
	float			m_XMotorSpeed;
	float			m_XMotorPosition;
	float			m_XMotorMoveLeft;
	float			m_XMotorMoveRight;
//	float			m_XMotorEncoder;

	DISTANCE_UNIT   m_YMotorUnit;        // PCI控制卡、Y轴系统的数据
	float			m_YMotorSpeed;
	float			m_YMotorPosition;
//	float			m_YMotorEncoder;
	float			m_YMotorMoveBack;
	float			m_YMotorMoveFrontad;

// 内部操作
private:
	UINT_PTR				m_nTimer;
	int						m_nTimerPeriod;   // ms

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// Interface
public:
	void  UpdateImage(void);

// 生成的消息映射函数
	DECLARE_MESSAGE_MAP()   

public:
	// MCU系统
// 	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar); // 调光
// 	afx_msg void OnBnClickedBtnLight();										 // 调光
	afx_msg void OnPaint();													 // 显示摄像机图像 
	afx_msg void OnCbnSelchangeComboZUnit();								 // 设置Z轴的距离单位
	afx_msg void OnBnClickedBtnZOrigin();									 // Z轴步进电机寻找零点
 	afx_msg void OnBnClickedBtnZStop();										 // 停止Z轴步进电机的运动
	afx_msg void OnBnClickedBtnZSpeed();									 // 设置Z轴步进电机的速度
	afx_msg void OnBnClickedBtnZUp();										 // Z轴步进电机向上运动
	afx_msg void OnBnClickedBtnZDonwn();									 // Z轴步进电机向下运动
	afx_msg void OnBnClickedBtnZCheck();									 // 查询MCU系统状态

	// 摄像机系统
	afx_msg void OnBnClickedBtnExpotime();
	afx_msg void OnBnClickedBtnWhitebalance();

	// X轴系统
	afx_msg void OnCbnSelchangeComboXUnit();								// 设置X轴的距离单位
	afx_msg void OnBnClickedBtnXSpeed();
	afx_msg void OnBnClickedBtnXDown();
	afx_msg void OnBnClickedBtnXUp();
// 	afx_msg void OnBnClickedBtnXStop();
// 	afx_msg void OnBnClickedBtnXCheck();
// 	afx_msg void OnBnClickedBtnXOrigin();

	// Y轴系统
	afx_msg void OnCbnSelchangeComboYUnit();								// 设置Y轴的距离单位
	afx_msg void OnBnClickedBtnYSpeed();
	afx_msg void OnBnClickedBtnYDown();
	afx_msg void OnBnClickedBtnYUp();
// 	afx_msg void OnBnClickedBtnYStop();
// 	afx_msg void OnBnClickedBtnYCheck();
// 	afx_msg void OnBnClickedBtnYOrigin();

	virtual BOOL PreTranslateMessage(MSG* pMsg);                             // 用键盘控制XYZ平台的运动
	virtual void OnCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);								 // 实时更新XYZ系统状态在界面上的显示	
	afx_msg void OnBnClickedButtonSaveimage();
// 	afx_msg void OnBnClickedBtnLightRecord();
	afx_msg void OnBnClickedBtnForceStop();
};
