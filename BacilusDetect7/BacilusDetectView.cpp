
// BacilusDetectView.cpp : CBacilusDetectView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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

// ȫ���ⲿ�豸
CMechanism		g_Mechanism;


// ͼ�����̲߳����ṹ
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

// CBacilusDetectView ����/����

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
	// �˵�ʹ�ܳ�ʼ��
	m_MenuItemStatus.Menu_ControlScan = TRUE;
	m_MenuItemStatus.Menu_ControlSelect = FALSE;
	m_MenuItemStatus.Menu_ControlOrigin = FALSE;
	m_MenuItemStatus.Menu_ControlMicroscope = FALSE;
	m_MenuItemStatus.Menu_ControlCamera = FALSE;
	m_MenuItemStatus.Menu_ViewBacilus = FALSE;

	m_ClientMinSize.cx = 0;
	m_ClientMinSize.cy = 0;

	// ��ʼ������������ɨ��ʾ��ͼ
	g_Mechanism.GetScanRoute(m_ScanRoute);	
	UpdateScanRouteBmp(); // ����ɨ�����ʾ��ͼ�е�ɨ��·��
	
	m_DefaultDirectory = _T("D:\\BacilusDetectTemp");   // ��ʱ�ļ�Ŀ¼

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
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CFormView::PreCreateWindow(cs);
}

void CBacilusDetectView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();

	// ����������������ֵ�λ�úʹ�С
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
					   (int)(x+CAMERA_IMAGE_WIDTH*0.5), (int)(y+CAMERA_IMAGE_HEIGHT*0.5) ); // ���ڴ����ͼ�����������е�λ��

	x = m_ImageBmp_Rect.Width() + 2 * delta;
	m_SlideScanBmp_Rect.SetRect( x,						 y, 
								 x+m_SlideScanBmp_Width, y+m_SlideScanBmp_Height );         // ɨ��ʾ��ͼ���������е�λ��

	m_StaticPrompt_Rect.left = m_SlideScanBmp_Rect.left;                                                
	m_StaticPrompt_Rect.right = m_SlideScanBmp_Rect.right;
	m_StaticPrompt_Rect.top = m_SlideScanBmp_Rect.bottom + 3 * delta;
	m_StaticPrompt_Rect.bottom = m_ImageBmp_Rect.bottom;
	this->GetDlgItem(IDC_STATIC_PROMPT)->MoveWindow(&m_StaticPrompt_Rect);					// ������ʾ��̬�ؼ����������е�λ��

	m_EditcPrompt_Rect.left = m_StaticPrompt_Rect.left + 3;												
	m_EditcPrompt_Rect.right = m_StaticPrompt_Rect.right - 3;
	m_EditcPrompt_Rect.top = m_StaticPrompt_Rect.top + 4 * delta;
	m_EditcPrompt_Rect.bottom = m_StaticPrompt_Rect.bottom - 3;
	this->GetDlgItem(IDC_EDIT_PROMPT)->MoveWindow(&m_EditcPrompt_Rect);						// ������ʾEdit�ؼ����������е�λ��
			
	x = m_SlideScanBmp_Rect.left + m_SlideIndexInSlideScanBmp.x;
	y = m_SlideScanBmp_Rect.top + m_SlideIndexInSlideScanBmp.y;
	m_SlideIndex_Rect.SetRect( x, y, x+17, y+17 );											// ��ǰ��λ������������е�λ��	

	x = m_SlideScanBmp_Rect.left + m_SlideIDInSlideScanBmp.x;
	y = m_SlideScanBmp_Rect.top + m_SlideIDInSlideScanBmp.y;
	m_SlideID_Rect.SetRect(x, y, m_SlideScanBmp_Rect.right, y+23 );							// ��ǰ��Ƭ������������е�λ��	
	
	// ���������湤������Ĵ�С
	m_ClientSize.cx = m_ImageBmp_Rect.Width() + m_SlideScanBmp_Rect.Width() + 3 * delta;
	m_ClientSize.cy = m_ImageBmp_Rect.bottom + delta;
	SetScrollSizes(MM_TEXT, m_ClientSize);   
	ResizeParentToFit(FALSE);
	m_ClientMinSize = m_ClientSize;

	// ��ʼ�����潻����Ӧ�ӿ�
	EnableGUI(GUI_MENU_SCANMODE | GUI_BUTTON_START | GUI_BUTTON_EXIT);	
	m_OperationPrompt = _T("1. ϵͳ�ɹ���ʼ��֮�󣬲��ܼ�������ɨ��������\r\n\r\n2. ����ͨ��\"ϵͳ����-->ɨ�跽ʽ����\"�˵���Ĭ�ϵ�ɨ�跽ʽ�����޸�");
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
// CBacilusDetectView ���

#ifdef _DEBUG
void CBacilusDetectView::AssertValid() const
{
	CFormView::AssertValid();
}

void CBacilusDetectView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CBacilusDetectDoc* CBacilusDetectView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBacilusDetectDoc)));
	return (CBacilusDetectDoc*)m_pDocument;
}
#endif //_DEBUG

//============================================================================================//

// �ڲ�����
// ��ֹ���еĽ�����Ӧ�ӿڣ��������˵�����ť
void  CBacilusDetectView::DisableALLGUI()       
{
	// ��ֹ��صİ�ť
	(CButton*) ( this->GetDlgItem(IDC_BUTTON_START) )->EnableWindow(FALSE);
	(CButton*) ( this->GetDlgItem(IDC_BUTTON_SLIDEINFO))->EnableWindow(FALSE);
	(CButton*) ( this->GetDlgItem(IDC_BUTTON_SCAN))->EnableWindow(FALSE);
	(CButton*) ( this->GetDlgItem(IDC_BUTTON_SCANSTOP) )->EnableWindow(FALSE);
	(CButton*) ( this->GetDlgItem(IDC_BUTTON_JUDGE) )->EnableWindow(FALSE);
	(CButton*) ( this->GetDlgItem(IDC_BUTTON_CHANGEBOX) )->EnableWindow(FALSE);
	(CButton*) ( this->GetDlgItem(IDC_BUTTON_EXIT) )->EnableWindow(FALSE);
	
	// ��ֹ��صĲ˵�
	m_MenuItemStatus.Menu_ControlScan = FALSE;
	m_MenuItemStatus.Menu_ControlSelect = FALSE;
	m_MenuItemStatus.Menu_ControlOrigin = FALSE;
	m_MenuItemStatus.Menu_ControlMicroscope = FALSE;
	m_MenuItemStatus.Menu_ControlCamera = FALSE;
	m_MenuItemStatus.Menu_ViewBacilus = FALSE;	
}

// ʹ�ܽ�����Ӧ�ӿڣ��������˵�����ť
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

// ��ֹ������Ӧ�ӿڣ��������˵�����ť
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

