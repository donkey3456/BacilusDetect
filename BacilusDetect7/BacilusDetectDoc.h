
// BacilusDetectDoc.h : CBacilusDetectDoc 类的接口
//


#pragma once

#include "define.h"
#include <algorithm> 
#include <vector>
#include <windows.h>
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>
#include <stdio.h>
#include <odbcinst.h>
#include <sqlucode.h>
#include <sqlncli.h>
#include <msdasql.h>
#include <msdadc.h>
#include <ole2.h>

struct FOVData 
{
	int		nIndex;
	int		nTargetCount;
};

bool UDgreater ( FOVData elem1, FOVData elem2 );

class CBacilusDetectDoc : public CDocument
{
protected: // 仅从序列化创建
	CBacilusDetectDoc();
	DECLARE_DYNCREATE(CBacilusDetectDoc)

// 特性
public:

// 数据
private:
	SLIDEDATA	m_SlideBoxData;

// 内部操作
private:
	

// 操作
public:
	void	CleanSlideBoxData();			// 清除 m_SlideBoxData 中的数据
	void    SetDirectory( const CString strDirectory);		// 设置 m_SlideBoxData 中的临时文件目录
	void    SetSlideScanStatus( BOOL bScan = TRUE);			// 设置 m_SlideBoxData 中的玻片扫描状态标志
	void    SetSlideJudgeStatus( BOOL bJudge = TRUE);		// 设置 m_SlideBoxData 中的玻片辅助诊断状态标志
	void    InitialFOVCount( int nCount);					// 初始化 m_SlideBoxData 中的扫描视野个数
	void    SetFOVCount( int nCount);						// 设置 m_SlideBoxData 中的扫描视野个数
	void    SetFOVPosition( int nFOVIndex, float x, float y, float z);		// 设置 m_SlideBoxData 中的一个视野的位置
	void    SetTargetCountInFOV( int nFOVIndex, int nCount);		 // 设置 m_SlideBoxData 中的一个视野中疑似结核杆菌数量
	void    SetJudgedTargetCountInFOV( int nFOVIndex, int nCount);// 设置 m_SlideBoxData 中的一个视野中确定的结核杆菌数量
	void    SetTargetRectInFOV(int nFOVIndex, int nTargetIndex, CvRect rectTarget); // 设置 m_SlideBoxData 中的一个视野中一个疑似结核杆菌的位置和大小
	void    InitialSimpleTargetCountInFOV( int nFOVIndex, int nCount);	 // 初始化 m_SlideBoxData 中的一个视野中简单无分支结核杆菌数量
	void    SetSimpleTargetRectInFOV( int nFOVIndex, int nTargetIndex, CvRect rectTarget); // 设置 m_SlideBoxData 中的一个视野中一个简单无分支结核杆菌的位置和大小
	void    InitialOverlapTargetCountInFOV( int nFOVIndex, int nCount);	 // 初始化 m_SlideBoxData 中的一个视野中重叠粘连结核杆菌数量
	void    SetOverlapTargetRectInFOV( int nFOVIndex, int nTargetIndex, CvRect rectTarget); // 设置 m_SlideBoxData 中的一个视野中重叠粘连结核杆菌的位置和大小
	void	SetFOVSequence();										 // 对 m_SlideBoxData 中一个玻片中的视野进行排序
	void    SetFOVCheckedStatus( int nFOVIndex, BOOL bChecked = TRUE);				  // 设置 m_SlideBoxData 中的视野的辅助检查状态
	
	PSLIDEINFO    GetSlideInfo();									// 获得 m_SlideBoxData 中的玻片信息的地址
	CString GetSlideID();									// 获得 m_SlideBoxData 中的玻片ID
	CString GetDirectory();										// 获得 m_SlideBoxData 中的临时文件目录
	void    GetScanJudgeStatus(BOOL& bScan, BOOL& bJudge);// 获得 m_SlideBoxData 中的玻片扫描检测状态
	int     GetFOVCount();										// 获得 m_SlideBoxData 中的扫描视野个数
	int		GetFOVIndex( int nFOVSequenceIndex);					// 获得 m_SlideBoxData 中排序结果数组中保存的视野编号
	int     GetTargetCountInFOV(int nFOVIndex);					// 获得 m_SlideBoxData 中的一个视野中疑似结核杆菌数量
	int     GetJudgedTargetCountInFOV( int nFOVIndex);			// 获得 m_SlideBoxData 中的一个视野中确诊的结核杆菌数量
	CArray<CvRect, CvRect>*	GetTargetInfoInFOV( int nFOVIndex);	// 获得 m_SlideBoxData 中的一个视野中结核杆菌的相关信息
	CArray<CvRect, CvRect>*	GetSimpleTargetInfoInFOV( int nFOVIndex);	// 获得 m_SlideBoxData 中的一个视野中结核杆菌的相关信息
	CArray<CvRect, CvRect>*	GetOverlapTargetInfoInFOV( int nFOVIndex);	// 获得 m_SlideBoxData 中的一个视野中结核杆菌的相关信息
	int     GetTargetCountInSlide();								// 获得 m_SlideBoxData 中的一个玻片中疑似结核杆菌数量
	int     GetJudgedTargetCountInSlide();						// 获得 m_SlideBoxData 中的一个玻片中确诊的结核杆菌数量
	BOOL	GetFOVCheckedStatus(int nFOVIndex);				    // 获得 m_SlideBoxData 中的视野的辅助检查状态
	cv::Point3f   GetFOVPosition(int nFOVIndex);				// 获得 m_SlideBoxData 中的一个视野的位置

// Xu Chao // Datase

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CBacilusDetectDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	void SaveSlideData();
	CString VariantToCString(VARIANT var);
};

