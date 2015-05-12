#pragma once

#include "opencv2\opencv.hpp"

// �Լ�����Ľṹ����ȹ���

#define MSG_SCANROUTE_NOTIFY		(WM_APP+1)
#define MSG_CAMERA_NOTIFY			(WM_APP+2)
#define MSG_MCURESPONSE_NOTIFY	    (WM_APP+3)
#define MSG_SCANTHREAD_NOTIFY	    (WM_APP+4)
#define MSG_JUDGEDLGRETURN_NOTIFY	(WM_APP+6)
#define MSG_JUDGEDLGOBSERVE_NOTIFY	(WM_APP+7)

#define CAMERA_IMAGE_WIDTH     1360
#define CAMERA_IMAGE_HEIGHT    1024

typedef struct tagMATRIXINDEX
{
    int      row;             // matrix row index
    int      col;          // matrix column index
} MATRIXINDEX, *PMATRIXINDEX, *LPMATRIXINDEX;

typedef struct tagSCANROUTE
{
    CArray<cv::Point2f, cv::Point2f>   ScanTopology; 
	CArray<MATRIXINDEX, MATRIXINDEX>     ScanSequence;
	UINT	ColNum;
	UINT	RowNum;
} SCANROUTE, *PSCANROUTE, *LPSCANROUTE;


enum GENDER{ GENDER_NONE = -1, GENDER_MALE = 0, GENDER_FEMALE = 1 };
typedef struct tagSLIDEINFO
{
    CString			strSlideID;            //��ƬID
	CString			strType;               // �걾���
	COleDateTime	dtDetectData;          //�ͼ�����
	COleDateTime	dtDetectTime;
	CString			strQuantity;          //̵�걾��
	CString			strStayTime;//��̵ʱ��
	CString			strMethod;//Ⱦɫ����
	CString			strSendDoctor;//�ͼ�ҽ��
	CString			strJudge;   //�ٴ����
	CString			strDetectResult;  //������
	CString			strBacilusDensity; //ϸ��Ũ��ֵ
	CString			strName; 
	CString			strPatientID;
	CString			strDepartment;
	CString			strBedID;
	GENDER			nGender;
	int				nYear;
	CString			strOccupation;
	CString			strTelephone;
	CString			strAddress;
	CString			strReportDoctor;      //========Report// ����ҽʦ
	COleDateTime	dtReportData;         // ��������
	CString			strCompareJudge;      // �ٴ����
	CString			strCompareDetectResult; // ������
	CString			strCompareBacilusDensity; // ϸ��Ũ��ֵ
} SLIDEINFO, *PSLIDEINFO, *LPSLIDEINFO;

typedef struct tagTARGETINFOV
{
	cv::Point3f				FOVPosition;           // Database
	CArray<CvRect, CvRect>		rectTarget;            // ϸ������� // Database 
	CArray<CvRect, CvRect>		rectSimpleTarget;            // ϸ������� // Database 
	CArray<CvRect, CvRect>		rectOverlapTarget;            // ϸ������� // Database 
	int							nTargetCount;          // ���Ÿ�����90%����ϸ���ĸ���
 	int							nJudgedTargetCount;    // Database
	BOOL						bChecked;              // Database
} TARGETINFOV;

typedef struct tagSLIDEDATA
{
	SLIDEINFO							SlideInfo;    // Database
	CArray<TARGETINFOV, TARGETINFOV>	TargetInFOV;  // Database
	CArray<int, int>					nFOVSequence;
	int									nFOVCount;    // Database
	CString								strDirectory; // ��ʱ�ļ�Ŀ¼
	BOOL								bScan;        // Database
	BOOL								bJudge;       // Database
} SLIDEDATA;

enum AUTOSCAN_STATUS {AUTOLIGHT, AUTOSCAN_VIEW, AUTOSCAN_END, AUTOSCAN_SLIDE_END, AUTOSCAN_FOCUS_TERMINATE, AUTOSCAN_POSTPROCESS};

const int	 globalBaudRate = 115200;
const int    global232BitN  = 10;

const double globalConfirmProbability = 0.9; 
const double globalLikelyProbability = 0.85;
const double globalSomewhateProbability = 0.8;