// ��ɨ���������˽ṹ�е���Ұ����������ת������ɨ�����ʾ��ͼ�е��߼�����
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

// ��ɨ��·���ӵ�ɨ�����ʾ��ͼ��
void  CBacilusDetectView::addScanRoute2Bmp()
{	
	int i,j,k;
	
	m_SlideScanBmp.DeleteObject();
	if( ! m_SlideScanBmp.LoadBitmapW(IDB_SLIDE_SCAN) )
	{
		AfxMessageBox(_T("���ܼ���ɨ��·��ʾ��ͼ��λͼ��Դ"));
		return;
	}
	LPCOLORREF pBmpData = new COLORREF[m_SlideScanBmp_Width*m_SlideScanBmp_Height];
	m_SlideScanBmp.GetBitmapBits(m_SlideScanBmp_byteCount, (LPVOID)pBmpData );  //���ͼ������

	// �޸�ͼ������
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
	m_SlideScanBmp.SetBitmapBits(m_SlideScanBmp_byteCount, (const void*)pBmpData );  //����Ҫ��ʾ��ͼ��

	delete [] pBmpData;
}

// ����ɨ�����ʾ��ͼ�е�ɨ��·��
void  CBacilusDetectView::UpdateScanRouteBmp()
{
	PhPtoScanBmpLP();
	addScanRoute2Bmp();
}
// 
// // �˶���ָ���Ĺ�λ
// void  CBacilusDetectView::GotoWorkPosition(int index)
// {
// 	CCamera* pCam = CCamera::GetInstance();
// 	pCam->EnableNotifyMsg(FALSE);  // �������ֹ���ͼ��
// 	
// 	g_Mechanism.GotoOrigin();
// 
// 	this->InvalidateRect(&m_SlideIndex_Rect, 0); // ��λ��
// 	this->InvalidateRect(&m_SlideID_Rect, 0);    // ��Ƭ���
// 
// 	UpdateScanRouteBmp(); // ����ɨ�����ʾ��ͼ�е�ɨ��·��
// 	this->InvalidateRect(&m_SlideScanBmp_Rect, 0);
// 	pCam->EnableNotifyMsg(TRUE);  // ������������ͼ��
// }

// ��΢��ƽ̨�˶���ָ���Ĳ�Ƭ��λ
// void  CBacilusDetectView::WorkPositionSelect(int index)
// {
// 	CCamera* pCam = CCamera::GetInstance();
// 	pCam->EnableNotifyMsg(FALSE);  // �������ֹ���ͼ��
// 	DisableALLGUI();// ��ֹ���еĽ�����Ӧ�ӿ�
// 
// 	GotoWorkPosition(index);		  //΢��ƽ̨�˶�����ǰ��Ƭ��ʼ����ɨ��ʱ��ĵ�һ����Ұ��λ��
// 		
// 	EnableGUI(0); // �ָ�����ӿ���Ӧ״̬
// 
// 	if ( !GetDocument()->GetSlideID( g_Mechanism.GetWorkPositionID() ).IsEmpty() )
// 	{
// 		EnableGUI(GUI_BUTTON_SCAN|GUI_BUTTON_AUTOSCAN);
// 		m_OperationPrompt = _T("1. ����ͨ��\"ϵͳ����-->ɨ�跽ʽ����\"�˵���Ĭ�ϵ�ɨ�跽ʽ�����޸�\r\n\r\n2. ���\"��ʼ����\"��ť��ϵͳ����Ե�ǰ��λ�ϵĲ�Ƭ����ɨ��\r\n\r\n3.���\"ȫ�Զ�����\"��ť��ϵͳ����Բ�Ƭ������Ӧ��λ�ϵĲ�Ƭ����ɨ��");
// 		this->UpdateData(FALSE);
// 	}
// 	else
// 	{
// 		DisableGUI(GUI_BUTTON_SCAN);
// 		m_OperationPrompt = _T("��ǰ��λ�ϵĲ�Ƭ���Ϊ�գ����ܽ���ɨ����飬��ͨ��\"��Ƭ��Ϣ\"��ť���������Ϣ���߸�����λ��������Ƭ����ɨ�����");
// 		if ( m_GUIStatus & GUI_BUTTON_AUTOSCAN )
// 		{
// 			m_OperationPrompt = _T("1. ��ǰ��λ�ϵĲ�Ƭ���Ϊ�գ����ܽ���ɨ����飬��ͨ��\"��Ƭ��Ϣ\"��ť���������Ϣ���߸�����λ��������Ƭ����ɨ�����\r\n\r\n2. ���\"ȫ�Զ�����\"��ť��ϵͳ����Բ�Ƭ������Ӧ��λ�ϵĲ�Ƭ����ɨ��");
// 		}
// 		this->UpdateData(FALSE);
// 	}
// 	pCam->EnableNotifyMsg(TRUE);  // ������������ͼ��
// }

// ��XYƽ̨�ϵľ������꣨unit��um��ת������ɨ�����ʾ��ͼ�е��߼�����
CPoint   CBacilusDetectView::XYPhPtoScanBmpLP(cv::Point2f XYpos)	
{	
	CPoint position_in_SlideBmp(0,0);

	// ���㵱ǰ���������ڵ�ǰ��λ�ϵĲ�Ƭ����ϵ�ϵ��������
	cv::Point2f slidepos = g_Mechanism.GetRelativeXY(XYpos);   
		
	//����ɨ�����ʾ��ͼ�е��߼�����
	position_in_SlideBmp.x = LONG( slidepos.x / m_SlideScanBmp_Display_Ratio );
	position_in_SlideBmp.y = LONG( slidepos.y / m_SlideScanBmp_Display_Ratio );
	position_in_SlideBmp = m_SlideScanBmp_Origin - position_in_SlideBmp;	  

	return position_in_SlideBmp;
}

// ����XYƽ̨�Ͼ��������ʾ�ĵ���ɨ�����ʾ��ͼ�б�ǳɺ�ɫ
void  CBacilusDetectView::AddPoint2SlideScanBmp(cv::Point2f XYpos)
{
	CPoint point = XYPhPtoScanBmpLP(XYpos);
	int i,j;
	LPCOLORREF pBmpData = new COLORREF[m_SlideScanBmp_Width*m_SlideScanBmp_Height];

	m_SlideScanBmp.GetBitmapBits(m_SlideScanBmp_byteCount, (LPVOID)pBmpData );  //���ͼ������
	// �޸�ͼ������
	for ( i=0; i<3; i++ )
	{
		for ( j=0; j<2; j++ )
		{
			pBmpData[(point.y+j) * m_SlideScanBmp_Width + (point.x + i)] = RGB(255,0,0); 
		}
	}
	m_SlideScanBmp.SetBitmapBits(m_SlideScanBmp_byteCount, (const void*)pBmpData );  //����Ҫ��ʾ��ͼ��

	delete [] pBmpData;
}

