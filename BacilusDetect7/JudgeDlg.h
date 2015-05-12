#pragma once
#include "BacilusDetectDoc.h"
#include "ImageProcess.h"
#include "afxcmn.h"
#include "Report.h"
#include "afxwin.h"

// CJudgeDlg 对话框
// 辅助诊断对话框

class CJudgeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CJudgeDlg)

public:
	CJudgeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CJudgeDlg();

// 对话框数据
	enum { IDD = IDD_DLG_JUDGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CBacilusDetectDoc* m_pDocument;
	CView* m_pView;

public:
	virtual BOOL OnInitDialog();

// 控件数据
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

// 内部数据
private:
	int		m_nFOVSequenceIndex;
	int		m_nFOVIndex;
	int		m_JudgedTargetCount;

// 病理报告对话框
private:
	CReport  m_Report;

// 内部操作
public:
	void	InitiateWorkPositionCombo();
	void	UpdateWorkPosition();
	void	DrawPicture2HDC();   // 将OpenCV图像显示到控件上

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
