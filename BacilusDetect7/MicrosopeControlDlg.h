#pragma once

#include "define.h"
#include "afxwin.h"
#include "MotorController.h"
#include "Camera.h"

// CMicrosopeControlDlg �Ի���
//��΢�����ƶԻ���

class CMicrosopeControlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMicrosopeControlDlg)

public:
	CMicrosopeControlDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMicrosopeControlDlg();
	virtual BOOL OnInitDialog();

// �Ի�������
	enum { IDD = IDD_DLG_MICROSCOPE_CONTOL };

// �ؼ�����
private:  
	CRect			m_RTImageBmp_Rect;   // ��ʾ��ͼ���ڽ����е�λ��
          
	DISTANCE_UNIT   m_ZMotorUnit;		// MCUϵͳ��Z��ϵͳ������
	float			m_ZMotorSpeed;
	float			m_ZMotorPosition;
//	float			m_ZMotorEncoder;
	float			m_ZMotorMoveUp;
	float			m_ZMotorMoveDown;

//	int				m_Light;
	// ��΢����Դ��������ع�ʱ��
	float			m_expotime_ms;
	ULONG			m_expotime_us;

	DISTANCE_UNIT   m_XMotorUnit;        // PCI���ƿ���X��ϵͳ������
	float			m_XMotorSpeed;
	float			m_XMotorPosition;
	float			m_XMotorMoveLeft;
	float			m_XMotorMoveRight;
//	float			m_XMotorEncoder;

	DISTANCE_UNIT   m_YMotorUnit;        // PCI���ƿ���Y��ϵͳ������
	float			m_YMotorSpeed;
	float			m_YMotorPosition;
//	float			m_YMotorEncoder;
	float			m_YMotorMoveBack;
	float			m_YMotorMoveFrontad;

// �ڲ�����
private:
	UINT_PTR				m_nTimer;
	int						m_nTimerPeriod;   // ms

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// Interface
public:
	void  UpdateImage(void);

// ���ɵ���Ϣӳ�亯��
	DECLARE_MESSAGE_MAP()   

public:
	// MCUϵͳ
// 	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar); // ����
// 	afx_msg void OnBnClickedBtnLight();										 // ����
	afx_msg void OnPaint();													 // ��ʾ�����ͼ�� 
	afx_msg void OnCbnSelchangeComboZUnit();								 // ����Z��ľ��뵥λ
	afx_msg void OnBnClickedBtnZOrigin();									 // Z�Ჽ�����Ѱ�����
 	afx_msg void OnBnClickedBtnZStop();										 // ֹͣZ�Ჽ��������˶�
	afx_msg void OnBnClickedBtnZSpeed();									 // ����Z�Ჽ��������ٶ�
	afx_msg void OnBnClickedBtnZUp();										 // Z�Ჽ����������˶�
	afx_msg void OnBnClickedBtnZDonwn();									 // Z�Ჽ����������˶�
	afx_msg void OnBnClickedBtnZCheck();									 // ��ѯMCUϵͳ״̬

	// �����ϵͳ
	afx_msg void OnBnClickedBtnExpotime();
	afx_msg void OnBnClickedBtnWhitebalance();

	// X��ϵͳ
	afx_msg void OnCbnSelchangeComboXUnit();								// ����X��ľ��뵥λ
	afx_msg void OnBnClickedBtnXSpeed();
	afx_msg void OnBnClickedBtnXDown();
	afx_msg void OnBnClickedBtnXUp();
// 	afx_msg void OnBnClickedBtnXStop();
// 	afx_msg void OnBnClickedBtnXCheck();
// 	afx_msg void OnBnClickedBtnXOrigin();

	// Y��ϵͳ
	afx_msg void OnCbnSelchangeComboYUnit();								// ����Y��ľ��뵥λ
	afx_msg void OnBnClickedBtnYSpeed();
	afx_msg void OnBnClickedBtnYDown();
	afx_msg void OnBnClickedBtnYUp();
// 	afx_msg void OnBnClickedBtnYStop();
// 	afx_msg void OnBnClickedBtnYCheck();
// 	afx_msg void OnBnClickedBtnYOrigin();

	virtual BOOL PreTranslateMessage(MSG* pMsg);                             // �ü��̿���XYZƽ̨���˶�
	virtual void OnCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);								 // ʵʱ����XYZϵͳ״̬�ڽ����ϵ���ʾ	
	afx_msg void OnBnClickedButtonSaveimage();
// 	afx_msg void OnBnClickedBtnLightRecord();
	afx_msg void OnBnClickedBtnForceStop();
};