//============================================================================================//

// CBacilusDetectView ��Ϣ�������
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

	// ��ʾ���ڽ��в�������Ұͼ��
	CBitmap	 BmpImage;                     //��������������ʾ��ͼ��	
	CCamera* pCam = CCamera::GetInstance();
	if(pCam->isCapturing())
	{
		cv::Mat   matImageTemp = pCam->GetImage();
		BmpImage.CreateBitmap(matImageTemp.cols, matImageTemp.rows, 1, 32, NULL);		
		cv::Mat matImage4C = cv::Mat(matImageTemp.rows,matImageTemp.cols,CV_8UC4);
		cv::cvtColor(matImageTemp, matImage4C, CV_BGR2BGRA);
		BmpImage.SetBitmapBits(matImage4C.total() * matImage4C.elemSize(), (const void*)matImage4C.ptr() );  //����Ҫ��ʾ��Ұ��ͼ��
	}
	else
	{			
		BmpImage.LoadBitmapW(IDB_MICROSCOPE_IMAGE);  // ��ʼ���������������ڴ����ͼ��
	}
	pOldBmp = MemDC.SelectObject(&BmpImage);
	pDC->StretchBlt( m_ImageBmp_Rect.left, m_ImageBmp_Rect.top,         // Display Destination
		             m_ImageBmp_Rect.Width(), m_ImageBmp_Rect.Height(), 
		             &MemDC, 0, 0, CAMERA_IMAGE_WIDTH, CAMERA_IMAGE_HEIGHT, // Source
					 SRCCOPY );

	// ��ʾɨ�����ʾ��ͼ
	MemDC.SelectObject(&m_SlideScanBmp);
	pDC->StretchBlt( m_SlideScanBmp_Rect.left, m_SlideScanBmp_Rect.top,         // Display Destination
		             m_SlideScanBmp_Rect.Width(), m_SlideScanBmp_Rect.Height(), 
		             &MemDC, 0, 0, m_SlideScanBmp_Width, m_SlideScanBmp_Height, // Source
					 SRCCOPY );
	MemDC.SelectObject(&pOldBmp);

// 	// ��ʾ��λ��
// 	CString			strWorkPositionNo;		
// 	strWorkPositionNo.Format( _T("%d"), g_Mechanism.GetWorkPositionID() + 1 );
// 	pDC->TextOutW(m_SlideIndex_Rect.left, m_SlideIndex_Rect.top, strWorkPositionNo);// ɨ��ʾ��ͼ�еĹ�λ��

	// ��ʾ��Ƭ���
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
	//		this->GetDlgItem(IDC_STATIC_PROMPT)->MoveWindow(&m_StaticPrompt_Rect);	// ������ʾ��̬�ؼ����������е�λ��

	//		m_EditcPrompt_Rect.left = m_StaticPrompt_Rect.left + 3;												
	//		m_EditcPrompt_Rect.right = m_StaticPrompt_Rect.right - 3;
	//		m_EditcPrompt_Rect.top = m_StaticPrompt_Rect.top + delta;
	//		m_EditcPrompt_Rect.bottom = m_StaticPrompt_Rect.bottom - 3;
	//		this->GetDlgItem(IDC_EDIT_PROMPT)->MoveWindow(&m_EditcPrompt_Rect);		// ������ʾEdit�ؼ����������е�λ��
		
			SetScrollSizes(MM_TEXT, m_ClientMinSize);   
			ResizeParentToFit(FALSE);
		}
	}	
}

// ����ɨ�跽ʽ���öԻ��򷢳���ɨ��·��������Ϣ
afx_msg LRESULT CBacilusDetectView::OnMsgScanrouteNotify(WPARAM wParam, LPARAM lParam)
{
	CCamera* pCam = CCamera::GetInstance();
	CMotorController* pMC = CMotorController::GetInstance();
	g_Mechanism.GetScanRoute(m_ScanRoute);	// ���ɨ��·��
	UpdateScanRouteBmp(); // ����ɨ�����ʾ��ͼ�е�ɨ��·��
	this->InvalidateRect(&m_SlideScanBmp_Rect, 0);
	switch (wParam)
	{
	case 0:       // Apply button is pressed
		if ( pMC->isConnecting() )
		{
			pCam->EnableNotifyMsg(FALSE);  // �������ֹ���ͼ��

			g_Mechanism.GotoOrigin();		  // �ص���ǰ��λ����ʼ��

			pCam->EnableNotifyMsg(TRUE);   // ������������ͼ��

		}
		break;

	case 1:       // OK button is pressed
		break;

	default:
		break;
	}
	
	return 0;
}

// ���������������ͼ�������Ϣ
afx_msg LRESULT CBacilusDetectView::OnMsgCameraNotify(WPARAM wParam, LPARAM lParam)
{
	if ( CWnd::GetActiveWindow() == &m_MicroscopeControlDlg) // �����΢�����ƶԻ����ڼ���״̬������������ʾ��ͼ��
	{
		m_MicroscopeControlDlg.UpdateImage();
	}
	else  //������������ʾ��ͼ��
	{
		this->InvalidateRect(&m_ImageBmp_Rect, 0);
	}

	return 0;
}

