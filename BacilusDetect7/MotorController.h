#pragma once

//////////////////////////////////////////////////////////////////////////
/*
	GRBL XYZ MOTOR CONTROLLER 
				Written by Yujie LU
				22\04\2015
	
	�������ڿ�������GRBL��ADRUINO�塣������λ�������Ѿ�ȷ������ʱ�����и��ģ�����
	��Ҫ����GRBL�Ĺ��ܡ�

	�����Ƚ���GRBL��һЩ���ƣ���������û�з�����ȷ��ʹ�÷�������
		1.	�����������ͬʱ�˶������ٶȱ�����ͬ�����򽫻�һ��һ����
		2.	ֻ��ִ����ǰ�����еĵ��������Ż�ִ����һ������������ѯ��״̬��ʱ������
		3.	��Ҫֹͣ�������ͬʱֹͣ�����Ҳ���������ֹͣ�����Ǽ���ֹͣ����Ȼ�ᶪ��
		4.	�޷���õ���һ��������˶�״̬��ֻ�ܻ�ȡϵͳ�ǲ�����running


	������������
		����Ϊ����ģʽ�࣬���ṩ���������������ֻ�ṩһ��ȫ�ֺ���GetInstance���û�
		���ʵ����ָ�롣�رճ���ʱ�������release��ʵ���ͷš�


	������رգ�

		����ADRUINO�壬ֻ�����Connect������������Ϊ�ɹ���debug��������˿���� 
		Find Device!�ú������Զ��򿪵�һ�����ڲ����Ƶ���Զ����ػ�еԭ�㣬����еԭ
		����Ϊ�����㡣������ʱ�Ѿ����ӣ�������ֱ�ӷ���TRUE����ʲôҲ������debug
		��������˿����Comm is Opened!

		�����Ҫ�ȹر������ӣ�����ʹ��Reconnect()���ȵ���Close��Connect

		�����ؼ٣���Ϊʧ�ܣ�ʧ�ܿɽ�һ����Ϊ���¼������
			1. ���ڸú������Զ�����ƥ��Ĵ��ڣ���û��ƥ��Ĵ��ڵ�ʱ�򣬺����᷵��
			����debug��������˿����Can't find Device!���Ѳ��ԣ�
			2. �����ڱ�ռ�ã�debug��������˿����COM is not available!��������
			����һ�����ڣ��Ѳ��ԣ�
			3. �ú�������֤���ڷ��ص���Ϣ�������ƥ��ʱ��debug��������˿����
			Return wrong message!�����鿴��һ�����ڣ�δ��֤��
			4. ��֤���ڷ�����Ϣʱ����ʱ��Ϊ3�룬�����ʱ������Ϣ����debug�������
			�˿����Return wrong message!���Ѳ��ԣ�
		����״̬����isConnecting()��ѯ������Ҫ��ʽ�رմ���,���������л��Զ��رմ���

		�ر�ʱϵͳ���Զ�ֹͣ�����������������

	ʵʱ���ƺ�����
		Stop
		Reset
		KillAlarm
		ForceAbort
		���ຯ��Ϊ��Ƭ������ʵʱ������������ܼ��������ƺ����ڡ�

	�������ƺ���:

		�������ƺ���Ϊ���Ƶ���ĺ�������λ����ִ�и��ຯ�������������ʱ�����һִ�У�
		����ͬʱִ�У����緢������ʹƽ̨��X���ƶ�10mm,��y���ƶ�10mm����λ�������ƶ�
		X�ᣬ���ƶ�y�ᣬ������ͬʱ�ƶ���
		
		������ִ��ʱ����һ���̣߳�ר���������Ͷ������Ƶ�������ж������ƺ����ڵ���ʱ
		�Ὣһ���������m_CmdQueue������m_hSendCmd��֪ͨ����������̣߳�ͬʱ�����Ὣ
		m_hIdle���ã�֪ͨ�ⲿ���������ת���漴���������ƺ������᷵�أ������ȴ���λ��
		��ɲ���������Ҫ�����صȴ��������˶��������WaitUntilDone

		���������߳�CmdThread�����ն��������������������һ���͸���λ���������̻߳����һ����
		��ɺ�Żᷢ����һ���������������겢���н������߳�Ϊ��m_hIdle��1��

		��������������������ǿ��ֹͣ����ɵ���Stop()

			Stop ������ȴ���ǰ�����������أ������ȴ���λ��������񣩣�Ҳ��ȴ���
			�������̶߳�����Ĳ�ѯ����ȡ����Щ�������Ǽ�ʱ��ɣ���֮��stop��ֹͣ
			�������ֲ�������������������գ���ǿ��ֹͣ��λ������������ɴ˲���������
			alarm����ʱϵͳ��������Ϊ��ȷ�ġ�

		���ֻҪ����alarm����ʹ��KillAlarm��������������������������̵�alarm��
		�ڲ���alarm��ʱ����ʱ�򲻽���Ҫkillalarm����Ҫreset����ʱ�����reset���Ƿ���Ҫ
		reset����killalarm����ͨ��isNeedReset������isNeedKillAarm��������ѯ

		�������ṩ��һ���ǳ�Ұ���ķ�ʽForceAbort������ֱ��reset������������ֱ������
		�����alarm�������ַ�ʽ���ܻᶪ����һ������벻Ҫʹ�á�

			���ڴ򻬲�����Alarm�޷��������Ҫʹ��Restart����ϵͳ�����½���
		
		�������ƺ����У�
			Home();	
				ʹϵͳ���ػ�е���
			SetZero();
				����ǰ����Ϊ�����㣬�벻Ҫ���øú������ú����ڳ�������ʱ�Զ����ã�
				��Ӳ�������Ϊ������
			XMotorRelativeMove(float fIncremental, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
			YMotorRelativeMove(float fIncremental, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
			ZMotorRelativeMove(float fIncremental, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
				X\Y\Z�����˶����������Ϊ�����������λĬ��Ϊ���ף���λҲ����ʹ�ò���΢��
				������΢����X������Ϊ�ң�������Ϊ�󣨵�ǰ������������СΪ1/4�����˶���ΧΪ��0,53��(ע��������ԭϵͳ��ͬ)
							Y������Ϊ�󣬸�����Ϊǰ����ǰ������������СΪ1/4�����˶���ΧΪ��-38��0��(ע��������ԭϵͳ��ͬ)
							Z������Ϊ�£�������Ϊ�ϣ���ǰ������������СΪ1/4�����˶���ΧΪ��0, 15.76��(ע��������ԭϵͳ��ͬ)
							��΢����Ӳ����������������´���(ע��ԭ����ԭϵͳ��ͬ)
			XYMotorRelativeMove(float fIncremental, CONTROL_METHOD nMethod = CONTROL_MILLIMETER);
				XY�����˶����������Ϊ�����������λĬ��Ϊ���ף���λҲ����ʹ�ò���΢��
				���XY������ٶ���ͬ����ͬʱ�˶��������ͬ��X�����˶���Y�����˶�
					XMove2(float fPos, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
					YMove2(float fPos, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
					ZMove2(float fPos, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
				X,Y,Z�����ƶ����������Ϊ�������꣬��λĬ��Ϊ���ף���λҲ����ʹ�ò���΢��
					XYMove2(float fXIncremental, float fYIncremental, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
					XYMove2(cv::Point2f ptPos, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
				XY�����ƶ����������Ϊ�������꣬��λĬ��Ϊ���ף���λҲ����ʹ�ò���΢��
				���XY������ٶ���ͬ����ͬʱ�˶��������ͬ��X�����˶���Y�����˶�


	�ٶȻ�ȡ�޸ĺ�����

		SetXMotorSpeed(float fSpeed,DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
		SetYMotorSpeed(float fSpeed,DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
		SetZMotorSpeed(float fSpeed,DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
		GetXMotorSpeed(DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
		GetYMotorSpeed(DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
		GetZMotorSpeed(DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
		�����ٶȵĵ�λΪmm/s��XY�������ٶ����Ϊ2000mm/min��Z����ǰ״̬�ٶ����Ϊ50mm/min
		�����ٶȲ�û��ֱ�Ӵ��͸���λ���������ڵ���MOVE��MOVE2ϵ�к�����ʱ�򣬻�������Ӧ�Ĵ��룬
		���ڵ���MOVE��MOVE2ϵ�к�����ʱ���ٶ�ֵ��Ϊ�������е��ٶ�ֵ����ʵ���˶���ʼʱ���ٶ�ֵ
		�޹�


	��ȡ��ǰ״̬������

		GetCurrentPos(float& fXpos, float& fYpos, float& fZPos);
		GetCurrentStatus(CString &str);
		GetCurrentPosAndStatus(float& fXpos, float& fYpos, float& fZPos, CString &str);
		GetCurrentStatus(MOTOR_STATUS &status);
		�⼸�������Ļ���ԭ����ͬ��������QueryCurrentStatus����ѯ��λ��״̬���ٽ���ͬ�Ĳ�������
		�������λ����ǰ״̬ΪALARM�����޷���ȡ��λ����ǰ״̬����ʱϵͳ��������һ�ε�״̬ͬ
		������������أ���ʱ��������FALSE�� ��QueryCurrentStatus˳�����У��򷵻�TRUE��

		��ȡ��ǰ״̬�����������鵱ǰ���ӣ���ʹ��ǰ����ȷ������״����

	��ȡ�¼�������
		
		GetAlarmHandle(){return m_hAlarm;}
		GetIdleHandle(){return m_hIdle;}
		GetStopHandle()
		�ֱ��ȡIDLE�¼���ALARM�¼�������ALARM�¼��Ĵ������ȼ��ϸߣ�һ�㽨��ʹ��
		WaitForMultipleObjects�ȴ�������ɣ��������η���stop��ALARM��idle�¼�������λ��

	��λ���㺯����
		Ϊ���㷽�㣬�ṩ��λ���㺯����Ĭ�ϵ�λΪ���ף�Ҳ��ʹ��um�Ͳ�
		ZUnit2Milli(float fUnit,DISTANCE_UNIT unit = DISTANCE_UNIT_MM);
		ZMilli2Unit(float fMilli,DISTANCE_UNIT unit = DISTANCE_UNIT_MM);
		XYUnit2Milli(float fUnit,DISTANCE_UNIT unit = DISTANCE_UNIT_MM);
		XYMilli2Unit(float fMilli,DISTANCE_UNIT unit = DISTANCE_UNIT_MM);

	�����еȴ�������
		�ȴ����������߳���alarm��������̣߳��ṩtimeout����
		WaitUntilDone����
		�����alarm��WaitUntilDone���Զ����alarm

	������
		GetMCUStatus����
		Ϊ�˺��ϳ���ƥ����ʱ���ϵĺ�����û��ִ�С�

*/
//////////////////////////////////////////////////////////////////////////


