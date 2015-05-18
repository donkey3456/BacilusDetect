#include "StdAfx.h"
#include "Camera.h"
#include "define.h"


CCamera* CCamera::m_pInstance = NULL;

CCamera::CCamera(void)
{
	m_bCapturing = FALSE;

	m_hCamera = INVALID_HANDLE_VALUE;
	m_hThread = INVALID_HANDLE_VALUE;

	m_matImage.create(CAMERA_IMAGE_HEIGHT,CAMERA_IMAGE_WIDTH,CV_8UC3);
	m_matBuffer.create(CAMERA_IMAGE_HEIGHT,CAMERA_IMAGE_WIDTH,CV_8UC3);
	m_matImage = 0;
	m_matBuffer = 0;

	InitializeCriticalSection(&m_WndsPro);

	m_ulFrameNum = 0;

}

CCamera::~CCamera(void)
{
	// �ر������
	if ( m_bCapturing )
	{
		close();
	}
	DeleteCriticalSection(&m_WndsPro);
}

//�ڲ�����

// �ӿ�
BOOL CCamera::connect(HWND hParent, UINT NotifyMsg)
{
	if (!Open())
	{
		return FALSE;
	}

	AddOutputWnd(hParent,NotifyMsg);
	return TRUE;

}

void CCamera::close(void)
{
	// �ر������
	if (m_bCapturing)
	{
		m_bCapturing = FALSE;
		xiCloseDevice(m_hCamera);
		WaitForSingleObject(m_hThread,INFINITE);
		m_hCamera = INVALID_HANDLE_VALUE;
		m_hThread = INVALID_HANDLE_VALUE;
		m_ulFrameNum = 0;
	}
}

// ��ֹ��ʹ�������������Ϣ
void  CCamera::EnableNotifyMsg(BOOL Enable)
{
	for (int i = 0; i < m_OutputWnds.size(); i++)
	{
		m_OutputWnds[i].bNotify = Enable;
	}
}

void CCamera::EnableNotifyMsg( HWND hWnd, BOOL bEnable )
{
	for (int i = 0; i < m_OutputWnds.size(); i++)
	{
		if (m_OutputWnds[i].hWnd == hWnd)
		{
			m_OutputWnds[i].bNotify = bEnable;
			break;
		}
	}
}

// �Զ���ƽ��
BOOL  CCamera::AutoWhiteBalance()
{
	if ( m_bCapturing )
	{
		XI_RETURN res = xiSetParamInt(m_hCamera, XI_PRM_MANUAL_WB, 0);
		if (res == XI_OK)
		{
			return TRUE;
		}
		MessageBox(NULL,_T("���������Զ���ƽ��"),_T("Error"),MB_OK);
		return FALSE;
	}
	MessageBox(NULL,_T("δ�������"),_T("Error"),MB_OK);
	return FALSE;
}

// �Զ��ع�ʹ�ܺͽ�ֹ
BOOL    CCamera::AutoExpoEnable(BOOL bAutoExposure)
{
	if ( m_bCapturing )
	{
		XI_RETURN res = xiSetParamInt(m_hCamera, XI_PRM_AEAG, bAutoExposure);
		if (res == XI_OK)
		{
			return TRUE;
		}
		MessageBox(NULL,_T("���������Զ��ع�"),_T("Error"),MB_OK);
		return FALSE;
	}
	MessageBox(NULL,_T("δ�������"),_T("Error"),MB_OK);
	return FALSE;
}

// �Զ��ع��ֹʱ���ֶ������ع�ʱ��, unit: us
BOOL	CCamera::SetExposureTime(ULONG ExposureTime)
{
	if ( m_bCapturing )
	{
		XI_RETURN res = xiSetParamInt(m_hCamera, XI_PRM_EXPOSURE, ExposureTime);
		if (res == XI_OK)
		{
			return TRUE;
		}
		MessageBox(NULL,_T("���������ع�ʱ��"),_T("Error"),MB_OK);
		return FALSE;
	}
	MessageBox(NULL,_T("δ�������"),_T("Error"),MB_OK);
	return FALSE;
}

int	CCamera::GetExposureTime()
{
	int nTime = 0;
	if ( m_bCapturing )
	{
		XI_RETURN res = xiGetParamInt(m_hCamera, XI_PRM_EXPOSURE, &nTime);
		if (res != XI_OK)
		{
			return 0;
		}
	}
	return nTime;
}

CCamera* CCamera::GetInstance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CCamera();  
	}
	return m_pInstance; 
}

void CCamera::Release()
{
	if(NULL != m_pInstance)  
	{  
		delete m_pInstance;  
		m_pInstance = NULL;  
	}  
}

