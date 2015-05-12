// ScanFashion.cpp : 实现文件
//

#include "stdafx.h"
#include "BacilusDetect.h"
#include "ScanFashionDlg.h"
#include "afxdialogex.h"
#include <memory>
#include <math.h>
#include "Mechanism.h"

extern CMechanism		g_Mechanism;

// CScanFashionDlg 对话框
// 实现扫描方式设置

IMPLEMENT_DYNAMIC(CScanFashionDlg, CDialogEx)

CScanFashionDlg::CScanFashionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScanFashionDlg::IDD, pParent)
	, m_hSlidBitmap(NULL)
	, m_slidebmp_width(380)
	, m_slidebmp_height(552)
	, m_slidebmp_byteCount(m_slidebmp_width*m_slidebmp_height*4)
	, m_scan_fashion_display_ratio(25000.0/285)
	, m_slidebmp_origin(332,454)
	, m_ninepoint_horizon_distance(6)   // mm
	, m_ninepoint_vertical_distance(6)  // mm
	, m_continue_horizon_overlap(0)
	, m_continue_vertical_overlap(0)
	, m_area_updown(0)
	, m_area_leftright(0)
	, m_mechanical_horizon(25000)
	, m_mechanical_vertical(26000)
	, m_view_width(87.72)     // um
	, m_view_height(66.048)   // um
{
	// default position of the first view
	m_rightdown_position.x = 6500;  // um
	m_rightdown_position.y = 6000;  // um

	// default distributed fashion: average
	m_view_distribute = VIEW_DISTRIBUTE_AVERAGE;
	m_view_column_num = 20;
	m_view_row_num = 15;
	m_average_horizon_distance = 631.579;      // um
	m_average_vertical_distance = 857.143;     // um

	// default scan sequence
	m_bBestScanRoute = TRUE;
	m_scan_origin = SCAN_ORIGIN_RIGHT_DOWN;   // start view: rightdown
	m_scan_fashion = SCAN_FASHION_HORIZON;   // vertical

	control_change();

	g_Mechanism.SetScanRoute(m_view_row_num, m_view_column_num, m_area_topology, m_area_scan_sequence);
}

CScanFashionDlg::~CScanFashionDlg()
{
}

// 类为外界提供数据的接口
void  CScanFashionDlg::getScanRoute(SCANROUTE& scanroute)
// Output: scanroute
{
	scanroute.RowNum = m_view_row_num;
	scanroute.ColNum = m_view_column_num;

	scanroute.ScanTopology.SetSize(m_area_topology.GetSize());
	scanroute.ScanTopology.Copy(m_area_topology);

	scanroute.ScanSequence.SetSize(m_area_scan_sequence.GetSize());
	scanroute.ScanSequence.Copy(m_area_scan_sequence);
}

void CScanFashionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TOTAL_NUM, m_view_total_num);
	DDV_MinMaxUInt(pDX, m_view_total_num, 1, 160000);
	DDX_Text(pDX, IDC_EDIT_HORIZON_NUM, m_view_column_num);
	DDV_MinMaxUInt(pDX, m_view_column_num, 1, 400);
	DDX_Text(pDX, IDC_EDIT_VERTICAL_NUM, m_view_row_num);
	DDV_MinMaxUInt(pDX, m_view_row_num, 1, 400);
	DDX_Text(pDX, IDC_AVERAGE_HORIZON_DISTANCE, m_average_horizon_distance);
	DDV_MinMaxUInt(pDX, m_average_horizon_distance, 0, 25000);
	DDX_Text(pDX, IDC_AVERAGE_VERTICAL_DISTANCE, m_average_vertical_distance);
	DDV_MinMaxUInt(pDX, m_average_vertical_distance, 0, 25000);
	DDX_Text(pDX, IDC_NINEPOINT_HORIZON_DISTANCE, m_ninepoint_horizon_distance);
	DDV_MinMaxFloat(pDX, m_ninepoint_horizon_distance, 0, 25);
	DDX_Text(pDX, IDC_NINEPOINT_VERTICAL_DISTANCE, m_ninepoint_vertical_distance);
	DDV_MinMaxFloat(pDX, m_ninepoint_vertical_distance, 0, 25);
	DDX_Text(pDX, IDC_CONTINUE_HORIZON_OVERLAP, m_continue_horizon_overlap);
	DDV_MinMaxFloat(pDX, m_continue_horizon_overlap, 0, 1);
	DDX_Text(pDX, IDC_CONTINUE_VERTICAL_OVERLAP, m_continue_vertical_overlap);
	DDV_MinMaxFloat(pDX, m_continue_vertical_overlap, 0, 1);
	DDX_Radio(pDX, IDC_RADIO_AVERAGE, m_view_distribute);
	DDV_MinMaxInt(pDX, m_view_distribute, 0, 2);
	DDX_Radio(pDX, IDC_RADIO_LEFTTOP, m_scan_origin);
	DDV_MinMaxInt(pDX, m_scan_origin, 0, 3);
	DDX_Radio(pDX, IDC_RADIO_VERTICAL, m_scan_fashion);
	DDV_MinMaxInt(pDX, m_scan_fashion, 0, 1);
	DDX_Text(pDX, IDC_AREA_HORIZON, m_area_horizon);
	DDX_Text(pDX, IDC_AREA_VERTICAL, m_area_vertical);
	DDX_Control(pDX, IDC_PICTURE_SLIDE, m_slide_scan_area);
	DDX_Text(pDX, IDC_AREA_UPDOWN, m_area_updown);
	DDV_MinMaxFloat(pDX, m_area_updown, 0, 28);
	DDX_Text(pDX, IDC_AREA_LEFTRIGHT, m_area_leftright);
	DDV_MinMaxFloat(pDX, m_area_leftright, 0, 26);
	DDX_Control(pDX, IDC_RADIO_VERTICAL, m_ctl_scan_fashion_vertical);
	DDX_Control(pDX, IDC_RADIO_HORIZON, m_ctl_scan_fashion_horizon);
	DDX_Control(pDX, IDC_RADIO_LEFTTOP, m_ctl_scan_origin_lefttop);
	DDX_Control(pDX, IDC_RADIO_LEFTDOWN, m_ctl_scan_origin_leftdown);
	DDX_Control(pDX, IDC_RADIO_RIGHTTOP, m_ctl_scan_origin_righttop);
	DDX_Control(pDX, IDC_RADIO_RIGHTDOWN, m_ctl_scan_origin_rightdown);

	if (m_hSlidBitmap)
	{
		m_slide_scan_area.SetBitmap(m_hSlidBitmap);		
	}	
	DDX_Control(pDX, IDC_CHECK_BESTSCANROUTE, m_ctl_BestScanRoute);
}


