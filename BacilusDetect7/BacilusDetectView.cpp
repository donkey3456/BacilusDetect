
// BacilusDetectView.cpp : CBacilusDetectView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "BacilusDetect.h"
#endif

#include "BacilusDetectView.h"
#include <direct.h>
//#include "ProSingleImage.h"
#include "Mechanism.h"
//#include "zypUtils.h"
#include "AutoAdjustLight.h"
#include "ProSingleImage.h"
#include "AutoFocus.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 全局外部设备
CMechanism		g_Mechanism;


// 图像处理线程参数结构
typedef struct tagImageProcessWorkTreadParam
{
	int							iScanCount;
	volatile BOOL *				pbImageProcessEnd;
	CString						strFileDirectory;
	CBacilusDetectDoc*			pDocument;
	int *						pnImageProcessSequenceIndex;
	queue <STRUCTIMAGEINFO> *	pqImageInfo;
	HWND						hWnd;
	UINT						uMsg;
	HANDLE						hHandleStop;
} StructImageProcessWorkTreadParam;

// CBacilusDetectView

IMPLEMENT_DYNCREATE(CBacilusDetectView, CFormView)

BEGIN_MESSAGE_MAP(CBacilusDetectView, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_CONTROL_SCAN, &CBacilusDetectView::OnControlScan)
	ON_MESSAGE(MSG_SCANROUTE_NOTIFY, &CBacilusDetectView::OnMsgScanrouteNotify)
	ON_COMMAND(ID_CONTROL_MICROSCOPE, &CBacilusDetectView::OnControlMicroscope)
	ON_BN_CLICKED(IDC_BUTTON_START, &CBacilusDetectView::OnBnClickedButtonStart)
	ON_MESSAGE(MSG_CAMERA_NOTIFY, &CBacilusDetectView::OnMsgCameraNotify)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CBacilusDetectView::OnBnClickedButtonExit)
	ON_UPDATE_COMMAND_UI(ID_CONTROL_MICROSCOPE, &CBacilusDetectView::OnUpdateControlMicroscope)
// 	ON_BN_CLICKED(IDC_BUTTON_NEXTSLIDE, &CBacilusDetectView::OnBnClickedButtonNextslide)
	ON_COMMAND(ID_CONTROL_ORIGIN, &CBacilusDetectView::OnControlOrigin)
	ON_UPDATE_COMMAND_UI(ID_CONTROL_ORIGIN, &CBacilusDetectView::OnUpdateControlOrigin)
	ON_BN_CLICKED(IDC_BUTTON_CHANGEBOX, &CBacilusDetectView::OnBnClickedButtonChangebox)
// 	ON_COMMAND(ID_CONTROL_SELECT1, &CBacilusDetectView::OnControlSelect1)
// 	ON_UPDATE_COMMAND_UI(ID_CONTROL_SELECT1, &CBacilusDetectView::OnUpdateControlSelect1)
// 	ON_COMMAND(ID_CONTROL_SELECT2, &CBacilusDetectView::OnControlSelect2)
// 	ON_COMMAND(ID_CONTROL_SELECT3, &CBacilusDetectView::OnControlSelect3)
// 	ON_UPDATE_COMMAND_UI(ID_CONTROL_SELECT3, &CBacilusDetectView::OnUpdateControlSelect3)
// 	ON_COMMAND(ID_CONTROL_SELECT4, &CBacilusDetectView::OnControlSelect4)
// 	ON_UPDATE_COMMAND_UI(ID_CONTROL_SELECT4, &CBacilusDetectView::OnUpdateControlSelect4)
// 	ON_COMMAND(ID_CONTROL_SELECT5, &CBacilusDetectView::OnControlSelect5)
// 	ON_UPDATE_COMMAND_UI(ID_CONTROL_SELECT5, &CBacilusDetectView::OnUpdateControlSelect5)
//	ON_UPDATE_COMMAND_UI(ID_CONTROL_SELECT2, &CBacilusDetectView::OnUpdateControlSelect2)
	ON_BN_CLICKED(IDC_BUTTON_SLIDEINFO, &CBacilusDetectView::OnBnClickedButtonSlideinfo)
	ON_BN_CLICKED(IDC_BUTTON_SCAN, &CBacilusDetectView::OnBnClickedButtonScan)
	ON_BN_CLICKED(IDC_BUTTON_SCANSTOP, &CBacilusDetectView::OnBnClickedButtonScanstop)
	ON_UPDATE_COMMAND_UI(ID_CONTROL_SCAN, &CBacilusDetectView::OnUpdateControlScan)
	ON_MESSAGE(MSG_SCANTHREAD_NOTIFY, &CBacilusDetectView::OnMsgScanthreadNotify)
	ON_BN_CLICKED(IDC_BUTTON_JUDGE, &CBacilusDetectView::OnBnClickedButtonJudge)
	ON_MESSAGE(MSG_JUDGEDLGRETURN_NOTIFY, &CBacilusDetectView::OnMsgJudgedlgreturnNotify)
	ON_MESSAGE(MSG_JUDGEDLGOBSERVE_NOTIFY, &CBacilusDetectView::OnMsgJudgedlgobserveNotify)
// 	ON_BN_CLICKED(IDC_BUTTON_AUTOSCAN, &CBacilusDetectView::OnBnClickedButtonAutoscan)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CBacilusDetectView 构造/析构

CBacilusDetectView::CBacilusDetectView()
	: CFormView(CBacilusDetectView::IDD)
	, m_ImageDate_PixelCount(CAMERA_IMAGE_WIDTH * CAMERA_IMAGE_HEIGHT)
	, m_ImageDate_byteCount(CAMERA_IMAGE_WIDTH * CAMERA_IMAGE_HEIGHT * 4)
	, m_SlideScanBmp_Width(266)
	, m_SlideScanBmp_Height(349)
	, m_SlideScanBmp_byteCount(m_SlideScanBmp_Width*m_SlideScanBmp_Height*4)
	, m_SlideScanBmp_Origin(m_SlideScanBmp_Width-1, 322)
	, m_SlideIndexInSlideScanBmp(164, 6)
	, m_SlideIDInSlideScanBmp(80,27)
	, m_SlideScanBmp_Display_Ratio(25000.0/m_SlideScanBmp_Width)
	, m_OperationPrompt(_T(""))
{
	// 菜单使能初始化
	m_MenuItemStatus.Menu_ControlScan = TRUE;
	m_MenuItemStatus.Menu_ControlSelect = FALSE;
	m_MenuItemStatus.Menu_ControlOrigin = FALSE;
	m_MenuItemStatus.Menu_ControlMicroscope = FALSE;
	m_MenuItemStatus.Menu_ControlCamera = FALSE;
	m_MenuItemStatus.Menu_ViewBacilus = FALSE;

	m_ClientMinSize.cx = 0;
	m_ClientMinSize.cy = 0;

	// 初始化在主界面中扫描示意图
	g_Mechanism.GetScanRoute(m_ScanRoute);	
	UpdateScanRouteBmp(); // 更新扫描过程示意图中的扫描路径
	
	m_DefaultDirectory = _T("D:\\BacilusDetectTemp");   // 临时文件目录

	m_DeviceOK = FALSE;

	m_ScanSequenceIndex = 0;

	m_pAutoScanWorkThread = NULL;

	m_pJudgeDlg = new CJudgeDlg(this);

	m_GUIStatus = 0;
	m_AutoScanBox = FALSE;
	m_nImageProcessSequenceIndex = -1;
	m_bScanEnd = FALSE;
	m_bImageProcess = FALSE;
	m_bImageProcessEnd = FALSE;
	m_bImageProcessThread = FALSE;
	m_bScanPostProcess = FALSE;
}

CBacilusDetectView::~CBacilusDetectView()
{
	delete m_pJudgeDlg;
}

void CBacilusDetectView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PROMPT, m_OperationPrompt);
}

BOOL CBacilusDetectView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CFormView::PreCreateWindow(cs);
}

void CBacilusDetectView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();

	// 设置主界面各个部分的位置和大小
	CRect rectButton;
	this->GetDlgItem(IDC_BUTTON_START)->GetWindowRect(&rectButton);
	CRect rectClient;
	this->GetWindowRect(&rectClient);

	int delta = rectButton.top - rectClient.top;
	_ASSERTE(delta>0);
	int y = rectButton.bottom  - rectClient.top;
	_ASSERTE(y>0);

	int x = delta;
	y += delta;
	m_ImageBmp_Rect.SetRect( x,                               y, 
					   (int)(x+CAMERA_IMAGE_WIDTH*0.5), (int)(y+CAMERA_IMAGE_HEIGHT*0.5) ); // 正在处理的图像在主界面中的位置

	x = m_ImageBmp_Rect.Width() + 2 * delta;
	m_SlideScanBmp_Rect.SetRect( x,						 y, 
								 x+m_SlideScanBmp_Width, y+m_SlideScanBmp_Height );         // 扫描示意图在主界面中的位置

	m_StaticPrompt_Rect.left = m_SlideScanBmp_Rect.left;                                                
	m_StaticPrompt_Rect.right = m_SlideScanBmp_Rect.right;
	m_StaticPrompt_Rect.top = m_SlideScanBmp_Rect.bottom + 3 * delta;
	m_StaticPrompt_Rect.bottom = m_ImageBmp_Rect.bottom;
	this->GetDlgItem(IDC_STATIC_PROMPT)->MoveWindow(&m_StaticPrompt_Rect);					// 操作提示静态控件在主界面中的位置

	m_EditcPrompt_Rect.left = m_StaticPrompt_Rect.left + 3;												
	m_EditcPrompt_Rect.right = m_StaticPrompt_Rect.right - 3;
	m_EditcPrompt_Rect.top = m_StaticPrompt_Rect.top + 4 * delta;
	m_EditcPrompt_Rect.bottom = m_StaticPrompt_Rect.bottom - 3;
	this->GetDlgItem(IDC_EDIT_PROMPT)->MoveWindow(&m_EditcPrompt_Rect);						// 操作提示Edit控件在主界面中的位置
			
	x = m_SlideScanBmp_Rect.left + m_SlideIndexInSlideScanBmp.x;
	y = m_SlideScanBmp_Rect.top + m_SlideIndexInSlideScanBmp.y;
	m_SlideIndex_Rect.SetRect( x, y, x+17, y+17 );											// 当前工位编号在主界面中的位置	

	x = m_SlideScanBmp_Rect.left + m_SlideIDInSlideScanBmp.x;
	y = m_SlideScanBmp_Rect.top + m_SlideIDInSlideScanBmp.y;
	m_SlideID_Rect.SetRect(x, y, m_SlideScanBmp_Rect.right, y+23 );							// 当前玻片编号在主界面中的位置	
	
	// 设置主界面工作区域的大小
	m_ClientSize.cx = m_ImageBmp_Rect.Width() + m_SlideScanBmp_Rect.Width() + 3 * delta;
	m_ClientSize.cy = m_ImageBmp_Rect.bottom + delta;
	SetScrollSizes(MM_TEXT, m_ClientSize);   
	ResizeParentToFit(FALSE);
	m_ClientMinSize = m_ClientSize;

	// 初始化界面交互响应接口
	EnableGUI(GUI_MENU_SCANMODE | GUI_BUTTON_START | GUI_BUTTON_EXIT);	
	m_OperationPrompt = _T("1. 系统成功初始化之后，才能继续进行扫描检验操作\r\n\r\n2. 可以通过\"系统控制-->扫描方式设置\"菜单对默认的扫描方式进行修改");
	this->UpdateData(FALSE);	
}