// �����Զ�ɨ������̷߳����Ľ��������Ϣ
afx_msg LRESULT CBacilusDetectView::OnMsgScanthreadNotify(WPARAM wParam, LPARAM lParam)
{
	BOOL bScan, bJudge;
	CString str;
	switch (wParam)
	{
	case AUTOSCAN_END:
		// ���½���ӿ���Ӧ״̬
		DisableGUI(GUI_BUTTON_STOPSCAN); 
		EnableGUI(	GUI_MENU_SCANMODE | GUI_MENU_MICROSCOPECONTROL | GUI_MENU_ORIGIN | GUI_MENU_SELECT
					| GUI_BUTTON_START | GUI_BUTTON_EXIT | GUI_BUTTON_SLIDEINFO | GUI_BUTTON_NEXTSLIDE | GUI_BUTTON_CHANGEBOX
					| GUI_BUTTON_SCAN	| GUI_BUTTON_AUTOSCAN | GUI_BUTTON_JUDGE ); 
		m_OperationPrompt = _T("ɨ��������\r\n\r\n����\"�������\"��ť����ɨ��Ĳ�Ƭ����ȷ��");
		this->UpdateData(FALSE);
		break;

// 	case AUTOSCAN_SLIDE_END:
// 		this->GetDocument()->GetScanJudgeStatus(bScan, bJudge);
// 		if ( bScan )
// 		{
// 			m_OperationPrompt.Format(_T("ϵͳ���ڶԲ�Ƭ������Ӧ�Ĳ�Ƭ����ɨ����飬�����ĵȴ�\r\n\r\n�Ѿ���ɹ�λ %d �ϵĲ�Ƭɨ�蹤��"), lParam+1);
// 		}
// 		else
// 		{
// 			m_OperationPrompt.Format(_T("ϵͳ���ڶԲ�Ƭ������Ӧ�Ĳ�Ƭ����ɨ����飬�����ĵȴ�\r\n\r\n������ɹ�λ %d �ϲ�Ƭ��ɨ��������"), lParam+1);
// 		}
// 		this->UpdateData(FALSE);
// 		break;

	case AUTOLIGHT:
		if ( m_AutoScanBox )
		{
			m_OperationPrompt.Format(_T("ϵͳ���ڶԲ�Ƭ������Ӧ�Ĳ�Ƭ����ɨ����飬�����ĵȴ�\r\n\r\n���ڽ����Զ��������"));
		}
		else
		{
			m_OperationPrompt.Format(_T("ϵͳ���ڶԲ�Ƭ����ɨ����飬�����ĵȴ�\r\n\r\n���ڽ����Զ��������"));
		}
		this->UpdateData(FALSE);
		break;
		
	case AUTOSCAN_VIEW:
// 		if ( m_AutoScanBox )
// 		{
// 			m_OperationPrompt = _T("ϵͳ���ڶԲ�Ƭ������Ӧ�Ĳ�Ƭ����ɨ����飬�����ĵȴ�");
// 			
// 		}
// 		else
// 		{
		m_OperationPrompt = _T("ϵͳ���ڶԲ�Ƭ����ɨ����飬�����ĵȴ�");		
/*		}*/	

		if ( lParam > 0  )			// ͼ�����߳��Ѿ���ʼ����
		{
			m_bImageProcess = TRUE;
		}

		if ( m_bImageProcess )			// ͼ�����߳��Ѿ���ʼ����
		{
			str.Format(_T("\r\n\r\n���ڲɼ��� %d ����Ұ��ͼ��\r\n\r\n���ڶԵ� %d ����Ұ��ͼ����м���"), m_ScanSequenceIndex+1, m_nImageProcessSequenceIndex+1);
			if ( m_bScanPostProcess )
			{
				str.Format(_T("\r\n\r\n���ڶԵ� %d ����Ұ��ͼ��������²ɼ�\r\n\r\n���ڶԵ� %d ����Ұ��ͼ����м���"), m_ScanSequenceIndex+1, m_nImageProcessSequenceIndex+1);
			}

			if ( m_bScanEnd )
			{
				str.Format(_T("\r\n\r\n��ǰ��Ƭ�ϵ���Ұͼ��ɼ����\r\n\r\n���ڶԵ� %d ����Ұ��ͼ����м���"), m_nImageProcessSequenceIndex+1);
			}
		}
		else
		{		
			str.Format(_T("\r\n\r\n���ڲɼ��� %d ����Ұ��ͼ��"), m_ScanSequenceIndex+1);
			if ( m_bScanPostProcess )
			{
				str.Format(_T("\r\n\r\n���ڶԵ� %d ����Ұ��ͼ��������²ɼ�"), m_ScanSequenceIndex+1);
			}

			if ( m_bScanEnd )
			{
				str.Format(_T("\r\n\r\n��ǰ��Ƭ�ϵ���Ұͼ��ɼ����"));
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
			m_OperationPrompt.Format(_T("ϵͳ���ڶԲ�Ƭ������Ӧ�Ĳ�Ƭ����ɨ����飬�����ĵȴ�\r\n\r\n��λ %d �ϵĲ�Ƭ�Զ��۽�ʧ�ܣ��������ɨ��������"), lParam+1);
		}
		else
		{			
			AfxMessageBox(_T("�Զ��۽�ʧ�ܣ��������ɨ��������"));
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
			m_OperationPrompt = _T("ϵͳ���ڶԲ�Ƭ������Ӧ�Ĳ�Ƭ����ɨ����飬�����ĵȴ�\r\n\r\n��ǰ��λ�ϵĲ�Ƭ��Ұ�ɼ����\r\n\r\n���ڽ��к���");
		}
		else
		{
			m_OperationPrompt = _T("ϵͳ���ڶԲ�Ƭ����ɨ����飬�����ĵȴ�\r\n\r\n��Ƭ��Ұͼ��ɼ����\r\n\r\n���ڽ��к���");
		}	
		this->UpdateData(FALSE);
		break;
	default:
		break;
	}

	return 0;
}

// ������϶Ի����˳�ʱ��������Ϣ
afx_msg LRESULT CBacilusDetectView::OnMsgJudgedlgreturnNotify(WPARAM wParam, LPARAM lParam)
{
	// ���½���ӿ���Ӧ״̬
	CCamera* pCam = CCamera::GetInstance();
	DisableGUI(GUI_BUTTON_STOPSCAN); 
	EnableGUI(	GUI_MENU_SCANMODE | GUI_MENU_MICROSCOPECONTROL | GUI_MENU_ORIGIN | GUI_MENU_SELECT
			  | GUI_BUTTON_START | GUI_BUTTON_EXIT | GUI_BUTTON_SLIDEINFO | GUI_BUTTON_CHANGEBOX
			 | GUI_BUTTON_JUDGE );  
	pCam->EnableNotifyMsg(TRUE);  // ������������ͼ��
	m_OperationPrompt = _T("1. ����ͨ��\"��һ�Ų�Ƭ\"��ť����\"ϵͳ����-->��λѡ��\"�˵��ı䲣Ƭ��λ\r\n\r\n2. ����ͨ��\"�û���Ƭ��\"��ť������Ƭ���еĲ�Ƭ,������һ���β�Ƭ��ɨ����鹤��");
	this->UpdateData(FALSE);
	return 0;
}

// ������϶Ի��򷢳��۲쵱ǰ��Ұ����Ϣ
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
		fFPS = pCam->getFPS();   // �����֡Ƶ��
	} while (fFPS < 5);
	float fZMotorSpeed = fFPS;                 // Z�Ჽ����������˶����ٶ�
	pMC->SetZMotorSpeed(fZMotorSpeed, DISTANCE_UNIT_UM);      // ����Z�����˶��ٶ�

	DisableGUI(0xFFFFFFFF);  // ��ȫ��ֹ�˵��Ͱ�ťʹ��״̬
	pCam->EnableNotifyMsg(TRUE);  // ������������ͼ��
	m_MicroscopeControlDlg.DoModal();

	return 0;
}

//============================================================================================//

// Menu
// ����ɨ��·���Ի���
void CBacilusDetectView::OnControlScan()
{
	CCamera* pCam = CCamera::GetInstance();
	pCam->EnableNotifyMsg(FALSE);  // �������ֹ���ͼ��
	DisableALLGUI();// ��ֹ���еĽ�����Ӧ�ӿ�
	m_ScanFashionDlg.DoModal();
	EnableGUI(0); // �ָ�����ӿ���Ӧ״̬
	pCam->EnableNotifyMsg(TRUE);  // ������������ͼ��
}