BEGIN_MESSAGE_MAP(CScanFashionDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_HORIZON_NUM, &CScanFashionDlg::OnEnChangeEditHorizonNum)
	ON_EN_CHANGE(IDC_EDIT_VERTICAL_NUM, &CScanFashionDlg::OnEnChangeEditVerticalNum)
	ON_BN_CLICKED(IDC_RADIO_NINEPOINT, &CScanFashionDlg::OnBnClickedRadioNinepoint)
	ON_BN_CLICKED(IDC_RADIO_AVERAGE, &CScanFashionDlg::OnBnClickedRadioAverage)
	ON_BN_CLICKED(IDC_RADIO_CONTINUE, &CScanFashionDlg::OnBnClickedRadioContinue)
	ON_BN_CLICKED(IDC_BUTTON_AREA_UP, &CScanFashionDlg::OnBnClickedButtonAreaUp)
	ON_BN_CLICKED(IDC_BUTTON_AREA_DOWN, &CScanFashionDlg::OnBnClickedButtonAreaDown)
	ON_BN_CLICKED(IDC_BUTTON_AREA_LEFT, &CScanFashionDlg::OnBnClickedButtonAreaLeft)
	ON_BN_CLICKED(IDC_BUTTON_AREA_RIGHT, &CScanFashionDlg::OnBnClickedButtonAreaRight)
	ON_BN_CLICKED(IDC_RADIO_LEFTTOP, &CScanFashionDlg::OnBnClickedRadioLefttop)
	ON_BN_CLICKED(IDC_RADIO_RIGHTTOP, &CScanFashionDlg::OnBnClickedRadioRighttop)
	ON_BN_CLICKED(IDC_RADIO_LEFTDOWN, &CScanFashionDlg::OnBnClickedRadioLeftdown)
	ON_BN_CLICKED(IDC_RADIO_RIGHTDOWN, &CScanFashionDlg::OnBnClickedRadioRightdown)
	ON_BN_CLICKED(IDC_RADIO_VERTICAL, &CScanFashionDlg::OnBnClickedRadioVertical)
	ON_BN_CLICKED(IDC_RADIO_HORIZON, &CScanFashionDlg::OnBnClickedRadioHorizon)
	ON_BN_CLICKED(IDC_APPLY, &CScanFashionDlg::OnBnClickedApply)
	ON_BN_CLICKED(IDC_CHECK_BESTSCANROUTE, &CScanFashionDlg::OnBnClickedCheckBestscanroute)
END_MESSAGE_MAP()

