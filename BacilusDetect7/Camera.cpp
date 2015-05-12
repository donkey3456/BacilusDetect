#include "StdAfx.h"
#include "Camera.h"
#include "define.h"

std::vector<TCaptureDevice> dscap_enum_device()
{
	std::vector<TCaptureDevice> vec;

	CComPtr<ICreateDevEnum> spCreateDevEnum;
	HRESULT hr = spCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER);
	if (spCreateDevEnum)
	{
		CComPtr<IEnumMoniker> spEm;
		hr = spCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &spEm, 0);
		if (hr == NOERROR)
		{
			spEm->Reset();

			ULONG cFetched;
			IMoniker* pM;
			while (hr = spEm->Next(1, &pM, &cFetched), hr == S_OK)
			{
				WCHAR* wszDisplayName = NULL;
				if (SUCCEEDED(pM->GetDisplayName(NULL, NULL, &wszDisplayName)))
				{
#ifdef _USE_ISTOUPCAMPRESENT_
					if (!IsToupCamPresent(wszDisplayName))
					{
						CoTaskMemFree(wszDisplayName);
						continue;
					}
#endif
					CComPtr<IPropertyBag> spBag;
					hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void**)&spBag);
					if (spBag)
					{
						CComVariant var;
						var.vt = VT_BSTR;
						hr = spBag->Read(L"FriendlyName", &var, NULL);
						if (hr == NOERROR)
						{
							TCaptureDevice dev;
							dev.DisplayName = wszDisplayName;
							dev.FriendlyName = var.bstrVal;
							vec.push_back(dev);
						}
					}
					CoTaskMemFree(wszDisplayName);
				}

				pM->Release();
			}
		}
	}
	return vec;
}

CCamera* CCamera::m_pInstance = NULL;

CCamera::CCamera(void)
{
	m_bCapturing = FALSE;

	m_pBuilder = NULL;
	m_pFg = NULL;
	m_pSource = NULL;

	m_matImage.create(CAMERA_IMAGE_HEIGHT,CAMERA_IMAGE_WIDTH,CV_8UC3);
	m_matBuffer.create(CAMERA_IMAGE_HEIGHT,CAMERA_IMAGE_WIDTH,CV_8UC3);
	m_matImage = 0;
}

CCamera::~CCamera(void)
{
	// 关闭摄像机
	if ( m_bCapturing )
	{
		close();
	}
}

//内部操作

// 接口
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
	// 关闭摄像机
	if (m_bCapturing)
	{
		StopPreview();
		ReleaseFG();
		m_bCapturing = FALSE;
	}
}

// 禁止和使能摄像机发送消息
void  CCamera::EnableNotifyMsg(BOOL Enable)
{
	m_SampleGrabberCBImp.EnableNotifyMsg(Enable);
}

void CCamera::EnableNotifyMsg( HWND hWnd, BOOL bEnable )
{
	m_SampleGrabberCBImp.EnableNotifyMsg(hWnd,bEnable);
}

// 自动白平衡
BOOL  CCamera::AutoWhiteBalance(RECT* pAuxRect)
{
	if ( m_bCapturing )
	{
		_ASSERTE(m_pSource != NULL);
		CComPtr<IToupcam> spToupcam;
		m_pSource->QueryInterface(IID_IToupcam, (void**)&spToupcam);
		if (spToupcam == NULL)
		{
			return FALSE;
		}
		spToupcam->put_AuxRect(pAuxRect);
		const int aGain[3] = {0,0,0};
		spToupcam->put_AWBInit(&(CCamera::fnWBProc(aGain,NULL)), NULL);
	}
	else
	{
		AfxMessageBox(_T("摄像机没有启动"));
		return FALSE;
	}
	return TRUE;
}