// ����ɨ�跽ʽ���ò˵�״̬
void CBacilusDetectView::OnUpdateControlScan(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_MenuItemStatus.Menu_ControlScan);
}

// ������΢�����ƶԻ���
void CBacilusDetectView::OnControlMicroscope()
{
	CCamera* pCam = CCamera::GetInstance();
	pCam->EnableNotifyMsg(TRUE);  // ������������ͼ��
	DisableALLGUI();// ��ֹ���еĽ�����Ӧ�ӿ�
	m_MicroscopeControlDlg.DoModal();
	Sleep(100);
	EnableGUI(0); // �ָ�����ӿ���Ӧ״̬	
	this->InvalidateRect(&m_ImageBmp_Rect, 0); // �����������е�ͼ��
}

// ������΢�����Ʋ˵�״̬
void CBacilusDetectView::OnUpdateControlMicroscope(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_MenuItemStatus.Menu_ControlMicroscope);
}

// ΢��ƽ̨�˶�����ǰ��Ƭ��ʼ����ɨ��ʱ��ĵ�һ����Ұ��λ��
void CBacilusDetectView::OnControlOrigin()
{
	CCamera* pCam = CCamera::GetInstance();
	pCam->EnableNotifyMsg(FALSE);  // �������ֹ���ͼ��
	DisableALLGUI();// ��ֹ���еĽ�����Ӧ�ӿ�

	g_Mechanism.GotoOrigin();		  // �ص���ǰ��λ����ʼ��
	
	pCam->EnableNotifyMsg(TRUE);  // ������������ͼ��
	EnableGUI(0); // �ָ�����ӿ���Ӧ״̬
}

// ���»ص�ɨ����ʼλ�ò˵�״̬
void CBacilusDetectView::OnUpdateControlOrigin(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_MenuItemStatus.Menu_ControlOrigin);
}


//============================================================================================//

// Button
// ϵͳ��ʼ��
void CBacilusDetectView::OnBnClickedButtonStart()
{
	DWORD  spantime = 0, temp = 0;
	DWORD  curtime = 0, starttime = 0;
	CCamera* pCam = CCamera::GetInstance();
	CMotorController* pMC = CMotorController::GetInstance();
	cv::Point3f	f3DStopPos = g_Mechanism.GetStopPosition();
	
	CButton* pBtn = NULL;
	int i=0;
	
	DisableGUI(0xFFFFFFFF);// ��ֹ���еĽ�����Ӧ�ӿ�
	EnableGUI(GUI_MENU_SCANMODE); 

	HCURSOR hCur  =  AfxGetApp()->LoadStandardCursor( IDC_WAIT ) ;
	::SetCursor(hCur);


	if ( ! pMC->isConnecting() )
	{
		if (!pMC->Connect())
		{
			AfxMessageBox(_T("��������MCUϵͳ"));
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
				pCam->EnableNotifyMsg(FALSE);  // �������ֹ���ͼ��
				break;
			}
			Sleep(1000);
		}
		if ( i>=3 )
		{
			AfxMessageBox(_T("�������������"));
			EnableGUI(	GUI_BUTTON_START | GUI_BUTTON_EXIT ); 
			hCur  =  AfxGetApp()->LoadStandardCursor(  IDC_ARROW ) ;
			SetCursor(hCur);
			return;
		}
	}

	pMC->WaitUntilDone();

	m_OperationPrompt = _T("ϵͳ�����ƶ���Ƭ��Ԥ��λ�ã������ĵȴ�");
	this->UpdateData(FALSE);

	if ( AfxMessageBox( _T("�뽫��Ƭ��������΢������̨�ϣ�ע�⣺��Ҫ��ײ�ﾵ�������ȷ����ťϵͳ��������"), 
			            MB_OKCANCEL|MB_ICONINFORMATION ) == IDOK )
	{
		
		// ��ʼ��ɨ��·�� 
		g_Mechanism.GetScanRoute(m_ScanRoute);
		UpdateScanRouteBmp(); // ����ɨ�����ʾ��ͼ�е�ɨ��·��
		this->InvalidateRect(&m_SlideScanBmp_Rect, 0);

		m_ScanSequenceIndex = 0;           // ��ʼ��ɨ����Ұ���
				
		DeleteDirectory(m_DefaultDirectory);  // ɾ����ʱ�ļ�Ŀ¼
		GetDocument()->CleanSlideBoxData();   // ��� m_SlideBoxData �е�����

		// XYZ���˶�����ʼλ��
		g_Mechanism.GotoOrigin();
		
		while( !pCam->AutoExpoEnable(FALSE) ) {Sleep(500);}    // ��ֹ������Զ��ع�

		// ���½���ӿ���Ӧ״̬
		EnableGUI(	GUI_MENU_SCANMODE | GUI_MENU_MICROSCOPECONTROL | GUI_MENU_ORIGIN | GUI_MENU_SELECT
				  | GUI_BUTTON_START | GUI_BUTTON_EXIT | GUI_BUTTON_SLIDEINFO | GUI_BUTTON_NEXTSLIDE | GUI_BUTTON_CHANGEBOX |GUI_BUTTON_JUDGE
				 ); 
		pCam->EnableNotifyMsg(TRUE);  // ������������ͼ��
		
		this->InvalidateRect(&m_SlideIndex_Rect, 0);
		m_OperationPrompt = _T("1. �����벣Ƭ����Ϣ�����\"��Ƭ��Ϣ\"��ť��\r\n\r\n");
		this->UpdateData(FALSE);
		hCur  =  AfxGetApp()->LoadStandardCursor(  IDC_ARROW ) ;
		SetCursor(hCur);
		return;
	}

	EnableGUI(	GUI_MENU_MICROSCOPECONTROL | GUI_BUTTON_START | GUI_BUTTON_EXIT ); 
	hCur  =  AfxGetApp()->LoadStandardCursor(  IDC_ARROW ) ;
	SetCursor(hCur);
}