BOOL CCamera::Open()
{
	//����Ѿ���
	if (m_bCapturing)
	{
		return TRUE;
	}

	//�����
	XI_RETURN stat = xiOpenDevice(0, &m_hCamera);
	if(stat != XI_OK)
	{
		MessageBox(NULL,_T("��ⲻ���������"),_T("Error"),MB_OK);
		return FALSE;
	}

	//��ʼ�ɼ�
	stat = xiStartAcquisition(m_hCamera);
	if(stat != XI_OK)
	{
		MessageBox(NULL,_T("�޷���ȡͼ��"),_T("Error"),MB_OK);
		xiCloseDevice(m_hCamera);
		return FALSE;
	}

	//���ó�ʼ����
	stat = xiSetParamInt(m_hCamera,XI_PRM_IMAGE_DATA_FORMAT,XI_RGB24);
	if(stat != XI_OK)
	{
		MessageBox(NULL,_T("�޷��������ݸ�ʽ����ʹ�ò�ɫ���"),_T("Error"),MB_OK);
		xiCloseDevice(m_hCamera);
		return FALSE;
	}
	stat = xiSetParamInt(m_hCamera,XI_PRM_WIDTH,CAMERA_IMAGE_WIDTH);
	if(stat != XI_OK)
	{
		MessageBox(NULL,_T("�޷�����ͼ����"),_T("Error"),MB_OK);
		xiCloseDevice(m_hCamera);
		return FALSE;
	}
	stat = xiSetParamInt(m_hCamera,XI_PRM_HEIGHT,CAMERA_IMAGE_HEIGHT);
	if(stat != XI_OK)
	{
		MessageBox(NULL,_T("�޷�����ͼ�񳤶�"),_T("Error"),MB_OK);
		xiCloseDevice(m_hCamera);
		return FALSE;
	}

	stat = xiSetParamInt(m_hCamera, XI_PRM_AUTO_WB, 0);
	if(stat != XI_OK)
	{
		MessageBox(NULL,_T("�޷��ر��Զ���ƽ��"),_T("Error"),MB_OK);
		xiCloseDevice(m_hCamera);
		return FALSE;
	}

	stat = xiSetParamInt(m_hCamera, XI_PRM_AEAG, 0);
	if(stat != XI_OK)
	{
		MessageBox(NULL,_T("�޷��ر��Զ��ع�"),_T("Error"),MB_OK);
		xiCloseDevice(m_hCamera);
		return FALSE;
	}
	m_hThread = AfxBeginThread(CCamera::ReadBuffer,this)->m_hThread;
	m_bCapturing = TRUE;
	DWORD dwRes = WaitForSingleObject(m_EventCaptured,3000);

	if (dwRes == WAIT_TIMEOUT)
	{
		xiCloseDevice(m_hCamera);
		m_bCapturing = FALSE;
		return FALSE;
	}

	return TRUE;
}

void CCamera::AddOutputWnd( HWND hParent, UINT NotifyMsg )
{
	EnterCriticalSection(&m_WndsPro);
	int nIndex = FindWndIndex(hParent);
	if (nIndex != -1)
	{
		m_OutputWnds[nIndex].uiNotifyMsg = NotifyMsg;
		m_OutputWnds[nIndex].bNotify = FALSE;
	}
	else
	{
		OUTPUT_WINDOW_INFO info;
		info.hWnd = hParent;
		info.bNotify = FALSE;
		info.uiNotifyMsg = NotifyMsg;
		m_OutputWnds.push_back(info);
	}
	LeaveCriticalSection(&m_WndsPro);
}

void CCamera::DeleteOutputWnd( HWND hParent )
{
	EnterCriticalSection(&m_WndsPro);
	int nIndex = FindWndIndex(hParent);
	if (nIndex != -1)
	{
		m_OutputWnds.erase(m_OutputWnds.begin() + nIndex);
	}
	LeaveCriticalSection(&m_WndsPro);
}

cv::Size CCamera::GetCameraImageSize()
{
	return m_matImage.size();
}

cv::Size2f CCamera::GetCameraFOVSize()
{
	return cv::Size2f( m_matImage.cols * CAMERA_PIXEL_SIZE_UM / LENS_RATIO, m_matImage.rows * CAMERA_PIXEL_SIZE_UM / LENS_RATIO );
}

uchar* CCamera::GetBufferPtr()
{
	return m_matBuffer.ptr();
}

void CCamera::SwitchBuffer()
{
	m_matImage = m_matBuffer;	//��ԭm_matImage release����ʹm_matImage���m_matBuffer
	cv::Mat temp;
	temp.create(CAMERA_IMAGE_HEIGHT,CAMERA_IMAGE_WIDTH,CV_8UC3);
	m_matBuffer = temp;
}

ULONG CCamera::getFrameNum()
{
	return m_ulFrameNum;
}

float CCamera::getFPS()
{
	float fFPS = -1;
	XI_RETURN res = xiGetParamFloat(m_hCamera,XI_PRM_FRAMERATE,&fFPS);
	return fFPS;
}

int CCamera::FindWndIndex( HWND hWnd )
{
	for (int i = 0; i < m_OutputWnds.size(); i++)
	{
		if (m_OutputWnds[i].hWnd == hWnd)
		{
			return i;
		}
	}
	return -1;
}

UINT CCamera::ReadBuffer( LPVOID pParam )
{
	CCamera* pThis = (CCamera*)pParam;

	while (pThis->m_bCapturing)
	{
		xiGetImage(pThis->m_hCamera,2000,&(pThis->m_ImageBuffer));
		memcpy(pThis->GetBufferPtr(), pThis->m_ImageBuffer.bp, CAMERA_IMAGE_WIDTH * CAMERA_IMAGE_HEIGHT * 3);
		pThis->SwitchBuffer();
		pThis->m_EventCaptured.SetEvent();
		pThis->m_ulFrameNum++;
		EnterCriticalSection(&pThis->m_WndsPro);
		if (pThis->m_OutputWnds.size() > 0)
		{
			for(int i = 0; i < pThis->m_OutputWnds.size();i++)
			{
				if (pThis->m_OutputWnds[i].bNotify)
				{
					PostMessage(pThis->m_OutputWnds[i].hWnd,pThis->m_OutputWnds[i].uiNotifyMsg,0,0);
				}
			}
		}	
		LeaveCriticalSection(&pThis->m_WndsPro);
	}
	return 0;
}