BOOL CScanFashionDlg::generate_area_topology()
{
	int i, j, sub_i, sub_j;
	UINT sub_area_dim;
	float tf;
	cv::Point2f view_position; 

	// Step1: Compute the row and column numbers
	m_view_total_num = m_view_row_num * m_view_column_num;
	if ( m_view_total_num <= 0 )
	{
		return FALSE;
	}

	m_area_topology.RemoveAll();

	switch (m_view_distribute)
	{
	case VIEW_DISTRIBUTE_AVERAGE:

		// Step2: Generate the topology of the views in the scan area
		m_area_topology.SetSize(m_view_total_num);
		for ( i=0; i<m_view_row_num; i++ )
		{			  
			tf = m_rightdown_position.y + (i * m_average_vertical_distance);
			if ( (tf<0) || (tf > m_mechanical_vertical) )
			{
				AfxMessageBox(_T("扫描范围超出机械行程"));
				return FALSE;
			}

			view_position.y = tf;
			for ( j=0; j<m_view_column_num; j++ )
			{
				tf = m_rightdown_position.x + (j * m_average_horizon_distance);
				if ( (tf<0) || (tf > m_mechanical_horizon) )
				{
					AfxMessageBox(_T("扫描范围超出机械行程"));
					return FALSE;
				}

				view_position.x = tf;
				m_area_topology[i*m_view_column_num+j] = view_position;
			}
		}

		// Step3: Compute total scan area
		m_area_horizon = (m_view_column_num - 1) * m_average_horizon_distance / 1000;          // mm
		m_area_vertical = (m_view_row_num - 1) * m_average_vertical_distance / 1000;           // mm
	
		break;

	case VIEW_DISTRIBUTE_NINEPOINT:

		// Step1: ReCompute the row and column numbers	
		if ( m_view_total_num < 9 )
		{
			m_view_total_num = 9;
		}
		sub_area_dim = ceil(sqrt(m_view_total_num/9.0));
		m_view_row_num = sub_area_dim * 3;
		m_view_column_num = sub_area_dim * 3;
		m_view_total_num = m_view_row_num * m_view_column_num;

		// Step2: Generate the topology of the views in the scan area
		m_area_topology.SetSize(m_view_total_num);
		for ( sub_i=0; sub_i<3; sub_i++ )
		{
			for ( sub_j=0; sub_j<3; sub_j++ )
			{
				for ( i=0; i<sub_area_dim; i++ )
				{
					tf = m_rightdown_position.y + (sub_i*m_ninepoint_vertical_distance*1000) + (i * m_view_height);
					if ( (tf<0) || (tf > m_mechanical_vertical) )
					{
						AfxMessageBox(_T("扫描范围超出机械行程"));
						return FALSE;
					}

					view_position.y = tf;
					for ( j=0; j<sub_area_dim; j++ )
					{
						tf = m_rightdown_position.x + (sub_j*m_ninepoint_horizon_distance*1000) + (j * m_view_width);
						if ( (tf<0) || (tf > m_mechanical_horizon) )
						{
							AfxMessageBox(_T("扫描范围超出机械行程"));
							return FALSE;
						}

						view_position.x = tf;
						m_area_topology[(sub_i*sub_area_dim+i)*m_view_column_num+(sub_j*sub_area_dim+j)] = view_position;
					}
				}
			}
		}

		// Step3: Compute total scan area
		m_area_horizon = 2 * m_ninepoint_horizon_distance + sub_area_dim*m_view_width/1000;          // mm
		m_area_vertical = 2 * m_ninepoint_vertical_distance + sub_area_dim*m_view_height/1000;       // mm

		break;

	case VIEW_DISTRIBUTE_CONTINUE:

		// Step2: Generate the topology of the views in the scan area
		m_area_topology.SetSize(m_view_total_num);
		for ( i=0; i<m_view_row_num; i++ )
		{
			tf = m_rightdown_position.y + (i * m_view_width * (1-m_continue_vertical_overlap) );
			if ( (tf<0) || (tf > m_mechanical_vertical) )
			{
				AfxMessageBox(_T("扫描范围超出机械行程"));
				return FALSE;
			}

			view_position.y = tf;
			for ( j=0; j<m_view_column_num; j++ )
			{
				tf = m_rightdown_position.x + (j * m_view_height * (1-m_continue_horizon_overlap) );
				if ( (tf<0) || (tf > m_mechanical_horizon) )
				{
					AfxMessageBox(_T("扫描范围超出机械行程"));
					return FALSE;
				}

				view_position.x = tf;
				m_area_topology[i*m_view_column_num+j] = view_position;
			}
		}

		// Step3: Compute total scan area
		m_area_horizon = m_view_column_num * m_view_width * (1-m_continue_vertical_overlap) /1000;              // mm
		m_area_vertical = m_view_row_num * m_view_height * (1-m_continue_horizon_overlap) /1000;           // mm

		break;

	default:
		AfxMessageBox(_T("Error"));
		return FALSE;
	}

	return TRUE;
}

