
// BacilusDetectView.h : CBacilusDetectView 类的接口
//

#pragma once

#include <memory>
#include "resource.h"
#include "define.h"
#include "BacilusDetectDoc.h"
#include "ScanFashionDlg.h"
#include "MicrosopeControlDlg.h"
#include "Camera.h"
#include "MotorController.h"
#include "SlideInfoDlg.h"
#include "ImageProcess.h"
#include "JudgeDlg.h"
// #include "AutoFocus.h"
// #include "AutoAdjustLight.h"
#include <queue> 

class CBacilusDetectView;

UINT __cdecl AutoScanSlideWorkThread(LPVOID pParam);// 自动扫描检验玻片线程
UINT __cdecl ImageProcessWorkThread(LPVOID pParam); // 图像处理线程
void WhetherStopWorkThread(CBacilusDetectView* pDetectView);

typedef struct tagIMAGEINFO
{
    int		nScanSequenceIndex;
	CString strFileName;
} STRUCTIMAGEINFO;

typedef struct tagMENUITEMSTATUS
{
    BOOL Menu_ControlScan;
	BOOL Menu_ControlSelect;
	BOOL Menu_ControlOrigin;
	BOOL Menu_ControlMicroscope;
	BOOL Menu_ControlCamera;
	BOOL Menu_ViewBacilus;
} MENUITEMSTATUS;

enum GUI_ENABLE_MASK 
{
	GUI_MENU_SCANMODE			= 1<<1, 
	GUI_MENU_MICROSCOPECONTROL	= 1<<2, 
	GUI_MENU_ORIGIN				= 1<<3, 
	GUI_MENU_SELECT				= 1<<4,
	GUI_MENU_CAMERA				= 1<<5,
	GUI_MENU_VIEWBACILUS		= 1<<6,
	GUI_BUTTON_START			= 1<<16,
	GUI_BUTTON_SLIDEINFO		= 1<<17,
	GUI_BUTTON_SCAN				= 1<<18,
	GUI_BUTTON_AUTOSCAN			= 1<<19,
	GUI_BUTTON_STOPSCAN			= 1<<20,
	GUI_BUTTON_JUDGE			= 1<<21,
	GUI_BUTTON_NEXTSLIDE		= 1<<22,
	GUI_BUTTON_CHANGEBOX		= 1<<23,
	GUI_BUTTON_EXIT				= 1<<24
};

class CBacilusDetectView : public CFormView
{
protected: // 仅从序列化创建
	CBacilusDetectView();
	DECLARE_DYNCREATE(CBacilusDetectView)

public:
	enum{ IDD = IDD_BACILUSDETECT_FORM };

public:
// 自动扫描检验线程
	CWinThread*		m_pAutoScanWorkThread;
	CString			m_Directory;          // 临时文件目录
// 自动扫描检验线程所需要使用的数据
public:
	//int				m_SlideWorkPositionIndex;
	int				m_ScanSequenceIndex;
	int				m_nImageProcessSequenceIndex;
	CEvent				m_EventStopAutoScan;	    // 全局同步事件
	CEvent				m_EventStopImageProcess;	// 全局同步事件
	volatile BOOL		m_bImageProcessEnd;         // 全局同步变量：图像处理线程结束
	volatile BOOL		m_bImageProcessThread;      // 全局同步变量：图像处理线程已经开启
	volatile BOOL		m_bImageProcess;			// 全局同步变量：图像处理线程已经开始处理对立里的图像
	volatile BOOL		m_bScanPostProcess;         // 全局同步变量：第一次扫描采集图像工作结束，进行后处理，将有肯能错误聚焦的视野进行重新聚焦
	volatile BOOL		m_bScanEnd;                 // 全局同步变量：视野扫描采集图像工作结束
	queue <STRUCTIMAGEINFO>	m_qImageInfo;			// 全局同步变量：待处理图像队列

// 接口
public:
	void  SystemExiting();    // 程序退出之前，关闭各种设备，释放资源

// 特性
public:
	CBacilusDetectDoc* GetDocument() const;

// 显示数据
private:
	const DWORD				m_ImageDate_PixelCount;
	const DWORD				m_ImageDate_byteCount;
	CString					m_DefaultDirectory;          // 临时文件目录

	CMicrosopeControlDlg	m_MicroscopeControlDlg;      //显微镜控制对话框

private:          // 显示扫描过程示意图
	CScanFashionDlg m_ScanFashionDlg;					//扫描方式设置对话框
	// ==============扫描过程示意图信息========= //
	CBitmap         m_SlideScanBmp;
	const UINT		m_SlideScanBmp_Width;   
	const UINT		m_SlideScanBmp_Height;
	const DWORD		m_SlideScanBmp_byteCount;
	const CPoint	m_SlideScanBmp_Origin;	
	const float     m_SlideScanBmp_Display_Ratio;
	const CPoint	m_SlideIndexInSlideScanBmp;
	const CPoint    m_SlideIDInSlideScanBmp;
	CArray<POINT, POINT> m_Topology_in_SlideScanBmp;