// 自动曝光使能和禁止
BOOL    CCamera::AutoExpoEnable(BOOL bAutoExposure)
{
	if ( m_bCapturing )
	{
		_ASSERTE(m_pSource != NULL);
		CComPtr<IToupcam> spToupcam;
		m_pSource->QueryInterface(IID_IToupcam, (void**)&spToupcam);
		if (spToupcam == NULL)
			return FALSE;

		spToupcam->put_AutoExpoEnable(bAutoExposure);
	}
	else
	{
		AfxMessageBox(_T("摄像机没有启动"));
		return FALSE;
	}
	return TRUE;
}

// 自动曝光禁止时，手动设置曝光时间, unit: us
BOOL	CCamera::SetExposureTime(ULONG ExposureTime)
{
	if ( m_bCapturing )
	{
		_ASSERTE(m_pSource != NULL);
		CComPtr<IToupcam> spToupcam;
		m_pSource->QueryInterface(IID_IToupcam, (void**)&spToupcam);
		if (spToupcam == NULL)
			return FALSE;

		spToupcam->put_ExpoTime(ExposureTime);
	}
	else
	{
		AfxMessageBox(_T("摄像机没有启动"));
		return FALSE;
	}
	return TRUE;
}

ULONG	CCamera::GetExposureTime()
{
	ULONG Time = 0;
	if ( m_bCapturing )
	{
		_ASSERTE(m_pSource != NULL);
		CComPtr<IToupcam> spToupcam;
		m_pSource->QueryInterface(IID_IToupcam, (void**)&spToupcam);
		if (spToupcam != NULL)
		{
			spToupcam->get_ExpoTime(&Time);
		}
	}
	return Time;
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
	//如果已经打开
	if (m_bCapturing)
	{
		return TRUE;
	}
	
	//init grabber
	if (!MakeBuilderAndGraph())
	{
		ReleaseFG();
		return FALSE;
	}
	if (!InitFilters())
	{
		ReleaseFG();
		return FALSE;
	}

	if (!StartPreview())
	{
		ReleaseFG();
		return FALSE;
	}

	DWORD dwRes = WaitForSingleObject(m_EventCaptured,3000);

	if (dwRes == WAIT_TIMEOUT)
	{
		ReleaseFG();
		return FALSE;
	}
	m_bCapturing = TRUE;

	return TRUE;
}

BOOL CCamera::AddOutputWnd( HWND hParent, UINT NotifyMsg )
{
	m_SampleGrabberCBImp.AddOutputWnd(hParent,NotifyMsg);
	return FALSE;
}

BOOL CCamera::DeleteOutputWnd( HWND hParent )
{
	m_SampleGrabberCBImp.DeleteWnd(hParent);
	return FALSE;
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
	m_matImage = m_matBuffer;	//将原m_matImage release，并使m_matImage变成m_matBuffer
	cv::Mat temp;
	temp.create(CAMERA_IMAGE_HEIGHT,CAMERA_IMAGE_WIDTH,CV_8UC3);
	m_matBuffer = temp;
}

BOOL CCamera::StartPreview()
{
	// run the graph
	CComPtr<IMediaControl> spMC;
	HRESULT hr = m_pFg->QueryInterface(IID_IMediaControl, (void**)&spMC);
	if (SUCCEEDED(hr) && spMC)
	{
		hr = spMC->Run();
		if (FAILED(hr))
		{
			// stop parts that ran
			spMC->Stop();
			return FALSE;
		}
	}

	return TRUE;
}


