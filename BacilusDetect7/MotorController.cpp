#include "StdAfx.h"
#include "MotorController.h"

CMotorController* CMotorController::m_pInstance = NULL;

CMotorController::CMotorController(void)
{
	m_bOpen = FALSE;

	//�ٶȳ�ʼ��Ϊ���ֵ
	m_fXSpeed = 2000 / 60.0;
	m_fYSpeed = 2000 / 60.0;
	m_fZSpeed = 50 / 60.0;

	m_fXPos = 0;
	m_fYPos = 0;
	m_fZPos = 0;
	m_nStatus = MOTOR_UNKNOWN;

	m_hBeginThread = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hStop = CreateEvent(NULL,TRUE,FALSE,NULL);

	m_hSendCmd = CreateEvent(NULL,TRUE,FALSE,NULL);

	m_hAlarm = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hOneJobDone = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hIdle = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hNeedReset = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hNeedHomeOrUnlock = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hUpdated = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hConnected = CreateEvent(NULL,TRUE,FALSE,NULL);

	m_hSendThread = INVALID_HANDLE_VALUE;
	m_hRecieveThread = INVALID_HANDLE_VALUE;

	m_dwCheckInterval = 10;

	InitializeCriticalSection(&m_protection);
}


CMotorController::~CMotorController(void)
{
	Close();

	DeleteCriticalSection(&m_protection);

	//������Դ
	CloseHandle(m_hBeginThread);
	CloseHandle(m_hStop);
	CloseHandle(m_hSendCmd);
	CloseHandle(m_hAlarm);
	CloseHandle(m_hOneJobDone);
	CloseHandle(m_hIdle);
	CloseHandle(m_hNeedReset);
	CloseHandle(m_hNeedHomeOrUnlock);
	CloseHandle(m_hUpdated);
	CloseHandle(m_hConnected);

	m_hBeginThread		= INVALID_HANDLE_VALUE;
	m_hStop				= INVALID_HANDLE_VALUE;
	m_hSendCmd			= INVALID_HANDLE_VALUE;
	m_hAlarm			= INVALID_HANDLE_VALUE;
	m_hOneJobDone		= INVALID_HANDLE_VALUE;
	m_hIdle				= INVALID_HANDLE_VALUE;
	m_hNeedReset		= INVALID_HANDLE_VALUE;
	m_hNeedHomeOrUnlock	= INVALID_HANDLE_VALUE;
	m_hUpdated			= INVALID_HANDLE_VALUE;
	m_hConnected		= INVALID_HANDLE_VALUE;

	m_hSendThread = INVALID_HANDLE_VALUE;
	m_hRecieveThread = INVALID_HANDLE_VALUE;


}

std::vector<int> CMotorController::FindDevices()
{
	HKEY hkey;  
	int result;
	int i = 0;
	std::vector<int> res;

	//ʹ��ע����ѯ
	result = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
		_T( "Hardware\\DeviceMap\\SerialComm" ),
		NULL,
		KEY_READ,
		&hkey );

	if( ERROR_SUCCESS == result )   
	{   
		TCHAR portName[ 0x100 ], commName[ 0x100 ];
		DWORD dwLong, dwSize;
		do
		{   
			//��ע����в�ѯ������Ϣ
			dwSize = sizeof( portName ) / sizeof( TCHAR );
			dwLong = dwSize;
			result = RegEnumValue( hkey, i, portName, &dwLong, NULL, NULL, ( LPBYTE )commName, &dwSize );
			if( ERROR_NO_MORE_ITEMS == result )
			{
				break;  
			}

			//portName ƥ��ż����豸�б�
			CString strDrName = portName;
			int len = strDrName.ReverseFind( '\\');
			CString fileName = strDrName.Right(strDrName.GetLength() - len-1);
			strDrName = fileName.Left(3);
			strDrName.MakeUpper();
			if (strDrName.Compare(CString("VCP"))==0)
			{
				strDrName = commName;
				fileName = strDrName.Right(strDrName.GetLength() - 3);
				int nCom = _ttoi(fileName);
				res.push_back(nCom);
			}

			i = i + 1;
		}while (1);
	}
	return res;
}

