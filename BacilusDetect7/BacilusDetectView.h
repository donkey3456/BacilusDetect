
// BacilusDetectView.h : CBacilusDetectView ��Ľӿ�
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

UINT __cdecl AutoScanSlideWorkThread(LPVOID pParam);// �Զ�ɨ����鲣Ƭ�߳�
UINT __cdecl ImageProcessWorkThread(LPVOID pParam); // ͼ�����߳�
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
protected: // �������л�����
	CBacilusDetectView();
	DECLARE_DYNCREATE(CBacilusDetectView)

public:
	enum{ IDD = IDD_BACILUSDETECT_FORM };

public:
// �Զ�ɨ������߳�
	CWinThread*		m_pAutoScanWorkThread;
	CString			m_Directory;          // ��ʱ�ļ�Ŀ¼
// �Զ�ɨ������߳�����Ҫʹ�õ�����
public:
	//int				m_SlideWorkPositionIndex;
	int				m_ScanSequenceIndex;
	int				m_nImageProcessSequenceIndex;
	CEvent				m_EventStopAutoScan;	    // ȫ��ͬ���¼�
	CEvent				m_EventStopImageProcess;	// ȫ��ͬ���¼�
	volatile BOOL		m_bImageProcessEnd;         // ȫ��ͬ��������ͼ�����߳̽���
	volatile BOOL		m_bImageProcessThread;      // ȫ��ͬ��������ͼ�����߳��Ѿ�����
	volatile BOOL		m_bImageProcess;			// ȫ��ͬ��������ͼ�����߳��Ѿ���ʼ����������ͼ��
	volatile BOOL		m_bScanPostProcess;         // ȫ��ͬ����������һ��ɨ��ɼ�ͼ�������������к������п��ܴ���۽�����Ұ�������¾۽�
	volatile BOOL		m_bScanEnd;                 // ȫ��ͬ����������Ұɨ��ɼ�ͼ��������
	queue <STRUCTIMAGEINFO>	m_qImageInfo;			// ȫ��ͬ��������������ͼ�����

// �ӿ�
public:
	void  SystemExiting();    // �����˳�֮ǰ���رո����豸���ͷ���Դ

// ����
public:
	CBacilusDetectDoc* GetDocument() const;

// ��ʾ����
private:
	const DWORD				m_ImageDate_PixelCount;
	const DWORD				m_ImageDate_byteCount;
	CString					m_DefaultDirectory;          // ��ʱ�ļ�Ŀ¼

	CMicrosopeControlDlg	m_MicroscopeControlDlg;      //��΢�����ƶԻ���

private:          // ��ʾɨ�����ʾ��ͼ
	CScanFashionDlg m_ScanFashionDlg;					//ɨ�跽ʽ���öԻ���
	// ==============ɨ�����ʾ��ͼ��Ϣ========= //
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

// ��������ʾ�����и����Ӳ��ֵ�λ��
private:
	CSize			m_ClientSize;       	// ��������ʾ����Ĵ�С
	CSize			m_ClientMinSize;       	// ��̬�ı�����С�ǣ���������ʾ�������Сֵ
	CRect			m_ImageBmp_Rect;        // ���ڽ��в�������Ұͼ�����������е�λ��
	CRect			m_SlideScanBmp_Rect;    // ɨ��ʾ��ͼ���������е�λ��
	CRect			m_StaticPrompt_Rect;    // ������ʾ��̬�ؼ����������е�λ��
	CRect			m_EditcPrompt_Rect;     // ������ʾEdit�ؼ����������е�λ��
	CRect			m_SlideIndex_Rect;      // ��ǰ��λ������������е�λ��	
	CRect			m_SlideID_Rect;         // ��ǰ��Ƭ������������е�λ��	

private: 
	CSlideInfoDlg   m_SlideInfoDlg;						//��Ƭ��Ϣ����Ի���
	SLIDEINFO		m_CurSlideInfo;

public:
	CJudgeDlg*      m_pJudgeDlg;

// GUI״̬
private:
	CString			m_OperationPrompt;   // ��ʾϵͳ������ʾ��Ϣ
	BOOL			m_AutoScanBox;
	DWORD			m_GUIStatus;
	MENUITEMSTATUS  m_MenuItemStatus;
	
// �ڲ���������
private:
	BOOL            m_DeviceOK;

// �ڲ�����
private:
	void  PhPtoScanBmpLP();   // ��ɨ���������˽ṹ�е���Ұ����������ת������ɨ�����ʾ��ͼ�е��߼�����
	void  addScanRoute2Bmp(); // ��ɨ��·���ӵ�ɨ�����ʾ��ͼ��

public:	
	CPoint XYPhPtoScanBmpLP(cv::Point2f XYpos); // ��XYƽ̨�ϵľ������꣨unit��um��ת������ɨ�����ʾ��ͼ�е��߼�����
	void  AddPoint2SlideScanBmp(cv::Point2f XYpos); // ����XYƽ̨�Ͼ��������ʾ�ĵ���ɨ�����ʾ��ͼ�б�ǳɺ�ɫ
	void  DisableALLGUI();					// ��ֹ���еĽ�����Ӧ�ӿڣ��������˵�����ť
	void  EnableGUI(DWORD dMask);			// ʹ�ܽ�����Ӧ�ӿڣ��������˵�����ť
	void  DisableGUI(DWORD dMask);			// ��ֹ������Ӧ�ӿڣ��������˵�����ť
	
	void  UpdateScanRouteBmp(); // ����ɨ�����ʾ��ͼ�е�ɨ��·��
// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnInitialUpdate(); // ������һ�ε���
// ʵ��
public:
	virtual ~CBacilusDetectView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // BacilusDetectView.cpp �еĵ��԰汾
inline CBacilusDetectDoc* CBacilusDetectView::GetDocument() const
   { return reinterpret_cast<CBacilusDetectDoc*>(m_pDocument); }
#endif