	SCANROUTE		m_ScanRoute;        // unit: um

// 主界面显示区域中各个子部分的位置
private:
	CSize			m_ClientSize;       	// 主界面显示区域的大小
	CSize			m_ClientMinSize;       	// 动态改变界面大小是，主界面显示区域的最小值
	CRect			m_ImageBmp_Rect;        // 正在进行操作的视野图像在主界面中的位置
	CRect			m_SlideScanBmp_Rect;    // 扫描示意图在主界面中的位置
	CRect			m_StaticPrompt_Rect;    // 操作提示静态控件在主界面中的位置
	CRect			m_EditcPrompt_Rect;     // 操作提示Edit控件在主界面中的位置
	CRect			m_SlideIndex_Rect;      // 当前工位编号在主界面中的位置	
	CRect			m_SlideID_Rect;         // 当前玻片编号在主界面中的位置	

private: 
	CSlideInfoDlg   m_SlideInfoDlg;						//玻片信息输入对话框
	SLIDEINFO		m_CurSlideInfo;

public:
	CJudgeDlg*      m_pJudgeDlg;

// GUI状态
private:
	CString			m_OperationPrompt;   // 显示系统操作提示信息
	BOOL			m_AutoScanBox;
	DWORD			m_GUIStatus;
	MENUITEMSTATUS  m_MenuItemStatus;
	
// 内部操作数据
private:
	BOOL            m_DeviceOK;

// 内部操作
private:
	void  PhPtoScanBmpLP();   // 将扫描区域拓扑结构中的视野的物理坐标转换成在扫描过程示意图中的逻辑坐标
	void  addScanRoute2Bmp(); // 将扫描路径加到扫描过程示意图中

public:	
	CPoint XYPhPtoScanBmpLP(cv::Point2f XYpos); // 将XY平台上的绝对坐标（unit：um）转换成在扫描过程示意图中的逻辑坐标
	void  AddPoint2SlideScanBmp(cv::Point2f XYpos); // 将由XY平台上绝对坐标表示的点在扫描过程示意图中标记成红色
	void  DisableALLGUI();					// 禁止所有的界面响应接口，包括：菜单、按钮
	void  EnableGUI(DWORD dMask);			// 使能界面响应接口，包括：菜单、按钮
	void  DisableGUI(DWORD dMask);			// 禁止界面响应接口，包括：菜单、按钮
	
	void  UpdateScanRouteBmp(); // 更新扫描过程示意图中的扫描路径
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnInitialUpdate(); // 构造后第一次调用
// 实现
public:
	virtual ~CBacilusDetectView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg LRESULT OnMsgScanrouteNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgCameraNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgScanthreadNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgJudgedlgreturnNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgJudgedlgobserveNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	virtual void OnDraw(CDC* pDC);
	afx_msg void OnControlScan();
	afx_msg void OnControlMicroscope();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnUpdateControlMicroscope(CCmdUI *pCmdUI);
/*	afx_msg void OnBnClickedButtonNextslide();*/
	afx_msg void OnControlOrigin();
	afx_msg void OnUpdateControlOrigin(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedButtonChangebox();
// 	afx_msg void OnControlSelect1();
// 	afx_msg void OnUpdateControlSelect1(CCmdUI *pCmdUI);
// 	afx_msg void OnControlSelect2();
// 	afx_msg void OnControlSelect3();
// 	afx_msg void OnUpdateControlSelect3(CCmdUI *pCmdUI);
// 	afx_msg void OnControlSelect4();
// 	afx_msg void OnUpdateControlSelect4(CCmdUI *pCmdUI);
// 	afx_msg void OnControlSelect5();
// 	afx_msg void OnUpdateControlSelect5(CCmdUI *pCmdUI);
// 	afx_msg void OnUpdateControlSelect2(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedButtonSlideinfo();
	afx_msg void OnBnClickedButtonScan();
	afx_msg void OnBnClickedButtonScanstop();
	afx_msg void OnUpdateControlScan(CCmdUI *pCmdUI);	
	afx_msg void OnBnClickedButtonJudge();
// 	afx_msg void OnBnClickedButtonAutoscan();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // BacilusDetectView.cpp 中的调试版本
inline CBacilusDetectDoc* CBacilusDetectView::GetDocument() const
   { return reinterpret_cast<CBacilusDetectDoc*>(m_pDocument); }
#endif