void CBacilusDetectView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CBacilusDetectView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

//============================================================================================//
// CBacilusDetectView 诊断

#ifdef _DEBUG
void CBacilusDetectView::AssertValid() const
{
	CFormView::AssertValid();
}

void CBacilusDetectView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CBacilusDetectDoc* CBacilusDetectView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBacilusDetectDoc)));
	return (CBacilusDetectDoc*)m_pDocument;
}
#endif //_DEBUG

//============================================================================================//

// 内部操作
// 禁止所有的界面响应接口，包括：菜单、按钮
void  CBacilusDetectView::DisableALLGUI()       
{
	// 禁止相关的按钮
	(CButton*) ( this->GetDlgItem(IDC_BUTTON_START) )->EnableWindow(FALSE);
	(CButton*) ( this->GetDlgItem(IDC_BUTTON_SLIDEINFO))->EnableWindow(FALSE);
	(CButton*) ( this->GetDlgItem(IDC_BUTTON_SCAN))->EnableWindow(FALSE);
	(CButton*) ( this->GetDlgItem(IDC_BUTTON_SCANSTOP) )->EnableWindow(FALSE);
	(CButton*) ( this->GetDlgItem(IDC_BUTTON_JUDGE) )->EnableWindow(FALSE);
	(CButton*) ( this->GetDlgItem(IDC_BUTTON_CHANGEBOX) )->EnableWindow(FALSE);
	(CButton*) ( this->GetDlgItem(IDC_BUTTON_EXIT) )->EnableWindow(FALSE);
	
	// 禁止相关的菜单
	m_MenuItemStatus.Menu_ControlScan = FALSE;
	m_MenuItemStatus.Menu_ControlSelect = FALSE;
	m_MenuItemStatus.Menu_ControlOrigin = FALSE;
	m_MenuItemStatus.Menu_ControlMicroscope = FALSE;
	m_MenuItemStatus.Menu_ControlCamera = FALSE;
	m_MenuItemStatus.Menu_ViewBacilus = FALSE;	
}

// 使能界面响应接口，包括：菜单、按钮
void  CBacilusDetectView::EnableGUI(DWORD dMask)
{
	int i;
	for (i=0; i<32; i++)
	{
		if ( dMask & (1<<i) )
		{
			m_GUIStatus |= (1<<i);
		}
	}
	
	if ( m_GUIStatus & GUI_MENU_SCANMODE )			{	m_MenuItemStatus.Menu_ControlScan = TRUE; }
	if ( m_GUIStatus & GUI_MENU_MICROSCOPECONTROL )	{	m_MenuItemStatus.Menu_ControlMicroscope = TRUE; }
	if ( m_GUIStatus & GUI_MENU_ORIGIN )			{	m_MenuItemStatus.Menu_ControlOrigin = TRUE; }
	if ( m_GUIStatus & GUI_MENU_SELECT )			{	m_MenuItemStatus.Menu_ControlSelect = TRUE; }
	if ( m_GUIStatus & GUI_MENU_CAMERA )			{	m_MenuItemStatus.Menu_ControlCamera = TRUE; }
	if ( m_GUIStatus & GUI_MENU_VIEWBACILUS )		{	m_MenuItemStatus.Menu_ViewBacilus = TRUE; }

	if ( m_GUIStatus & GUI_BUTTON_START )		{	(CButton*) ( this->GetDlgItem(IDC_BUTTON_START) )->EnableWindow(TRUE); }
	if ( m_GUIStatus & GUI_BUTTON_SLIDEINFO )	{	(CButton*) ( this->GetDlgItem(IDC_BUTTON_SLIDEINFO) )->EnableWindow(TRUE); }
	if ( m_GUIStatus & GUI_BUTTON_SCAN )		{	(CButton*) ( this->GetDlgItem(IDC_BUTTON_SCAN) )->EnableWindow(TRUE); }
// 	if ( m_GUIStatus & GUI_BUTTON_AUTOSCAN )	{	(CButton*) ( this->GetDlgItem(IDC_BUTTON_AUTOSCAN) )->EnableWindow(TRUE); }
	if ( m_GUIStatus & GUI_BUTTON_STOPSCAN )	{	(CButton*) ( this->GetDlgItem(IDC_BUTTON_SCANSTOP) )->EnableWindow(TRUE); }
	if ( m_GUIStatus & GUI_BUTTON_JUDGE )		{	(CButton*) ( this->GetDlgItem(IDC_BUTTON_JUDGE) )->EnableWindow(TRUE); }
// 	if ( m_GUIStatus & GUI_BUTTON_NEXTSLIDE )	{	(CButton*) ( this->GetDlgItem(IDC_BUTTON_NEXTSLIDE) )->EnableWindow(TRUE); }
	if ( m_GUIStatus & GUI_BUTTON_CHANGEBOX )	{	(CButton*) ( this->GetDlgItem(IDC_BUTTON_CHANGEBOX) )->EnableWindow(TRUE); }
	if ( m_GUIStatus & GUI_BUTTON_EXIT )		{	(CButton*) ( this->GetDlgItem(IDC_BUTTON_EXIT) )->EnableWindow(TRUE); }
}

// 禁止界面响应接口，包括：菜单、按钮
void  CBacilusDetectView::DisableGUI(DWORD dMask)
{
	int i;
	for (i=0; i<32; i++)
	{
		if ( dMask & (1<<i) )
		{
			m_GUIStatus &= ~(1<<i);
		}
	}	

	if ( (m_GUIStatus & GUI_MENU_SCANMODE) == 0 )			{	m_MenuItemStatus.Menu_ControlScan = FALSE; }
	if ( (m_GUIStatus & GUI_MENU_MICROSCOPECONTROL) == 0 )	{	m_MenuItemStatus.Menu_ControlMicroscope = FALSE; }
	if ( (m_GUIStatus & GUI_MENU_ORIGIN) == 0 )				{	m_MenuItemStatus.Menu_ControlOrigin = FALSE; }
	if ( (m_GUIStatus & GUI_MENU_SELECT) == 0 )				{	m_MenuItemStatus.Menu_ControlSelect = FALSE; }
	if ( (m_GUIStatus & GUI_MENU_CAMERA) == 0 )				{	m_MenuItemStatus.Menu_ControlCamera = FALSE; }
	if ( (m_GUIStatus & GUI_MENU_VIEWBACILUS) == 0 )		{	m_MenuItemStatus.Menu_ViewBacilus = FALSE; }

	if ( (m_GUIStatus & GUI_BUTTON_START) == 0 )		{	(CButton*) ( this->GetDlgItem(IDC_BUTTON_START) )->EnableWindow(FALSE); }
	if ( (m_GUIStatus & GUI_BUTTON_SLIDEINFO) == 0 )	{	(CButton*) ( this->GetDlgItem(IDC_BUTTON_SLIDEINFO) )->EnableWindow(FALSE); }
	if ( (m_GUIStatus & GUI_BUTTON_SCAN) == 0 )			{	(CButton*) ( this->GetDlgItem(IDC_BUTTON_SCAN) )->EnableWindow(FALSE); }
	if ( (m_GUIStatus & GUI_BUTTON_STOPSCAN) == 0 )		{	(CButton*) ( this->GetDlgItem(IDC_BUTTON_SCANSTOP) )->EnableWindow(FALSE); }
	if ( (m_GUIStatus & GUI_BUTTON_JUDGE) == 0 )		{	(CButton*) ( this->GetDlgItem(IDC_BUTTON_JUDGE) )->EnableWindow(FALSE); }
	if ( (m_GUIStatus & GUI_BUTTON_CHANGEBOX) == 0 )	{	(CButton*) ( this->GetDlgItem(IDC_BUTTON_CHANGEBOX) )->EnableWindow(FALSE); }
	if ( (m_GUIStatus & GUI_BUTTON_EXIT) == 0 )			{	(CButton*) ( this->GetDlgItem(IDC_BUTTON_EXIT) )->EnableWindow(FALSE); }
}

