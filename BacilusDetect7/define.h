#pragma once

#include "opencv2\opencv.hpp"

// 自己定义的结构、宏等功能

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
    CString			strSlideID;            //玻片ID
	CString			strType;               // 标本类别
	COleDateTime	dtDetectData;          //送检日期
	COleDateTime	dtDetectTime;
	CString			strQuantity;          //痰标本量
	CString			strStayTime;//留痰时间
	CString			strMethod;//染色方法
	CString			strSendDoctor;//送检医生
	CString			strJudge;   //临床诊断
	CString			strDetectResult;  //检验结果
	CString			strBacilusDensity; //细菌浓度值
	CString			strName; 
	CString			strPatientID;
	CString			strDepartment;
	CString			strBedID;
	GENDER			nGender;
	int				nYear;
	CString			strOccupation;
	CString			strTelephone;
	CString			strAddress;
	CString			strReportDoctor;      //========Report// 报告医师
	COleDateTime	dtReportData;         // 报告日期
	CString			strCompareJudge;      // 临床诊断
	CString			strCompareDetectResult; // 检验结果
	CString			strCompareBacilusDensity; // 细菌浓度值
} SLIDEINFO, *PSLIDEINFO, *LPSLIDEINFO;

typedef struct tagTARGETINFOV
{
	cv::Point3f				FOVPosition;           // Database
	CArray<CvRect, CvRect>		rectTarget;            // 细菌的外框 // Database 
	CArray<CvRect, CvRect>		rectSimpleTarget;            // 细菌的外框 // Database 
	CArray<CvRect, CvRect>		rectOverlapTarget;            // 细菌的外框 // Database 
	int							nTargetCount;          // 置信概率在90%以上细菌的个数
 	int							nJudgedTargetCount;    // Database
	BOOL						bChecked;              // Database
} TARGETINFOV;

typedef struct tagSLIDEDATA
{
	SLIDEINFO							SlideInfo;    // Database
	CArray<TARGETINFOV, TARGETINFOV>	TargetInFOV;  // Database
	CArray<int, int>					nFOVSequence;
	int									nFOVCount;    // Database
	CString								strDirectory; // 临时文件目录
	BOOL								bScan;        // Database
	BOOL								bJudge;       // Database
} SLIDEDATA;

enum AUTOSCAN_STATUS {AUTOLIGHT, AUTOSCAN_VIEW, AUTOSCAN_END, AUTOSCAN_SLIDE_END, AUTOSCAN_FOCUS_TERMINATE, AUTOSCAN_POSTPROCESS};

const int	 globalBaudRate = 115200;
const int    global232BitN  = 10;

const double globalConfirmProbability = 0.9; 
const double globalLikelyProbability = 0.85;
const double globalSomewhateProbability = 0.8;