BOOL CMotorController::Connect()
{
	//���Ѵ򿪴���
	if (m_bOpen)
	{
		TRACE("Comm is Opened!@Connect\n");
		return TRUE;
	}
	
	//��������ƥ���Devices
	std::vector<int> devices = FindDevices();
	//��δ�ҵ�
	if (devices.size() == 0 )
	{
		TRACE("Can't find Device!@Connect\n");
		return FALSE;
	}

	ResetEvent(m_hBeginThread);
	ResetEvent(m_hStop);
	ResetEvent(m_hSendCmd);
	ResetEvent(m_hAlarm);
	ResetEvent(m_hOneJobDone);
	ResetEvent(m_hIdle);
	ResetEvent(m_hNeedReset);
	ResetEvent(m_hNeedHomeOrUnlock);
	ResetEvent(m_hUpdated);
	ResetEvent(m_hConnected);

	//��ʼ�����߳�
	m_hRecieveThread = AfxBeginThread(CMotorController::ReadingThread,this)->m_hThread;
	WaitForSingleObject(m_hBeginThread,INFINITE);
	ResetEvent(m_hBeginThread);
	int i = 0;
	//���ҵ������ҷ���ƥ���Devices
	for (; i < devices.size(); i++)
	{
		//���Դ򿪴���
		
		try
		{
			int nCom = devices[i];
			m_port.Open(nCom,115200);
		}
		//��ô����ѱ����������
		catch (CSerialException* e)
		{
			TRACE("COM is not available!@Connect\n");
			m_port.Close();
			continue;
		}
		//������Դ�,�ȴ�MCU��feedback
		
		DWORD dwRes = WaitForSingleObject(m_hConnected,1500);
		if (dwRes == WAIT_OBJECT_0)
		{
			//����õ���ȷ�Ļظ�����տ��ܽӵ����¼���������֪�����Ҳ��ټ�������,
			ResetEvent(m_hConnected);
			ResetEvent(m_hNeedHomeOrUnlock);
			break;
		}
		else
		{
			//���δ�õ���ȷ�ظ�������һ��reset����
			char cmdReset = 0x18;
			m_port.Write(&cmdReset,1);
			m_port.Flush();
			TRACE("����д�룺reset@Connect\n");
			dwRes = WaitForSingleObject(m_hConnected, 1500);

			if (dwRes == WAIT_OBJECT_0)
			{
				//����õ���ȷ�Ļظ�����տ��ܽӵ����¼���������֪�����Ҳ��ټ�������,
				ResetEvent(m_hConnected);
				ResetEvent(m_hNeedHomeOrUnlock);
				break;
			}
			//����ظ�������ȷ����˵����device�������ǵİ���
			TRACE("Return wrong message!@Connect\n");
			m_port.Close();
			//�����¼��Է��򿪴��ں����
			ResetEvent(m_hAlarm);
			ResetEvent(m_hOneJobDone);
			ResetEvent(m_hIdle);
			ResetEvent(m_hNeedReset);
			ResetEvent(m_hNeedHomeOrUnlock);
			ResetEvent(m_hUpdated);
			ResetEvent(m_hConnected);
		}
	}

	if (i == devices.size())
	{
		//���û�ҵ�,�رս����߳�
		TRACE("No Device!@Connect\n");
		SetEvent(m_hStop);
		WaitForSingleObject(m_hRecieveThread,INFINITE);
		ResetEvent(m_hStop);
		m_hRecieveThread = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	//����ҵ�����ʼ�����߳�
	TRACE("Find Device!@Connect\n");
	m_hSendThread = AfxBeginThread(CMotorController::SendThread,this)->m_hThread;
	WaitForSingleObject(m_hBeginThread,INFINITE);
	ResetEvent(m_hBeginThread);

	m_bOpen = TRUE;
	//����
	Home();
	SetZero(0,0,MOTOR_Z_OFFSET_MM);
	//�ߵ�������Ƭ��λ��
	XMotorRelativeMove(MOTOR_CHANGE_SLIDE_X_MM);

// 	ZMotorRelativeMove(MOTOR_Z_OFFSET);
// 	SetZero();
	//KillAlarm();
	//SetEvent(m_hIdle);
	return TRUE;

}

void CMotorController::Close()
{
	if (m_bOpen)
	{
		TRACE("Close MotorController!@Close\n");
		//������磬��ֹʹ��
		m_bOpen = FALSE;
		//�رյ��,����������
		ForceAbort();

		//�ر��߳�
		SetEvent(m_hStop);
		HANDLE hThreads[2];
		hThreads[0] = m_hSendThread;
		hThreads[1] = m_hRecieveThread;
		WaitForMultipleObjects(2,hThreads,TRUE,INFINITE);

		//�رմ���
		m_port.Close();

		m_hSendThread = INVALID_HANDLE_VALUE;
		m_hRecieveThread = INVALID_HANDLE_VALUE;
	}
}

BOOL CMotorController::isConnecting()
{
	return m_bOpen;
}

BOOL CMotorController::QueryCurrentStatus()
{
	//�����Ѹ����¼�
	ResetEvent(m_hUpdated);

	//д��
	m_port.Write("?",1);
	m_port.Flush();
	TRACE("����д�룺?@QueryCurrentStatus\n");

	DWORD dwRes = WaitForSingleObject(m_hUpdated,300);
	//����ȵ���
	if (dwRes == WAIT_OBJECT_0)
	{
		ResetEvent(m_hUpdated);
		return TRUE;
	}
	else
	{
		TRACE("TIME OUT!@QueryCurrentStatus\n");
		return FALSE;
	}
}

BOOL CMotorController::Home()
{	
	if (!m_bOpen)
	{
		TRACE("The Comm is not opened.@Home\n");
		return FALSE;
	}
	//�Ȼص���翪��λ��
	CString strCmd = _T("$H");
	EnterCriticalSection(&m_protection);
	m_CmdQueue.push_back(strCmd);
	SetEvent(m_hSendCmd);
	ResetEvent(m_hIdle);
	LeaveCriticalSection(&m_protection);

	//�������ƶ���ԭ��,ʵ���ȡ��ע��
	//ZMotorRelativeMove(MOTOR_Z_OFFSET);
	return TRUE;
}

BOOL CMotorController::_waitUntilDone(DWORD dwInterval)
{
	BOOL bRecievedOK = FALSE;

	//�ȵ����ʼִ��
	while (1)
	{
		HANDLE handles[3];
		handles[0] = m_hStop;
		handles[1] = m_hAlarm;
		handles[2] = m_hOneJobDone;
		DWORD res = WaitForMultipleObjects(3,handles,FALSE,dwInterval);
		//�������ok���򷵻�TRUE
		if (res == WAIT_OBJECT_0 || res == WAIT_OBJECT_0 + 1)
		{
			return FALSE;
		}
		if (res == WAIT_OBJECT_0 + 2)
		{
			bRecievedOK = TRUE;
			break;
		}
		QueryCurrentStatus();
	}
	
	//double check because sometimes the controller will return "ok" first
	QueryCurrentStatus();
	QueryCurrentStatus();
	while(m_nStatus != MOTOR_ALARM && m_nStatus != MOTOR_IDLE)
	{
		DWORD res = WaitForSingleObject(m_hStop,dwInterval);
		QueryCurrentStatus();
	}

	if (m_nStatus != MOTOR_IDLE)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CMotorController::ForceAbort()
{
	if (!m_port.IsOpen())
	{
		TRACE("The Comm is not opened!@ForceAbort\n");
		return FALSE;
	}

	EnterCriticalSection(&m_protection);
	//����������
	m_nStatus = MOTOR_ALARM;
	m_CmdQueue.clear();
	ResetEvent(m_hSendCmd);
	
	//RESET
	ResetEvent(m_hConnected);
	ResetEvent(m_hNeedHomeOrUnlock);
	char reset;
	reset = 0x18;
	m_port.Write(&reset,1);
	m_port.Flush();
	TRACE("����д�룺reset@ForceAbort\n");

	DWORD dwRes = WaitForSingleObject(m_hConnected,3000);
	ResetEvent(m_hConnected);
	
	QueryCurrentStatus();

	if (m_nStatus == MOTOR_IDLE)
	{
		//�������״̬Ϊidle,�����resetǰ��Ϊidle�������������
		SetEvent(m_hIdle);
		LeaveCriticalSection(&m_protection);
		return TRUE;
	}

	//��������£�����Ҫunlock

	ResetEvent(m_hOneJobDone);
	m_port.Write("$X\r",3);
	m_port.Flush();
	TRACE("����д�룺$X@ForceAbort\n");
	
	dwRes = WaitForSingleObject(m_hOneJobDone,1000);
	ResetEvent(m_hOneJobDone);

	if (dwRes == WAIT_OBJECT_0)
	{
		ResetEvent(m_hNeedHomeOrUnlock);
		LeaveCriticalSection(&m_protection);
		return TRUE;
	}

	//failed, Try it again!
	TRACE("Still have problem!@ForceAbort\n");
	LeaveCriticalSection(&m_protection);
	return FALSE;
}

BOOL CMotorController::GetCurrentPos( float& fXpos, float& fYpos, float& fZPos )
{

	BOOL bCurrenOrLast = QueryCurrentStatus();

	fXpos = m_fXPos;
	fYpos = m_fYPos;
	fZPos = m_fZPos;
	return bCurrenOrLast;
}

BOOL CMotorController::GetCurrentStatus( CString &str )
{

	BOOL bCurrenOrLast = QueryCurrentStatus();

	switch (m_nStatus)
	{
	case MOTOR_ALARM:
		str = _T("Alarm");
		break;
	case MOTOR_IDLE:
		str = _T("Idle");
		break;
	case MOTOR_QUEUE:
		str = _T("Queue");
		break;
	case MOTOR_RUN:
		str = _T("Run");
		break;
	default:
		str = _T("Unknown");
	}
	return bCurrenOrLast;
}

BOOL CMotorController::GetCurrentStatus( MOTOR_STATUS &status )
{
	BOOL bCurrenOrLast = QueryCurrentStatus();//��ǰ״̬��������һ����ȡ����״̬
	status = m_nStatus;
	return bCurrenOrLast;
}

BOOL CMotorController::GetCurrentPosAndStatus( float& fXpos, float& fYpos, float& fZPos, CString &str )
{
	BOOL bCurrenOrLast = QueryCurrentStatus();

	switch (m_nStatus)
	{
	case MOTOR_ALARM:
		str = _T("Alarm");
		break;
	case MOTOR_IDLE:
		str = _T("Idle");
		break;
	case MOTOR_QUEUE:
		str = _T("Queue");
		break;
	case MOTOR_RUN:
		str = _T("Run");
		break;
	default:
		str = _T("Unknown");
	}
	fXpos = m_fXPos;
	fYpos = m_fYPos;
	fZPos = m_fZPos;
	return bCurrenOrLast;
}

UINT CMotorController::SendThread( LPVOID lpParam )
{
	//�̺߳���
	CMotorController* pThis = (CMotorController*) lpParam;
	SetEvent(pThis->m_hBeginThread);
	while(1)
	{
		//�ȴ�����Ҫ���͵�����
		HANDLE handles[2];
		handles[0] = pThis->m_hStop;
		handles[1] = pThis->m_hSendCmd;
		DWORD res = WaitForMultipleObjects(2,handles,FALSE,INFINITE);
		if (res == WAIT_OBJECT_0)
		{
			//�رմ���
			break;
		}
		if (res == WAIT_OBJECT_0 + 1)
		{
			while(1)
			{
				if (!pThis->m_port.IsOpen())
				{
					//�������û�д򿪣��ص���һ��loop
					EnterCriticalSection(&pThis->m_protection);
					pThis->m_CmdQueue.clear();
					ResetEvent(pThis->m_hSendCmd);
					//SetEvent(pThis->m_hIdle);
					LeaveCriticalSection(&pThis->m_protection);
					break;
				}
				//��鵱ǰ�Ƿ���δ��������
				EnterCriticalSection(&pThis->m_protection);
				if (pThis->m_CmdQueue.size() == 0)
				{
					//���û�У�����m_hSendCmd
					ResetEvent(pThis->m_hSendCmd);
					pThis->QueryCurrentStatus();
					//��ǰ���й����Ѿ����ã�ֻҪ����Alarm���Ͱ�Idle��1
					//��Ϊ�п��ܻ����MOTOR_QUEUE����һ�����Ͻ���
					if (pThis->m_nStatus != MOTOR_ALARM)
					{
						SetEvent(pThis->m_hIdle);
					}
					LeaveCriticalSection(&pThis->m_protection);
					break;
				}
				//����У�ȡ����һ���������ͣ��ȴ�ִ�����
				CString str;
				str = pThis->m_CmdQueue[0];
				pThis->m_CmdQueue.erase(pThis->m_CmdQueue.begin());
				LeaveCriticalSection(&pThis->m_protection);
				char* pDes = NULL;
				WChar2char(str.GetBuffer(),pDes,str.GetLength());
				pThis->m_port.Write(pDes,str.GetLength()+1);
				pThis->m_port.Flush();
				delete []pDes;

				str.Insert(0,_T("Cmd sent:"));
				str.Append(_T("@SendThread"));
				pDes = NULL;
				WChar2char(str.GetBuffer(),pDes,str.GetLength());
				TRACE(pDes);
				delete []pDes;
				pThis->_waitUntilDone();
				
			}
		}
	}
	return 0;
}

BOOL CMotorController::XMotorRelativeMove( float fIncremental, DISTANCE_UNIT nUnit /*= CONTROL_MILLIMETER*/ )
{
	if (!m_bOpen)
	{
		TRACE("The Comm is not opened!@XMotorRelativeMove\n");

		return FALSE;
	}

	fIncremental = XYUnit2Milli(fIncremental,nUnit);

	CString str;
	str.Format(_T("G91 G01 G21 X%f F%f"),-fIncremental,m_fXSpeed * 60);
	EnterCriticalSection(&m_protection);
	m_CmdQueue.push_back(str);
	SetEvent(m_hSendCmd);
	ResetEvent(m_hIdle);
	LeaveCriticalSection(&m_protection);
	return TRUE;
}

BOOL CMotorController::YMotorRelativeMove( float fIncremental, DISTANCE_UNIT nUnit /*= CONTROL_MILLIMETER*/ )
{
	if (!m_bOpen)
	{
		TRACE("The Comm is not opened!@YMotorRelativeMove\n");

		return FALSE;
	}

	fIncremental = XYUnit2Milli(fIncremental,nUnit);

	CString str;
	str.Format(_T("G91 G01 G21 Y%f F%f"),fIncremental,m_fYSpeed * 60);
	EnterCriticalSection(&m_protection);
	m_CmdQueue.push_back(str);
	SetEvent(m_hSendCmd);
	ResetEvent(m_hIdle);
	LeaveCriticalSection(&m_protection);
	return TRUE;
}

BOOL CMotorController::XYMotorRelativeMove( float fXPos, float fYPos , DISTANCE_UNIT nUnit/* = CONTROL_MILLIMETER*/)
{
	if (!m_bOpen)
	{
		TRACE("The Comm is not opened!@XYMotorRelativeMove\n");

		return FALSE;
	}

	fXPos = XYUnit2Milli(fXPos,nUnit);
	fYPos = XYUnit2Milli(fYPos,nUnit);

	if (m_fXSpeed == m_fYSpeed)
	{
		CString str;
		str.Format(_T("G91 G01 G21 X%f Y%f F%f"),-fXPos,fYPos,m_fYSpeed * 60);
		EnterCriticalSection(&m_protection);
		m_CmdQueue.push_back(str);
		SetEvent(m_hSendCmd);
		ResetEvent(m_hIdle);
		LeaveCriticalSection(&m_protection);
	}
	else
	{
		CString str1,str2;
		str1.Format(_T("G91 G01 G21 X%f F%f"),-fXPos,m_fXSpeed * 60);
		str2.Format(_T("G91 G01 G21 Y%f F%f"),fYPos,m_fYSpeed * 60);
		EnterCriticalSection(&m_protection);
		m_CmdQueue.push_back(str1);
		m_CmdQueue.push_back(str2);
		SetEvent(m_hSendCmd);
		ResetEvent(m_hIdle);
		LeaveCriticalSection(&m_protection);
	}
	return TRUE;
}

BOOL CMotorController::ZMotorRelativeMove( float fIncremental, DISTANCE_UNIT nUnit /*= CONTROL_MILLIMETER*/ )
{
	if (!m_bOpen)
	{
		TRACE("The Comm is not opened!@ZMotorRelativeMove\n");

		return FALSE;
	}
	fIncremental = ZUnit2Milli(fIncremental,nUnit);
	CString str;
	str.Format(_T("G91 G01 G21 Z%f F%f"),-fIncremental,m_fZSpeed * 60);
	EnterCriticalSection(&m_protection);
	m_CmdQueue.push_back(str);
	SetEvent(m_hSendCmd);
	ResetEvent(m_hIdle);
	LeaveCriticalSection(&m_protection);
	return TRUE;
}

BOOL CMotorController::XMove2( float fPos , DISTANCE_UNIT nUnit)
{
	if (!m_bOpen)
	{
		TRACE("The Comm is not opened!@XMove2\n");

		return FALSE;
	}

	fPos = XYUnit2Milli(fPos,nUnit);

	CString str;
	str.Format(_T("G90 G01 G21 X%f F%f"),-fPos,m_fXSpeed * 60);
	EnterCriticalSection(&m_protection);
	m_CmdQueue.push_back(str);
	SetEvent(m_hSendCmd);
	ResetEvent(m_hIdle);
	LeaveCriticalSection(&m_protection);
	return TRUE;
}

BOOL CMotorController::YMove2( float fPos, DISTANCE_UNIT nUnit)
{
	if (!m_bOpen)
	{
		TRACE("The Comm is not opened!@YMove2\n");

		return FALSE;
	}

	fPos = XYUnit2Milli(fPos,nUnit);

	CString str;
	str.Format(_T("G90 G01 G21 Y%f F%f"),fPos,m_fYSpeed * 60);
	EnterCriticalSection(&m_protection);
	m_CmdQueue.push_back(str);
	SetEvent(m_hSendCmd);
	ResetEvent(m_hIdle);
	LeaveCriticalSection(&m_protection);
	return TRUE;
}

BOOL CMotorController::XYMove2( float fXPos, float fYPos, DISTANCE_UNIT nUnit /*= CONTROL_MILLIMETER*/ )
{
	if (!m_bOpen)
	{
		TRACE("The Comm is not opened!@XYMove2\n");

		return FALSE;
	}
	fXPos = XYUnit2Milli(fXPos,nUnit);
	fYPos = XYUnit2Milli(fYPos,nUnit);

	if (m_fXSpeed == m_fYSpeed)
	{
		CString str;
		str.Format(_T("G90 G01 G21 X%f Y%f F%f"),-fXPos,fYPos,m_fYSpeed * 60);
		EnterCriticalSection(&m_protection);
		m_CmdQueue.push_back(str);
		SetEvent(m_hSendCmd);
		ResetEvent(m_hIdle);
		LeaveCriticalSection(&m_protection);
	}
	else
	{
		CString str1,str2;
		str1.Format(_T("G90 G01 G21 X%f F%f"),-fXPos,m_fXSpeed * 60);
		str2.Format(_T("G90 G01 G21 Y%f F%f"),fYPos,m_fYSpeed * 60);
		EnterCriticalSection(&m_protection);
		m_CmdQueue.push_back(str1);
		m_CmdQueue.push_back(str2);
		SetEvent(m_hSendCmd);
		ResetEvent(m_hIdle);
		LeaveCriticalSection(&m_protection);
	}
	return TRUE;
}

BOOL CMotorController::XYMove2( cv::Point2f ptPos, DISTANCE_UNIT nUnit /*= DISTANCE_UNIT_MM*/ )
{
	if (!m_bOpen)
	{
		TRACE("The Comm is not opened!@XYMove2\n");

		return FALSE;
	}
	ptPos.x = XYUnit2Milli(ptPos.x,nUnit);
	ptPos.y = XYUnit2Milli(ptPos.y,nUnit);

	if (m_fXSpeed == m_fYSpeed)
	{
		CString str;
		str.Format(_T("G90 G01 G21 X%f Y%f F%f"),-ptPos.x,ptPos.y,m_fYSpeed * 60);
		EnterCriticalSection(&m_protection);
		m_CmdQueue.push_back(str);
		SetEvent(m_hSendCmd);
		ResetEvent(m_hIdle);
		LeaveCriticalSection(&m_protection);
	}
	else
	{
		CString str1,str2;
		str1.Format(_T("G90 G01 G21 X%f F%f"),-ptPos.x,m_fXSpeed * 60);
		str2.Format(_T("G90 G01 G21 Y%f F%f"),ptPos.y,m_fYSpeed * 60);
		EnterCriticalSection(&m_protection);
		m_CmdQueue.push_back(str1);
		m_CmdQueue.push_back(str2);
		SetEvent(m_hSendCmd);
		ResetEvent(m_hIdle);
		LeaveCriticalSection(&m_protection);
	}
	return TRUE;
}


BOOL CMotorController::ZMove2( float fPos , DISTANCE_UNIT nUnit/* = CONTROL_MILLIMETER*/)
{
	if (!m_bOpen)
	{
		TRACE("The Comm is not opened!@ZMove2\n");

		return FALSE;
	}
	
	fPos = ZUnit2Milli(fPos,nUnit);

	CString str;
	str.Format(_T("G90 G01 G21 Z%f F%f"),-fPos,m_fZSpeed * 60);
	EnterCriticalSection(&m_protection);
	m_CmdQueue.push_back(str);
	SetEvent(m_hSendCmd);
	ResetEvent(m_hIdle);
	LeaveCriticalSection(&m_protection);
	return TRUE;
}

BOOL CMotorController::Reconnect()
{
	if(m_bOpen)
	{
		Close();
	}
	return Connect();

}

BOOL CMotorController::SetZero(float fX/* = 0*/, float fY /*= 0*/, float fZ/* = 0*/, DISTANCE_UNIT unit/* = DISTANCE_UNIT_MM*/)
{
	fX = XYMilli2Unit(fX,unit);
	fY = XYMilli2Unit(fY,unit);
	fZ = ZMilli2Unit(fZ,unit);

	CString strCmd1, strCmd2, strCmd3;
	strCmd1.Format(_T("G90 G10 L20 P0 X%f"),fX);
	strCmd2.Format(_T("G90 G10 L20 P0 Y%f"),fY);
	strCmd3.Format(_T("G90 G10 L20 P0 Z%f"),fZ);
	EnterCriticalSection(&m_protection);
	m_CmdQueue.push_back(strCmd1);
	m_CmdQueue.push_back(strCmd2);
	m_CmdQueue.push_back(strCmd3);
	SetEvent(m_hSendCmd);
	ResetEvent(m_hIdle);
	LeaveCriticalSection(&m_protection);

	return TRUE;
}

BOOL CMotorController::Restart()
{
	if (!m_bOpen)
	{
		return FALSE;
	}
	ForceAbort();
	Home();
	SetZero();

	return TRUE;
}

CMotorController* CMotorController::GetInstance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CMotorController();  
	}
	return m_pInstance; 
}