void  CScanFashionDlg::set_subarea_scan_sequence(UINT& scan_index,
												 MATRIXINDEX right_down, UINT width, UINT height,
												 SCAN_ORIGIN origin, SCAN_FASHION fashion)
{
	int i, j;

	int j_start, j_end, j_change;
	int i_even_start, i_even_end, i_even_change;
	int i_odd_start, i_odd_end, i_odd_change;

	int i_start, i_end, i_change;
	int j_even_start, j_even_end, j_even_change;
	int j_odd_start, j_odd_end, j_odd_change;

	BOOL even;

	if ( fashion == SCAN_FASHION_VERTICAL)
	{
		if ( (origin == SCAN_ORIGIN_LEFT_TOP) || (origin == SCAN_ORIGIN_LEFT_DOWN) )
		{
			j_start = right_down.col + width -1;
			j_end = right_down.col;
			j_change = -1;
		}
		else
		{
			j_start = right_down.col;
			j_end = right_down.col + width -1;
			j_change = 1;
		}

		if ( (origin == SCAN_ORIGIN_LEFT_TOP) || (origin == SCAN_ORIGIN_RIGHT_TOP) )
		{
			i_even_start = right_down.row + height - 1;
			i_even_end = right_down.row;
			i_even_change = -1;
			i_odd_start = right_down.row;
			i_odd_end = right_down.row + height - 1;
			i_odd_change = 1;
		}
		else
		{
			i_even_start = right_down.row;
			i_even_end = right_down.row + height - 1;
			i_even_change = 1;
			i_odd_start = right_down.row + height - 1;
			i_odd_end = right_down.row;
			i_odd_change = -1;
		}

		even = TRUE;
		for ( j=j_start; j!=(j_end+j_change); j+=j_change )
		{
			if ( even )
			{
				for ( i=i_even_start; i!=(i_even_end+i_even_change); i+=i_even_change )
				{
					m_area_scan_sequence[scan_index].row = i;
					m_area_scan_sequence[scan_index].col = j;
					scan_index++;
				}
				even = FALSE;
			}
			else
			{
				for ( i=i_odd_start; i!=(i_odd_end+i_odd_change); i+=i_odd_change )
				{
					m_area_scan_sequence[scan_index].row = i;
					m_area_scan_sequence[scan_index].col = j;
					scan_index++;
				}
				even = TRUE;
			}
		}
	}
	else   // Horizon first
	{
		if ( (origin == SCAN_ORIGIN_LEFT_TOP) || (origin == SCAN_ORIGIN_RIGHT_TOP) )
		{
			i_start = right_down.row + height -1;
			i_end = right_down.row;
			i_change = -1;
		}
		else
		{
			i_start = right_down.row;
			i_end = right_down.row + height -1;
			i_change = 1;
		}

		if ( (origin == SCAN_ORIGIN_LEFT_TOP) || (origin == SCAN_ORIGIN_LEFT_DOWN) )
		{
			j_even_start = right_down.col + width - 1;
			j_even_end = right_down.col;
			j_even_change = -1;
			j_odd_start = right_down.col;
			j_odd_end = right_down.col + width - 1;
			j_odd_change = 1;
		}
		else
		{
			j_even_start = right_down.col;
			j_even_end = right_down.col + width - 1;
			j_even_change = 1;
			j_odd_start = right_down.col + width - 1;
			j_odd_end = right_down.col;
			j_odd_change = -1;			
		}

		even = TRUE;
		for ( i=i_start; i!=(i_end+i_change); i+=i_change )
		{
			if ( even )
			{
				for ( j=j_even_start; j!=(j_even_end+j_even_change); j+=j_even_change )
				{
					m_area_scan_sequence[scan_index].row = i;
					m_area_scan_sequence[scan_index].col = j;
					scan_index++;
				}
				even = FALSE;
			}
			else
			{
				for ( j=j_odd_start; j!=(j_odd_end+j_odd_change); j+=j_odd_change )
				{
					m_area_scan_sequence[scan_index].row = i;
					m_area_scan_sequence[scan_index].col = j;
					scan_index++;
				}
				even = TRUE;
			}
		}
	}
}