// 将扫描区域拓扑结构中的视野的物理坐标转换成在扫描过程示意图中的逻辑坐标
void  CBacilusDetectView::PhPtoScanBmpLP()	
{	
	int k;
	CPoint view_position_in_SlideBmp;
	cv::Point2f viewCoordinate;

	m_Topology_in_SlideScanBmp.SetSize(m_ScanRoute.ScanTopology.GetSize());

	for ( k=0; k<m_ScanRoute.ScanTopology.GetSize(); k++ )
	{
		viewCoordinate = m_ScanRoute.ScanTopology[k];
		view_position_in_SlideBmp.x = LONG( viewCoordinate.x / m_SlideScanBmp_Display_Ratio );
		view_position_in_SlideBmp.y = LONG( viewCoordinate.y / m_SlideScanBmp_Display_Ratio );
		view_position_in_SlideBmp = m_SlideScanBmp_Origin - view_position_in_SlideBmp;	
		m_Topology_in_SlideScanBmp[k] = view_position_in_SlideBmp;
	}
}

// 将扫描路径加到扫描过程示意图中
void  CBacilusDetectView::addScanRoute2Bmp()
{	
	int i,j,k;
	
	m_SlideScanBmp.DeleteObject();
	if( ! m_SlideScanBmp.LoadBitmapW(IDB_SLIDE_SCAN) )
	{
		AfxMessageBox(_T("不能加载扫描路径示意图的位图资源"));
		return;
	}
	LPCOLORREF pBmpData = new COLORREF[m_SlideScanBmp_Width*m_SlideScanBmp_Height];
	m_SlideScanBmp.GetBitmapBits(m_SlideScanBmp_byteCount, (LPVOID)pBmpData );  //获得图像数据

	// 修改图像数据
	for ( k=0; k<m_Topology_in_SlideScanBmp.GetSize(); k++ )
	{
		// set black color as the representation of the views' position in the bitmap 
		for ( i=0; i<3; i++ )
		{
			for ( j=0; j<2; j++ )
			{
				pBmpData[(m_Topology_in_SlideScanBmp[k].y+j) * m_SlideScanBmp_Width + m_Topology_in_SlideScanBmp[k].x + i]
				        = RGB(128,128,128);
			}
		}
	}
	m_SlideScanBmp.SetBitmapBits(m_SlideScanBmp_byteCount, (const void*)pBmpData );  //设置要显示的图像

	delete [] pBmpData;
}

// 更新扫描过程示意图中的扫描路径
void  CBacilusDetectView::UpdateScanRouteBmp()
{
	PhPtoScanBmpLP();
	addScanRoute2Bmp();
}
// 
// // 运动到指定的工位
// void  CBacilusDetectView::GotoWorkPosition(int index)
// {
// 	CCamera* pCam = CCamera::GetInstance();
// 	pCam->EnableNotifyMsg(FALSE);  // 禁摄像机止输出图像
// 	
// 	g_Mechanism.GotoOrigin();
// 
// 	this->InvalidateRect(&m_SlideIndex_Rect, 0); // 工位号
// 	this->InvalidateRect(&m_SlideID_Rect, 0);    // 玻片编号
// 
// 	UpdateScanRouteBmp(); // 更新扫描过程示意图中的扫描路径
// 	this->InvalidateRect(&m_SlideScanBmp_Rect, 0);
// 	pCam->EnableNotifyMsg(TRUE);  // 允许摄像机输出图像
// }

// 显微镜平台运动到指定的玻片工位
// void  CBacilusDetectView::WorkPositionSelect(int index)
// {
// 	CCamera* pCam = CCamera::GetInstance();
// 	pCam->EnableNotifyMsg(FALSE);  // 禁摄像机止输出图像
// 	DisableALLGUI();// 禁止所有的界面响应接口
// 
// 	GotoWorkPosition(index);		  //微镜平台运动到当前玻片开始进行扫描时候的第一个视野的位置
// 		
// 	EnableGUI(0); // 恢复界面接口响应状态
// 
// 	if ( !GetDocument()->GetSlideID( g_Mechanism.GetWorkPositionID() ).IsEmpty() )
// 	{
// 		EnableGUI(GUI_BUTTON_SCAN|GUI_BUTTON_AUTOSCAN);
// 		m_OperationPrompt = _T("1. 可以通过\"系统控制-->扫描方式设置\"菜单对默认的扫描方式进行修改\r\n\r\n2. 点击\"开始检验\"按钮，系统将会对当前工位上的玻片进行扫描\r\n\r\n3.点击\"全自动检验\"按钮，系统将会对玻片盒中相应工位上的玻片进行扫描");
// 		this->UpdateData(FALSE);
// 	}
// 	else
// 	{
// 		DisableGUI(GUI_BUTTON_SCAN);
// 		m_OperationPrompt = _T("当前工位上的玻片编号为空，不能进行扫描检验，请通过\"玻片信息\"按钮输入相关信息或者更换工位对其它玻片进行扫描检验");
// 		if ( m_GUIStatus & GUI_BUTTON_AUTOSCAN )
// 		{
// 			m_OperationPrompt = _T("1. 当前工位上的玻片编号为空，不能进行扫描检验，请通过\"玻片信息\"按钮输入相关信息或者更换工位对其它玻片进行扫描检验\r\n\r\n2. 点击\"全自动检验\"按钮，系统将会对玻片盒中相应工位上的玻片进行扫描");
// 		}
// 		this->UpdateData(FALSE);
// 	}
// 	pCam->EnableNotifyMsg(TRUE);  // 允许摄像机输出图像
// }

// 将XY平台上的绝对坐标（unit：um）转换成在扫描过程示意图中的逻辑坐标
CPoint   CBacilusDetectView::XYPhPtoScanBmpLP(cv::Point2f XYpos)	
{	
	CPoint position_in_SlideBmp(0,0);

	// 计算当前物理坐标在当前工位上的玻片坐标系上的相对坐标
	cv::Point2f slidepos = g_Mechanism.GetRelativeXY(XYpos);   
		
	//计算扫描过程示意图中的逻辑坐标
	position_in_SlideBmp.x = LONG( slidepos.x / m_SlideScanBmp_Display_Ratio );
	position_in_SlideBmp.y = LONG( slidepos.y / m_SlideScanBmp_Display_Ratio );
	position_in_SlideBmp = m_SlideScanBmp_Origin - position_in_SlideBmp;	  

	return position_in_SlideBmp;
}

// 将由XY平台上绝对坐标表示的点在扫描过程示意图中标记成红色
void  CBacilusDetectView::AddPoint2SlideScanBmp(cv::Point2f XYpos)
{
	CPoint point = XYPhPtoScanBmpLP(XYpos);
	int i,j;
	LPCOLORREF pBmpData = new COLORREF[m_SlideScanBmp_Width*m_SlideScanBmp_Height];

	m_SlideScanBmp.GetBitmapBits(m_SlideScanBmp_byteCount, (LPVOID)pBmpData );  //获得图像数据
	// 修改图像数据
	for ( i=0; i<3; i++ )
	{
		for ( j=0; j<2; j++ )
		{
			pBmpData[(point.y+j) * m_SlideScanBmp_Width + (point.x + i)] = RGB(255,0,0); 
		}
	}
	m_SlideScanBmp.SetBitmapBits(m_SlideScanBmp_byteCount, (const void*)pBmpData );  //设置要显示的图像

	delete [] pBmpData;
}

//============================================================================================//

// CBacilusDetectView 消息处理程序
void CBacilusDetectView::OnDraw(CDC* pDC)
{
	CBacilusDetectDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDC->SetStretchBltMode(COLORONCOLOR);

	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	CBitmap * pOldBmp = NULL;

	// 显示正在进行操作的视野图像
	CBitmap	 BmpImage;                     //主界面中正在显示的图像	
	CCamera* pCam = CCamera::GetInstance();
	if(pCam->isCapturing())
	{
		cv::Mat   matImageTemp = pCam->GetImage();
		BmpImage.CreateBitmap(matImageTemp.cols, matImageTemp.rows, 1, 32, NULL);		
		cv::Mat matImage4C = cv::Mat(matImageTemp.rows,matImageTemp.cols,CV_8UC4);
		cv::cvtColor(matImageTemp, matImage4C, CV_BGR2BGRA);
		BmpImage.SetBitmapBits(matImage4C.total() * matImage4C.elemSize(), (const void*)matImage4C.ptr() );  //设置要显示视野的图像
	}
	else
	{			
		BmpImage.LoadBitmapW(IDB_MICROSCOPE_IMAGE);  // 初始化在主界面中正在处理的图像
	}
	pOldBmp = MemDC.SelectObject(&BmpImage);
	pDC->StretchBlt( m_ImageBmp_Rect.left, m_ImageBmp_Rect.top,         // Display Destination
		             m_ImageBmp_Rect.Width(), m_ImageBmp_Rect.Height(), 
		             &MemDC, 0, 0, CAMERA_IMAGE_WIDTH, CAMERA_IMAGE_HEIGHT, // Source
					 SRCCOPY );

	// 显示扫描过程示意图
	MemDC.SelectObject(&m_SlideScanBmp);
	pDC->StretchBlt( m_SlideScanBmp_Rect.left, m_SlideScanBmp_Rect.top,         // Display Destination
		             m_SlideScanBmp_Rect.Width(), m_SlideScanBmp_Rect.Height(), 
		             &MemDC, 0, 0, m_SlideScanBmp_Width, m_SlideScanBmp_Height, // Source
					 SRCCOPY );
	MemDC.SelectObject(&pOldBmp);

// 	// 显示工位号
// 	CString			strWorkPositionNo;		
// 	strWorkPositionNo.Format( _T("%d"), g_Mechanism.GetWorkPositionID() + 1 );
// 	pDC->TextOutW(m_SlideIndex_Rect.left, m_SlideIndex_Rect.top, strWorkPositionNo);// 扫描示意图中的工位号

	// 显示玻片编号
	pDC->TextOutW(m_SlideID_Rect.left, m_SlideID_Rect.top, GetDocument()->GetSlideID( ));
}