void CMotorController::Release()
{
	if(NULL != m_pInstance)  
	{  
		delete m_pInstance;  
		m_pInstance = NULL;  
	}  
}

float CMotorController::GetXMotorSpeed( DISTANCE_UNIT nUnit )
{
	return XYMilli2Unit(m_fXSpeed,nUnit);
}

float CMotorController::GetYMotorSpeed( DISTANCE_UNIT nUnit )
{
	return XYMilli2Unit(m_fYSpeed,nUnit);
}

float CMotorController::GetZMotorSpeed( DISTANCE_UNIT nUnit )
{
	return  ZMilli2Unit(m_fZSpeed,nUnit);
}


BOOL CMotorController::GetXMotorPosition( float& fXPos, DISTANCE_UNIT nUnit )
{
	BOOL bRes = QueryCurrentStatus();
	fXPos = XYMilli2Unit(m_fXPos,nUnit);
	return  bRes;
}

BOOL CMotorController::GetYMotorPosition( float& fYPos, DISTANCE_UNIT nUnit )
{
	BOOL bRes = QueryCurrentStatus();
	fYPos = XYMilli2Unit(m_fYPos,nUnit);
	return  bRes;
}

BOOL CMotorController::GetXYMotorPosition( float& fXPos, float& fYPos, DISTANCE_UNIT nUnit )
{
	BOOL bRes = QueryCurrentStatus();
	fXPos = XYMilli2Unit(m_fXPos,nUnit);
	fYPos = XYMilli2Unit(m_fYPos,nUnit);
	return  bRes;
}

