#pragma once
///////////////////////////////////////////////////////////////
//***********************************************************//
/*
					Written by Song Baoquan
					Modified by Lu Yujie 
												2015/04/19
	�����
		��������ݸı�������3��
			1. ����Ϊ����ģʽ�����ṩ���캯����ֻ�ṩһ��ȫ�ַ��ʵ�GetInstance
			2. ��������Opencv�汾Ϊ2.4.11
			3. �������ͬʱ�Ͷ�����ڽ��й���
		�ӿڷ��棺
			1. ���ڸ����Ϊ�˵���ģʽ�����캯���ӿڱ�ȡ������Ҫȫ����Ϊȫ�ַ��ʵ�
			2. ���⣬�����Open()AddOutputWnd()DeleteOutputWnd()���½ӿ�
				Open()���Բ����Ӵ��ڴ򿪲���ȡͼ��
				AddOutputWnd()���������µĴ��ڣ�ͬһ�����ڲ����������Σ����в���
				�Ժ�һ������Ϊ׼
				DeleteOutputWnd()�����ɾ�������ӵĴ���
			   ����ԭ�е�connect������Ȼ���棬��ֻ����һ�����ڵ�ʱ����Ȼ����ֻ
			   ʹ�øú���
			3. ͼ���ȡ����GetImage�ķ���ֵ��Ϊ��cv::Mat
		����������ļ�������
			TCaptureContext.h
			TCaptureContext.cpp
		�����������ļ����޸ģ���ʹ�ô˸�����ʱ�������ʹ�úʹ���ƥ����ļ�
															*/
//***********************************************************//
///////////////////////////////////////////////////////////////
#include "SampleGrabberCBImpl.h"
#include <vector>
#include "opencv2\opencv.hpp"

#define CAMERA_PIXEL_SIZE_UM 6.45
#define LENS_RATIO 100

class CCamera
{
	friend class SampleGrabberCBImpl;
public:
	//��ȡȫ�ַ��ʵ㣬ֻ����һ��ʵ�������ظ����ã�
	static CCamera* GetInstance();
	//�ͷſռ䣨���ظ����ã�
	static void Release();
protected:
	CCamera(void);
	~CCamera(void);

private:
	static CCamera*				m_pInstance;
    BOOL						m_bCapturing;
	cv::Mat						m_matImage;
	cv::Mat						m_matBuffer;
	CEvent						m_EventCaptured;

	ICaptureGraphBuilder2*		m_pBuilder;
	IGraphBuilder*				m_pFg;
	IBaseFilter*				m_pSource;
	SampleGrabberCBImpl			m_SampleGrabberCBImp;

//�ڲ�����
private:
	BOOL		MakeBuilderAndGraph();
	BOOL		InitFilters();
	BOOL		StartPreview();
	void		ReleaseFG();
	void		StopPreview();
	PITOUPCAM_WHITEBALANCE_CALLBACK fnWBProc(const int aGain[3], void* pCtx);

//�ӿ�
public:
	// �ɷ����������������ʾ
	// ������
	// hParent��the Handle of the window that will receive the message from Camera
	// NotifyMsg: message ID
	BOOL		connect(HWND hParent, UINT NotifyMsg);   
	// �·��������������
	BOOL		Open();
	// �·�����������ʾ
	BOOL		AddOutputWnd(HWND hParent, UINT NotifyMsg);
	// �·���������
	BOOL		DeleteOutputWnd(HWND hParent);
	// ��ȡ����ͼ��
	cv::Mat		GetImage(){return m_matImage;}
	//�ر����
	void		close(void);
	// ������ƺ���
	BOOL		AutoWhiteBalance(RECT* pAuxRect);
	BOOL		AutoExpoEnable(BOOL bAutoExposure);
	BOOL		SetExposureTime(ULONG ExposureTime);
	ULONG		GetExposureTime();
	// ��Ϣ��ȡ����
	BOOL		isCapturing() { return m_bCapturing; }
	HANDLE		CapturedEvent() { return HANDLE(m_EventCaptured);}
	ULONG		getFrameNum();
	float		getFPS();
	// ��ʾ���ƺ���
	void		EnableNotifyMsg(BOOL Enable = FALSE);
	void		EnableNotifyMsg(HWND hWnd, BOOL bEnable);

	cv::Size2f	GetCameraFOVSize();
	cv::Size	GetCameraImageSize();

private:
	void		SwitchBuffer();
	uchar*		GetBufferPtr();
};

typedef struct
{
	std::wstring	DisplayName;
	std::wstring	FriendlyName;
}TCaptureDevice;


extern std::vector<TCaptureDevice> dscap_enum_device();