void CBacilusDetectView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	
	// OnSize can be called before OnInitialUpdate
	// so make sure the view window has been created.
	if (m_ClientMinSize.cx > 0)
	{
		if ( (cx < m_ClientMinSize.cx) || (cy < m_ClientMinSize.cy) )
		{

		}
		else
		{
	//		float ratioX = (float)cx / (float)m_ClientSize.cx;
	//		float ratioY = (float)cy / (float)m_ClientSize.cy;
	//		float ratio = ( ratioX < ratioY ) ? ratioX  : ratioY ;
	//		m_ClientSize.cx = (int) (ratio * m_ClientSize.cx);
	//		m_ClientSize.cy = (int) (ratio * m_ClientSize.cy);
	//		SetScrollSizes(MM_TEXT, m_ClientSize);   
	//		ResizeParentToFit(FALSE);
	//		
	//		m_ImageBmp_Rect.left = (int) (ratio * m_ImageBmp_Rect.left);   
	//		m_ImageBmp_Rect.right = (int) (ratio * m_ImageBmp_Rect.right);
	//		m_ImageBmp_Rect.top = (int) (ratio * m_ImageBmp_Rect.top);
	//		m_ImageBmp_Rect.bottom = (int) (ratio * m_ImageBmp_Rect.bottom);

	//		m_SlideScanBmp_Rect.left = (int) (ratio * m_SlideScanBmp_Rect.left);   
	//		m_SlideScanBmp_Rect.right = (int) (ratio * m_SlideScanBmp_Rect.right);
	//		m_SlideScanBmp_Rect.top = (int) (ratio * m_SlideScanBmp_Rect.top);
	//		m_SlideScanBmp_Rect.bottom = (int) (ratio * m_SlideScanBmp_Rect.bottom);
	//		
	//		int delta = m_EditcPrompt_Rect.top - m_StaticPrompt_Rect.top;

	//		m_StaticPrompt_Rect.left = (int) (ratio * m_StaticPrompt_Rect.left);                                                
	//		m_StaticPrompt_Rect.right = (int) (ratio * m_StaticPrompt_Rect.right); 
	//		m_StaticPrompt_Rect.top = (int) (ratio * m_StaticPrompt_Rect.top); 
	//		m_StaticPrompt_Rect.bottom = (int) (ratio * m_StaticPrompt_Rect.bottom); 
	//		this->GetDlgItem(IDC_STATIC_PROMPT)->MoveWindow(&m_StaticPrompt_Rect);	// 操作提示静态控件在主界面中的位置

	//		m_EditcPrompt_Rect.left = m_StaticPrompt_Rect.left + 3;												
	//		m_EditcPrompt_Rect.right = m_StaticPrompt_Rect.right - 3;
	//		m_EditcPrompt_Rect.top = m_StaticPrompt_Rect.top + delta;
	//		m_EditcPrompt_Rect.bottom = m_StaticPrompt_Rect.bottom - 3;
	//		this->GetDlgItem(IDC_EDIT_PROMPT)->MoveWindow(&m_EditcPrompt_Rect);		// 操作提示Edit控件在主界面中的位置
		
			SetScrollSizes(MM_TEXT, m_ClientMinSize);   
			ResizeParentToFit(FALSE);
		}
	}	
}

// 处理扫描方式设置对话框发出的扫描路径更新消息
afx_msg LRESULT CBacilusDetectView::OnMsgScanrouteNotify(WPARAM wParam, LPARAM lParam)
{
	CCamera* pCam = CCamera::GetInstance();
	CMotorController* pMC = CMotorController::GetInstance();
	g_Mechanism.GetScanRoute(m_ScanRoute);	// 获得扫描路径
	UpdateScanRouteBmp(); // 更新扫描过程示意图中的扫描路径
	this->InvalidateRect(&m_SlideScanBmp_Rect, 0);
	switch (wParam)
	{
	case 0:       // Apply button is pressed
		if ( pMC->isConnecting() )
		{
			pCam->EnableNotifyMsg(FALSE);  // 禁摄像机止输出图像

			g_Mechanism.GotoOrigin();		  // 回到当前工位的起始点

			pCam->EnableNotifyMsg(TRUE);   // 允许摄像机输出图像

		}
		break;

	case 1:       // OK button is pressed
		break;

	default:
		break;
	}
	
	return 0;
}

// 处理摄像机发出的图像更新消息
afx_msg LRESULT CBacilusDetectView::OnMsgCameraNotify(WPARAM wParam, LPARAM lParam)
{
	if ( CWnd::GetActiveWindow() == &m_MicroscopeControlDlg) // 如果显微镜控制对话框处于激活状态，则在其中显示该图像
	{
		m_MicroscopeControlDlg.UpdateImage();
	}
	else  //在主界面中显示该图像
	{
		this->InvalidateRect(&m_ImageBmp_Rect, 0);
	}

	return 0;
}

// 处理自动扫描检验线程发出的界面更新消息
afx_msg LRESULT CBacilusDetectView::OnMsgScanthreadNotify(WPARAM wParam, LPARAM lParam)
{
	BOOL bScan, bJudge;
	CString str;
	switch (wParam)
	{
	case AUTOSCAN_END:
		// 更新界面接口响应状态
		DisableGUI(GUI_BUTTON_STOPSCAN); 
		EnableGUI(	GUI_MENU_SCANMODE | GUI_MENU_MICROSCOPECONTROL | GUI_MENU_ORIGIN | GUI_MENU_SELECT
					| GUI_BUTTON_START | GUI_BUTTON_EXIT | GUI_BUTTON_SLIDEINFO | GUI_BUTTON_NEXTSLIDE | GUI_BUTTON_CHANGEBOX
					| GUI_BUTTON_SCAN	| GUI_BUTTON_AUTOSCAN | GUI_BUTTON_JUDGE ); 
		m_OperationPrompt = _T("扫描检验结束\r\n\r\n请点击\"辅助诊断\"按钮，对扫描的玻片进行确诊");
		this->UpdateData(FALSE);
		break;

// 	case AUTOSCAN_SLIDE_END:
// 		this->GetDocument()->GetScanJudgeStatus(bScan, bJudge);
// 		if ( bScan )
// 		{
// 			m_OperationPrompt.Format(_T("系统正在对玻片盒中相应的玻片进行扫描检验，请耐心等待\r\n\r\n已经完成工位 %d 上的玻片扫描工作"), lParam+1);
// 		}
// 		else
// 		{
// 			m_OperationPrompt.Format(_T("系统正在对玻片盒中相应的玻片进行扫描检验，请耐心等待\r\n\r\n不能完成工位 %d 上玻片的扫描检测任务"), lParam+1);
// 		}
// 		this->UpdateData(FALSE);
// 		break;

	case AUTOLIGHT:
		if ( m_AutoScanBox )
		{
			m_OperationPrompt.Format(_T("系统正在对玻片盒中相应的玻片进行扫描检验，请耐心等待\r\n\r\n正在进行自动调光操作"));
		}
		else
		{
			m_OperationPrompt.Format(_T("系统正在对玻片进行扫描检验，请耐心等待\r\n\r\n正在进行自动调光操作"));
		}
		this->UpdateData(FALSE);
		break;
		
	case AUTOSCAN_VIEW:
// 		if ( m_AutoScanBox )
// 		{
// 			m_OperationPrompt = _T("系统正在对玻片盒中相应的玻片进行扫描检验，请耐心等待");
// 			
// 		}
// 		else
// 		{
		m_OperationPrompt = _T("系统正在对玻片进行扫描检验，请耐心等待");		
/*		}*/	

		if ( lParam > 0  )			// 图像处理线程已经开始工作
		{
			m_bImageProcess = TRUE;
		}

		if ( m_bImageProcess )			// 图像处理线程已经开始工作
		{
			str.Format(_T("\r\n\r\n正在采集第 %d 个视野的图像\r\n\r\n正在对第 %d 个视野的图像进行检验"), m_ScanSequenceIndex+1, m_nImageProcessSequenceIndex+1);
			if ( m_bScanPostProcess )
			{
				str.Format(_T("\r\n\r\n正在对第 %d 个视野的图像进行重新采集\r\n\r\n正在对第 %d 个视野的图像进行检验"), m_ScanSequenceIndex+1, m_nImageProcessSequenceIndex+1);
			}

			if ( m_bScanEnd )
			{
				str.Format(_T("\r\n\r\n当前玻片上的视野图像采集完毕\r\n\r\n正在对第 %d 个视野的图像进行检验"), m_nImageProcessSequenceIndex+1);
			}
		}
		else
		{		
			str.Format(_T("\r\n\r\n正在采集第 %d 个视野的图像"), m_ScanSequenceIndex+1);
			if ( m_bScanPostProcess )
			{
				str.Format(_T("\r\n\r\n正在对第 %d 个视野的图像进行重新采集"), m_ScanSequenceIndex+1);
			}

			if ( m_bScanEnd )
			{
				str.Format(_T("\r\n\r\n当前玻片上的视野图像采集完毕"));
			}
		}

		if ( lParam == 0 )
		{
			this->InvalidateRect(&m_SlideScanBmp_Rect, 0);
		}
		m_OperationPrompt += str;
		this->UpdateData(FALSE);
		break;
	
	case AUTOSCAN_FOCUS_TERMINATE:
		if ( m_AutoScanBox )
		{
			m_OperationPrompt.Format(_T("系统正在对玻片盒中相应的玻片进行扫描检验，请耐心等待\r\n\r\n工位 %d 上的玻片自动聚焦失败，不能完成扫描检测任务"), lParam+1);
		}
		else
		{			
			AfxMessageBox(_T("自动聚焦失败，不能完成扫描检测任务"));
		}	
		DisableGUI(GUI_BUTTON_STOPSCAN); 
		EnableGUI(	GUI_MENU_SCANMODE | GUI_MENU_MICROSCOPECONTROL | GUI_MENU_ORIGIN | GUI_MENU_SELECT
					| GUI_BUTTON_START | GUI_BUTTON_EXIT | GUI_BUTTON_SLIDEINFO | GUI_BUTTON_NEXTSLIDE | GUI_BUTTON_CHANGEBOX
					| GUI_BUTTON_SCAN	| GUI_BUTTON_AUTOSCAN | GUI_BUTTON_JUDGE ); 
		this->UpdateData(FALSE);
		break;

	case AUTOSCAN_POSTPROCESS:
		if ( m_AutoScanBox )
		{
			m_OperationPrompt = _T("系统正在对玻片盒中相应的玻片进行扫描检验，请耐心等待\r\n\r\n当前工位上的玻片视野采集完毕\r\n\r\n正在进行后处理");
		}
		else
		{
			m_OperationPrompt = _T("系统正在对玻片进行扫描检验，请耐心等待\r\n\r\n玻片视野图像采集完毕\r\n\r\n正在进行后处理");
		}	
		this->UpdateData(FALSE);
		break;
	default:
		break;
	}

	return 0;
}