BOOL CMotorController::GetXYMotorPosition( cv::Point2f& ptPos, DISTANCE_UNIT nUnit )
{
	BOOL bRes = QueryCurrentStatus();
	ptPos.x = XYMilli2Unit(m_fXPos,nUnit);
	ptPos.y = XYMilli2Unit(m_fYPos,nUnit);
	return  bRes;
}

BOOL CMotorController::GetZMotorPosition( float& fZPos, DISTANCE_UNIT nUnit )
{
	BOOL bCurrenOrLast = QueryCurrentStatus();//��ǰ״̬��������һ����ȡ����״̬
	fZPos = ZMilli2Unit(m_fZPos,nUnit);
	return bCurrenOrLast;
}

BOOL CMotorController::GetXYZMotorPosition( float& fXPos, float& fYPos,float& fZPos, DISTANCE_UNIT nUnit /*= DISTANCE_UNIT_MM*/ )
{
	BOOL bRes = QueryCurrentStatus();
	fXPos = XYMilli2Unit(m_fXPos,nUnit);
	fYPos = XYMilli2Unit(m_fYPos,nUnit);
	fZPos = XYMilli2Unit(m_fZPos,nUnit);
	return  bRes;
}

void CMotorController::SetXMotorSpeed( float fSpeed,DISTANCE_UNIT nUnit )
{
	if (XYUnit2Milli(fSpeed, nUnit) > 0)
	{
		m_fXSpeed = XYUnit2Milli(fSpeed,nUnit);
	}
}