void CScanFashionDlg::set_scan_sequence()
{
	UINT scan_index = 0;
	MATRIXINDEX right_down;

	// Set the scan sequence

	if ( m_view_distribute == VIEW_DISTRIBUTE_NINEPOINT)  // 九点法扫描顺序比较特殊，系统自动产生最佳路径
	{
		UINT sub_area_dim = (UINT) ceil(sqrt(m_view_total_num/9.0));
		m_view_row_num = sub_area_dim * 3;
		m_view_column_num = sub_area_dim * 3;
		m_view_total_num = m_view_row_num * m_view_column_num;

		m_area_scan_sequence.RemoveAll();
		m_area_scan_sequence.SetSize(m_view_total_num);

		scan_index = 0;    

		/*	|-----|-----|-----|
			|(2,2)|(2,1)|(2,0)|
			|-----|-----|-----|
			|(1,2)|(1,1)|(1,0)|
			|-----|-----|-----|
			|(0,2)|(0,1)|(0,0)|
			|-----|-----|-----|	
		*/

		for ( int i = 0; i < 3; i++ )
		{
			right_down.row = 0;
			right_down.col = i * sub_area_dim;
			set_subarea_scan_sequence( scan_index, right_down, sub_area_dim, sub_area_dim, 
									   SCAN_ORIGIN_RIGHT_DOWN, SCAN_FASHION_HORIZON);
		}
		
		for ( int i = 2; i >= 0; i-- )
		{
			right_down.row = sub_area_dim;
			right_down.col = i * sub_area_dim;
			set_subarea_scan_sequence( scan_index, right_down, sub_area_dim, sub_area_dim, 
									   SCAN_ORIGIN_LEFT_DOWN, SCAN_FASHION_HORIZON);
		}

		for ( int i = 0; i < 3; i++ )
		{
			right_down.row = 2*sub_area_dim;
			right_down.col = i * sub_area_dim;
			set_subarea_scan_sequence( scan_index, right_down, sub_area_dim, sub_area_dim, 
									   SCAN_ORIGIN_RIGHT_DOWN, SCAN_FASHION_HORIZON);
		}
	}
	else if ( m_view_distribute == VIEW_DISTRIBUTE_AVERAGE)  // 视野拓扑结构平局分布方式的扫描顺序比较特殊，首先根据自动聚焦策略的要求安排第一、二行（列）的扫描顺序，然后在按照要求生成其它视野的扫描顺序
	{
		m_area_scan_sequence.RemoveAll();
		m_area_scan_sequence.SetSize(m_view_total_num);
		
		if ( m_bBestScanRoute )
		{
			scan_index = 0;

			if ( (m_view_row_num == 1) || (m_view_column_num == 1) )
			{
				int iWidth, iHeight;
				right_down.row = (int)(ceilf((float)m_view_row_num / 2)) - 1;
				right_down.col = (int)(ceilf((float)m_view_column_num / 2)) - 1;
				iHeight = m_view_row_num - right_down.row;
				iWidth = m_view_column_num - right_down.col;				
				set_subarea_scan_sequence( scan_index, 
										   right_down, iWidth, iHeight,
										   SCAN_ORIGIN_RIGHT_DOWN, SCAN_FASHION_HORIZON );
				if ( m_view_row_num == 1 )
				{
					iHeight = 1;
					iWidth = right_down.col;
				}
				else
				{
					iHeight = right_down.row;
					iWidth = 1;
				}				
				if ( (iWidth > 0) || (iHeight>0) )
				{
					right_down.row = 0;
					right_down.col = 0;
					set_subarea_scan_sequence( scan_index, 
											   right_down, iWidth, iHeight,
											   SCAN_ORIGIN_LEFT_TOP, SCAN_FASHION_HORIZON );
				}
				return;
			}	
		
			MATRIXINDEX startIndex;
			int iWidth, iHeight;
			startIndex.row = (int)(ceilf((float)m_view_row_num / 2)) - 1;
			startIndex.col = (int)(ceilf((float)m_view_column_num / 2)) - 1;
			right_down = startIndex;
			iHeight = 2;
			iWidth = m_view_column_num - startIndex.col;						
			set_subarea_scan_sequence( scan_index, 
										right_down, iWidth, iHeight,
										SCAN_ORIGIN_RIGHT_DOWN, SCAN_FASHION_VERTICAL );
			iWidth = startIndex.col;
			if ( iWidth > 0 )
			{
				right_down.row = startIndex.row;
				right_down.col = 0;
				set_subarea_scan_sequence( scan_index, 
											right_down, iWidth, iHeight,
											SCAN_ORIGIN_LEFT_DOWN, SCAN_FASHION_VERTICAL );
			}

			iWidth = m_view_column_num;
			iHeight = startIndex.row;
			if ( iHeight > 0 )
			{
				right_down.row = 0;
				right_down.col = 0;
				set_subarea_scan_sequence( scan_index, 
											right_down, iWidth, iHeight,
											SCAN_ORIGIN_RIGHT_TOP, SCAN_FASHION_HORIZON );
			}

			iHeight = m_view_row_num - startIndex.row - 2;
			if ( iHeight > 0 )
			{
				right_down.row = startIndex.row + 2;
				right_down.col = 0;
				set_subarea_scan_sequence( scan_index, 
											right_down, iWidth, iHeight,
											SCAN_ORIGIN_RIGHT_DOWN, SCAN_FASHION_HORIZON );
			}

			return;			
		}
		else
		{
			scan_index = 0;
			right_down.row = 0;
			right_down.col = 0;

			if ( (m_view_row_num == 1) || (m_view_column_num == 1) )
			{			
				set_subarea_scan_sequence( scan_index, 
										   right_down, m_view_column_num, m_view_row_num,
										   (SCAN_ORIGIN) m_scan_origin, (SCAN_FASHION) m_scan_fashion );
				return;
			}

			if ( m_view_column_num == 2 )
			{
				set_subarea_scan_sequence( scan_index, 
										   right_down, m_view_column_num, m_view_row_num,
										   (SCAN_ORIGIN) m_scan_origin, SCAN_FASHION_HORIZON );
				return;
			}	

			if ( m_view_row_num == 2 )
			{
				set_subarea_scan_sequence( scan_index, 
										   right_down, m_view_column_num, m_view_row_num,
										   (SCAN_ORIGIN) m_scan_origin, SCAN_FASHION_VERTICAL );
				return;
			}	

			if ( (SCAN_FASHION) m_scan_fashion == SCAN_FASHION_VERTICAL)
			{
				scan_index = 0;	
				if ( ( m_scan_origin == SCAN_ORIGIN_RIGHT_DOWN ) || ( m_scan_origin == SCAN_ORIGIN_RIGHT_TOP ) )
				{
					right_down.row = 0;
					right_down.col = 0;
					set_subarea_scan_sequence( scan_index, 
											   right_down, 2, m_view_row_num,
											   (SCAN_ORIGIN) m_scan_origin, SCAN_FASHION_HORIZON );
					right_down.row = 0;
					right_down.col = 2;
					SCAN_ORIGIN  so;
					if ( m_scan_origin == SCAN_ORIGIN_RIGHT_DOWN )
					{
						so = SCAN_ORIGIN_RIGHT_TOP;
					}
					else
					{
						so = SCAN_ORIGIN_RIGHT_DOWN;				
					}
					set_subarea_scan_sequence( scan_index, 
											   right_down, m_view_column_num-2, m_view_row_num,
											   so, SCAN_FASHION_VERTICAL );
				}
				else
				{
					right_down.row = 0;
					right_down.col = m_view_column_num - 2;
					set_subarea_scan_sequence( scan_index, 
											   right_down, 2, m_view_row_num,
											   (SCAN_ORIGIN) m_scan_origin, SCAN_FASHION_HORIZON );
					right_down.row = 0;
					right_down.col = 0;
					SCAN_ORIGIN  so;
					if ( m_scan_origin == SCAN_ORIGIN_LEFT_DOWN )
					{
						so = SCAN_ORIGIN_LEFT_TOP;
					}
					else
					{
						so = SCAN_ORIGIN_LEFT_DOWN;				
					}
					set_subarea_scan_sequence( scan_index, 
											   right_down, m_view_column_num-2, m_view_row_num,
											   so, SCAN_FASHION_VERTICAL );
				}
			}
			else   // Horizon first
			{
				scan_index = 0;
				if ( ( m_scan_origin == SCAN_ORIGIN_RIGHT_DOWN ) || ( m_scan_origin == SCAN_ORIGIN_LEFT_DOWN ) )
				{
					right_down.row = 0;
					right_down.col = 0;
					set_subarea_scan_sequence( scan_index, 
											   right_down, m_view_column_num, 2,
											   (SCAN_ORIGIN) m_scan_origin, SCAN_FASHION_VERTICAL );
					right_down.row = 2;
					right_down.col = 0;
					SCAN_ORIGIN  so;
					if ( m_scan_origin == SCAN_ORIGIN_RIGHT_DOWN )
					{
						so = SCAN_ORIGIN_LEFT_DOWN;
					}
					else
					{
						so = SCAN_ORIGIN_RIGHT_DOWN;				
					}
					set_subarea_scan_sequence( scan_index, 
											   right_down, m_view_column_num, m_view_row_num-2,
											   so, SCAN_FASHION_HORIZON );
				
				}
				else
				{
					right_down.row = m_view_row_num - 2;
					right_down.col = 0;
					set_subarea_scan_sequence( scan_index, 
											   right_down, m_view_column_num, 2,
											   (SCAN_ORIGIN) m_scan_origin, SCAN_FASHION_VERTICAL );
					right_down.row = 0;
					right_down.col = 0;
					SCAN_ORIGIN  so;
					if ( m_scan_origin == SCAN_ORIGIN_RIGHT_TOP )
					{
						so = SCAN_ORIGIN_LEFT_TOP;
					}
					else
					{
						so = SCAN_ORIGIN_RIGHT_TOP;
					}
					set_subarea_scan_sequence( scan_index, 
											   right_down, m_view_column_num, m_view_row_num-2,
											   so, SCAN_FASHION_HORIZON );
				}
			}
		}				
	}
	else
	{
		m_area_scan_sequence.RemoveAll();
		m_area_scan_sequence.SetSize(m_view_total_num);
		scan_index = 0;
		right_down.row = 0;
		right_down.col = 0;
		set_subarea_scan_sequence( scan_index, 
			                       right_down, m_view_column_num, m_view_row_num,
								   (SCAN_ORIGIN) m_scan_origin, (SCAN_FASHION) m_scan_fashion);
	}
}