// ���뵱ǰ��Ƭ��Ϣ
void CBacilusDetectView::OnBnClickedButtonSlideinfo()
{   
	CCamera* pCam = CCamera::GetInstance();
	pCam->EnableNotifyMsg(FALSE);  // �������ֹ���ͼ��
	DisableALLGUI();// ��ֹ���еĽ�����Ӧ�ӿ�
	m_OperationPrompt = _T("ע�⣺�����벣Ƭ��Ϣ��Ϣʱ����Ƭ��Ų���Ϊ�ա�");
	this->UpdateData(FALSE);

	// ͨ����Ƭ��Ϣ�Ի������ò�Ƭ�������в�Ƭ����Ϣ
	m_SlideInfoDlg.DoModal();


	if ( !GetDocument()->GetSlideID( ).IsEmpty() )
	{
		EnableGUI(GUI_BUTTON_SCAN);// �ָ�����ӿ���Ӧ״̬

		m_OperationPrompt = _T("1. ����ͨ��\"ϵͳ����-->ɨ�跽ʽ����\"�˵���Ĭ�ϵ�ɨ�跽ʽ�����޸�\r\n\r\n2. ���\"��ʼ����\"��ť��ϵͳ����Ե�ǰ��Ƭ����ɨ��");
		this->InvalidateRect(&m_SlideID_Rect, 0);    // ���²�Ƭ���
		this->UpdateData(FALSE);	
		return;	
	}
	
	// ���½���ӿ���Ӧ״̬
	EnableGUI(GUI_BUTTON_SLIDEINFO);
	pCam->EnableNotifyMsg(TRUE);  // ������������ͼ��
	m_OperationPrompt = _T("1. �����벣Ƭ����Ϣ�����\"��Ƭ��Ϣ\"��ť��\r\n\r\n");
	this->UpdateData(FALSE);

}

// ��ʼ���в�Ƭɨ��
void CBacilusDetectView::OnBnClickedButtonScan()
{	
	m_AutoScanBox = FALSE;
	if ( GetDocument()->GetSlideID().IsEmpty() )
	{
		return;
	}

	UpdateScanRouteBmp(); // ����ɨ�����ʾ��ͼ�е�ɨ��·��
	this->InvalidateRect(&m_SlideScanBmp_Rect, 0);
	DisableGUI(0xFFFFFFFF);  // ��ȫ��ֹ�˵��Ͱ�ťʹ��״̬
	m_OperationPrompt = _T("ϵͳ���ڶԲ�Ƭ����ɨ����飬�����ĵȴ�");
	this->UpdateData(FALSE);
	
	// ������ָ��Ŀ¼�½���һ���Թ�λ���������ļ��У�Ȼ�����ڸ��ļ������潨��һ���Ե�ǰʱ���������ļ��У����ϵͳ��������ʱͼ�����Ϣ�������ڸ��ļ�����
	m_Directory = 	m_DefaultDirectory;   // ��ʱ�ļ�Ŀ¼
	::CreateDirectoryW(m_Directory, NULL);
	
	m_pAutoScanWorkThread = AfxBeginThread(AutoScanSlideWorkThread, (LPVOID)this);	
	EnableGUI(GUI_BUTTON_STOPSCAN);
}

// ֹͣ��Ƭɨ�衢����Ĺ���
void CBacilusDetectView::OnBnClickedButtonScanstop()
{
	DisableGUI(0xFFFFFFFF);              // ��ȫ��ֹ�˵��Ͱ�ťʹ��״̬
	m_EventStopAutoScan.SetEvent();      // ֪ͨ�Զ�ɨ���̣߳�����ɨ��ɼ�����
}

// �������
void CBacilusDetectView::OnBnClickedButtonJudge()
{
	DisableALLGUI();// ��ֹ���еĽ�����Ӧ�ӿ�
	m_pJudgeDlg->Create(CJudgeDlg::IDD, this);
	m_pJudgeDlg->ShowWindow(SW_SHOW);    
}

// ��һ����λ
// void CBacilusDetectView::OnBnClickedButtonNextslide()
// {
// 	WorkPositionSelect( g_Mechanism.GetWorkPositionID() + 1 );
// }

// �û���Ƭ�еĹ�����ϵͳ��ʼ��һ��
void CBacilusDetectView::OnBnClickedButtonChangebox()
{
	CCamera* pCam = CCamera::GetInstance();
	pCam->EnableNotifyMsg(FALSE);  // �������ֹ���ͼ��
	if ( AfxMessageBox(_T("�Ƿ��û���Ƭ��"), MB_OKCANCEL|MB_ICONQUESTION ) == IDOK )
	{
		OnBnClickedButtonStart();
	}
	else
	{
		pCam->EnableNotifyMsg(TRUE);  // ������������ͼ��
		return;
	}	
}

// ϵͳ�˳�
void CBacilusDetectView::OnBnClickedButtonExit()
{
	AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
}

