
// BacilusDetectDoc.h : CBacilusDetectDoc ��Ľӿ�
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
protected: // �������л�����
	CBacilusDetectDoc();
	DECLARE_DYNCREATE(CBacilusDetectDoc)

// ����
public:

// ����
private:
	SLIDEDATA	m_SlideBoxData;

// �ڲ�����
private:
	

// ����
public:
	void	CleanSlideBoxData();			// ��� m_SlideBoxData �е�����
	void    SetDirectory( const CString strDirectory);		// ���� m_SlideBoxData �е���ʱ�ļ�Ŀ¼
	void    SetSlideScanStatus( BOOL bScan = TRUE);			// ���� m_SlideBoxData �еĲ�Ƭɨ��״̬��־
	void    SetSlideJudgeStatus( BOOL bJudge = TRUE);		// ���� m_SlideBoxData �еĲ�Ƭ�������״̬��־
	void    InitialFOVCount( int nCount);					// ��ʼ�� m_SlideBoxData �е�ɨ����Ұ����
	void    SetFOVCount( int nCount);						// ���� m_SlideBoxData �е�ɨ����Ұ����
	void    SetFOVPosition( int nFOVIndex, float x, float y, float z);		// ���� m_SlideBoxData �е�һ����Ұ��λ��
	void    SetTargetCountInFOV( int nFOVIndex, int nCount);		 // ���� m_SlideBoxData �е�һ����Ұ�����ƽ�˸˾�����
	void    SetJudgedTargetCountInFOV( int nFOVIndex, int nCount);// ���� m_SlideBoxData �е�һ����Ұ��ȷ���Ľ�˸˾�����
	void    SetTargetRectInFOV(int nFOVIndex, int nTargetIndex, CvRect rectTarget); // ���� m_SlideBoxData �е�һ����Ұ��һ�����ƽ�˸˾���λ�úʹ�С
	void    InitialSimpleTargetCountInFOV( int nFOVIndex, int nCount);	 // ��ʼ�� m_SlideBoxData �е�һ����Ұ�м��޷�֧��˸˾�����
	void    SetSimpleTargetRectInFOV( int nFOVIndex, int nTargetIndex, CvRect rectTarget); // ���� m_SlideBoxData �е�һ����Ұ��һ�����޷�֧��˸˾���λ�úʹ�С
	void    InitialOverlapTargetCountInFOV( int nFOVIndex, int nCount);	 // ��ʼ�� m_SlideBoxData �е�һ����Ұ���ص�ճ����˸˾�����
	void    SetOverlapTargetRectInFOV( int nFOVIndex, int nTargetIndex, CvRect rectTarget); // ���� m_SlideBoxData �е�һ����Ұ���ص�ճ����˸˾���λ�úʹ�С
	void	SetFOVSequence();										 // �� m_SlideBoxData ��һ����Ƭ�е���Ұ��������
	void    SetFOVCheckedStatus( int nFOVIndex, BOOL bChecked = TRUE);				  // ���� m_SlideBoxData �е���Ұ�ĸ������״̬
	
	PSLIDEINFO    GetSlideInfo();									// ��� m_SlideBoxData �еĲ�Ƭ��Ϣ�ĵ�ַ
	CString GetSlideID();									// ��� m_SlideBoxData �еĲ�ƬID
	CString GetDirectory();										// ��� m_SlideBoxData �е���ʱ�ļ�Ŀ¼
	void    GetScanJudgeStatus(BOOL& bScan, BOOL& bJudge);// ��� m_SlideBoxData �еĲ�Ƭɨ����״̬
	int     GetFOVCount();										// ��� m_SlideBoxData �е�ɨ����Ұ����
	int		GetFOVIndex( int nFOVSequenceIndex);					// ��� m_SlideBoxData �������������б������Ұ���
	int     GetTargetCountInFOV(int nFOVIndex);					// ��� m_SlideBoxData �е�һ����Ұ�����ƽ�˸˾�����
	int     GetJudgedTargetCountInFOV( int nFOVIndex);			// ��� m_SlideBoxData �е�һ����Ұ��ȷ��Ľ�˸˾�����
	CArray<CvRect, CvRect>*	GetTargetInfoInFOV( int nFOVIndex);	// ��� m_SlideBoxData �е�һ����Ұ�н�˸˾��������Ϣ
	CArray<CvRect, CvRect>*	GetSimpleTargetInfoInFOV( int nFOVIndex);	// ��� m_SlideBoxData �е�һ����Ұ�н�˸˾��������Ϣ
	CArray<CvRect, CvRect>*	GetOverlapTargetInfoInFOV( int nFOVIndex);	// ��� m_SlideBoxData �е�һ����Ұ�н�˸˾��������Ϣ
	int     GetTargetCountInSlide();								// ��� m_SlideBoxData �е�һ����Ƭ�����ƽ�˸˾�����
	int     GetJudgedTargetCountInSlide();						// ��� m_SlideBoxData �е�һ����Ƭ��ȷ��Ľ�˸˾�����
	BOOL	GetFOVCheckedStatus(int nFOVIndex);				    // ��� m_SlideBoxData �е���Ұ�ĸ������״̬
	cv::Point3f   GetFOVPosition(int nFOVIndex);				// ��� m_SlideBoxData �е�һ����Ұ��λ��

// Xu Chao // Datase

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CBacilusDetectDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	void SaveSlideData();
	CString VariantToCString(VARIANT var);
};