BOOL CScanFashionDlg::control_change()
{
	int i, j, k;

	// Step1: ReCompute the row and column numbers	
    // Step2: Generate views' topology in the scan area
	// Step3: Compute total scan area
	if ( !generate_area_topology() )
	{
		return FALSE;
	}

	// Step4: Set scan sequence
	set_scan_sequence();

	// Step5: Display scan area
	// Step5.1: read the slide bitmap
	m_slidebmp.DeleteObject();
	if( ! m_slidebmp.LoadBitmapW(IDB_SLIDE) )
	{
		AfxMessageBox(_T("不能加载位图资源"));
		return FALSE;
	}
	std::auto_ptr<BYTE> pSlideBmpData(new BYTE[m_slidebmp_byteCount]);	
	LPCOLORREF pBmpData = NULL;
	pBmpData = (LPCOLORREF) pSlideBmpData.get();
	if (pBmpData)
	{
		m_slidebmp.GetBitmapBits(m_slidebmp_byteCount, (LPVOID)pBmpData );
	}
	else
	{
		AfxMessageBox(_T("内存不足，不能加载位图资源"));
		return FALSE;
	}
	
	// Step5.2: add the views in the slide bitmap
	CPoint view_position_in_SlideBmp;
	for ( k=0; k<m_view_total_num; k++ )
	{
		view_position_in_SlideBmp.x = LONG(m_area_topology[k].x/m_scan_fashion_display_ratio);
		view_position_in_SlideBmp.y = LONG(m_area_topology[k].y/m_scan_fashion_display_ratio);
		view_position_in_SlideBmp = m_slidebmp_origin - view_position_in_SlideBmp;

		if ( (view_position_in_SlideBmp.x < 0) || (view_position_in_SlideBmp.x > m_slidebmp_width-10 ) 
				|| (view_position_in_SlideBmp.y < 0 ) || (view_position_in_SlideBmp.y>m_slidebmp_height-10) )
		{
			AfxMessageBox(_T("扫描范围溢出显示示意图"));
			return FALSE;
		}

		// set black color as the representation of the views' position in the bitmap 
		for ( i=0; i<3; i++ )
		{
			for ( j=0; j<2; j++ )
			{
				pBmpData[(view_position_in_SlideBmp.y+j) * m_slidebmp_width + view_position_in_SlideBmp.x + i] = RGB(0,0,0);
			}
		}
	}

	// Step5.3: set red color as the start view in the bitmap
	MATRIXINDEX firstFOVIndex = m_area_scan_sequence[0];
	cv::Point2f firstFOVPosition = m_area_topology[firstFOVIndex.row * m_view_column_num + firstFOVIndex.col ];
	view_position_in_SlideBmp.x = LONG(firstFOVPosition.x/m_scan_fashion_display_ratio);
	view_position_in_SlideBmp.y = LONG(firstFOVPosition.y/m_scan_fashion_display_ratio);
	view_position_in_SlideBmp = m_slidebmp_origin - view_position_in_SlideBmp;
	for ( i = 0; i<5; i++ )
	{
		for ( j=0; j<5; j++ )
		{
			pBmpData[(view_position_in_SlideBmp.y+j) * m_slidebmp_width + view_position_in_SlideBmp.x + i ] = RGB(255,0,0);
		}
	}
	// Step5: Display scan area
	m_slidebmp.SetBitmapBits(m_slidebmp_byteCount, (const void*)pBmpData );
	pBmpData = NULL;
	m_hSlidBitmap = HBITMAP(m_slidebmp);

	return TRUE;
}