void CMotorController::SetYMotorSpeed( float fSpeed,DISTANCE_UNIT nUnit )
{
	if (XYUnit2Milli(fSpeed, nUnit) > 0)
	{
		m_fYSpeed = XYUnit2Milli(fSpeed,nUnit);
	}
}


void CMotorController::SetZMotorSpeed( float fSpeed,DISTANCE_UNIT nUnit )
{
	if (ZUnit2Milli(fSpeed, nUnit) > 0)
	{
		m_fZSpeed = ZUnit2Milli(fSpeed,nUnit);
	}
}

float CMotorController::ZUnit2Milli( float fUnit,DISTANCE_UNIT unit )
{
	float  fMilli;
	switch ( unit )
	{
	case DISTANCE_UNIT_STEP:
		fMilli = fUnit * MOTOR_Z_RESOLUTION / MOTOR_Z_DIVIDE / 1000;
		break;
	case DISTANCE_UNIT_UM:
		fMilli= fUnit / 1000 ;
		break;
	case DISTANCE_UNIT_MM:
		fMilli = fUnit;
		break;
	default:
		fMilli = -1;
		break;
	}
	return fMilli;
}

float CMotorController::ZMilli2Unit( float fMilli,DISTANCE_UNIT unit )
{
	float fRes;
	switch ( unit )
	{
	case DISTANCE_UNIT_STEP:
		fRes = fMilli * 1000 / MOTOR_Z_RESOLUTION * MOTOR_Z_DIVIDE; 
		break;
	case DISTANCE_UNIT_UM:
		fRes= fMilli * 1000;
		break;
	case DISTANCE_UNIT_MM:
		fRes = fMilli;
		break;
	default:
		fRes = -1;
		break;
	}
	return fRes;
}



