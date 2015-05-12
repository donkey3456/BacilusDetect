#ifndef __capture_h__
#define __capture_h__

#include <vector>
#include <string>
#include <dshow.h>
#include <Dshowasf.h>
#include <wmsdk.h>
#include <InitGuid.h>
#include "Winbase.h"
#include "toupcam.h"



//Grabber 虚基类
interface ISampleGrabberCB : public IUnknown
{
	virtual STDMETHODIMP SampleCB( double SampleTime, IMediaSample *pSample ) = 0;
	virtual STDMETHODIMP BufferCB( double SampleTime, BYTE *pBuffer, long BufferLen ) = 0;
};

static const IID IID_ISampleGrabberCB = { 0x0579154A, 0x2B53, 0x4994, { 0xB0, 0xD0, 0xE7, 0x73, 0x14, 0x8E, 0xFF, 0x85 } };
interface ISampleGrabber: public IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE SetOneShot( BOOL OneShot ) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetMediaType( const AM_MEDIA_TYPE *pType ) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType( AM_MEDIA_TYPE *pType ) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetBufferSamples( BOOL BufferThem ) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer( long *pBufferSize, long *pBuffer ) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCurrentSample( IMediaSample **ppSample ) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetCallback( ISampleGrabberCB *pCallback, long WhichMethodToCallback ) = 0;
};

#ifndef __ICaptureGraphBuilder2_INTERFACE_DEFINED__
#define __ICaptureGraphBuilder2_INTERFACE_DEFINED__
MIDL_INTERFACE("93E5A4E0-2D50-11d2-ABFA-00A0C9C6E38D")
ICaptureGraphBuilder2 : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetFiltergraph( 
        /* [in] */ IGraphBuilder *pfg) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetFiltergraph( 
        /* [out] */ 
        __out  IGraphBuilder **ppfg) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SetOutputFileName( 
        /* [in] */ const GUID *pType,
        /* [in] */ LPCOLESTR lpstrFile,
        /* [out] */ 
        __out  IBaseFilter **ppf,
        /* [out] */ 
        __out  IFileSinkFilter **ppSink) = 0;
    
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE FindInterface( 
        /* [in] */ 
        __in_opt  const GUID *pCategory,
        /* [in] */ 
        __in_opt  const GUID *pType,
        /* [in] */ IBaseFilter *pf,
        /* [in] */ REFIID riid,
        /* [out] */ 
        __out  void **ppint) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE RenderStream( 
        /* [in] */ 
        __in_opt  const GUID *pCategory,
        /* [in] */ const GUID *pType,
        /* [in] */ IUnknown *pSource,
        /* [in] */ IBaseFilter *pfCompressor,
        /* [in] */ IBaseFilter *pfRenderer) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE ControlStream( 
        /* [in] */ const GUID *pCategory,
        /* [in] */ const GUID *pType,
        /* [in] */ IBaseFilter *pFilter,
        /* [in] */ 
        __in_opt  REFERENCE_TIME *pstart,
        /* [in] */ 
        __in_opt  REFERENCE_TIME *pstop,
        /* [in] */ WORD wStartCookie,
        /* [in] */ WORD wStopCookie) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE AllocCapFile( 
        /* [in] */ LPCOLESTR lpstr,
        /* [in] */ DWORDLONG dwlSize) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE CopyCaptureFile( 
        /* [in] */ 
        __in  LPOLESTR lpwstrOld,
        /* [in] */ 
        __in  LPOLESTR lpwstrNew,
        /* [in] */ int fAllowEscAbort,
        /* [in] */ IAMCopyCaptureFileProgress *pCallback) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE FindPin( 
        /* [in] */ IUnknown *pSource,
        /* [in] */ PIN_DIRECTION pindir,
        /* [in] */ 
        __in_opt  const GUID *pCategory,
        /* [in] */ 
        __in_opt  const GUID *pType,
        /* [in] */ BOOL fUnconnected,
        /* [in] */ int num,
        /* [out] */ 
        __out  IPin **ppPin) = 0; 
};
#endif

static const IID IID_ISampleGrabber = { 0x6B652FFF, 0x11FE, 0x4fce, { 0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F } };
static const CLSID CLSID_SampleGrabber = { 0xC1F400A0, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };
static const CLSID CLSID_NullRenderer = { 0xC1F400A4, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };

typedef struct
{
	HWND	hWnd;
	UINT	nNotifyMsg;
	BOOL	bPostMsgEnable;
}OutputWndInf;

// grabber 回调函数调用的interface
class CCamera;
class SampleGrabberCBImpl : public ISampleGrabberCB
{
private:	
	LARGE_INTEGER				m_lastImageTime;
	LARGE_INTEGER				m_curImageTime;
	std::vector<OutputWndInf>	m_OutputWnds;

	CCamera*					m_pCam;
	ULONG						m_FrameNum;
	float						m_FPS;
	LARGE_INTEGER				m_frequency;
	CEvent*	    				m_pEventCaptured;
	CCriticalSection			m_CritSection;
public:
	SampleGrabberCBImpl();
	ULONG getFrameNum() const { return m_FrameNum; }
	float getFPS() const { return m_FPS; }

	void init_sample_image(CCamera* pCam, CEvent* pEventCaptured)
	{
		m_pCam = pCam;
		m_pEventCaptured = pEventCaptured;
	}

	void	AddOutputWnd(HWND hParent, UINT NotifyMsg, BOOL bEnable = FALSE);
	BOOL	DeleteWnd(HWND hWnd);
	BOOL	EnableNotifyMsg(HWND hWnd, BOOL bEnable = FALSE);
	void	ClearOutputWnds();

	void    EnableNotifyMsg(BOOL Enable = FALSE);

	virtual ULONG STDMETHODCALLTYPE AddRef() { return 2; }
	virtual ULONG STDMETHODCALLTYPE Release() { return 1; }
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);

	virtual STDMETHODIMP SampleCB( double SampleTime, IMediaSample *pSample ){return S_OK;}
	virtual STDMETHODIMP BufferCB( double SampleTime, BYTE *pBuffer, long BufferLen );
private:
	int FindWnd(HWND hWnd);
};

#endif