void CScanFashionDlg::EnableScanSet(BOOL bEnable)
{
	m_ctl_scan_fashion_vertical.EnableWindow(bEnable);
	m_ctl_scan_fashion_horizon.EnableWindow(bEnable);
	m_ctl_scan_origin_lefttop.EnableWindow(bEnable);
	m_ctl_scan_origin_leftdown.EnableWindow(bEnable);
	m_ctl_scan_origin_righttop.EnableWindow(bEnable);
	m_ctl_scan_origin_rightdown.EnableWindow(bEnable);
}

// CScanFashionDlg 消息处理程序

void CScanFashionDlg::OnEnChangeEditHorizonNum()
{
	UINT last_view_column_num;
	last_view_column_num = m_view_column_num;

	if ( ! UpdateData(TRUE) )
	{
		return;
	}
	m_view_total_num = m_view_row_num * m_view_column_num;

	if ( !control_change() )
	{
	    m_view_column_num = last_view_column_num;
	}
	m_view_total_num = m_view_row_num * m_view_column_num;

	UpdateData(FALSE);	
}

void CScanFashionDlg::OnEnChangeEditVerticalNum()
{
	UINT last_view_row_num;
	last_view_row_num = m_view_row_num;

	if ( ! UpdateData(TRUE) )
	{
		return;
	}
	m_view_total_num = m_view_row_num * m_view_column_num;

	if ( !control_change() )
	{
	    m_view_row_num = last_view_row_num;
	}
	m_view_total_num = m_view_row_num * m_view_column_num;

	UpdateData(FALSE);
}


void CScanFashionDlg::OnBnClickedRadioAverage()
{
	if ( ! UpdateData(TRUE) )
	{
		return;
	}
	m_ctl_BestScanRoute.EnableWindow(TRUE);
	EnableScanSet(TRUE);
	control_change();
	UpdateData(FALSE);
}

void CScanFashionDlg::OnBnClickedRadioNinepoint()
{
	if ( ! UpdateData(TRUE) )
	{
		return;
	}
	m_ctl_BestScanRoute.EnableWindow(FALSE);
	EnableScanSet(FALSE);
	m_scan_origin = SCAN_ORIGIN_RIGHT_DOWN;
	control_change();
	UpdateData(FALSE);
}

void CScanFashionDlg::OnBnClickedRadioContinue()
{
	if ( ! UpdateData(TRUE) )
	{
		return;
	}
	m_ctl_BestScanRoute.EnableWindow(FALSE);
	EnableScanSet(TRUE);	
	control_change();
	UpdateData(FALSE);
}


void CScanFashionDlg::OnBnClickedButtonAreaUp()
{
	if ( ! UpdateData(TRUE) )
	{
		return;
	}
	if (m_area_updown==0)
	{
		return;
	}

	float tf = m_rightdown_position.y + m_area_updown*1000;   // um
    
	if ( (tf+m_area_vertical*1000) > m_mechanical_vertical )
	{
		AfxMessageBox(_T("扫描范围超出机械行程"));
		return;
	}
	
	m_rightdown_position.y = tf;

	control_change();

	UpdateData(FALSE);
}

