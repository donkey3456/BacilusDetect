#pragma once
///////////////////////////////////////////////////////////////
//***********************************************************//
/*
					Written by Song Baoquan
					Modified by Lu Yujie 
												2015/04/19
	变更：
		该类的内容改变有如下3点
			1. 该类为单例模式，不提供构造函数，只提供一个全局访问点GetInstance
			2. 该类所用Opencv版本为2.4.11
			3. 该类可以同时和多个串口进行关联
		接口方面：
			1. 由于该类改为了单利模式，构造函数接口被取消，需要全部改为全局访问点
			2. 此外，添加了Open()AddOutputWnd()DeleteOutputWnd()等新接口
				Open()可以不连接窗口打开并获取图像
				AddOutputWnd()可以连接新的窗口，同一个窗口不会连接两次，所有参数
				以后一次连接为准
				DeleteOutputWnd()则可以删除已连接的窗口
			   该类原有的connect函数依然保存，在只连接一个窗口的时候，依然可以只
			   使用该函数
			3. 图像获取函数GetImage的返回值改为了cv::Mat
		该类的依赖文件有两个
			TCaptureContext.h
			TCaptureContext.cpp
		由于这两个文件有修改，在使用此更新类时，请务必使用和此类匹配的文件
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
	//获取全局访问点，只生成一个实例（可重复调用）
	static CCamera* GetInstance();
	//释放空间（可重复调用）
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
//内部操作
private:
	int FindWndIndex(HWND hWnd);
//接口
public:
	// 旧方法，连接相机与显示
	// 参数：
	// hParent：the Handle of the window that will receive the message from Camera
	// NotifyMsg: message ID
	BOOL		connect(HWND hParent, UINT NotifyMsg);   
	// 新方法，连接摄像机
	BOOL		Open();
	// 新方法，连接显示
	void		AddOutputWnd(HWND hParent, UINT NotifyMsg);
	// 新方法，连接
	void		DeleteOutputWnd(HWND hParent);
	// 获取最新图像
	cv::Mat		GetImage();
	//关闭相机
	void		close(void);
	// 相机控制函数
	BOOL		AutoWhiteBalance();
	BOOL		AutoExpoEnable(BOOL bAutoExposure);
	BOOL		SetExposureTime(ULONG ExposureTime);
	int			GetExposureTime();
	// 信息获取函数
	BOOL		isCapturing() { return m_bCapturing; }
	HANDLE		CapturedEvent() { return HANDLE(m_EventCaptured);}
	ULONG		getFrameNum();
	float		getFPS();
	// 显示控制函数
	void		EnableNotifyMsg(BOOL Enable = FALSE);
	void		EnableNotifyMsg(HWND hWnd, BOOL bEnable);

	cv::Size2f	GetCameraFOVSize();
	cv::Size	GetCameraImageSize();

private:
	void			SwitchBuffer();
	uchar*			GetBufferPtr();
	static UINT		ReadBuffer(LPVOID pParam);
};
