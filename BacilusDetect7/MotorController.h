#pragma once

//////////////////////////////////////////////////////////////////////////
/*
	GRBL XYZ MOTOR CONTROLLER 
				Written by Yujie LU
				22\04\2015
	
	此类用于控制烧了GRBL的ADRUINO板。由于下位机程序已经确定，暂时不进行更改，控制
	主要基于GRBL的功能。

	下面先介绍GRBL的一些限制（或者我们没有发现正确的使用方法）：
		1.	三个电机可以同时运动，但速度必须相同，否则将会一个一个动
		2.	只有执行完前面所有的电机的命令，才会执行下一个电机的命令，但询问状态随时都可以
		3.	想要停止电机必须同时停止，而且并不是立即停止，而是减速停止，不然会丢步
		4.	无法获得单独一个电机的运动状态，只能获取系统是不是在running


	创建及析构：
		此类为单例模式类，不提供构造和析构函数，只提供一个全局函数GetInstance供用户
		获得实例的指针。关闭程序时必须调用release将实例释放。


	连接与关闭：

		连接ADRUINO板，只需调用Connect函数，返回真为成功，debug调试输出端口输出 
		Find Device!该函数会自动打开第一个串口并控制电机自动返回机械原点，将机械原
		点置为软件零点。如连接时已经连接，函数将直接返回TRUE，并什么也不做。debug
		调试输出端口输出Comm is Opened!

		如果需要先关闭再连接，可以使用Reconnect()或先调用Close再Connect

		若返回假，则为失败，失败可进一步分为以下几种情况
			1. 由于该函数会自动查找匹配的串口，当没有匹配的串口的时候，函数会返回
			错误，debug调试输出端口输出Can't find Device!（已测试）
			2. 若串口被占用，debug调试输出端口输出COM is not available!并继续检
			查下一个串口（已测试）
			3. 该函数会验证串口返回的信息，如果不匹配时，debug调试输出端口输出
			Return wrong message!，并查看下一个串口（未验证）
			4. 验证串口返回信息时，超时设为3秒，如果超时返回信息，则debug调试输出
			端口输出Return wrong message!（已测试）
		连接状态可用isConnecting()查询，不需要显式关闭串口,析构函数中会自动关闭串口

		关闭时系统会自动停止电机，并清空命令队列

	实时控制函数：
		Stop
		Reset
		KillAlarm
		ForceAbort
		该类函数为单片机控制实时函数，具体介绍见堵塞控制函数内。

	堵塞控制函数:

		阻塞控制函数为控制电机的函数，下位机在执行该类函数所发的命令的时候回逐一执行，
		不会同时执行，例如发送命令使平台沿X轴移动10mm,沿y轴移动10mm，下位机会先移动
		X轴，再移动y轴，并不会同时移动。
		
		本类在执行时创建一个线程，专门用来发送堵塞控制的命令。所有堵塞控制函数在调用时
		会将一组命令存入m_CmdQueue，并置m_hSendCmd来通知发送命令的线程；同时函数会将
		m_hIdle重置，通知外部电机正在运转。随即，堵塞控制函数即会返回，并不等待下位机
		完成操作。如需要堵塞地等待电机完成运动，请调用WaitUntilDone

		发送命令线程CmdThread，接收堵塞函数发来的命令，并逐一发送给下位机；这里线程会等上一命令
		完成后才会发送下一个命令，当所有命令发完并运行结束后，线程为将m_hIdle置1。

		所有阻塞控制命令如需强制停止，则可调用Stop()

			Stop 函数会等待当前阻塞函数返回（并不等待下位机完成任务），也会等待发
			送命令线程对命令的查询和提取（这些几乎都是即时完成），之后stop会停止
			以上两种操作，并将发送命令清空，并强行停止下位机工作，解除由此操作产生的
			alarm。此时系统的坐标仍为正确的。

		如果只要消除alarm，请使用KillAlarm。它可用于消除由于命令超出量程的alarm。
		在产生alarm的时候有时候不仅需要killalarm，还要reset，这时请调用reset。是否需要
		reset或者killalarm可以通过isNeedReset（）和isNeedKillAarm（）来查询

		本程序还提供了一个非常野蛮的方式ForceAbort，它会直接reset，并消除由于直接消除
		引起的alarm，但这种方式可能会丢步，一般情况请不要使用。

			由于打滑产生的Alarm无法清除，需要使用Restart进行系统的重新矫正
		
		堵塞控制函数有：
			Home();	
				使系统返回机械零点
			SetZero();
				将当前点设为软件零点，请不要调用该函数，该函数在初次连接时自动调用，
				将硬件零点设为软件零点
			XMotorRelativeMove(float fIncremental, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
			YMotorRelativeMove(float fIncremental, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
			ZMotorRelativeMove(float fIncremental, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
				X\Y\Z方向差补运动，输入参数为相对增量，单位默认为毫米，单位也可以使用步和微米
				面向显微镜，X正方向为右，负方向为左（当前假设驱动器最小为1/4），运动范围为（0,53）(注：方向与原系统相同)
							Y正方向为后，负方向为前（当前假设驱动器最小为1/4），运动范围为（-38，0）(注：方向与原系统相同)
							Z正方向为下，负方向为上（当前假设驱动器最小为1/4），运动范围为（0, 15.76）(注：方向与原系统相同)
							显微镜的硬件与软件零点在左后下处。(注：原点与原系统不同)
			XYMotorRelativeMove(float fIncremental, CONTROL_METHOD nMethod = CONTROL_MILLIMETER);
				XY方向差补运动，输入参数为相对增量，单位默认为毫米，单位也可以使用步和微米
				如果XY方向的速度相同，则同时运动，如果不同，X轴先运动，Y轴再运动
					XMove2(float fPos, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
					YMove2(float fPos, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
					ZMove2(float fPos, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
				X,Y,Z方向移动，输入参数为绝对坐标，单位默认为毫米，单位也可以使用步和微米
					XYMove2(float fXIncremental, float fYIncremental, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
					XYMove2(cv::Point2f ptPos, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
				XY方向移动，输入参数为绝对坐标，单位默认为毫米，单位也可以使用步和微米
				如果XY方向的速度相同，则同时运动，如果不同，X轴先运动，Y轴再运动


	速度获取修改函数：

		SetXMotorSpeed(float fSpeed,DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
		SetYMotorSpeed(float fSpeed,DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
		SetZMotorSpeed(float fSpeed,DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
		GetXMotorSpeed(DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
		GetYMotorSpeed(DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
		GetZMotorSpeed(DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
		这里速度的单位为mm/s，XY方向上速度最大为2000mm/min，Z方向当前状态速度最大为50mm/min
		这里速度并没有直接传送给下位机，而是在调用MOVE和MOVE2系列函数的时候，会生成相应的代码，
		即在调用MOVE和MOVE2系列函数的时的速度值将为最终运行的速度值，和实际运动开始时的速度值
		无关


	获取当前状态函数：

		GetCurrentPos(float& fXpos, float& fYpos, float& fZPos);
		GetCurrentStatus(CString &str);
		GetCurrentPosAndStatus(float& fXpos, float& fYpos, float& fZPos, CString &str);
		GetCurrentStatus(MOTOR_STATUS &status);
		这几个函数的基本原理相同，都调用QueryCurrentStatus，查询下位机状态，再将不同的参数返回
		但如果下位机当前状态为ALARM，将无法读取下位机当前状态，此时系统将会把最近一次的状态同
		过输入参数返回，此时函数返回FALSE。 若QueryCurrentStatus顺利运行，则返回TRUE。

		获取当前状态函数并不会检查当前连接，请使用前自行确认连接状况。

	获取事件函数：
		
		GetAlarmHandle(){return m_hAlarm;}
		GetIdleHandle(){return m_hIdle;}
		GetStopHandle()
		分别获取IDLE事件和ALARM事件。由于ALARM事件的处理优先级较高，一般建议使用
		WaitForMultipleObjects等待工作完成，并将依次放入stop，ALARM和idle事件放于首位。

	单位换算函数：
		为计算方便，提供单位换算函数，默认单位为毫米，也可使用um和步
		ZUnit2Milli(float fUnit,DISTANCE_UNIT unit = DISTANCE_UNIT_MM);
		ZMilli2Unit(float fMilli,DISTANCE_UNIT unit = DISTANCE_UNIT_MM);
		XYUnit2Milli(float fUnit,DISTANCE_UNIT unit = DISTANCE_UNIT_MM);
		XYMilli2Unit(float fMilli,DISTANCE_UNIT unit = DISTANCE_UNIT_MM);

	堵塞行等待函数：
		等待电机走完或者出现alarm，会堵塞线程，提供timeout参数
		WaitUntilDone（）
		如产生alarm，WaitUntilDone会自动清空alarm

	其他：
		GetMCUStatus（）
		为了和老程序匹配暂时加上的函数，没有执行。

*/
//////////////////////////////////////////////////////////////////////////