void CScanFashionDlg::OnBnClickedButtonAreaDown()
{
	if ( ! UpdateData(TRUE) )
	{
		return;
	}
	if (m_area_updown==0)
	{
		return;
	}

	float tf = m_rightdown_position.y - m_area_updown*1000;   // um
    
	if ( tf < 0 )
	{
		AfxMessageBox(_T("扫描范围超出机械行程"));
		return;
	}

	m_rightdown_position.y = tf;

	control_change();

	UpdateData(FALSE);
}

void CScanFashionDlg::OnBnClickedButtonAreaLeft()
{
	if ( ! UpdateData(TRUE) )
	{
		return;
	}
	if (m_area_leftright==0)
	{
		return;
	}

	float tf = m_rightdown_position.x + m_area_leftright*1000;   // um
    
	if ( (tf+m_area_horizon*1000) >  m_mechanical_horizon )
	{
		AfxMessageBox(_T("扫描范围超出机械行程"));
		return;
	}
	
	m_rightdown_position.x = tf;

	control_change();

	UpdateData(FALSE);
}

void CScanFashionDlg::OnBnClickedButtonAreaRight()
{
	if ( ! UpdateData(TRUE) )
	{
		return;
	}
	if (m_area_leftright==0)
	{
		return;
	}

	float tf = m_rightdown_position.x - m_area_leftright*1000;   // um
    
	if ( tf < 0 )
	{
		AfxMessageBox(_T("扫描范围超出机械行程"));
		return;
	}
	
	m_rightdown_position.x = tf;

	control_change();

	UpdateData(FALSE);
}

void CScanFashionDlg::OnBnClickedCheckBestscanroute()
{
	if ( m_bBestScanRoute )
	{
		m_bBestScanRoute = FALSE;
		m_ctl_BestScanRoute.SetCheck(BST_UNCHECKED);
		EnableScanSet(TRUE);		
	}
	else
	{
		m_bBestScanRoute = TRUE;
		m_ctl_BestScanRoute.SetCheck(BST_CHECKED);
		EnableScanSet(FALSE);
	}

	control_change();
	UpdateData(FALSE);
}

void CScanFashionDlg::OnBnClickedRadioLefttop()
{
	if ( ! UpdateData(TRUE) )
	{
		return;
	}
	
	control_change();

	UpdateData(FALSE);
}

void CScanFashionDlg::OnBnClickedRadioRighttop()
{
	if ( ! UpdateData(TRUE) )
	{
		return;
	}
	
	control_change();

	UpdateData(FALSE);
}

void CScanFashionDlg::OnBnClickedRadioLeftdown()
{
	if ( ! UpdateData(TRUE) )
	{
		return;
	}
	
	control_change();

	UpdateData(FALSE);
}

void CScanFashionDlg::OnBnClickedRadioRightdown()
{
	if ( ! UpdateData(TRUE) )
	{
		return;
	}
	
	control_change();

	UpdateData(FALSE);
}

void CScanFashionDlg::OnBnClickedRadioVertical()
{
	if ( ! UpdateData(TRUE) )
	{
		return;
	}
	// Step5: Set scan sequence
	set_scan_sequence();
}

void CScanFashionDlg::OnBnClickedRadioHorizon()
{
	if ( ! UpdateData(TRUE) )
	{
		return;
	}
	// Step5: Set scan sequence
	set_scan_sequence();
}


// 向View视图发送消息，通知其更新扫描路径
void CScanFashionDlg::OnBnClickedApply()
{
	UpdateData(TRUE);
	control_change();

	SCANROUTE		scanroute; 
	getScanRoute(scanroute);
	g_Mechanism.SetScanRoute(&scanroute);

	if ( ! ( GetParentFrame()->GetActiveView()->PostMessageW(MSG_SCANROUTE_NOTIFY, 0L, 0L) ) )
	{
		AfxMessageBox(_T("不能将扫描方式设置对话框的消息发送出去"));
	}
}


void CScanFashionDlg::OnOK()
{
	UpdateData(TRUE);
	control_change();

	SCANROUTE		scanroute; 
	getScanRoute(scanroute);
	g_Mechanism.SetScanRoute(&scanroute);

	if ( ! ( GetParentFrame()->GetActiveView()->PostMessageW(MSG_SCANROUTE_NOTIFY, 1L, 0L) ) )
	{
		AfxMessageBox(_T("不能将扫描方式设置对话框的消息发送出去"));
	}
	CDialogEx::OnOK();
}


BOOL CScanFashionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if ( m_bBestScanRoute )
	{
		m_ctl_BestScanRoute.SetCheck(BST_CHECKED);
		EnableScanSet(FALSE);
	}
	else
	{
		m_ctl_BestScanRoute.SetCheck(BST_UNCHECKED);
		EnableScanSet(TRUE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
