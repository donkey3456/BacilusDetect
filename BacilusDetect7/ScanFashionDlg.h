#pragma once

#include "define.h"
#include "afxwin.h"

// CScanFashionDlg 对话框 
// 实现扫描方式设置

class CScanFashionDlg : public CDialogEx
{
	enum VIEW_DISTRIBUTE {VIEW_DISTRIBUTE_AVERAGE, VIEW_DISTRIBUTE_NINEPOINT, VIEW_DISTRIBUTE_CONTINUE};

	enum SCAN_ORIGIN {SCAN_ORIGIN_LEFT_TOP, SCAN_ORIGIN_RIGHT_TOP, SCAN_ORIGIN_LEFT_DOWN, SCAN_ORIGIN_RIGHT_DOWN};

	enum SCAN_FASHION {SCAN_FASHION_VERTICAL, SCAN_FASHION_HORIZON};

	DECLARE_DYNAMIC(CScanFashionDlg)

public:
	CScanFashionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CScanFashionDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SCAN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

// 控件数据
private:
	UINT m_view_total_num;
	UINT m_view_column_num;
	UINT m_view_row_num;
	float m_average_horizon_distance;
	float m_average_vertical_distance;
	float m_ninepoint_horizon_distance;    // mm
	float m_ninepoint_vertical_distance;   // mm
	float m_continue_horizon_overlap;
	float m_continue_vertical_overlap;
	int m_view_distribute;
	BOOL m_bBestScanRoute;
	int m_scan_origin;
	int m_scan_fashion;
	float m_area_horizon;        // mm
	float m_area_vertical;       // mm
	float m_area_updown;         // mm
	float m_area_leftright;     // mm

	CButton m_ctl_BestScanRoute;
	CButton m_ctl_scan_fashion_vertical;
	CButton m_ctl_scan_fashion_horizon;
	CButton m_ctl_scan_origin_lefttop;
	CButton m_ctl_scan_origin_leftdown;
	CButton m_ctl_scan_origin_righttop;
	CButton m_ctl_scan_origin_rightdown;

	CStatic       m_slide_scan_area;
	CBitmap       m_slidebmp;
	const UINT    m_slidebmp_width;
	const UINT    m_slidebmp_height;
	const CPoint  m_slidebmp_origin;	
	const float   m_scan_fashion_display_ratio;
	const DWORD   m_slidebmp_byteCount;
	HBITMAP       m_hSlidBitmap;

// 扫描路径数据
private:
	const float         m_mechanical_horizon;                // mechanic range, unit: um
	const float         m_mechanical_vertical;
	const float         m_view_width;                        // 一个视野所对应的物理距离，distance unit = um
	const float         m_view_height;
	cv::Point2f          m_rightdown_position;       	      // default position of the left-top of the scan area
	CArray<cv::Point2f, cv::Point2f> m_area_topology;    // distance unit = um, // 表示一个二维矩阵，下标(i,j)表示行和列
	CArray<MATRIXINDEX, MATRIXINDEX> m_area_scan_sequence;         // store the index of the matrix == m_area_topology
	                                                   // MATRIXINDEX.row表示矩阵的行索引，MATRIXINDEX.col表示矩阵的列索引

public:
	void  getScanRoute(SCANROUTE& scanroute);       // Interface

// 内部操作
private:
	BOOL  generate_area_topology();
	void  set_subarea_scan_sequence(UINT& scan_index,
									MATRIXINDEX right_down, UINT width, UINT height,
									SCAN_ORIGIN origin, SCAN_FASHION fashion);
	void  set_scan_sequence();
	BOOL  control_change();
	void EnableScanSet(BOOL bEnable = TRUE );

public:
	afx_msg void OnEnChangeEditHorizonNum();
	afx_msg void OnEnChangeEditVerticalNum();
	afx_msg void OnBnClickedRadioNinepoint();
	afx_msg void OnBnClickedRadioAverage();
	afx_msg void OnBnClickedRadioContinue();
	afx_msg void OnBnClickedButtonAreaUp();
	afx_msg void OnBnClickedButtonAreaDown();
	afx_msg void OnBnClickedButtonAreaLeft();
	afx_msg void OnBnClickedButtonAreaRight();
	afx_msg void OnBnClickedCheckBestscanroute();
	afx_msg void OnBnClickedRadioLefttop();
	afx_msg void OnBnClickedRadioRighttop();
	afx_msg void OnBnClickedRadioLeftdown();
	afx_msg void OnBnClickedRadioRightdown();
	afx_msg void OnBnClickedRadioVertical();
	afx_msg void OnBnClickedRadioHorizon();
	afx_msg void OnBnClickedApply();
	virtual void OnOK();	
	virtual BOOL OnInitDialog();
};