// 辅助诊断对话框退出时发出的消息
afx_msg LRESULT CBacilusDetectView::OnMsgJudgedlgreturnNotify(WPARAM wParam, LPARAM lParam)
{
	// 更新界面接口响应状态
	CCamera* pCam = CCamera::GetInstance();
	DisableGUI(GUI_BUTTON_STOPSCAN); 
	EnableGUI(	GUI_MENU_SCANMODE | GUI_MENU_MICROSCOPECONTROL | GUI_MENU_ORIGIN | GUI_MENU_SELECT
			  | GUI_BUTTON_START | GUI_BUTTON_EXIT | GUI_BUTTON_SLIDEINFO | GUI_BUTTON_CHANGEBOX
			 | GUI_BUTTON_JUDGE );  
	pCam->EnableNotifyMsg(TRUE);  // 允许摄像机输出图像
	m_OperationPrompt = _T("1. 可以通过\"下一张玻片\"按钮或者\"系统控制-->工位选择\"菜单改变玻片工位\r\n\r\n2. 可以通过\"置换玻片盒\"按钮更换玻片盒中的玻片,进行下一批次玻片的扫描检验工作");
	this->UpdateData(FALSE);
	return 0;
}

// 辅助诊断对话框发出观察当前视野的消息
afx_msg LRESULT CBacilusDetectView::OnMsgJudgedlgobserveNotify(WPARAM wParam, LPARAM lParam)
{
	CCamera* pCam = CCamera::GetInstance();
	CMotorController* pMC = CMotorController::GetInstance();
	int nWorkPosition = (int)wParam;
	int nFOVIndex = (int)lParam;
	cv::Point3f FOVPosition;
	FOVPosition = GetDocument()->GetFOVPosition( nFOVIndex );
	pMC->XYMove2(cv::Point2f(FOVPosition.x,FOVPosition.y), DISTANCE_UNIT_UM);
	pMC->WaitUntilDone();

// 	g_PCIControl.SetXMotorSpeed(100, DISTANCE_UNIT_UM);
// 	g_PCIControl.SetYMotorSpeed(100, DISTANCE_UNIT_UM);
	pMC->SetXMotorSpeed(1, DISTANCE_UNIT_MM);
	pMC->SetYMotorSpeed(1, DISTANCE_UNIT_MM);

	pMC->ZMove2(FOVPosition.z, DISTANCE_UNIT_UM);
	float fFPS;
	do
	{
		fFPS = pCam->getFPS();   // 摄像机帧频率
	} while (fFPS < 5);
	float fZMotorSpeed = fFPS;                 // Z轴步进电机连续运动的速度
	pMC->SetZMotorSpeed(fZMotorSpeed, DISTANCE_UNIT_UM);      // 设置Z轴电机运动速度

	DisableGUI(0xFFFFFFFF);  // 完全禁止菜单和按钮使能状态
	pCam->EnableNotifyMsg(TRUE);  // 允许摄像机输出图像
	m_MicroscopeControlDlg.DoModal();

	return 0;
}

//============================================================================================//

// Menu
// 产生扫描路径对话框
void CBacilusDetectView::OnControlScan()
{
	CCamera* pCam = CCamera::GetInstance();
	pCam->EnableNotifyMsg(FALSE);  // 禁摄像机止输出图像
	DisableALLGUI();// 禁止所有的界面响应接口
	m_ScanFashionDlg.DoModal();
	EnableGUI(0); // 恢复界面接口响应状态
	pCam->EnableNotifyMsg(TRUE);  // 允许摄像机输出图像
}

// 更新扫描方式设置菜单状态
void CBacilusDetectView::OnUpdateControlScan(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_MenuItemStatus.Menu_ControlScan);
}

// 产生显微镜控制对话框
void CBacilusDetectView::OnControlMicroscope()
{
	CCamera* pCam = CCamera::GetInstance();
	pCam->EnableNotifyMsg(TRUE);  // 允许摄像机输出图像
	DisableALLGUI();// 禁止所有的界面响应接口
	m_MicroscopeControlDlg.DoModal();
	Sleep(100);
	EnableGUI(0); // 恢复界面接口响应状态	
	this->InvalidateRect(&m_ImageBmp_Rect, 0); // 更新主界面中的图像
}

// 更新显微镜控制菜单状态
void CBacilusDetectView::OnUpdateControlMicroscope(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_MenuItemStatus.Menu_ControlMicroscope);
}

// 微镜平台运动到当前玻片开始进行扫描时候的第一个视野的位置
void CBacilusDetectView::OnControlOrigin()
{
	CCamera* pCam = CCamera::GetInstance();
	pCam->EnableNotifyMsg(FALSE);  // 禁摄像机止输出图像
	DisableALLGUI();// 禁止所有的界面响应接口

	g_Mechanism.GotoOrigin();		  // 回到当前工位的起始点
	
	pCam->EnableNotifyMsg(TRUE);  // 允许摄像机输出图像
	EnableGUI(0); // 恢复界面接口响应状态
}

// 更新回到扫描起始位置菜单状态
void CBacilusDetectView::OnUpdateControlOrigin(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_MenuItemStatus.Menu_ControlOrigin);
}


//============================================================================================//

// Button
// 系统初始化
void CBacilusDetectView::OnBnClickedButtonStart()
{
	DWORD  spantime = 0, temp = 0;
	DWORD  curtime = 0, starttime = 0;
	CCamera* pCam = CCamera::GetInstance();
	CMotorController* pMC = CMotorController::GetInstance();
	cv::Point3f	f3DStopPos = g_Mechanism.GetStopPosition();
	
	CButton* pBtn = NULL;
	int i=0;
	
	DisableGUI(0xFFFFFFFF);// 禁止所有的界面响应接口
	EnableGUI(GUI_MENU_SCANMODE); 

	HCURSOR hCur  =  AfxGetApp()->LoadStandardCursor( IDC_WAIT ) ;
	::SetCursor(hCur);


	if ( ! pMC->isConnecting() )
	{
		if (!pMC->Connect())
		{
			AfxMessageBox(_T("不能连接MCU系统"));
			EnableGUI(	GUI_BUTTON_START | GUI_BUTTON_EXIT ); 
			hCur  =  AfxGetApp()->LoadStandardCursor(  IDC_ARROW ) ;
			SetCursor(hCur);
			return;
		}
	}
	else
	{
		pMC->Restart();
	}


	if ( ! pCam->isCapturing() )
	{
		for ( i=0; i<3; i++)
		{
			if ( pCam->connect(this->GetSafeHwnd(),MSG_CAMERA_NOTIFY) )
			{
				pCam->EnableNotifyMsg(FALSE);  // 禁摄像机止输出图像
				break;
			}
			Sleep(1000);
		}
		if ( i>=3 )
		{
			AfxMessageBox(_T("不能连接摄像机"));
			EnableGUI(	GUI_BUTTON_START | GUI_BUTTON_EXIT ); 
			hCur  =  AfxGetApp()->LoadStandardCursor(  IDC_ARROW ) ;
			SetCursor(hCur);
			return;
		}
	}

	pMC->WaitUntilDone();

	m_OperationPrompt = _T("系统正在移动玻片至预定位置，请耐心等待");
	this->UpdateData(FALSE);

	if ( AfxMessageBox( _T("请将玻片放置在显微镜载物台上（注意：不要碰撞物镜），点击确定按钮系统继续运行"), 
			            MB_OKCANCEL|MB_ICONINFORMATION ) == IDOK )
	{
		
		// 初始化扫描路径 
		g_Mechanism.GetScanRoute(m_ScanRoute);
		UpdateScanRouteBmp(); // 更新扫描过程示意图中的扫描路径
		this->InvalidateRect(&m_SlideScanBmp_Rect, 0);

		m_ScanSequenceIndex = 0;           // 初始化扫描视野编号
				
		DeleteDirectory(m_DefaultDirectory);  // 删除临时文件目录
		GetDocument()->CleanSlideBoxData();   // 清除 m_SlideBoxData 中的数据

		// XYZ轴运动到初始位置
		g_Mechanism.GotoOrigin();
		
		while( !pCam->AutoExpoEnable(FALSE) ) {Sleep(500);}    // 禁止摄像机自动曝光

		// 更新界面接口响应状态
		EnableGUI(	GUI_MENU_SCANMODE | GUI_MENU_MICROSCOPECONTROL | GUI_MENU_ORIGIN | GUI_MENU_SELECT
				  | GUI_BUTTON_START | GUI_BUTTON_EXIT | GUI_BUTTON_SLIDEINFO | GUI_BUTTON_NEXTSLIDE | GUI_BUTTON_CHANGEBOX |GUI_BUTTON_JUDGE
				 ); 
		pCam->EnableNotifyMsg(TRUE);  // 允许摄像机输出图像
		
		this->InvalidateRect(&m_SlideIndex_Rect, 0);
		m_OperationPrompt = _T("1. 请输入玻片的信息（点击\"玻片信息\"按钮）\r\n\r\n");
		this->UpdateData(FALSE);
		hCur  =  AfxGetApp()->LoadStandardCursor(  IDC_ARROW ) ;
		SetCursor(hCur);
		return;
	}

	EnableGUI(	GUI_MENU_MICROSCOPECONTROL | GUI_BUTTON_START | GUI_BUTTON_EXIT ); 
	hCur  =  AfxGetApp()->LoadStandardCursor(  IDC_ARROW ) ;
	SetCursor(hCur);
}