#include <vector>
#include "SerialPort.h"

#define MOTOR_Z_RESOLUTION 0.5			//unit:um
#define MOTOR_XY_RESOLUTION 25.4		//unit:um
#define MOTOR_Z_DIVIDE 4
#define MOTOR_XY_DIVIDE 4
#define MOTOR_Z_OFFSET_MM -14.7				//有待测量
#define MOTOR_Z_SAFE_POS_MM 0 
#define MOTOR_CHANGE_SLIDE_X_MM 18


enum MOTOR_STATUS
{
	MOTOR_UNKNOWN = 0,
	MOTOR_IDLE = 1,
	MOTOR_QUEUE = 2,
	MOTOR_RUN = 3,
	MOTOR_ALARM = 4,
	MOTOR_HOLD = 5,
};

typedef enum {
	DISTANCE_UNIT_STEP = 0x00,
	DISTANCE_UNIT_UM,
	DISTANCE_UNIT_MM
} DISTANCE_UNIT;


class CMotorController
{
public:
	//获取全局访问点，只生成一个实例（可重复调用）
	static CMotorController* GetInstance();
	//释放空间（可重复调用）
	static void Release();

public:
	//开关设备
	BOOL				Connect();
	BOOL				Reconnect();
	BOOL				isConnecting();
	void				Close();

