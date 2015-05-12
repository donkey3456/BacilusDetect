#include "stdafx.h"
#include "SampleGrabberCBImpl.h"
#include <memory>
#include <Imapierror.h>
#include "Camera.h"
#ifdef _USE_TOUPCAMINSTANCE_
#include "ToupCamInstance.h"
#endif

#pragma comment(lib, "quartz.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "uuid.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "wmvcore.lib")


SampleGrabberCBImpl::SampleGrabberCBImpl()
{
	m_pCam = NULL;
	m_pEventCaptured = NULL;

	m_FrameNum = 0;
	m_lastImageTime.QuadPart = m_curImageTime.QuadPart = 0;
	m_FPS = 0;
	m_frequency.QuadPart = 0;
	if ( !QueryPerformanceFrequency(&m_frequency) )
	{
		m_frequency.QuadPart = 0;
	}
}

HRESULT SampleGrabberCBImpl::QueryInterface(REFIID iid, void** ppvObject)
{
	if (iid == IID_IUnknown) {
		*ppvObject = (IUnknown*)this;
		return NOERROR;
	}
	else if (iid == IID_ISampleGrabberCB) {
		*ppvObject = (ISampleGrabberCB*)this;
		return NOERROR;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP SampleGrabberCBImpl::BufferCB( double SampleTime, BYTE *pBuffer, long BufferLen )
{
	//防止多个线程同时调用bufferCB
	CSingleLock singleLock(&m_CritSection);
	singleLock.Lock();

	QueryPerformanceCounter(&m_curImageTime);
	double  timespan = (double) (m_curImageTime.QuadPart - m_lastImageTime.QuadPart);
	if (timespan > 0)
	{
		m_FPS = (float) (m_frequency.QuadPart / timespan);
		m_lastImageTime = m_curImageTime;
	}	
	m_FrameNum++;

	::memcpy(m_pCam->GetBufferPtr(), pBuffer, BufferLen);
	m_pCam->SwitchBuffer();
	m_pEventCaptured->SetEvent();
	if (m_OutputWnds.size() > 0)
	{
		for(int i = 0; i < m_OutputWnds.size();i++)
		{
			if (m_OutputWnds[i].bPostMsgEnable)
			{
				PostMessage(m_OutputWnds[i].hWnd,m_OutputWnds[i].nNotifyMsg,0,0);
			}
		}
	}			
	singleLock.Unlock();  
	return S_OK;
}

int SampleGrabberCBImpl::FindWnd( HWND hWnd )
{
	//如果尚未连接,则返回未找到
	int i, nCount;
	nCount = m_OutputWnds.size();
	if (nCount == 0)
	{
		return -1;
	}
	
	for(i = 0; i < m_OutputWnds.size(); i++)
	{
		if (m_OutputWnds[i].hWnd == hWnd)
		{
			//找到了
			break;
		}
	}
	//如果没有找到
	if (i == nCount)
	{
		return -1;
	}
	return i;
}

void SampleGrabberCBImpl::AddOutputWnd( HWND hParent, UINT NotifyMsg, BOOL bEnable /*= FALSE*/ )
{
	int nIndex = FindWnd(hParent);

	//如果没有找到，则加入
	if(nIndex == -1)
	{
		OutputWndInf newWnd;
		newWnd.hWnd = hParent;
		newWnd.nNotifyMsg = NotifyMsg;
		newWnd.bPostMsgEnable = bEnable;
		m_OutputWnds.push_back(newWnd);
	}
	else
	{
		//如果找到了，更新信息
		m_OutputWnds[nIndex].nNotifyMsg = NotifyMsg;
		m_OutputWnds[nIndex].bPostMsgEnable = bEnable;
	}

}

BOOL SampleGrabberCBImpl::DeleteWnd( HWND hWnd )
{
	int nIndex = FindWnd(hWnd);

	//如果没有找到返回错误
	if (nIndex == -1)
	{
		return FALSE;
	}
	else
	{
		//如果找到，移除
		m_OutputWnds.erase(m_OutputWnds.begin() + nIndex);
		return TRUE;
	}
}

BOOL SampleGrabberCBImpl::EnableNotifyMsg( HWND hWnd, BOOL bEnable /*= FALSE*/ )
{
	int nIndex = FindWnd(hWnd);

	//如果没有找到返回错误
	if (nIndex == -1)
	{
		return FALSE;
	}
	else
	{
		m_OutputWnds[nIndex].bPostMsgEnable = bEnable;
		return TRUE;
	}
}

void SampleGrabberCBImpl::EnableNotifyMsg( BOOL Enable /*= FALSE*/ )
{
	if (m_OutputWnds.size() > 0)
	{
		m_OutputWnds[0].bPostMsgEnable = Enable;
	}
}

void SampleGrabberCBImpl::ClearOutputWnds()
{
	m_OutputWnds.clear();
}