float CMotorController::XYUnit2Milli( float fUnit,DISTANCE_UNIT unit )
{
	float fRes;
	switch ( unit )
	{
	case DISTANCE_UNIT_STEP:
		fRes = fUnit / 1000 * MOTOR_XY_RESOLUTION / MOTOR_XY_DIVIDE; 
		break;
	case DISTANCE_UNIT_UM:
		fRes= fUnit / 1000;
		break;
	case DISTANCE_UNIT_MM:
		fRes = fUnit;
		break;
	default:
		fRes = -1;
		break;
	}
	return fRes;
}

float CMotorController::XYMilli2Unit( float fMilli,DISTANCE_UNIT unit )
{
	float fRes;
	switch ( unit )
	{
	case DISTANCE_UNIT_STEP:
		fRes = fMilli * 1000 / MOTOR_XY_RESOLUTION * MOTOR_XY_DIVIDE; 
		break;
	case DISTANCE_UNIT_UM:
		fRes= fMilli * 1000;
		break;
	case DISTANCE_UNIT_MM:
		fRes = fMilli;
		break;
	default:
		fRes = -1;
		break;
	}
	return fRes;
}


//2015/4/28
BOOL CMotorController::WaitUntilDone( DWORD dwTimeout )
{
	if (!m_bOpen)
	{
		return FALSE;
	}
	BOOL bRes;
	HANDLE handles[3];
	handles[0] = m_hStop;
	handles[1] = m_hAlarm;
	handles[2] = m_hIdle;
	DWORD dwRes = WaitForMultipleObjects(3,handles,FALSE,dwTimeout);
	switch(dwRes)
	{
	case WAIT_OBJECT_0:
		bRes = FALSE;
		break;
	case WAIT_OBJECT_0 + 1:
		if (isNeedReset())
		{
			Reset();
		}
		if (isNeedKillAarm())
		{
			KillAlarm();
		}
		bRes = FALSE;
		break;
	case WAIT_OBJECT_0 + 2:
		bRes = TRUE;
		break;
	default:
		bRes = FALSE;
		break;
	}
	return bRes;
}