BOOL CCamera::MakeBuilderAndGraph()
{
	HRESULT hr;

	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void**)&m_pBuilder);

	if (FAILED(hr))
	{
		return FALSE;
	}
	
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (LPVOID*)&m_pFg);

	if (FAILED(hr))
	{
		return FALSE;
	}

	hr = m_pBuilder->SetFiltergraph(m_pFg);
	if (FAILED(hr))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CCamera::InitFilters()
{
	//////////////////////////////////////////////////////////////////
	//set up source filter
	//////////////////////////////////////////////////////////////////
	//get device information
	std::vector<TCaptureDevice>	vecCaptureDevice = dscap_enum_device();   

	//find device
	UINT i;
	std::wstring cameratype ( L"EXCCD01400KPA" );
	for (i=0; i<vecCaptureDevice.size(); i++)
	{
		if ( 0 == cameratype.compare( vecCaptureDevice[i].FriendlyName.c_str() ) )   
		{
			//if we find a device
			break;
		}
	}

	//如果没有找到匹配的device
	if (i == vecCaptureDevice.size())
	{
		return FALSE;
	}

	std::wstring strDevice = vecCaptureDevice[i].DisplayName;              // Get the device name

	// get moniker 
	CComPtr<IMoniker> spMoniker;
	CComPtr<IBindCtx> spBC;
	HRESULT hr = CreateBindCtx(0, &spBC);
	if (FAILED(hr))
	{
		return FALSE;
	}

	DWORD dwEaten;
	if (FAILED(MkParseDisplayName(spBC, strDevice.c_str(), &dwEaten, &spMoniker)))
	{
		return FALSE;
	}

	
	hr = spMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_pSource);

	if (FAILED(hr))
	{
		return FALSE;
	}

	// Add the video capture filter to the graph with its friendly name
	hr = m_pFg->AddFilter(m_pSource, L"Source");
	if (FAILED(hr))
	{
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////
	//set up grabber
	//////////////////////////////////////////////////////////////////
	CComPtr<IBaseFilter> pSampleGrabber;

	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pSampleGrabber);
	if (FAILED(hr))
	{
		return FALSE;
	}

	hr = m_pFg->AddFilter(pSampleGrabber, L"SampleGrabber");
	if (FAILED(hr))
	{
		return FALSE;
	}

	//setup grabber parameters
	m_SampleGrabberCBImp.init_sample_image(this,&m_EventCaptured);

	CComPtr<ISampleGrabber> pGrabber;
	pSampleGrabber->QueryInterface(IID_ISampleGrabber, (void**)&pGrabber);
	if (pGrabber)
	{
		AM_MEDIA_TYPE mt = { 0 };
		mt.majortype = MEDIATYPE_Video;
		mt.subtype = MEDIASUBTYPE_RGB24;
		hr = pGrabber->SetMediaType(&mt);
		if (FAILED(hr))
		{
			return FALSE;
		}
		pGrabber->SetOneShot(FALSE);
		pGrabber->SetBufferSamples(TRUE);
		pGrabber->SetCallback(&m_SampleGrabberCBImp,1);
	} 

	//////////////////////////////////////////////////////////////////
	//set up render
	//////////////////////////////////////////////////////////////////
	CComPtr<IBaseFilter> pNull;
	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pNull);
	if (FAILED(hr))
	{
		return FALSE;
	}
	hr = m_pFg->AddFilter(pNull, L"NullRender");
	if (FAILED(hr))
	{
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////
	//connect filters
	//////////////////////////////////////////////////////////////////
	hr = m_pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pSource, pSampleGrabber, pNull);
	if (FAILED(hr))
	{
		return FALSE;
	}
	
	return TRUE;
}

void CCamera::ReleaseFG()
{
	if (m_pSource)
	{
		m_pSource->Release();
		m_pSource = NULL;
	}
	if (m_pBuilder)
	{
		m_pBuilder->Release();
		m_pBuilder = NULL;
	}

	if (m_pFg)
	{
		m_pFg->Release();
		m_pFg = NULL;
	}

	//其他的由CComPtr自动release
}

void CCamera::StopPreview()
{

	CComPtr<IMediaControl> spMC;
	HRESULT hr = m_pFg->QueryInterface(IID_IMediaControl, (void**)&spMC);
	if (SUCCEEDED(hr))
	{
		spMC->Stop();
	}
}

PITOUPCAM_WHITEBALANCE_CALLBACK CCamera::fnWBProc( const int aGain[3], void* pCtx )
{
	return S_OK;
}

ULONG CCamera::getFrameNum()
{
	return m_SampleGrabberCBImp.getFrameNum();
}

float CCamera::getFPS()
{
	return m_SampleGrabberCBImp.getFPS();
}
