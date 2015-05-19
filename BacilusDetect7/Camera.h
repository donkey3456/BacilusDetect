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
#include <vector>
#include "opencv2\opencv.hpp"

#ifdef WIN32
#include "xiApi.h"
#else
#include <m3api/xiApi.h>
#endif

#define CAMERA_PIXEL_SIZE_UM 5.5
#define LENS_RATIO 100




struct OUTPUT_WINDOW_INFO 
{
	HWND hWnd;
	UINT uiNotifyMsg;
	BOOL bNotify;
};
class CCamera
{
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

	HANDLE						m_hCamera;

	HANDLE						m_hThread;
	ULONG						m_ulFrameNum;
	std::vector<OUTPUT_WINDOW_INFO>	m_OutputWnds;
	CRITICAL_SECTION			m_WndsPro;
	CRITICAL_SECTION			m_BufferPro;
	XI_IMG						m_ImageBuffer;
//�ڲ�����
private:
	int FindWndIndex(HWND hWnd);
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
	void		AddOutputWnd(HWND hParent, UINT NotifyMsg);
	// �·���������
	void		DeleteOutputWnd(HWND hParent);
	// ��ȡ����ͼ��
	cv::Mat		GetImage();
	//�ر����
	void		close(void);
	// ������ƺ���
	BOOL		AutoWhiteBalance();
	BOOL		AutoExpoEnable(BOOL bAutoExposure);
	BOOL		SetExposureTime(ULONG ExposureTime);
	int			GetExposureTime();
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
	void			SwitchBuffer();
	uchar*			GetBufferPtr();
	static UINT		ReadBuffer(LPVOID pParam);
};