BOOL CMotorController::Reset()
{
	if (!m_port.IsOpen())
	{
		TRACE("The comm is not opened!@Reset\n");
		return FALSE;
	}
	
	char reset;
	reset = 0x18;
	m_port.Write(&reset,1);
	m_port.Flush();
	TRACE("����д�룺reset@Reset\n");
	ResetEvent(m_hNeedReset);

	return TRUE;
}

BOOL CMotorController::KillAlarm()
{
	if (!m_port.IsOpen())
	{
		TRACE("The comm is not opened!@KillAlarm\n");
		return FALSE;
	}

	ResetEvent(m_hOneJobDone);
	m_port.Write("$X\r",3);
	m_port.Flush();
	TRACE("����д�룺$X@KillAlarm\n");
	DWORD dwRes = WaitForSingleObject(m_hOneJobDone,1000);
	ResetEvent(m_hOneJobDone);
	if (dwRes == WAIT_OBJECT_0)
	{
		return TRUE;
	}
	return FALSE;
	
}

BOOL CMotorController::Stop()
{
	if (m_port.IsOpen())
	{
		//�����������
		EnterCriticalSection(&m_protection);
		m_CmdQueue.clear();
		ResetEvent(m_hSendCmd);
		LeaveCriticalSection(&m_protection);

		//дhold����
		m_port.Write("!",1);
		m_port.Flush();
		TRACE("����д�룺!@Stop\n");

		//��ѯ��ǰ״̬
		if (!QueryCurrentStatus())
		{
			if (!QueryCurrentStatus())
			{
				//������β�ѯ��û�еõ����
				//˵����homing�Ĺ����е�����Stop
				//��������ForceAbort();
				return FALSE;
			}
		}
		
		if (m_nStatus == MOTOR_ALARM)
		{
			//֮ǰ��Needunlock ˵������Ѿ�ͣ���ˡ�
			//��������������alarm����
			return FALSE;
		}

		if (m_nStatus == MOTOR_IDLE)
		{
			//��������ʱ�����Ϊidle��˵�����������ͣ�ţ�����û��ALARM
			return TRUE;
		}
		
		//�����ǰ״̬ΪHOLD����ʾ�������ֹͣ
		while(m_nStatus == MOTOR_HOLD)
		{
			WaitForSingleObject(m_hStop,100);
			QueryCurrentStatus();
		}

		//��״̬��ΪQUEUE��ʱ�򣬼��ɷ���reset
		ResetEvent(m_hConnected);
		char cmdReset = 0x18;
		m_port.Write(&cmdReset,1);
		m_port.Flush();

		DWORD dwRes = WaitForSingleObject(m_hConnected,100);
		switch(dwRes)
		{
		case WAIT_OBJECT_0:
			SetEvent(m_hIdle);
			return TRUE;
		case WAIT_OBJECT_0 + 1:
			break;
		default:
			break;
		}
	}
	return FALSE;
}

UINT CMotorController::ReadingThread( LPVOID lpParam )
{
	CMotorController* pThis = (CMotorController*)lpParam;
	SetEvent(pThis->m_hBeginThread);

	while(1)
	{
		//�ȴ�m_dwCheckInterval����
		DWORD dwRes = WaitForSingleObject(pThis->m_hStop,pThis->m_dwCheckInterval);
		//����رմ���
		if (dwRes == WAIT_OBJECT_0)
		{
			break;
		}
		//���������δ��
		if (!pThis->m_port.IsOpen())
		{
			continue;
		}
		//��������Ѿ�����δҪ�رգ����ѯ�Ƿ������ݶ���
		try
		{
			int nBytes = pThis->m_port.BytesWaiting();
			if (nBytes > 0)
			{
				//�������ݣ�����
				char* pBuffer = new char[nBytes + 1];
				pThis->m_port.Read(pBuffer,nBytes);
				pBuffer[nBytes] = '\0';
				CString strNew;
				strNew = pBuffer;
				delete []pBuffer;
				//
				pThis->FindReturnCmd(strNew);
			}
		}
		catch (CSerialException* e)
		{
			continue;
		}

	}

	return 0;
}