	//简单控制
	BOOL				Restart();
	BOOL				Home();
	BOOL				ForceAbort();
	BOOL				Stop();			//stop motor including hold and reset
	
	BOOL				Reset();		//just reset
	BOOL				KillAlarm();	//just kill alarm

	BOOL				isNeedReset();
	BOOL				isNeedKillAarm();

	HANDLE				GetAlarmHandle(){return m_hAlarm;}
	HANDLE				GetIdleHandle(){return m_hIdle;}
	HANDLE				GetStopHandle(){return m_hStop;}
	//XY 控制
	BOOL				XYMove2(float fXIncremental, float fYIncremental, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
	BOOL				XYMove2(cv::Point2f ptPos, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
	BOOL				XMove2(float fPos, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
	BOOL				YMove2(float fPos, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
	BOOL				ZMove2(float fPos, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);

	float				ZMotorSmoothMove2AbsolutePos(float fpos, DISTANCE_UNIT unit = DISTANCE_UNIT_MM);
	void				XYMotorSmoothMove2Point(cv::Point2f ptPos, DISTANCE_UNIT unit = DISTANCE_UNIT_MM);

	//相对运动
	BOOL				XMotorRelativeMove(float fIncremental, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	BOOL				YMotorRelativeMove(float fIncremental, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	BOOL				XYMotorRelativeMove(float fXPos, float fYPos, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	BOOL				ZMotorRelativeMove(float fIncremental, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);

	//Speed 控制
	void				SetXMotorSpeed(float fSpeed,DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	void				SetYMotorSpeed(float fSpeed,DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	void				SetZMotorSpeed(float fSpeed,DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);

	//Speed获取
	float				GetXMotorSpeed(DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	float				GetYMotorSpeed(DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	float				GetZMotorSpeed(DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);

	//获取当前位置
	BOOL				GetYMotorPosition(float& fYPos, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	BOOL				GetXMotorPosition(float& fXPos, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	BOOL				GetXYMotorPosition(float& fXPos, float& fYPos, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	BOOL				GetXYMotorPosition(cv::Point2f& ptPos, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	BOOL				GetZMotorPosition(float& fZPos, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	BOOL				GetXYZMotorPosition(float& fXPos, float& fYPos,float& fZPos, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	//获取当前状态
	BOOL				GetCurrentPos(float& fXpos, float& fYpos, float& fZPos);
	BOOL				GetCurrentStatus(CString &str);
	BOOL				GetCurrentPosAndStatus(float& fXpos, float& fYpos, float& fZPos, CString &str);
	BOOL				GetCurrentStatus(MOTOR_STATUS &status);

	//单位转换,系统默认为毫米制
	float				ZUnit2Milli(float fUnit,DISTANCE_UNIT unit = DISTANCE_UNIT_MM);
	float				ZMilli2Unit(float fMilli,DISTANCE_UNIT unit = DISTANCE_UNIT_MM);
	float				XYUnit2Milli(float fUnit,DISTANCE_UNIT unit = DISTANCE_UNIT_MM);
	float				XYMilli2Unit(float fMilli,DISTANCE_UNIT unit = DISTANCE_UNIT_MM);

	BOOL				WaitUntilDone(DWORD dwTimeout = INFINITE);

	BOOL				QueryCurrentStatus();

	float				GetYMotorPosition(){return m_fYPos;}
	float				GetXMotorPosition(){return m_fXPos;}
	cv::Point2f			GetXYMotorPosition(){return cv::Point2f(m_fXPos,m_fYPos);}
	float				GetZMotorPosition(){return m_fZPos;}
	cv::Point3f			GetXYZMotorPosition(){return cv::Point3f(m_fXPos,m_fYPos,m_fZPos);}
	MOTOR_STATUS		GetCurrentStatus(){return m_nStatus;}
protected:
	CMotorController(void);
	~CMotorController(void);
private:
	//线程
	static UINT			SendThread(LPVOID lpParam);
	BOOL				_waitUntilDone(DWORD dwInterval = 100);

	static UINT			ReadingThread(LPVOID lpParam);
	void				FindReturnCmd(CString strNewlyRecieved);

	//内部函数
	std::vector<int>	FindDevices();

	BOOL				SetZero(float fX = 0, float fY = 0, float fZ = 0, DISTANCE_UNIT unit = DISTANCE_UNIT_MM);



	static void			WChar2char(wchar_t* pSrc,char*& pDes,int nLength);
private:
	//单利模式指针
	static CMotorController* m_pInstance;
	//串口
	CSerialPort m_port;

	//Send thread 参数
	std::vector<CString> m_CmdQueue;
	CRITICAL_SECTION m_protection;	//用来保护m_CmdQueue和m_hSendCmd

	//当前状态
	BOOL m_bOpen;
	//待改
	MOTOR_STATUS m_nStatus;
	float m_fXPos;
	float m_fYPos;
	float m_fZPos;

	float m_fXSpeed;
	float m_fYSpeed;
	float m_fZSpeed;

	int m_nSentNoReply;

	//Recieve Thread 内部参数
	DWORD m_dwCheckInterval;
	CString m_strBuffer;

	//HANDLE
	HANDLE m_hBeginThread;			//开始线程句柄
	HANDLE m_hStop;					//结束句柄

	HANDLE m_hSendThread;			//发送线程句柄
	HANDLE m_hRecieveThread;		//接收线程句柄

	HANDLE m_hSendCmd;				//有cmd需要发送线程

	HANDLE m_hAlarm;				//警报句柄
	HANDLE m_hOneJobDone;			//当前cmd运行结束句柄（接收到ok)
	HANDLE m_hIdle;					//空闲句柄（所有堵塞命令运行结束且没有alarm）
	HANDLE m_hNeedReset;			//需要重置句柄
	HANDLE m_hNeedHomeOrUnlock;		//需要解锁句柄
	HANDLE m_hUpdated;				//已经更新数据句柄
	HANDLE m_hConnected;			//连接句柄
	
	
};