// �ӿ�
// �����˳�֮ǰ���رո����豸���ͷ���Դ
void  CBacilusDetectView::SystemExiting()
{
	cv::Point3f	f3DStopPos = g_Mechanism.GetStopPosition();
	CCamera* pCam = CCamera::GetInstance();
	CMotorController* pMC = CMotorController::GetInstance();
	DWORD  spantime = 0, starttime = 0, curtime = 0;

	if ( pMC->isConnecting() )
	{
		pCam->EnableNotifyMsg(FALSE);  // �������ֹ���ͼ��
	}
	DisableALLGUI();
	//DeleteDirectory(m_DefaultDirectory);  // ɾ����ʱ�ļ�Ŀ¼

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



/****************************** �̺߳��� *************************************/
// ֹͣ���鰴ť�����£�ɨ����鹤���̱߳���ֹ
void WhetherStopWorkThread(CBacilusDetectView* pDetectView)
{
	HWND hWnd = pDetectView->GetSafeHwnd();	
	HANDLE hHandleStop = HANDLE(pDetectView->m_EventStopAutoScan);
	if ( WAIT_OBJECT_0 == ::WaitForSingleObject(hHandleStop,0) )   // ֹͣ���鰴ť�����£�ɨ����鹤���̱߳���ֹ
	{
		pDetectView->m_bScanEnd = TRUE;                                                       // ��Ұɨ��ɼ�ͼ��������
		::PostMessage ( hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_VIEW, (LPARAM)(-1) );   // ����������	
		if ( pDetectView->m_bImageProcessThread )
		{
			(pDetectView->m_EventStopImageProcess).SetEvent();				                      // ֪ͨͼ�����̣߳�ɨ��ɼ�ͼ�����Ѿ����	
			while( !pDetectView->m_bImageProcessEnd  )	{	;	}								  // �ȴ�ͼ�����߳̽���					                                   
		}
		::PostMessage (hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_END, (LPARAM)0);			  // ����������
		::AfxEndThread(0, TRUE);
	}
}


void AutoScanSlide(CBacilusDetectView* pDetectView)
{
	CTime curtime = CTime::GetCurrentTime(); 

	HWND hWnd = pDetectView->GetSafeHwnd();	
	HANDLE hHandleStop = HANDLE(pDetectView->m_EventStopAutoScan);
	
	// ��ʼ���Զ�ɨ�衢�۽���ͼ���������
	PSCANROUTE pScanRoute = g_Mechanism.GetScanRoute();
	CBacilusDetectDoc* m_pDocument =(CBacilusDetectDoc*) pDetectView->GetDocument();	// Document��ַ
	int& nScanSequenceIndex = pDetectView->m_ScanSequenceIndex;							// ��Ұ��ţ����߳��лᱻ�ı�
	int* pnImageProcessSequenceIndex = &(pDetectView->m_nImageProcessSequenceIndex);	// ͼ������ţ����߳��лᱻ�ı�
	volatile BOOL* g_pbImageProcessEnd = &(pDetectView->m_bImageProcessEnd);		    // ȫ��ͬ��������ͼ�����߳̽���
	volatile BOOL& g_bImageProcessThread = pDetectView->m_bImageProcessThread;			// ȫ��ͬ��������ͼ�����߳��Ѿ�����
	volatile BOOL& g_bImageProcess = pDetectView->m_bImageProcess;						// ȫ��ͬ��������ͼ�����߳��Ѿ���ʼ����������ͼ��
	volatile BOOL& g_bScanPostProcess = pDetectView->m_bScanPostProcess;				// ȫ��ͬ����������һ��ɨ��ɼ�ͼ�������������к������п��ܴ���۽�����Ұ�������¾۽�
	volatile BOOL& g_bScanEnd = pDetectView->m_bScanEnd;				                // ȫ��ͬ����������Ұɨ��ɼ�ͼ��������
	CCamera* pCam = CCamera::GetInstance();
	CMotorController* pMC = CMotorController::GetInstance();

	int iScanCount = pScanRoute->ScanSequence.GetSize();  // �����Ҫɨ����Ұ������
	m_pDocument->InitialFOVCount( iScanCount);			// ��ʼ����Ұ�ĸ���
	cv::Size      ImageSize = pCam->GetCameraImageSize();					// ��������㵽��ͼ��Ĵ�С	
	CCriticalSection    ProcessCritSection;
	CSingleLock singleLock(&ProcessCritSection);

	queue <STRUCTIMAGEINFO>* pqImageInfo = &(pDetectView->m_qImageInfo);
	while ( ! pqImageInfo->empty() )                                     // ���ͼ�����
	{
		pqImageInfo->pop();		
	}

	// ��ʼ��ȫ��ͬ������
	*pnImageProcessSequenceIndex = 0;
	*g_pbImageProcessEnd = FALSE;	
	g_bScanPostProcess = FALSE;
	g_bScanEnd = FALSE;
	g_bImageProcessThread = FALSE;
	g_bImageProcess = FALSE;



	// ��ʼ���Զ��۽���
	// �˶��������Զ������λ��, �����Զ�����
	CAutoAdjustLight AutoAdjustLight;
	::PostMessage(hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOLIGHT, (LPARAM)0);              // ����������
	BOOL bSuccess = FALSE;
	if ( AutoAdjustLight.InitAutoAdjustLight(pDetectView) )
	{
		bSuccess = AutoAdjustLight.Work();		
	}
	if ( !bSuccess )
	{
		AfxMessageBox(_T("�Զ�����ʧ�ܣ���ͨ��������ť����΢���Ĺ�Դ���ȵ���Ȼ�����½�����Ӧ�Ĳ���"));
		::SendMessage(hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_END, (LPARAM)0);  // ����������
		::AfxEndThread(0, TRUE);
	}


	// ���Զ�����֮���ͼ�񱣴�Ϊ�ο�ͼ��
	cv::Mat   pcvImage = pCam->GetImage(); // ��������㵽��ͼ��
	::WaitForSingleObject(pCam->CapturedEvent(),INFINITE);
	::WaitForSingleObject(pCam->CapturedEvent(),INFINITE);  //�ȴ��µ���������㵽��ͼ������
	SaveImage2BMP(pDetectView->m_Directory, 0, pcvImage);

	CTime t1= CTime::GetCurrentTime(); 
	//====================================================================================//	

	// ��������ͼ�����ʱ�ļ�Ŀ¼		
	CString strDirectory = pDetectView->m_Directory;                  // ��ʱ�ļ�Ŀ¼
	CString str1;
	str1.Format(_T("\\Temp"));
	strDirectory += str1;
	::CreateDirectoryW(strDirectory, NULL);	
	str1 = curtime.Format(_T("\\%Y-%m-%d_%H-%M-%S"));
	strDirectory += str1;
	::CreateDirectoryW(strDirectory, NULL);                           
	m_pDocument->SetDirectory( strDirectory);     // ����ص����ݴ洢�� Document�� m_SlideBoxData ���ݽṹ֮��


	// ��ʼ���Զ��۽���
	CAutoFocus   AutoFocusWork;
	if ( ! AutoFocusWork.InitAotoFocus(pDetectView) )  // ���ɨ����Ұ���˽ṹ���ݣ���ʼ���Զ��۽���
	{
	AfxMessageBox(_T("�Զ��۽���ʼ��ʧ��"));
	::SendMessage(hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_FOCUS_TERMINATE, (LPARAM)0);  // ����������
	::AfxEndThread(0, TRUE);
	}

	// ���ò�Ƭ�ϵĺ�ʮ�ֹ���Ŀ��㽹ƽ��ĳ�ʼλ��
	//if ( ! AutoFocusWork.EstInitFocusPosition() )
	//{
	//	AfxMessageBox(_T("�Զ��۽�ʧ�ܣ��뽫��ʼɨ��������ڲ�Ƭ��ʮ�ָ�����Ȼ�����¼��"));
	//	::SendMessage(hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_FOCUS_TERMINATE, (LPARAM)nWorkPositionIndex);  // ����������
	//	::AfxEndThread(0, TRUE);
	//}
///

	
	CTime t2= CTime::GetCurrentTime(); 

	// ����ͼ�����߳�
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

	// �Զ�ɨ�衢�۽�����ÿ����Ұ������ͼ�񱣴�����ʱ�ļ�Ŀ¼�У�ͬʱ��ͼ�����ڴ��еĵ�ַ����FIFO�Ķ������ݽṹ֮�У��Թ�Ŀ��ʶ����߳�ʹ��	
	for ( nScanSequenceIndex=0; nScanSequenceIndex<iScanCount; nScanSequenceIndex++ )
	{		
		// �˶�����Ҫ�۽�����Ұ 
		g_Mechanism.GotoFOV(nScanSequenceIndex); // �˶�������Ұ�� 
		cv::Point2f curpos;
		pMC->GetXYMotorPosition(curpos,DISTANCE_UNIT_UM);			// ��õ�ǰ��Ұ��XYƽ̨�ϵľ�������
		pDetectView->AddPoint2SlideScanBmp(curpos);										// ����ǰ����Ұ��ɨ�����ʾ��ͼ�б�ʶ����
		::PostMessage (hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_VIEW, (LPARAM)0);  // ����������

		// ��õ�ǰ��Ұ�����ͼ��		
		pCam->EnableNotifyMsg(TRUE);

		cv::Mat pcvBestImage ;
		float fBestFocusPos = AutoFocusWork.GetCurFOVBestImage(pcvBestImage, curpos, nScanSequenceIndex);

// 		cv::Mat pcvBestImage;
// 		pCam->GetImage().copyTo(pcvBestImage);
//		float fBestFocusPos = 0;

		pCam->EnableNotifyMsg(FALSE);

		// �����ͼ�񱣴�����ʱ�ļ�Ŀ¼��
		CString strFileName = SaveImage2JPG(strDirectory, nScanSequenceIndex, pcvBestImage);  //����ͼ��

		singleLock.Lock();

		// ����ص����ݴ洢�� Document�� m_SlideBoxData ���ݽṹ֮��
		m_pDocument->SetSlideScanStatus(TRUE);			 // ��ɨ��
		m_pDocument->SetFOVCount( nScanSequenceIndex+1);  // ��ǰ��ɨ����Ұ����Ŀ
		m_pDocument->SetFOVPosition( nScanSequenceIndex, curpos.x, curpos.y, fBestFocusPos);  // ��ǰ��Ұλ��
		m_pDocument->SetFOVCheckedStatus(nScanSequenceIndex, FALSE);						 // ��ǰ��Ұδ���	

		MATRIXINDEX curFOVMatrix = pScanRoute->ScanSequence[nScanSequenceIndex];
		pFOV3DPosTopology[ pScanRoute->ColNum * curFOVMatrix.row + curFOVMatrix.col ] = cv::Point3f( curpos.x, curpos.y, fBestFocusPos );
		pnIndex[ pScanRoute->ColNum * curFOVMatrix.row + curFOVMatrix.col ] = nScanSequenceIndex;

		// ��ͼ��ĵ�ַ����FIFO�Ķ���
		STRUCTIMAGEINFO structImageInfo;
		structImageInfo.nScanSequenceIndex = nScanSequenceIndex;
		structImageInfo.strFileName = strFileName;
		pqImageInfo->push(structImageInfo);

		singleLock.Unlock(); 

		WhetherStopWorkThread(pDetectView);			// ֹͣ���鰴ť�����£�ɨ����鹤���̱߳���ֹ
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
		File.Write(& pScanRoute->ColNum, sizeof(UINT));     // ����
		File.Write(& pScanRoute->RowNum, sizeof(UINT));		// ����
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

	//// ����ɨ�����Ұ�۽�������з���������ɨ�����˽ṹ�ж���Щ��Ұ��Ҫ���¾۽�
	//g_bScanPostProcess = TRUE;
	//int iPostProcessFOVCount = AutoFocusWork.AnalyzeScanedFOVs();
	//if ( iPostProcessFOVCount > 0 )
	//{
	//	for ( int i = 0; i < iPostProcessFOVCount; i++ )
	//	{
	//		::PostMessage (hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_VIEW, (LPARAM)0);  // ����������

	//		cv::Mat pcvBestImage;
	//		cv::Point3f  cvFOV3DPos;
	//		nScanSequenceIndex = AutoFocusWork.GetPostProcessFOVBestImage(&pcvBestImage, &cvFOV3DPos, i);

	//		CString str1;
	//		str1.Format(_T("%3.0f(%3d)   "), cvFOV3DPos.z, nScanSequenceIndex);
	//		str += str1;
	//		
	//		// �����ͼ�񱣴�����ʱ�ļ�Ŀ¼��
	//		CString strFileName = SaveImage2JPG(strDirectory, nScanSequenceIndex, pcvBestImage);  //����ͼ��

	//		m_pDocument->SetFOVPosition(nWorkPositionIndex, nScanSequenceIndex, cvFOV3DPos.x, cvFOV3DPos.y, cvFOV3DPos.z);  // ��ǰ��Ұλ��

	//		WhetherStopWorkThread(pDetectView);			// ֹͣ���鰴ť�����£�ɨ����鹤���̱߳���ֹ
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
	// �Զ�����˸˾�����
	g_bScanEnd = TRUE;                                                                    // ��Ұɨ��ɼ�ͼ��������
	::PostMessage ( hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_VIEW, (LPARAM)(-1) );   // ����������	
	if ( g_bImageProcessThread )
	{
		(pDetectView->m_EventStopImageProcess).SetEvent();									  // ֪ͨͼ�����̣߳�ɨ��ɼ�ͼ�����Ѿ����	
		while( !(*g_pbImageProcessEnd)  ) 	{ ;	}		           						      // �ȴ�ͼ�����߳̽���
	}
::SendMessage(hWnd, MSG_SCANTHREAD_NOTIFY, (WPARAM)AUTOSCAN_END, (LPARAM)0);  // ����������

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
// �Զ�ɨ����鲣Ƭ�߳�
UINT __cdecl AutoScanSlideWorkThread(LPVOID pParam)
{	
	CBacilusDetectView* pDetectView = (CBacilusDetectView*) pParam;
	
	AutoScanSlide(pDetectView);

	return 0;
}

// ͼ�����߳�
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

	// ��ȡ�ο�ͼ�� 
	CString strFileName = strDirectory + _T("\\0.BMP");
	cv::Mat  pcvImage = LoadImage2Color(strFileName);
	cv::Mat pcvGrayImg;
	cv::cvtColor(pcvImage, pcvGrayImg, CV_BGR2GRAY);//ת���ɻҶ�ͼ��


	
	while(1)
	{
		// ��ʼ���������ʣ�µ�ͼ��
		if ( ! pqImageInfo->empty() )
		{
			singleLock.Lock();
			STRUCTIMAGEINFO structImageInfo = pqImageInfo->front();
			singleLock.Unlock();

			(*pnImageProcessSequenceIndex) = structImageInfo.nScanSequenceIndex;	
			::PostMessage( hWnd, uMsg, (WPARAM)AUTOSCAN_VIEW, (LPARAM)1 );   // ����������				

			cv::Mat  pcvImage = LoadImage2Color(structImageInfo.strFileName);

			CProSingleImage imgpro(pcvImage);  // ��˸˾�����㷨��
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

		if ( WAIT_OBJECT_0 == ::WaitForSingleObject(hHandleStop,0) )    // �յ�ֹͣͼ��ɼ�ֹͣ����
		{
			bStop = TRUE;
		}
		
		if ( bStop && pqImageInfo->empty() )   // �յ�ֹͣͼ��ɼ�ֹͣ������Ҷ�����û��ͼ�������̱߳���ֹ
		{
			::PostMessage( hWnd, uMsg, (WPARAM)AUTOSCAN_POSTPROCESS, (LPARAM)1 );   // ����������	
						
			// �������ֹͣ��Ƭɨ�衢����Ĺ���
			singleLock.Lock();
			(*pbImageProcessEnd) = TRUE;
			singleLock.Unlock(); 
			return 0;
		}		
	}
	
	(*pbImageProcessEnd) = TRUE;
	return 0;
}