// 输入当前玻片信息
void CBacilusDetectView::OnBnClickedButtonSlideinfo()
{   
	CCamera* pCam = CCamera::GetInstance();
	pCam->EnableNotifyMsg(FALSE);  // 禁摄像机止输出图像
	DisableALLGUI();// 禁止所有的界面响应接口
	m_OperationPrompt = _T("注意：在输入玻片信息信息时，玻片编号不能为空。");
	this->UpdateData(FALSE);

	// 通过玻片信息对话框设置玻片盒上所有玻片的信息
	m_SlideInfoDlg.DoModal();


	if ( !GetDocument()->GetSlideID( ).IsEmpty() )
	{
		EnableGUI(GUI_BUTTON_SCAN);// 恢复界面接口响应状态

		m_OperationPrompt = _T("1. 可以通过\"系统控制-->扫描方式设置\"菜单对默认的扫描方式进行修改\r\n\r\n2. 点击\"开始检验\"按钮，系统将会对当前玻片进行扫描");
		this->InvalidateRect(&m_SlideID_Rect, 0);    // 更新玻片编号
		this->UpdateData(FALSE);	
		return;	
	}
	
	// 更新界面接口响应状态
	EnableGUI(GUI_BUTTON_SLIDEINFO);
	pCam->EnableNotifyMsg(TRUE);  // 允许摄像机输出图像
	m_OperationPrompt = _T("1. 请输入玻片的信息（点击\"玻片信息\"按钮）\r\n\r\n");
	this->UpdateData(FALSE);

}

// 开始进行玻片扫描
void CBacilusDetectView::OnBnClickedButtonScan()
{	
	m_AutoScanBox = FALSE;
	if ( GetDocument()->GetSlideID().IsEmpty() )
	{
		return;
	}

	UpdateScanRouteBmp(); // 更新扫描过程示意图中的扫描路径
	this->InvalidateRect(&m_SlideScanBmp_Rect, 0);
	DisableGUI(0xFFFFFFFF);  // 完全禁止菜单和按钮使能状态
	m_OperationPrompt = _T("系统正在对玻片进行扫描检验，请耐心等待");
	this->UpdateData(FALSE);
	
	// 首先在指定目录下建立一个以工位号命名的文件夹，然后再在该文件夹下面建立一个以当前时间命名的文件夹，软件系统的所有临时图像等信息都保存在该文件夹里
	m_Directory = 	m_DefaultDirectory;   // 临时文件目录
	::CreateDirectoryW(m_Directory, NULL);
	
	m_pAutoScanWorkThread = AfxBeginThread(AutoScanSlideWorkThread, (LPVOID)this);	
	EnableGUI(GUI_BUTTON_STOPSCAN);
}

// 停止玻片扫描、检验的工作
void CBacilusDetectView::OnBnClickedButtonScanstop()
{
	DisableGUI(0xFFFFFFFF);              // 完全禁止菜单和按钮使能状态
	m_EventStopAutoScan.SetEvent();      // 通知自动扫描线程，结束扫描采集工作
}

// 辅助诊断
void CBacilusDetectView::OnBnClickedButtonJudge()
{
	DisableALLGUI();// 禁止所有的界面响应接口
	m_pJudgeDlg->Create(CJudgeDlg::IDD, this);
	m_pJudgeDlg->ShowWindow(SW_SHOW);    
}

// 下一个工位
// void CBacilusDetectView::OnBnClickedButtonNextslide()
// {
// 	WorkPositionSelect( g_Mechanism.GetWorkPositionID() + 1 );
// }

// 置换玻片盒的功能与系统初始化一样
void CBacilusDetectView::OnBnClickedButtonChangebox()
{
	CCamera* pCam = CCamera::GetInstance();
	pCam->EnableNotifyMsg(FALSE);  // 禁摄像机止输出图像
	if ( AfxMessageBox(_T("是否置换玻片？"), MB_OKCANCEL|MB_ICONQUESTION ) == IDOK )
	{
		OnBnClickedButtonStart();
	}
	else
	{
		pCam->EnableNotifyMsg(TRUE);  // 允许摄像机输出图像
		return;
	}	
}

// 系统退出
void CBacilusDetectView::OnBnClickedButtonExit()
{
	AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
}

// 接口
// 程序退出之前，关闭各种设备，释放资源
void  CBacilusDetectView::SystemExiting()
{
	cv::Point3f	f3DStopPos = g_Mechanism.GetStopPosition();
	CCamera* pCam = CCamera::GetInstance();
	CMotorController* pMC = CMotorController::GetInstance();
	DWORD  spantime = 0, starttime = 0, curtime = 0;

	if ( pMC->isConnecting() )
	{
		pCam->EnableNotifyMsg(FALSE);  // 禁摄像机止输出图像
	}
	DisableALLGUI();
	//DeleteDirectory(m_DefaultDirectory);  // 删除临时文件目录

	if (pMC->isConnecting() )
	{
// 		pMC->Home();
		pMC->Close();
	}


	if ( pCam->isCapturing() )
	{
		pCam->close();
	}
}



/****************************** 线程函数 *************************************/
// 停止检验按钮被按下，扫描检验工作线程被终止
void WhetherStopWorkThread(CBacilusDetectView* pDetectView)
{
	HWND hWnd = pDetectView->GetSafeHwnd();	
	HANDLE hHandleStop = HANDLE(pDetectView->m_EventStopAutoScan);
	if ( WAIT_OBJECT_0 == ::WaitForSingleObject(hHandleStop,0) )   // 停止检验按钮被按下，扫描检验工作线程被终止
	{
		pDetectView->m_bScanEnd = TRUE;                                                       // 视野扫描采集图像工作结束
		::PostMessage ( hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_VIEW, (LPARAM)(-1) );   // 更新主界面	
		if ( pDetectView->m_bImageProcessThread )
		{
			(pDetectView->m_EventStopImageProcess).SetEvent();				                      // 通知图像处理线程，扫描采集图像工作已经完成	
			while( !pDetectView->m_bImageProcessEnd  )	{	;	}								  // 等待图像处理线程结束					                                   
		}
		::PostMessage (hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_END, (LPARAM)0);			  // 更新主界面
		::AfxEndThread(0, TRUE);
	}
}


