#pragma once
#include "BacilusDetectDoc.h"
#include "ImageProcess.h"
#include "afxcmn.h"
#include "Report.h"
#include "afxwin.h"

// CJudgeDlg �Ի���
// ������϶Ի���

class CJudgeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CJudgeDlg)

public:
	CJudgeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CJudgeDlg();

// �Ի�������
	enum { IDD = IDD_DLG_JUDGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	CBacilusDetectDoc* m_pDocument;
	CView* m_pView;

public:
	virtual BOOL OnInitDialog();

// �ؼ�����
private:
	CComboBox		m_comboSequenceIndex;
//	CComboBox		m_comboWorkPositionIndex;

	CString			m_SlideID;
	int				m_TargetCount;	
	int				m_Total_TargetCount;
	int				m_Total_JudgedTargetCount;
	CSpinButtonCtrl m_spinJudgedTargetCount;
	cv::Mat			m_matImage;
	double			m_targetProbability;

// �ڲ�����
private:
	int		m_nFOVSequenceIndex;
	int		m_nFOVIndex;
	int		m_JudgedTargetCount;

// ������Ի���
private:
	CReport  m_Report;

// �ڲ�����
public:
	void	InitiateWorkPositionCombo();
	void	UpdateWorkPosition();
	void	DrawPicture2HDC();   // ��OpenCVͼ����ʾ���ؼ���

public:
//	afx_msg void OnCbnSelchangeComboWorkposition();
	afx_msg void OnCbnSelchangeComboFov();
	afx_msg void OnEnChangeJudgedTargetcountinfov();
	afx_msg void OnBnClickedBtnJudge();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnClickedBtnReport();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