#include <vector>
#include "SerialPort.h"

#define MOTOR_Z_RESOLUTION 0.5			//unit:um
#define MOTOR_XY_RESOLUTION 25.4		//unit:um
#define MOTOR_Z_DIVIDE 4
#define MOTOR_XY_DIVIDE 4
#define MOTOR_Z_OFFSET_MM -14.7				//�д�����
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
	//��ȡȫ�ַ��ʵ㣬ֻ����һ��ʵ�������ظ����ã�
	static CMotorController* GetInstance();
	//�ͷſռ䣨���ظ����ã�
	static void Release();

public:
	//�����豸
	BOOL				Connect();
	BOOL				Reconnect();
	BOOL				isConnecting();
	void				Close();

	//�򵥿���
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
	//XY ����
	BOOL				XYMove2(float fXIncremental, float fYIncremental, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
	BOOL				XYMove2(cv::Point2f ptPos, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
	BOOL				XMove2(float fPos, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
	BOOL				YMove2(float fPos, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);
	BOOL				ZMove2(float fPos, DISTANCE_UNIT nMethod = DISTANCE_UNIT_MM);

	float				ZMotorSmoothMove2AbsolutePos(float fpos, DISTANCE_UNIT unit = DISTANCE_UNIT_MM);
	void				XYMotorSmoothMove2Point(cv::Point2f ptPos, DISTANCE_UNIT unit = DISTANCE_UNIT_MM);

	//����˶�
	BOOL				XMotorRelativeMove(float fIncremental, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	BOOL				YMotorRelativeMove(float fIncremental, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	BOOL				XYMotorRelativeMove(float fXPos, float fYPos, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	BOOL				ZMotorRelativeMove(float fIncremental, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);

	//Speed ����
	void				SetXMotorSpeed(float fSpeed,DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	void				SetYMotorSpeed(float fSpeed,DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	void				SetZMotorSpeed(float fSpeed,DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);

	//Speed��ȡ
	float				GetXMotorSpeed(DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	float				GetYMotorSpeed(DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	float				GetZMotorSpeed(DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);

	//��ȡ��ǰλ��
	BOOL				GetYMotorPosition(float& fYPos, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	BOOL				GetXMotorPosition(float& fXPos, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	BOOL				GetXYMotorPosition(float& fXPos, float& fYPos, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	BOOL				GetXYMotorPosition(cv::Point2f& ptPos, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	BOOL				GetZMotorPosition(float& fZPos, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	BOOL				GetXYZMotorPosition(float& fXPos, float& fYPos,float& fZPos, DISTANCE_UNIT nUnit = DISTANCE_UNIT_MM);
	//��ȡ��ǰ״̬
	BOOL				GetCurrentPos(float& fXpos, float& fYpos, float& fZPos);
	BOOL				GetCurrentStatus(CString &str);
	BOOL				GetCurrentPosAndStatus(float& fXpos, float& fYpos, float& fZPos, CString &str);
	BOOL				GetCurrentStatus(MOTOR_STATUS &status);

	//��λת��,ϵͳĬ��Ϊ������
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
	//�߳�
	static UINT			SendThread(LPVOID lpParam);
	BOOL				_waitUntilDone(DWORD dwInterval = 100);

	static UINT			ReadingThread(LPVOID lpParam);
	void				FindReturnCmd(CString strNewlyRecieved);

	//�ڲ�����
	std::vector<int>	FindDevices();

	BOOL				SetZero(float fX = 0, float fY = 0, float fZ = 0, DISTANCE_UNIT unit = DISTANCE_UNIT_MM);



	static void			WChar2char(wchar_t* pSrc,char*& pDes,int nLength);
private:
	//����ģʽָ��
	static CMotorController* m_pInstance;
	//����
	CSerialPort m_port;

	//Send thread ����
	std::vector<CString> m_CmdQueue;
	CRITICAL_SECTION m_protection;	//��������m_CmdQueue��m_hSendCmd

	//��ǰ״̬
	BOOL m_bOpen;
	//����
	MOTOR_STATUS m_nStatus;
	float m_fXPos;
	float m_fYPos;
	float m_fZPos;

	float m_fXSpeed;
	float m_fYSpeed;
	float m_fZSpeed;

	int m_nSentNoReply;

	//Recieve Thread �ڲ�����
	DWORD m_dwCheckInterval;
	CString m_strBuffer;

	//HANDLE
	HANDLE m_hBeginThread;			//��ʼ�߳̾��
	HANDLE m_hStop;					//�������

	HANDLE m_hSendThread;			//�����߳̾��
	HANDLE m_hRecieveThread;		//�����߳̾��

	HANDLE m_hSendCmd;				//��cmd��Ҫ�����߳�

	HANDLE m_hAlarm;				//�������
	HANDLE m_hOneJobDone;			//��ǰcmd���н�����������յ�ok)
	HANDLE m_hIdle;					//���о�������ж����������н�����û��alarm��
	HANDLE m_hNeedReset;			//��Ҫ���þ��
	HANDLE m_hNeedHomeOrUnlock;		//��Ҫ�������
	HANDLE m_hUpdated;				//�Ѿ��������ݾ��
	HANDLE m_hConnected;			//���Ӿ��
	
	
};