void AutoScanSlide(CBacilusDetectView* pDetectView)
{
	CTime curtime = CTime::GetCurrentTime(); 

	HWND hWnd = pDetectView->GetSafeHwnd();	
	HANDLE hHandleStop = HANDLE(pDetectView->m_EventStopAutoScan);
	
	// 初始化自动扫描、聚焦、图像处理等任务
	PSCANROUTE pScanRoute = g_Mechanism.GetScanRoute();
	CBacilusDetectDoc* m_pDocument =(CBacilusDetectDoc*) pDetectView->GetDocument();	// Document地址
	int& nScanSequenceIndex = pDetectView->m_ScanSequenceIndex;							// 视野序号：在线程中会被改变
	int* pnImageProcessSequenceIndex = &(pDetectView->m_nImageProcessSequenceIndex);	// 图像处理序号：在线程中会被改变
	volatile BOOL* g_pbImageProcessEnd = &(pDetectView->m_bImageProcessEnd);		    // 全局同步变量：图像处理线程结束
	volatile BOOL& g_bImageProcessThread = pDetectView->m_bImageProcessThread;			// 全局同步变量：图像处理线程已经开启
	volatile BOOL& g_bImageProcess = pDetectView->m_bImageProcess;						// 全局同步变量：图像处理线程已经开始处理对立里的图像
	volatile BOOL& g_bScanPostProcess = pDetectView->m_bScanPostProcess;				// 全局同步变量：第一次扫描采集图像工作结束，进行后处理，将有肯能错误聚焦的视野进行重新聚焦
	volatile BOOL& g_bScanEnd = pDetectView->m_bScanEnd;				                // 全局同步变量：视野扫描采集图像工作结束
	CCamera* pCam = CCamera::GetInstance();
	CMotorController* pMC = CMotorController::GetInstance();

	int iScanCount = pScanRoute->ScanSequence.GetSize();  // 获得需要扫描视野的数量
	m_pDocument->InitialFOVCount( iScanCount);			// 初始化视野的个数
	cv::Size      ImageSize = pCam->GetCameraImageSize();					// 摄像机拍摄到的图像的大小	
	CCriticalSection    ProcessCritSection;
	CSingleLock singleLock(&ProcessCritSection);

	queue <STRUCTIMAGEINFO>* pqImageInfo = &(pDetectView->m_qImageInfo);
	while ( ! pqImageInfo->empty() )                                     // 清空图像队列
	{
		pqImageInfo->pop();		
	}

	// 初始化全局同步变量
	*pnImageProcessSequenceIndex = 0;
	*g_pbImageProcessEnd = FALSE;	
	g_bScanPostProcess = FALSE;
	g_bScanEnd = FALSE;
	g_bImageProcessThread = FALSE;
	g_bImageProcess = FALSE;



	// 初始化自动聚焦类
	// 运动到进行自动调光的位置, 进行自动调光
	CAutoAdjustLight AutoAdjustLight;
	::PostMessage(hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOLIGHT, (LPARAM)0);              // 更新主界面
	BOOL bSuccess = FALSE;
	if ( AutoAdjustLight.InitAutoAdjustLight(pDetectView) )
	{
		bSuccess = AutoAdjustLight.Work();		
	}
	if ( !bSuccess )
	{
		AfxMessageBox(_T("自动调光失败，请通过调光旋钮将显微镜的光源亮度调大，然后重新进行相应的操作"));
		::SendMessage(hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_END, (LPARAM)0);  // 更新主界面
		::AfxEndThread(0, TRUE);
	}


	// 将自动调光之后的图像保存为参考图像
	cv::Mat   pcvImage = pCam->GetImage(); // 摄像机拍摄到的图像
	::WaitForSingleObject(pCam->CapturedEvent(),INFINITE);
	::WaitForSingleObject(pCam->CapturedEvent(),INFINITE);  //等待新的摄像机拍摄到的图像数据
	SaveImage2BMP(pDetectView->m_Directory, 0, pcvImage);

	CTime t1= CTime::GetCurrentTime(); 
	//====================================================================================//	

	// 创建保存图像的临时文件目录		
	CString strDirectory = pDetectView->m_Directory;                  // 临时文件目录
	CString str1;
	str1.Format(_T("\\Temp"));
	strDirectory += str1;
	::CreateDirectoryW(strDirectory, NULL);	
	str1 = curtime.Format(_T("\\%Y-%m-%d_%H-%M-%S"));
	strDirectory += str1;
	::CreateDirectoryW(strDirectory, NULL);                           
	m_pDocument->SetDirectory( strDirectory);     // 将相关的数据存储到 Document的 m_SlideBoxData 数据结构之中


	// 初始化自动聚焦类
	CAutoFocus   AutoFocusWork;
	if ( ! AutoFocusWork.InitAotoFocus(pDetectView) )  // 获得扫描视野拓扑结构数据，初始化自动聚焦类
	{
	AfxMessageBox(_T("自动聚焦初始化失败"));
	::SendMessage(hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_FOCUS_TERMINATE, (LPARAM)0);  // 更新主界面
	::AfxEndThread(0, TRUE);
	}

	// 利用玻片上的红十字估计目标层焦平面的初始位置
	//if ( ! AutoFocusWork.EstInitFocusPosition() )
	//{
	//	AfxMessageBox(_T("自动聚焦失败，请将初始扫描点设置在玻片红十字附近，然后重新检测"));
	//	::SendMessage(hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_FOCUS_TERMINATE, (LPARAM)nWorkPositionIndex);  // 更新主界面
	//	::AfxEndThread(0, TRUE);
	//}
///

	
	CTime t2= CTime::GetCurrentTime(); 

	// 开启图像处理线程
	StructImageProcessWorkTreadParam  structImgProcessParam;
	structImgProcessParam.iScanCount = iScanCount;
	structImgProcessParam.pbImageProcessEnd = g_pbImageProcessEnd;
	structImgProcessParam.strFileDirectory = pDetectView->m_Directory;
	structImgProcessParam.pDocument = m_pDocument;
	structImgProcessParam.pnImageProcessSequenceIndex = pnImageProcessSequenceIndex;
	structImgProcessParam.pqImageInfo = pqImageInfo;
	structImgProcessParam.hWnd = hWnd;
	structImgProcessParam.uMsg = MSG_SCANTHREAD_NOTIFY;
	structImgProcessParam.hHandleStop = HANDLE(pDetectView->m_EventStopImageProcess);
	singleLock.Lock();
	AfxBeginThread(ImageProcessWorkThread, (LPVOID)(&structImgProcessParam) );
	g_bImageProcessThread = TRUE;
	singleLock.Unlock(); 

	cv::Point3f* pFOV3DPosTopology = new cv::Point3f[iScanCount];
	int * pnIndex = new int[iScanCount];

	// 自动扫描、聚焦，将每个视野的清晰图像保存在临时文件目录中，同时将图像在内存中的地址放入FIFO的队列数据结构之中，以供目标识别的线程使用	
	for ( nScanSequenceIndex=0; nScanSequenceIndex<iScanCount; nScanSequenceIndex++ )
	{		
		// 运动到需要聚焦的视野 
		g_Mechanism.GotoFOV(nScanSequenceIndex); // 运动到该视野处 
		cv::Point2f curpos;
		pMC->GetXYMotorPosition(curpos,DISTANCE_UNIT_UM);			// 获得当前视野在XY平台上的绝对坐标
		pDetectView->AddPoint2SlideScanBmp(curpos);										// 将当前的视野在扫描过程示意图中标识出来
		::PostMessage (hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_VIEW, (LPARAM)0);  // 更新主界面

		// 获得当前视野的最佳图像		
		pCam->EnableNotifyMsg(TRUE);

		cv::Mat pcvBestImage ;
		float fBestFocusPos = AutoFocusWork.GetCurFOVBestImage(pcvBestImage, curpos, nScanSequenceIndex);

// 		cv::Mat pcvBestImage;
// 		pCam->GetImage().copyTo(pcvBestImage);
//		float fBestFocusPos = 0;

		pCam->EnableNotifyMsg(FALSE);

		// 将最佳图像保存在临时文件目录中
		CString strFileName = SaveImage2JPG(strDirectory, nScanSequenceIndex, pcvBestImage);  //保存图像

		singleLock.Lock();

		// 将相关的数据存储到 Document的 m_SlideBoxData 数据结构之中
		m_pDocument->SetSlideScanStatus(TRUE);			 // 已扫描
		m_pDocument->SetFOVCount( nScanSequenceIndex+1);  // 当前已扫描视野总数目
		m_pDocument->SetFOVPosition( nScanSequenceIndex, curpos.x, curpos.y, fBestFocusPos);  // 当前视野位置
		m_pDocument->SetFOVCheckedStatus(nScanSequenceIndex, FALSE);						 // 当前视野未诊断	

		MATRIXINDEX curFOVMatrix = pScanRoute->ScanSequence[nScanSequenceIndex];
		pFOV3DPosTopology[ pScanRoute->ColNum * curFOVMatrix.row + curFOVMatrix.col ] = cv::Point3f( curpos.x, curpos.y, fBestFocusPos );
		pnIndex[ pScanRoute->ColNum * curFOVMatrix.row + curFOVMatrix.col ] = nScanSequenceIndex;

		// 将图像的地址放入FIFO的队列
		STRUCTIMAGEINFO structImageInfo;
		structImageInfo.nScanSequenceIndex = nScanSequenceIndex;
		structImageInfo.strFileName = strFileName;
		pqImageInfo->push(structImageInfo);

		singleLock.Unlock(); 

		WhetherStopWorkThread(pDetectView);			// 停止检验按钮被按下，扫描检验工作线程被终止
	}
		
	CString FileName = strDirectory + _T("\\ScanInfo.txt");
	CTime t3= CTime::GetCurrentTime(); 
	CTimeSpan ts;
	try
	{		
		CStdioFile ScanInfoFile( FileName,	CFile::modeCreate | CFile::modeWrite | CFile::typeText );
		str1 = _T("Slide ID = ") + m_pDocument->GetSlideID() + _T("\n\n");
		ScanInfoFile.WriteString(str1);
		CString stime, time;
		ts = t1 - curtime;
		stime = ts.Format(_T("%H : %M : %S"));
		time = _T("AdjustLight Time = ") + stime + _T("\n\n");
		ScanInfoFile.WriteString(time);

		ts = t2 - t1;
		stime = ts.Format(_T("%H : %M : %S"));
		time = _T("InitAotoFocus Time = ") + stime + _T("\n\n");
		ScanInfoFile.WriteString(time);

		ts = t3 - t2;
		stime = ts.Format(_T("%H : %M : %S"));
		time = _T("Scanning Time = ") + stime + _T("\n\n");
		ScanInfoFile.WriteString(time);

		ts = t3 - curtime;
		stime = ts.Format(_T("%H : %M : %S"));
		time = _T("----------------------------------\n\nTotal Scanning Time = ") + stime + _T("\n\n");
		ScanInfoFile.WriteString(time);

		CString str = _T("=======================================================\n\n");
		ScanInfoFile.WriteString(str);

		str = _T("	FOV Topology\n\n");
		ScanInfoFile.WriteString(str);
		int i, j;
		for( i = 0; i < pScanRoute->RowNum; i++ )
		{
			for ( j = 0; j < pScanRoute->ColNum; j++ )
			{
				str.Format(_T("%3.0f   "), pFOV3DPosTopology[ pScanRoute->ColNum * i + j].z);
				ScanInfoFile.WriteString(str);
			}
			ScanInfoFile.WriteString(_T("\n\n"));
		}

		str = _T("=======================================================\n\n");
		ScanInfoFile.WriteString(str);

		str = _T("	FOV Topology\n\n");
		ScanInfoFile.WriteString(str);
		for( i = 0; i < pScanRoute->RowNum; i++ )
		{
			for ( j = 0; j < pScanRoute->ColNum; j++ )
			{
				str.Format(_T("%3.0f(%3d)   "), pFOV3DPosTopology[ pScanRoute->ColNum * i + j].z, pnIndex[ pScanRoute->ColNum * i + j]);
				ScanInfoFile.WriteString(str);
			}
			ScanInfoFile.WriteString(_T("\n\n"));
		}

		str = _T("=======================================================\n\n");
		ScanInfoFile.WriteString(str);

		ScanInfoFile.Close();

		CString fileName = strDirectory + _T("\\Focus.data");
		CFile File(fileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		File.Write(& pScanRoute->ColNum, sizeof(UINT));     // 列数
		File.Write(& pScanRoute->RowNum, sizeof(UINT));		// 行数
		for( i = 0; i < pScanRoute->RowNum; i++ )
		{
			for ( j = 0; j < pScanRoute->ColNum; j++ )
			{
				File.Write(&pFOV3DPosTopology[ pScanRoute->ColNum * i + j].x, sizeof(float));
				File.Write(&pFOV3DPosTopology[ pScanRoute->ColNum * i + j].y, sizeof(float));
				File.Write(&pFOV3DPosTopology[ pScanRoute->ColNum * i + j].z, sizeof(float));
			}
		}
		File.Close();
	}
	catch(CFileException* pe)
	{
		TRACE( _T("File could not be opened, cause = %d\n"), pe->m_cause );
		pe->Delete();
	}

	//// 对已扫描的视野聚焦情况进行分析，根据扫描拓扑结构判断哪些视野需要重新聚焦
	//g_bScanPostProcess = TRUE;
	//int iPostProcessFOVCount = AutoFocusWork.AnalyzeScanedFOVs();
	//if ( iPostProcessFOVCount > 0 )
	//{
	//	for ( int i = 0; i < iPostProcessFOVCount; i++ )
	//	{
	//		::PostMessage (hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_VIEW, (LPARAM)0);  // 更新主界面

	//		cv::Mat pcvBestImage;
	//		cv::Point3f  cvFOV3DPos;
	//		nScanSequenceIndex = AutoFocusWork.GetPostProcessFOVBestImage(&pcvBestImage, &cvFOV3DPos, i);

	//		CString str1;
	//		str1.Format(_T("%3.0f(%3d)   "), cvFOV3DPos.z, nScanSequenceIndex);
	//		str += str1;
	//		
	//		// 将最佳图像保存在临时文件目录中
	//		CString strFileName = SaveImage2JPG(strDirectory, nScanSequenceIndex, pcvBestImage);  //保存图像

	//		m_pDocument->SetFOVPosition(nWorkPositionIndex, nScanSequenceIndex, cvFOV3DPos.x, cvFOV3DPos.y, cvFOV3DPos.z);  // 当前视野位置

	//		WhetherStopWorkThread(pDetectView);			// 停止检验按钮被按下，扫描检验工作线程被终止
	//	}
	//}

	//str += _T("\n\n");
	//try
	//{
	//	CStdioFile ScanInfoFile( FileName,	CFile::modeNoTruncate | CFile::modeWrite | CFile::typeText );
	//	ScanInfoFile.SeekToEnd();
	//	ScanInfoFile.WriteString(str);
	//}
	//catch(CFileException* pe)
	//{
	//	TRACE( _T("File could not be opened, cause = %d\n"), pe->m_cause );
	//	pe->Delete();
	//}

	delete [] pFOV3DPosTopology;
	delete [] pnIndex;
// 	
	// 自动检测结核杆菌后处理
	g_bScanEnd = TRUE;                                                                    // 视野扫描采集图像工作结束
	::PostMessage ( hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_VIEW, (LPARAM)(-1) );   // 更新主界面	
	if ( g_bImageProcessThread )
	{
		(pDetectView->m_EventStopImageProcess).SetEvent();									  // 通知图像处理线程，扫描采集图像工作已经完成	
		while( !(*g_pbImageProcessEnd)  ) 	{ ;	}		           						      // 等待图像处理线程结束
	}
::SendMessage(hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_END, (LPARAM)0);  // 更新主界面

	CTime curtime3 = CTime::GetCurrentTime();
	ts = curtime3 -curtime;
	try
	{
		CStdioFile ScanInfoFile( FileName,	CFile::modeNoTruncate | CFile::modeWrite | CFile::typeText );
		ScanInfoFile.SeekToEnd();
		CString stime = ts.Format(_T("%H : %M : %S"));
		CString time = _T("Scaning and Checking Time = ") + stime + _T("\n\n");
		ScanInfoFile.WriteString(time);
	}
	catch(CFileException* pe)
	{
		TRACE( _T("File could not be opened, cause = %d\n"), pe->m_cause );
		pe->Delete();
	}	
}
// 
// 自动扫描检验玻片线程
UINT __cdecl AutoScanSlideWorkThread(LPVOID pParam)
{	
	CBacilusDetectView* pDetectView = (CBacilusDetectView*) pParam;
	
	AutoScanSlide(pDetectView);

	return 0;
}

// 图像处理线程
UINT __cdecl ImageProcessWorkThread(LPVOID pParam)
{
	StructImageProcessWorkTreadParam*  pstructImgProcessParam = (StructImageProcessWorkTreadParam*)(pParam);
	int iScanCount = pstructImgProcessParam->iScanCount;
	volatile BOOL* pbImageProcessEnd = pstructImgProcessParam->pbImageProcessEnd;    /* Output */
	CBacilusDetectDoc* m_pDocument = pstructImgProcessParam->pDocument;     /* Input / Output  */
	int* pnImageProcessSequenceIndex = pstructImgProcessParam->pnImageProcessSequenceIndex;  /* Output */
	queue <STRUCTIMAGEINFO> * pqImageInfo = pstructImgProcessParam->pqImageInfo;    /* Input / Output */
	HWND  hWnd = pstructImgProcessParam->hWnd;							     /* Input */
	UINT  uMsg = pstructImgProcessParam->uMsg;                               /* Input */
	HANDLE hHandleStop = pstructImgProcessParam->hHandleStop;                /* Input */
	CString strDirectory = pstructImgProcessParam->strFileDirectory;         /* Input */

	CCriticalSection    ProcessCritSection;
	CSingleLock singleLock(&ProcessCritSection);
	BOOL bStop = FALSE;

	// 获取参考图像 
	CString strFileName = strDirectory + _T("\\0.BMP");
	cv::Mat  pcvImage = LoadImage2Color(strFileName);
	cv::Mat pcvGrayImg;
	cv::cvtColor(pcvImage, pcvGrayImg, CV_BGR2GRAY);//转换成灰度图像


	
	while(1)
	{
		// 开始处理队列中剩下的图像
		if ( ! pqImageInfo->empty() )
		{
			singleLock.Lock();
			STRUCTIMAGEINFO structImageInfo = pqImageInfo->front();
			singleLock.Unlock();

			(*pnImageProcessSequenceIndex) = structImageInfo.nScanSequenceIndex;	
			::PostMessage( hWnd, uMsg, (WPARAM)AUTOSCAN_VIEW, (LPARAM)1 );   // 更新主界面				

			cv::Mat  pcvImage = LoadImage2Color(structImageInfo.strFileName);

			CProSingleImage imgpro(pcvImage);  // 结核杆菌检测算法类
			cv::Mat maskimg = imgpro.MakeLensNoiseMaskImage(pcvGrayImg);
			imgpro.SetLensNoiseMaskImg(maskimg);
			imgpro.SegmentColorImageMixture4();
			imgpro.ClassifyObject();

			int tb1 = imgpro.GetTB1Num();
			int tb2 = imgpro.GetTB2Num();

			m_pDocument->SetTargetCountInFOV(structImageInfo.nScanSequenceIndex, tb1 + tb2);
			m_pDocument->SetJudgedTargetCountInFOV(structImageInfo.nScanSequenceIndex, tb1 + tb2);
			m_pDocument->InitialSimpleTargetCountInFOV(structImageInfo.nScanSequenceIndex, tb1);
			m_pDocument->InitialOverlapTargetCountInFOV(structImageInfo.nScanSequenceIndex, tb2);

			cv::Size size1 = pcvImage.size();
			cv::Size size2 = imgpro.m_pSrcImg.size();
			float fRatioX = float( size1.width ) / float( size2.width );
			float fRatioY = float( size1.height ) / float( size2.height );

			int i,j;
			CvRect rect;
			for( i = 0; i < tb1; i++ )
			{
				rect = imgpro.m_rectTB1[i];
				rect.x *= fRatioX;
				rect.y *= fRatioY;
				rect.width *= fRatioX;
				rect.height *= fRatioY;
				m_pDocument->SetTargetRectInFOV(structImageInfo.nScanSequenceIndex, i, rect);
				m_pDocument->SetSimpleTargetRectInFOV(structImageInfo.nScanSequenceIndex, i, rect);
			}
			for( j = 0; j < tb2; j++, i++)
			{
				rect = imgpro.m_rectTB2[j];
				rect.x *= fRatioX;
				rect.y *= fRatioY;
				rect.width *= fRatioX;
				rect.height *= fRatioY;
				m_pDocument->SetTargetRectInFOV(structImageInfo.nScanSequenceIndex, i, rect);
				m_pDocument->SetOverlapTargetRectInFOV(structImageInfo.nScanSequenceIndex, j, rect);
			}
		
			singleLock.Lock();
			pqImageInfo->pop();  							
			singleLock.Unlock();
		}

		if ( WAIT_OBJECT_0 == ::WaitForSingleObject(hHandleStop,0) )    // 收到停止图像采集停止命令
		{
			bStop = TRUE;
		}
		
		if ( bStop && pqImageInfo->empty() )   // 收到停止图像采集停止命令，并且队列里没有图像，则工作线程被终止
		{
			::PostMessage( hWnd, uMsg, (WPARAM)AUTOSCAN_POSTPROCESS, (LPARAM)1 );   // 更新主界面	
						
			// 检验完毕停止玻片扫描、检验的工作
			singleLock.Lock();
			(*pbImageProcessEnd) = TRUE;
			singleLock.Unlock(); 
			return 0;
		}		
	}
	
	(*pbImageProcessEnd) = TRUE;
	return 0;
}