void CMotorController::FindReturnCmd( CString strNewlyRecieved )
{
	m_strBuffer.Append(strNewlyRecieved);

	int pos;
	int nNextStart = m_strBuffer.Find(_T("\r\n"));
	while(nNextStart != -1)
	{
		
		CString strReturnCmd;
		strReturnCmd = m_strBuffer.Left(nNextStart + 2);
		m_strBuffer.Delete(0,nNextStart + 2);

		nNextStart = m_strBuffer.Find(_T("\r\n"));
		CString strOutput = strReturnCmd;
		strOutput.Insert(0,_T("serial read:"));
		TRACE(strOutput);

		//���ֻ�յ�����������鿴��һ������
		if (strReturnCmd.GetLength() == 2)
		{
			continue;
		}
		//�յ�ok������֮ǰ���͵�һ��0x0A��β���������
		else if (strReturnCmd.Find(_T("ok")) == 0)
		{
			SetEvent(m_hOneJobDone);
			continue;
		}
		else
		{
			//�յ����������ok,������յ�������Ϣ
			CString strCapital;
			strCapital = strReturnCmd;
			strCapital.MakeUpper();
			if (strCapital.Find(_T("ALARM")) != -1)
			{
				m_nStatus = MOTOR_ALARM;
				SetEvent(m_hAlarm);
			}

			if (strReturnCmd.Find(_T("['$H'|'$X' to unlock]")) != -1)
			{
				//��ҪUNLOCK
				SetEvent(m_hNeedHomeOrUnlock);
			}

			if (strReturnCmd.Find(_T("[Reset to continue]")) != -1)
			{
				//��ҪRESET
				SetEvent(m_hNeedReset);
			} 

			if (strReturnCmd.Find(_T("Grbl 0.9g ['$' for help]")) != -1)
			{
				SetEvent(m_hConnected);
			}
			if ((strReturnCmd.Find(_T("<")) != -1) && (strReturnCmd.Find(_T(">")) != -1))
			{
				//���յ�������Ϣ
				if (strCapital.Find(_T("IDLE")) != -1)
				{
					m_nStatus = MOTOR_IDLE;
					ResetEvent(m_hAlarm);
				}
				else if (strCapital.Find(_T("RUN")) != -1)
				{
					m_nStatus = MOTOR_RUN;
					ResetEvent(m_hAlarm);
				}
				else if (strCapital.Find(_T("QUEUE")) != -1 )
				{
					m_nStatus = MOTOR_QUEUE;
					ResetEvent(m_hAlarm);
				}
				else 
				{
					m_nStatus = MOTOR_UNKNOWN;
					ResetEvent(m_hAlarm);
				}
				CString strPos;
				pos = strReturnCmd.ReverseFind('>');
				strReturnCmd.Delete(pos,strReturnCmd.GetLength() - pos);
				pos = strReturnCmd.ReverseFind(',');
				strPos = strReturnCmd.Right(strReturnCmd.GetLength() - pos - 1);
				m_fZPos = -_ttof(strPos);

				strReturnCmd.Delete(pos,strReturnCmd.GetLength() - pos);
				pos = strReturnCmd.ReverseFind(',');
				strPos = strReturnCmd.Right(strReturnCmd.GetLength() - pos - 1);
				m_fYPos = _ttof(strPos);

				strReturnCmd.Delete(pos,strReturnCmd.GetLength() - pos);
				pos = strReturnCmd.ReverseFind(':');
				strPos = strReturnCmd.Right(strReturnCmd.GetLength() - pos - 1);
				m_fXPos = -_ttof(strPos);

				SetEvent(m_hUpdated);
			}
		}

	}
}

BOOL CMotorController::isNeedReset()
{
	DWORD dwRes = WaitForSingleObject(m_hNeedReset,0);
	return dwRes == WAIT_OBJECT_0;
}

BOOL CMotorController::isNeedKillAarm()
{
	DWORD dwRes = WaitForSingleObject(m_hNeedHomeOrUnlock,0);
	return dwRes == WAIT_OBJECT_0;
}

void CMotorController::WChar2char( wchar_t* pSrc,char*& pDes,int nLength )
{
	pDes = new char[nLength+2];
	int i = 0;
	for (; i< nLength; i++)
	{
		pDes[i] = *((char*)pSrc+i*2);
	}
	pDes[i] = '\r';
	pDes[i+1] = '\0';
}

float CMotorController::ZMotorSmoothMove2AbsolutePos( float fpos, DISTANCE_UNIT unit  )
{
	int i = 0;

	float destpos = fpos;
	if ( unit != DISTANCE_UNIT_MM )
	{
		destpos = ZUnit2Milli(fpos,unit);
	}
	//��ȡ��ǰ״̬
	QueryCurrentStatus();

	float  delta = fabs(destpos-m_fZPos);
	if (delta > 1)   // ������ʼ��̫Զ������1mm
	{
		SetZMotorSpeed(3, DISTANCE_UNIT_MM);                      // ����Z�����˶��ٶ�: 3 mm/s		
	}
	else if (delta > 0.36)
	{
		SetZMotorSpeed(3*delta, DISTANCE_UNIT_MM);                  // ����Z�����˶��ٶ�: 1/3 s ����ȫ��
	}
	else if ( delta > 0.06 )
	{
		SetZMotorSpeed(__min(delta/0.12, 1), DISTANCE_UNIT_MM);   // ����Z�����˶��ٶ�: 0.12 s ����ȫ��
	}
	else
	{
		SetZMotorSpeed(500, DISTANCE_UNIT_UM);                   // ����Z�����˶��ٶ�: 0.5 mm/s 
	}

	ZMotorRelativeMove(destpos - m_fZPos);
// 	while(fabs(destpos-m_fZPos) > MOTOR_Z_RESOLUTION / MOTOR_Z_DIVIDE)
// 	{
// 		ZMotorRelativeMove(destpos - m_fZPos);
// 		WaitUntilDone();
// 		QueryCurrentStatus();
// 	}
	QueryCurrentStatus();
	return m_fZPos;  // �����ƶ���ĵ�ǰλ�ã� unit: um
}

void CMotorController::XYMotorSmoothMove2Point( cv::Point2f ptPos, DISTANCE_UNIT unit /*= DISTANCE_UNIT_MM*/ )
{
	cv::Point2f point;
	QueryCurrentStatus();

	if ( unit != DISTANCE_UNIT_MM )
	{
		point.x = XYUnit2Milli(ptPos.x,unit);
		point.y = XYUnit2Milli(ptPos.y,unit);
	}

	// ���ôӵ�ǰλ���˶�����һ��λ�õ��ٶ�
	float fDelta, fSpeed;  // UNIT��mm, mm/s
	fDelta = fabs(point.x - m_fXPos);
	fSpeed = fDelta / 0.25;  // 250ms����ȫ��
	if ( fSpeed > 8 )    // 8mm/s
	{
		fSpeed = 8;
	}
	if ( fSpeed < 1 )     // 1mm/s
	{
		fSpeed = 1;
	}
	SetXMotorSpeed(fSpeed);  

	fDelta = fabs(point.y - m_fYPos);
	fSpeed = fDelta / 0.25;  // 250ms����ȫ��
	if ( fSpeed > 8 )     // 8mm/s
	{
		fSpeed = 8;
	}
	if ( fSpeed < 1 )     // 1mm/s
	{
		fSpeed = 1;
	}
	SetYMotorSpeed(fSpeed);

	XYMotorRelativeMove((point.x-m_fXPos),(point.y-m_fYPos));

}




