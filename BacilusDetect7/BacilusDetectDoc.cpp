
// BacilusDetectDoc.cpp : CBacilusDetectDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "BacilusDetect.h"
#endif

#include "BacilusDetectDoc.h"
#include "ODBCErrors.h"
#define COPYBUFFERSIZE		524288
#include <propkey.h>
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
 
 extern _ConnectionPtr m_pCon;  //ADO���Ӷ���
 extern _RecordsetPtr m_pRs,m_pRs1;    //ADO��¼������

bool UDgreater ( FOVData elem1, FOVData elem2 )
{
	return elem1.nTargetCount > elem2.nTargetCount;
}

// CBacilusDetectDoc

IMPLEMENT_DYNCREATE(CBacilusDetectDoc, CDocument)

BEGIN_MESSAGE_MAP(CBacilusDetectDoc, CDocument)
END_MESSAGE_MAP()


// CBacilusDetectDoc ����/����

CBacilusDetectDoc::CBacilusDetectDoc()
{
	CleanSlideBoxData(); // ��� m_SlideBoxData �е�����
}

CBacilusDetectDoc::~CBacilusDetectDoc()
{
}

BOOL CBacilusDetectDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	this->SetTitle(_T("����Ƽ�"));

	return TRUE;
}




// CBacilusDetectDoc ���л�

void CBacilusDetectDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CBacilusDetectDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// ������������֧��
void CBacilusDetectDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:  strSearchContent = _T("point;rectangle;circle;ole object;")��
	SetSearchContent(strSearchContent);
}

void CBacilusDetectDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CBacilusDetectDoc ���

#ifdef _DEBUG
void CBacilusDetectDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBacilusDetectDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// �ڲ�����

// ����
// ��� m_SlideBoxData �е�����
void	CBacilusDetectDoc::CleanSlideBoxData()  
{

	m_SlideBoxData.bScan = FALSE;
	m_SlideBoxData.bJudge = FALSE;

	m_SlideBoxData.strDirectory.Empty();

	m_SlideBoxData.SlideInfo.strSlideID.Empty();
	m_SlideBoxData.SlideInfo.strType.Empty();
	m_SlideBoxData.SlideInfo.dtDetectData = COleDateTime::GetCurrentTime();
	m_SlideBoxData.SlideInfo.dtDetectTime = COleDateTime::GetCurrentTime();
	m_SlideBoxData.SlideInfo.strQuantity.Empty();
	m_SlideBoxData.SlideInfo.strStayTime.Empty();
	m_SlideBoxData.SlideInfo.strMethod.Empty();
	m_SlideBoxData.SlideInfo.strSendDoctor.Empty();
	m_SlideBoxData.SlideInfo.strJudge.Empty();
	m_SlideBoxData.SlideInfo.strDetectResult.Empty();
	m_SlideBoxData.SlideInfo.strBacilusDensity.Empty();
	m_SlideBoxData.SlideInfo.strName.Empty();
	m_SlideBoxData.SlideInfo.strPatientID.Empty();
	m_SlideBoxData.SlideInfo.strDepartment.Empty();
	m_SlideBoxData.SlideInfo.strBedID.Empty();
	m_SlideBoxData.SlideInfo.nGender = GENDER_NONE;
	m_SlideBoxData.SlideInfo.nYear = 0;
	m_SlideBoxData.SlideInfo.strOccupation.Empty();
	m_SlideBoxData.SlideInfo.strTelephone.Empty();
	m_SlideBoxData.SlideInfo.strAddress.Empty();
	m_SlideBoxData.SlideInfo.strReportDoctor.Empty();
	m_SlideBoxData.SlideInfo.dtReportData = COleDateTime::GetCurrentTime();
	m_SlideBoxData.SlideInfo.strCompareJudge.Empty();
	m_SlideBoxData.SlideInfo.strCompareDetectResult.Empty();
	m_SlideBoxData.SlideInfo.strCompareBacilusDensity.Empty();

	for ( int j = 0; j < m_SlideBoxData.TargetInFOV.GetSize(); j++ )
	{
		m_SlideBoxData.TargetInFOV[j].rectTarget.RemoveAll();
		m_SlideBoxData.TargetInFOV[j].rectSimpleTarget.RemoveAll();
		m_SlideBoxData.TargetInFOV[j].rectOverlapTarget.RemoveAll();
	}
	m_SlideBoxData.TargetInFOV.RemoveAll();
	m_SlideBoxData.nFOVSequence.RemoveAll();
	m_SlideBoxData.nFOVCount = 0;

}

// ���� m_SlideBoxData �е���ʱ�ļ�Ŀ¼
void    CBacilusDetectDoc::SetDirectory( const CString strDirectory)
{
	m_SlideBoxData.strDirectory = strDirectory;
}

// ���� m_SlideBoxData �еĲ�Ƭɨ��״̬��־
void    CBacilusDetectDoc::SetSlideScanStatus( BOOL bScan)
{
	m_SlideBoxData.bScan = bScan;
}

// ���� m_SlideBoxData �еĲ�Ƭ�������״̬��־
void    CBacilusDetectDoc::SetSlideJudgeStatus( BOOL bJudge)
{
	m_SlideBoxData.bJudge = bJudge;
}

// ��ʼ�� m_SlideBoxData �е�ɨ����Ұ����
void    CBacilusDetectDoc::InitialFOVCount( int nCount)	
{
	_ASSERTE( nCount >= 0 );
	m_SlideBoxData.TargetInFOV.SetSize(nCount);
	m_SlideBoxData.nFOVSequence.SetSize(nCount);
	
	int i;
	for ( i=0; i<nCount; i++ )
	{
		m_SlideBoxData.nFOVSequence[i] = i;
	}
}

// ���� m_SlideBoxData �е�ɨ����Ұ����
void    CBacilusDetectDoc::SetFOVCount( int nCount)
{
	_ASSERTE( nCount >= 0 );
	m_SlideBoxData.nFOVCount = nCount;
}

// ���� m_SlideBoxData �е�һ����Ұ��λ��
void    CBacilusDetectDoc::SetFOVPosition( int nFOVIndex, float x, float y, float z)
{
	_ASSERTE( (nFOVIndex>=0) && (nFOVIndex<m_SlideBoxData.nFOVCount) );
	m_SlideBoxData.TargetInFOV[nFOVIndex].FOVPosition.x = x;
	m_SlideBoxData.TargetInFOV[nFOVIndex].FOVPosition.y = y;
	m_SlideBoxData.TargetInFOV[nFOVIndex].FOVPosition.z = z;
}

// ���� m_SlideBoxData �е�һ����Ұ�����ƽ�˸˾�����
void    CBacilusDetectDoc::SetTargetCountInFOV( int nFOVIndex, int nCount)
{
	_ASSERTE( (nFOVIndex>=0) && (nFOVIndex<m_SlideBoxData.nFOVCount) );
	_ASSERTE( nCount >= 0 );
	m_SlideBoxData.TargetInFOV[nFOVIndex].nTargetCount = nCount;

	// ��ʼ�� m_SlideBoxData �е�һ����Ұ�����ƽ�˸˾�����
	m_SlideBoxData.TargetInFOV[nFOVIndex].rectTarget.SetSize(nCount);
}

// ���� m_SlideBoxData �е�һ����Ұ��ȷ���Ľ�˸˾�����
void    CBacilusDetectDoc::SetJudgedTargetCountInFOV( int nFOVIndex, int nCount)
{
	_ASSERTE( (nFOVIndex>=0) && (nFOVIndex<m_SlideBoxData.nFOVCount));
	_ASSERTE( nCount >= 0 );
	m_SlideBoxData.TargetInFOV[nFOVIndex].nJudgedTargetCount = nCount;
}

// ���� m_SlideBoxData �е���Ұ�ĸ������״̬
void    CBacilusDetectDoc::SetFOVCheckedStatus( int nFOVIndex, BOOL bChecked )
{
	_ASSERTE( (nFOVIndex>=0) && (nFOVIndex<m_SlideBoxData.nFOVCount) );
	m_SlideBoxData.TargetInFOV[nFOVIndex].bChecked = bChecked;
}

// ���� m_SlideBoxData �е�һ����Ұ��һ�����ƽ�˸˾���λ�úʹ�С
void    CBacilusDetectDoc::SetTargetRectInFOV( int nFOVIndex, int nTargetIndex, CvRect rectTarget)
{
	_ASSERTE( (nFOVIndex>=0) && (nFOVIndex<m_SlideBoxData.nFOVCount) );
	_ASSERTE( nTargetIndex >= 0 );
	m_SlideBoxData.TargetInFOV[nFOVIndex].rectTarget[nTargetIndex] = rectTarget;
}

 // ��ʼ�� m_SlideBoxData �е�һ����Ұ�м��޷�֧��˸˾�����
void     CBacilusDetectDoc::InitialSimpleTargetCountInFOV( int nFOVIndex, int nCount)
{
	_ASSERTE( (nFOVIndex>=0) && (nFOVIndex<m_SlideBoxData.nFOVCount) );
	_ASSERTE( nCount >= 0 );
	m_SlideBoxData.TargetInFOV[nFOVIndex].rectSimpleTarget.SetSize(nCount);
}

// ���� m_SlideBoxData �е�һ����Ұ��һ�����޷�֧Ŀ���˸˾���λ�úʹ�С
void    CBacilusDetectDoc::SetSimpleTargetRectInFOV( int nFOVIndex, int nTargetIndex, CvRect rectTarget)
{
	_ASSERTE( (nFOVIndex>=0) && (nFOVIndex<m_SlideBoxData.nFOVCount) );
	_ASSERTE( nTargetIndex >= 0 );
	m_SlideBoxData.TargetInFOV[nFOVIndex].rectSimpleTarget[nTargetIndex] = rectTarget;
}
 
// ��ʼ�� m_SlideBoxData �е�һ����Ұ���ص�ճ����˸˾�����
void    CBacilusDetectDoc::InitialOverlapTargetCountInFOV( int nFOVIndex, int nCount)
{
	_ASSERTE( (nFOVIndex>=0) && (nFOVIndex<m_SlideBoxData.nFOVCount) );
	_ASSERTE( nCount >= 0 );
	m_SlideBoxData.TargetInFOV[nFOVIndex].rectOverlapTarget.SetSize(nCount);
}

// ���� m_SlideBoxData �е�һ����Ұ���ص�ճ����˸˾���λ�úʹ�С
void    CBacilusDetectDoc::SetOverlapTargetRectInFOV( int nFOVIndex, int nTargetIndex, CvRect rectTarget)
{
	_ASSERTE( (nFOVIndex>=0) && (nFOVIndex<m_SlideBoxData.nFOVCount) );
	_ASSERTE( nTargetIndex >= 0 );
	m_SlideBoxData.TargetInFOV[nFOVIndex].rectOverlapTarget[nTargetIndex] = rectTarget;
}

// �� m_SlideBoxData ��һ����Ƭ�е���Ұ��������
void	CBacilusDetectDoc::SetFOVSequence()
{
	
	int nCount = m_SlideBoxData.nFOVCount;
	
	// ��ʼ��
	m_SlideBoxData.nFOVSequence.SetSize(nCount);
	int i;
	for ( i=0; i<nCount; i++ )
	{
		m_SlideBoxData.nFOVSequence[i] = i;
	}

	if ( nCount <= 1 )
	{
		return;
	}

	// ����
	FOVData tempFOV;
	using namespace std;
	vector <FOVData> v1;
	vector <FOVData>::iterator Iter1;
	for ( i = 0; i< nCount; i++ )
	{
		tempFOV.nIndex = i;
		tempFOV.nTargetCount = m_SlideBoxData.TargetInFOV[i].nJudgedTargetCount;
		v1.push_back(tempFOV);
	}
	sort( v1.begin( ), v1.end( ), UDgreater);
	i = 0;
	for ( Iter1 = v1.begin( ) ; Iter1 != v1.end( ) ; Iter1++ )
	{
		m_SlideBoxData.nFOVSequence[i] = (*Iter1).nIndex;
		i++;
	}
}

// ��� m_SlideBoxData �еĲ�Ƭ��Ϣ�ĵ�ַ
PSLIDEINFO    CBacilusDetectDoc::GetSlideInfo()
{

	return & ( m_SlideBoxData.SlideInfo); 
}
	
// ��� m_SlideBoxData �еĲ�ƬID
CString  CBacilusDetectDoc::GetSlideID()
{
	return m_SlideBoxData.SlideInfo.strSlideID;
}

// ��� m_SlideBoxData �е���ʱ�ļ�Ŀ¼
CString CBacilusDetectDoc::GetDirectory()		
{
	return m_SlideBoxData.strDirectory;
}

// ��� m_SlideBoxData �еĲ�Ƭɨ����״̬
void    CBacilusDetectDoc::GetScanJudgeStatus(BOOL& bScan /*output*/, BOOL& bJudge /*output*/)
{
	bScan = m_SlideBoxData.bScan;
	bJudge = m_SlideBoxData.bJudge;
}

// ��� m_SlideBoxData �е�ɨ����Ұ����
int     CBacilusDetectDoc::GetFOVCount()
{
	return m_SlideBoxData.nFOVCount;
}

// ��� m_SlideBoxData �������������б������Ұ���
int		CBacilusDetectDoc::GetFOVIndex( int nFOVSequenceIndex)
{
	_ASSERTE( (nFOVSequenceIndex>=0) && (nFOVSequenceIndex<m_SlideBoxData.nFOVCount) );
	return m_SlideBoxData.nFOVSequence[nFOVSequenceIndex];
}

// ��� m_SlideBoxData �е�һ����Ұ�����ƽ�˸˾�����
int      CBacilusDetectDoc::GetTargetCountInFOV( int nFOVIndex)
{
	_ASSERTE( (nFOVIndex>=0) && (nFOVIndex<m_SlideBoxData.nFOVCount) );
	return m_SlideBoxData.TargetInFOV[nFOVIndex].nTargetCount;
}

// ��� m_SlideBoxData �е�һ����Ұ��ȷ��Ľ�˸˾�����
int    CBacilusDetectDoc::GetJudgedTargetCountInFOV( int nFOVIndex)
{
	_ASSERTE( (nFOVIndex>=0) && (nFOVIndex<m_SlideBoxData.nFOVCount) );
	return m_SlideBoxData.TargetInFOV[nFOVIndex].nJudgedTargetCount;
}

// ��� m_SlideBoxData �е�һ����Ұ�н�˸˾��������Ϣ
CArray<CvRect, CvRect>*	CBacilusDetectDoc::GetTargetInfoInFOV( int nFOVIndex)
{
	_ASSERTE( (nFOVIndex>=0) && (nFOVIndex<m_SlideBoxData.nFOVCount) );
	return &(m_SlideBoxData.TargetInFOV[nFOVIndex].rectTarget);
}

CArray<CvRect, CvRect>*	CBacilusDetectDoc::GetSimpleTargetInfoInFOV( int nFOVIndex)
{
	_ASSERTE( (nFOVIndex>=0) && (nFOVIndex<m_SlideBoxData.nFOVCount) );
	return &(m_SlideBoxData.TargetInFOV[nFOVIndex].rectSimpleTarget);
}
	
CArray<CvRect, CvRect>*	CBacilusDetectDoc::GetOverlapTargetInfoInFOV( int nFOVIndex)
{
	_ASSERTE( (nFOVIndex>=0) && (nFOVIndex<m_SlideBoxData.nFOVCount) );
	return &(m_SlideBoxData.TargetInFOV[nFOVIndex].rectOverlapTarget);
}

// ��� m_SlideBoxData �е�һ����Ƭ�����ƽ�˸˾�����
int     CBacilusDetectDoc::GetTargetCountInSlide()
{
	int i;
	int sum = 0;
	int nFOVCount = m_SlideBoxData.nFOVCount;
	for ( i =0; i<nFOVCount; i++ )
	{
		sum += m_SlideBoxData.TargetInFOV[i].nTargetCount;
	} 
	return sum;
}

// ��� m_SlideBoxData �е�һ����Ƭ��ȷ��Ľ�˸˾�����
int     CBacilusDetectDoc::GetJudgedTargetCountInSlide()
{
	int i;
	int sum = 0;
	int nFOVCount = m_SlideBoxData.TargetInFOV.GetCount();
	for ( i =0; i<nFOVCount; i++ )
	{
		sum += m_SlideBoxData.TargetInFOV[i].nJudgedTargetCount;
	} 
	return sum;
}

 // ��� m_SlideBoxData �е���Ұ�ĸ������״̬
BOOL	CBacilusDetectDoc::GetFOVCheckedStatus( int nFOVIndex)
{
	_ASSERTE( (nFOVIndex>=0) && (nFOVIndex<m_SlideBoxData.nFOVCount) );
	return m_SlideBoxData.TargetInFOV[nFOVIndex].bChecked;
}

// ��� m_SlideBoxData �е�һ����Ұ��λ��
cv::Point3f   CBacilusDetectDoc::GetFOVPosition(int nFOVIndex)
{
	_ASSERTE( (nFOVIndex>=0) && (nFOVIndex<m_SlideBoxData.nFOVCount) );
	return m_SlideBoxData.TargetInFOV[nFOVIndex].FOVPosition;
}

// CBacilusDetectDoc ����

void CBacilusDetectDoc::SaveSlideData()
{
	CString sql;
    m_pRs->raw_Close();
	sql.Format(_T("select *from BacilusDetect.dbo.PatientPerInfo where SlideID = '%s'"),m_SlideBoxData.SlideInfo.strSlideID);
	m_pRs->Open((_variant_t)sql,m_pCon.GetInterfacePtr(),adOpenKeyset,adLockOptimistic,adCmdText);
	
	if(m_pRs->adoEOF)//���֮ǰû��SlideID��Ϣ��
	{

	try
		{		
			m_pRs->raw_Close();
	        //���˸�����Ϣ��
			m_pRs->Open("SELECT * FROM BacilusDetect.dbo.PatientPerInfo",_variant_t((IDispatch*)m_pCon),adOpenStatic,adLockOptimistic,adCmdText); 
		    m_pRs->AddNew();//���һ���¼�¼
		   	m_pRs->Fields->GetItem(_variant_t("Name"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strName;
			m_pRs->Fields->GetItem(_variant_t("PatientID"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strPatientID;
			m_pRs->Fields->GetItem(_variant_t("Department"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strDepartment;
			m_pRs->Fields->GetItem(_variant_t("BedID"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strBedID;
			m_pRs->Fields->GetItem(_variant_t("Gender"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.nGender;
			m_pRs->Fields->GetItem(_variant_t("Year"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.nYear;
			m_pRs->Fields->GetItem(_variant_t("Occupation"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strOccupation;
			m_pRs->Fields->GetItem(_variant_t("Telephone"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strTelephone;
			m_pRs->Fields->GetItem(_variant_t("Address"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strAddress;
			m_pRs->Fields->GetItem(_variant_t("SlideID"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strSlideID;
			m_pRs->Update();
		 AfxMessageBox(_T("���˸�����Ϣ����ɹ�!"));
		}
	   catch(...)
	   {
		AfxMessageBox(_T("���˸�����Ϣ����ʧ��!"));
	   }
	   
	   try
		{		
			m_pRs->raw_Close();
	        //���˲�Ƭ��Ϣ��
			m_pRs->Open("SELECT * FROM BacilusDetect.dbo.PatientSlideInfo",_variant_t((IDispatch*)m_pCon),adOpenStatic,adLockOptimistic,adCmdText); 
		    m_pRs->AddNew();//���һ���¼�¼
		   	m_pRs->Fields->GetItem(_variant_t("SlideID"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strSlideID;
			m_pRs->Fields->GetItem(_variant_t("Type"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strType;
			
			_variant_t var;
			var.vt=VT_DATE;
			var.date=int(m_SlideBoxData.SlideInfo.dtDetectData); 
			m_pRs->Fields->GetItem(_variant_t("DetectData"))->Value=(_bstr_t)var;
			
			_variant_t vart;
			vart.vt=VT_DATE;
			vart.date=DATE(m_SlideBoxData.SlideInfo.dtDetectTime); 
			m_pRs->Fields->GetItem(_variant_t("DetectTime"))->Value=(_bstr_t)vart;
			m_pRs->Fields->GetItem(_variant_t("Quantity"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strQuantity;
			m_pRs->Fields->GetItem(_variant_t("StayTime"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strStayTime;
			m_pRs->Fields->GetItem(_variant_t("Method"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strMethod;
			m_pRs->Fields->GetItem(_variant_t("SendDoctor"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strSendDoctor;
			m_pRs->Fields->GetItem(_variant_t("Judge"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strJudge;
			m_pRs->Fields->GetItem(_variant_t("DetectResult"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strDetectResult;
			m_pRs->Fields->GetItem(_variant_t("BacilusDensity"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strBacilusDensity;
			m_pRs->Fields->GetItem(_variant_t("FOVCount"))->Value=(_bstr_t)m_SlideBoxData.nFOVCount;
			m_pRs->Fields->GetItem(_variant_t("bScan"))->Value=(_bstr_t)m_SlideBoxData.bScan;
			m_pRs->Fields->GetItem(_variant_t("bJudge"))->Value=(_bstr_t)m_SlideBoxData.bJudge;
			m_pRs->Fields->GetItem(_variant_t("TargetCount"))->Value=(_bstr_t)GetTargetCountInSlide();
			m_pRs->Fields->GetItem(_variant_t("JudgedTargetCount"))->Value=(_bstr_t)GetJudgedTargetCountInSlide();
			m_pRs->Update();
		 AfxMessageBox(_T("���˲�Ƭ��Ϣ����ɹ�!"));
		}
	   catch(...)
	   {
		AfxMessageBox(_T("���˲�Ƭ��Ϣ����ʧ��!"));
	   }


	   try
		{		
			m_pRs->raw_Close();
	        //���˱�����Ϣ��
			m_pRs->Open("SELECT * FROM BacilusDetect.dbo.PatientRepInfo",_variant_t((IDispatch*)m_pCon),adOpenStatic,adLockOptimistic,adCmdText); 
		    m_pRs->AddNew();//���һ���¼�¼
		   	m_pRs->Fields->GetItem(_variant_t("ReportDoctor"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strReportDoctor;

			_variant_t varr;
			varr.vt=VT_DATE;
			varr.date=int(m_SlideBoxData.SlideInfo.dtReportData); 
			m_pRs->Fields->GetItem(_variant_t("ReportData"))->Value=(_bstr_t)varr;

			m_pRs->Fields->GetItem(_variant_t("CompareJudge"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strCompareJudge;
			m_pRs->Fields->GetItem(_variant_t("CompareDetectResult"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strCompareDetectResult;
			m_pRs->Fields->GetItem(_variant_t("CompareBacilusDensity"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strCompareBacilusDensity;
			m_pRs->Fields->GetItem(_variant_t("SlideID"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strSlideID;
			_variant_t var;
			var.vt=VT_DATE;
			var.date=int(m_SlideBoxData.SlideInfo.dtDetectData); 
			m_pRs->Fields->GetItem(_variant_t("DetectData"))->Value=(_bstr_t)var;
			
			_variant_t vart;
			vart.vt=VT_DATE;
			vart.date=DATE(m_SlideBoxData.SlideInfo.dtDetectTime); 
			m_pRs->Fields->GetItem(_variant_t("DetectTime"))->Value=(_bstr_t)vart;

			//���ϵͳʱ��
			COleDateTime   oletimeVar=COleDateTime::GetCurrentTime();
			_variant_t   vars;   					
			vars.vt=VT_DATE;   
			vars.date=DATE(oletimeVar);   // ��oletimeVar��ֵ;   
			m_pRs->Fields->GetItem(_variant_t("ReportTime"))->Value=_bstr_t(vars);
			m_pRs->Update();
		 AfxMessageBox(_T("���˱�����Ϣ����ɹ�!"));
		}
	   catch(...)
	   {
		AfxMessageBox(_T("���˱�����Ϣ����ʧ��!"));
	   }
	   	
	 //ͼƬ��Ϣ��

	  //     LARGE_INTEGER       t0;
		 //  LARGE_INTEGER       t1;
		 //  LARGE_INTEGER       frequency;
		 //  QueryPerformanceFrequency(&frequency);
		 //  QueryPerformanceCounter(&t0);
	  //    for(int nFOVIndex=0;nFOVIndex<m_SlideBoxData[nWorkPositionIndex].nFOVCount;nFOVIndex++)
		 // { 
			//try
			//{
			///*	CString str;
			//	str.Format(_T("\\%d.JPG"), nFOVIndex);*/
			//	
			//	CFile f;
			//	CFileException e;
			//	CString str;
	
			//	str.Format(_T("\\%d.JPG"), nFOVIndex);
			//	f.Open((GetDirectory(nWorkPositionIndex)+str), CFile::modeRead | CFile::typeBinary, &e); //��ָ����ͼ���ļ�
			//
			//	int nSize = f.GetLength();          //�ȵõ� ͼ�� �ļ�����
			//		
			//	BYTE* pBuffer = new BYTE [nSize];   //���ļ��Ĵ�С�ڶ�������һ���ڴ� pBuffer
			//	
			//	if (f.Read(pBuffer, nSize) > 0 )
			//	{
			//		//BYTE* pBuf = pBuffer;			//������һ����ǰ�pBuffer���ͼ�����ݷŵ�����
			//		//VARIANT	varBLOB;		
			//		//SAFEARRAY *psa;
			//		//SAFEARRAYBOUND rgsabound[1];
			//		//if(pBuf)
			//		//{    
			//		//	rgsabound[0].lLbound = 0;
			//		//	rgsabound[0].cElements = nSize;
			//		//							
			//		//	psa = SafeArrayCreate(VT_UI1, 1, rgsabound); // ���� SAFEARRAY ���� psa
			//		//	for (long i = 0; i < (long)nSize; i++)
			//		//	SafeArrayPutElement (psa, &i, pBuf++);	 // �� bBuf ָ��Ķ���������
			//		//												// ���浽 psa ��
			//		//	varBLOB.vt = VT_ARRAY | VT_UI1;	// �� varBLOB ������Ϊ BYTE ��������
			//		//	varBLOB.parray = psa;			// ���� psa �� varBLOB
			//		//}   
			//			CString strbyte;
			//			strbyte.Format(_T("%s"),pBuffer);

			//			_variant_t var;
			//			var.vt=VT_DATE;
			//			var.date=int(m_SlideBoxData[nWorkPositionIndex].SlideInfo.dtDetectData); 

			//			_variant_t vart;
			//			vart.vt=VT_DATE;
			//			vart.date=DATE(m_SlideBoxData[nWorkPositionIndex].SlideInfo.dtDetectTime);

	  // 					m_pRs->raw_Close();
			//			sql.Format(_T("insert into BacilusDetect.dbo.PictureInfo values('%s','%s','%s',%d,%d,%d,%d,%f,%f,%f,CAST('%s' as image))"),
			//						   m_SlideBoxData[nWorkPositionIndex].SlideInfo.strSlideID,(LPCTSTR)(_bstr_t)var,(LPCTSTR)(_bstr_t)vart,nFOVIndex,GetTargetCountInFOV(nWorkPositionIndex, nFOVIndex),
			//						   GetJudgedTargetCountInFOV(nWorkPositionIndex, nFOVIndex),GetFOVCheckedStatus(nWorkPositionIndex, nFOVIndex),
			//						   m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].FOVPosition.x,m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].FOVPosition.y,
			//						   m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].FOVPosition.z,strbyte);
			//			/*m_pRs->raw_Close();
			//			sql.Format(_T("insert into BacilusDetect.dbo.Text values(newid (),1,CAST('%s' as varbinary(max)))"),(GetDirectory(nWorkPositionIndex)+str));*/
			//			m_pRs->Open((_bstr_t)sql,m_pCon.GetInterfacePtr(),adOpenKeyset,adLockOptimistic,adCmdText);
			//    }
			//}
			//catch(...)
			//{
			//	AfxMessageBox(_T("ͼƬ��Ϣ����ʧ��!"));
			//}	
		 // }
		 //   QueryPerformanceCounter(&t1);
			//double  timespan = (double) (t1.QuadPart - t0.QuadPart) / frequency.QuadPart;
			//CString string;
			//string.Format(_T("%f"), timespan);
			//AfxMessageBox(string);


    LARGE_INTEGER       t0;
	LARGE_INTEGER       t1;
	LARGE_INTEGER       frequency;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t0);
	 m_pRs->raw_Close();	 
	 m_pRs->Open ("ChartInfo", _variant_t((IDispatch *) m_pCon, true), adOpenStatic, adLockOptimistic, adCmdTable);
	/* for(int nFOVIndex=0;nFOVIndex<m_SlideBoxData[nWorkPositionIndex].nFOVCount;nFOVIndex++)
	 {	*/     
		 //try
		 //    {	
			//  				
			//	CString str;	
			//	str.Format(_T("\\%d.JPG"), nFOVIndex);//GetDirectory(nWorkPositionIndex)+str			
			//
			//		    
			//		    
			//			//ͼƬ��Ϣ��				
			//			
			//			m_pRs->AddNew();//���һ���¼�¼               
			//			
		 //  				m_pRs->Fields->GetItem(_variant_t("SlideID"))->Value=(_bstr_t)m_SlideBoxData[nWorkPositionIndex].SlideInfo.strSlideID;
			//			
			//			_variant_t var;
			//			var.vt=VT_DATE;
			//			var.date=int(m_SlideBoxData[nWorkPositionIndex].SlideInfo.dtDetectData); 
			//			m_pRs->Fields->GetItem(_variant_t("DetectData"))->Value=(_bstr_t)var;
			//
			//			_variant_t vart;
			//			vart.vt=VT_DATE;
			//			vart.date=DATE(m_SlideBoxData[nWorkPositionIndex].SlideInfo.dtDetectTime); 
			//			m_pRs->Fields->GetItem(_variant_t("DetectTime"))->Value=(_bstr_t)vart;
			//			m_pRs->Fields->GetItem(_variant_t("PictureID"))->Value=(_bstr_t)nFOVIndex;
			//			m_pRs->Fields->GetItem(_variant_t("TargetCount"))->Value=(_bstr_t)GetTargetCountInFOV(nWorkPositionIndex, nFOVIndex);
			//			m_pRs->Fields->GetItem(_variant_t("JudgedTargetCount"))->Value=(_bstr_t)GetJudgedTargetCountInFOV(nWorkPositionIndex, nFOVIndex);
			//			m_pRs->Fields->GetItem(_variant_t("bChecked"))->Value=(_bstr_t)GetFOVCheckedStatus(nWorkPositionIndex, nFOVIndex);
			//			
			//			m_pRs->Fields->GetItem(_variant_t("FOVPositionX"))->Value=(_bstr_t)m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].FOVPosition.x;
			//	        m_pRs->Fields->GetItem(_variant_t("FOVPositionY"))->Value=(_bstr_t)m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].FOVPosition.y;
			//	        m_pRs->Fields->GetItem(_variant_t("FOVPositionZ"))->Value=(_bstr_t)m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].FOVPosition.z;
			//									
			//			m_pRs->Update(); 

			//			if(nFOVIndex==m_SlideBoxData[nWorkPositionIndex].nFOVCount-1)
			//			{
			//				AfxMessageBox(_T("ͼƬ��Ϣ����ɹ�!"));
			//			}
			//			
			//			
			//	}		   

			//   catch(...)
			//   {
			//	 AfxMessageBox(_T("ͼƬ��Ϣ����ʧ��!"));
			//   }

			SQLCHAR transactionToken[32];
    
			SQLHANDLE henv  = SQL_NULL_HANDLE;
			SQLHANDLE hdbc  = SQL_NULL_HANDLE;
			SQLHANDLE hstmt = SQL_NULL_HANDLE;

			LARGE_INTEGER  t3,t4, Frequency;
			double totaltime = 0;
			QueryPerformanceFrequency(&Frequency);
			QueryPerformanceCounter(&t3);
	
			try
			{
						//These statements Initialize ODBC for the client application and
						//connect to the database.

						if ( SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv) != SQL_SUCCESS )
							throw new ODBCErrors(__LINE__, SQL_HANDLE_ENV, henv);

						if ( SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION,(void*)SQL_OV_ODBC3, NULL) != SQL_SUCCESS )
							throw new ODBCErrors(__LINE__, SQL_HANDLE_ENV, henv);

						if ( SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc) != SQL_SUCCESS )
							throw new ODBCErrors(__LINE__, SQL_HANDLE_ENV, henv);

						//This code assumes that the dataset name "Sql Server FILESTREAM"
						//has been previously created on the client computer system. An
						//ODBC DSN is created with the ODBC Data Source item in
						//the Windows Control Panel.

						if ( SQLConnect(hdbc, TEXT("BacilusDetect"),//����������Դ
								SQL_NTS, NULL, 0, NULL, 0) <= 0 )
							throw new ODBCErrors(__LINE__, SQL_HANDLE_DBC, hdbc);

						//FILESTREAM requires that all read and write operations occur
						//within a transaction.
						if ( SQLSetConnectAttr(hdbc,
							SQL_ATTR_AUTOCOMMIT,
							(SQLPOINTER)SQL_AUTOCOMMIT_OFF,
							SQL_IS_UINTEGER) != SQL_SUCCESS )
							throw new ODBCErrors(__LINE__, SQL_HANDLE_DBC, hdbc);

						if ( SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) != SQL_SUCCESS )
							throw new ODBCErrors(__LINE__, SQL_HANDLE_DBC, hdbc);

						SQLUSMALLINT mode = SQL_COMMIT;
						SQLINTEGER cbTransactionToken;
						TCHAR dstFilePath[1024];
						SQLINTEGER cbDstFilePath;
						wchar_t sqlDBQuery[1024];

						for ( int nFOVIndex=0;nFOVIndex<m_SlideBoxData.nFOVCount;nFOVIndex++)
						{
							_variant_t var;
							var.vt=VT_DATE;
							var.date=int(m_SlideBoxData.SlideInfo.dtDetectData); 
			
							_variant_t vart;
							vart.vt=VT_DATE;
							vart.date=DATE(m_SlideBoxData.SlideInfo.dtDetectTime); 

							CString str;	
			                str.Format(_T("\\%d.JPG"), nFOVIndex);//GetDirectory(nWorkPositionIndex)+str
							
							//swprintf_s(sqlDBQuery, 1024, L"INSERT INTO BacilusDetect.dbo.ChartInfo(SlideID,DetectData,DetectTime,PictureID,TargetCount,JudgedTargetCount,bChecked,FOVPositionX,FOVPositionY,FOVPositionZ,Id,Chart)\
							//			OUTPUT GET_FILESTREAM_TRANSACTION_CONTEXT(), inserted.Chart.PathName()\
							//			VALUES (%s,CONVERT(date, '%s'),CONVERT(datetime, '%s'),%d,%d,%d,%d,%f,%f,%f,newid(), CONVERT(VARBINARY, '**Temp**'))",m_SlideBoxData[nWorkPositionIndex].SlideInfo.strSlideID,
							//			m_SlideBoxData[nWorkPositionIndex].SlideInfo.dtDetectData,m_SlideBoxData[nWorkPositionIndex].SlideInfo.dtDetectTime,nFOVIndex,GetTargetCountInFOV(nWorkPositionIndex, nFOVIndex),
							//			GetJudgedTargetCountInFOV(nWorkPositionIndex, nFOVIndex),GetFOVCheckedStatus(nWorkPositionIndex, nFOVIndex),m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].FOVPosition.x,
							//			m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].FOVPosition.y,m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].FOVPosition.z);

							swprintf_s(sqlDBQuery, 1024, L"INSERT INTO Archive.dbo.Records2(Id,SerialNumber,Chart,date1)\
										OUTPUT GET_FILESTREAM_TRANSACTION_CONTEXT(), inserted.Chart.PathName()\
										VALUES (newid(),23,CONVERT(VARBINARY, '**Temp**'),CONVERT(date, '2012-12-14'))");

							if ( SQLExecDirect(hstmt, sqlDBQuery, SQL_NTS) != SQL_SUCCESS )
								throw new ODBCErrors(__LINE__, SQL_HANDLE_STMT, hstmt);

							//Retrieve the transaction token.
							if ( SQLFetch(hstmt) != SQL_SUCCESS )
								throw new ODBCErrors(__LINE__, SQL_HANDLE_STMT, hstmt);

    						if ( SQLGetData(hstmt, 1,
								SQL_C_BINARY,
								transactionToken,
								sizeof(transactionToken),
								&cbTransactionToken) != SQL_SUCCESS )
								throw new ODBCErrors(__LINE__, SQL_HANDLE_STMT, hstmt);

							//Retrieve the file path for the inserted record.
							if ( SQLGetData(hstmt, 2, SQL_C_TCHAR, dstFilePath, sizeof(dstFilePath), &cbDstFilePath) != SQL_SUCCESS )
								throw new ODBCErrors(__LINE__, SQL_HANDLE_STMT, hstmt);

    						if ( SQLCloseCursor(hstmt) != SQL_SUCCESS )
								throw new ODBCErrors(__LINE__, SQL_HANDLE_STMT, hstmt);

							QueryPerformanceCounter(&t0);
							//if ( ! CopyFileToSQL( (LPTSTR)(LPCTSTR)(GetDirectory(nWorkPositionIndex)+str), dstFilePath, transactionToken, cbTransactionToken) )
							//{
							//	mode = SQL_ROLLBACK;
							//	break;
							//}
							QueryPerformanceCounter(&t1);
							double  timespan = (double) (t1.QuadPart - t0.QuadPart) / Frequency.QuadPart;
							totaltime += timespan;
						}
	
						SQLEndTran(SQL_HANDLE_DBC, hdbc, mode);	
					}
					catch(ODBCErrors *pErrors)
					{
						pErrors->Print();
						delete pErrors;
					}

					if ( hstmt != SQL_NULL_HANDLE )
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

					if ( hdbc != SQL_NULL_HANDLE )
						SQLDisconnect(hdbc);

					if ( hdbc != SQL_NULL_HANDLE )
						SQLFreeHandle(SQL_HANDLE_DBC, hdbc); 

					if ( henv != SQL_NULL_HANDLE )
						SQLFreeHandle(SQL_HANDLE_ENV, henv);
	
					QueryPerformanceCounter(&t4);
					//wprintf_s(TEXT("Time: %f\n"), (double) (t4.QuadPart - t3.QuadPart) / Frequency.QuadPart);
					//wprintf_s(TEXT("Time: %f\n"), totaltime);
	
					if (m_pRs)
						if (m_pRs->State == adStateOpen)
							m_pRs->Close();
					if (m_pCon)
						if (m_pCon->State == adStateOpen)
							m_pCon->Close();
	
					::CoUninitialize();
					// TODO: �ڴ���ӿؼ�֪ͨ����������
					AfxMessageBox(_T("ͼ�񱣴����"));
	  
           
	 
      //ϸ����Ϣ��
	  for(int nFOVIndex=0;nFOVIndex<m_SlideBoxData.nFOVCount;nFOVIndex++)
	 {	     
		     
		 for(int TargetCountIndex=0; TargetCountIndex<GetTargetCountInFOV( nFOVIndex);TargetCountIndex++)  
		 {
			 try
		    {	
				m_pRs->raw_Close();
				//ϸ����Ϣ��
				m_pRs->Open("SELECT * FROM BacilusDetect.dbo.BacilusInfo",_variant_t((IDispatch*)m_pCon),adOpenStatic,adLockOptimistic,adCmdText); 
				m_pRs->AddNew();//���һ���¼�¼
		   		m_pRs->Fields->GetItem(_variant_t("SlideID"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strSlideID;
						
				_variant_t var;
				var.vt=VT_DATE;
				var.date=int(m_SlideBoxData.SlideInfo.dtDetectData); 
				m_pRs->Fields->GetItem(_variant_t("DetectData"))->Value=(_bstr_t)var;
			
				_variant_t vart;
				vart.vt=VT_DATE;
				vart.date=DATE(m_SlideBoxData.SlideInfo.dtDetectTime); 
				m_pRs->Fields->GetItem(_variant_t("DetectTime"))->Value=(_bstr_t)vart;
				m_pRs->Fields->GetItem(_variant_t("PictureID"))->Value=(_bstr_t)nFOVIndex;
				m_pRs->Fields->GetItem(_variant_t("BacilusID"))->Value=(_bstr_t)TargetCountIndex;				
				//m_pRs->Fields->GetItem(_variant_t("RPositionX"))->Value=(_bstr_t)m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].rectTarget[TargetCountIndex].x;
				//m_pRs->Fields->GetItem(_variant_t("RPositionY"))->Value=(_bstr_t)m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].rectTarget[TargetCountIndex].y;
				//m_pRs->Fields->GetItem(_variant_t("RPositionWidth"))->Value=(_bstr_t)m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].rectTarget[TargetCountIndex].width;
				//m_pRs->Fields->GetItem(_variant_t("RPositionHeight"))->Value=(_bstr_t)m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].rectTarget[TargetCountIndex].height;
				m_pRs->Update();
			}

			catch(...)
			{
				AfxMessageBox(_T("��Ϣ����ʧ��!"));
			}	   		 
		 }
	  }
	  AfxMessageBox(_T("��Ϣ����ɹ�!"));

	  }
	  else
	{
	    int nvalue = AfxMessageBox(_T("��SlideID����Ϣ�Ѵ���,�Ƿ��������?"),4+32+256);
		if (nvalue==6)
		{
		
			try
		{		
			m_pRs->raw_Close();
	        //���˲�Ƭ��Ϣ��
			m_pRs->Open("SELECT * FROM BacilusDetect.dbo.PatientSlideInfo",_variant_t((IDispatch*)m_pCon),adOpenStatic,adLockOptimistic,adCmdText); 
		    m_pRs->AddNew();//���һ���¼�¼
		   	m_pRs->Fields->GetItem(_variant_t("SlideID"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strSlideID;
			m_pRs->Fields->GetItem(_variant_t("Type"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strType;
			
			_variant_t var;
			var.vt=VT_DATE;
			var.date=int(m_SlideBoxData.SlideInfo.dtDetectData); 
			m_pRs->Fields->GetItem(_variant_t("DetectData"))->Value=(_bstr_t)var;
			
			_variant_t vart;
			vart.vt=VT_DATE;
			vart.date=DATE(m_SlideBoxData.SlideInfo.dtDetectTime); 
			m_pRs->Fields->GetItem(_variant_t("DetectTime"))->Value=(_bstr_t)vart;
			m_pRs->Fields->GetItem(_variant_t("Quantity"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strQuantity;
			m_pRs->Fields->GetItem(_variant_t("StayTime"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strStayTime;
			m_pRs->Fields->GetItem(_variant_t("Method"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strMethod;
			m_pRs->Fields->GetItem(_variant_t("SendDoctor"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strSendDoctor;
			m_pRs->Fields->GetItem(_variant_t("Judge"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strJudge;
			m_pRs->Fields->GetItem(_variant_t("DetectResult"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strDetectResult;
			m_pRs->Fields->GetItem(_variant_t("BacilusDensity"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strBacilusDensity;
			m_pRs->Fields->GetItem(_variant_t("FOVCount"))->Value=(_bstr_t)m_SlideBoxData.nFOVCount;
			m_pRs->Fields->GetItem(_variant_t("bScan"))->Value=(_bstr_t)m_SlideBoxData.bScan;
			m_pRs->Fields->GetItem(_variant_t("bJudge"))->Value=(_bstr_t)m_SlideBoxData.bJudge;
			m_pRs->Fields->GetItem(_variant_t("TargetCount"))->Value=(_bstr_t)GetTargetCountInSlide();
			m_pRs->Fields->GetItem(_variant_t("JudgedTargetCount"))->Value=(_bstr_t)GetJudgedTargetCountInSlide();
			m_pRs->Update();
		 AfxMessageBox(_T("���˲�Ƭ��Ϣ����ɹ�!"));
		}
	   catch(...)
	   {
		AfxMessageBox(_T("���˲�Ƭ��Ϣ����ʧ��!"));
	   }


	   try
		{		
			m_pRs->raw_Close();
	        //���˱�����Ϣ��
			m_pRs->Open("SELECT * FROM BacilusDetect.dbo.PatientRepInfo",_variant_t((IDispatch*)m_pCon),adOpenStatic,adLockOptimistic,adCmdText); 
		    m_pRs->AddNew();//���һ���¼�¼
		   	m_pRs->Fields->GetItem(_variant_t("ReportDoctor"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strReportDoctor;

			_variant_t varr;
			varr.vt=VT_DATE;
			varr.date=int(m_SlideBoxData.SlideInfo.dtReportData); 
			m_pRs->Fields->GetItem(_variant_t("ReportData"))->Value=(_bstr_t)varr;

			m_pRs->Fields->GetItem(_variant_t("CompareJudge"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strCompareJudge;
			m_pRs->Fields->GetItem(_variant_t("CompareDetectResult"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strCompareDetectResult;
			m_pRs->Fields->GetItem(_variant_t("CompareBacilusDensity"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strCompareBacilusDensity;
			m_pRs->Fields->GetItem(_variant_t("SlideID"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strSlideID;
			_variant_t var;
			var.vt=VT_DATE;
			var.date=int(m_SlideBoxData.SlideInfo.dtDetectData); 
			m_pRs->Fields->GetItem(_variant_t("DetectData"))->Value=(_bstr_t)var;
			
			_variant_t vart;
			vart.vt=VT_DATE;
			vart.date=DATE(m_SlideBoxData.SlideInfo.dtDetectTime); 
			m_pRs->Fields->GetItem(_variant_t("DetectTime"))->Value=(_bstr_t)vart;

			//���ϵͳʱ��
			COleDateTime   oletimeVar=COleDateTime::GetCurrentTime();
			_variant_t   vars;   					
			vars.vt=VT_DATE;   
			vars.date=DATE(oletimeVar);   // ��oletimeVar��ֵ;   
			m_pRs->Fields->GetItem(_variant_t("ReportTime"))->Value=_bstr_t(vars);
			m_pRs->Update();
		 AfxMessageBox(_T("���˱�����Ϣ����ɹ�!"));
		}
	   catch(...)
	   {
		AfxMessageBox(_T("���˱�����Ϣ����ʧ��!"));
	   }
	  
	 //ͼƬ��Ϣ��

	  // for(int nFOVIndex=0;nFOVIndex<m_SlideBoxData[nWorkPositionIndex].nFOVCount;nFOVIndex++)
		 // { 
			//try
			//{


			///*	CString str;
			//	str.Format(_T("\\%d.JPG"), nFOVIndex);*/
			//	
			//	CFile f;
			//	CFileException e;
			//	CString str;
	
			//	str.Format(_T("\\%d.JPG"), nFOVIndex);
			//	f.Open((GetDirectory(nWorkPositionIndex)+str), CFile::modeRead | CFile::typeBinary, &e); //��ָ����ͼ���ļ�
			//
			//	int nSize = f.GetLength();          //�ȵõ� ͼ�� �ļ�����
			//		
			//	BYTE* pBuffer = new BYTE [nSize];   //���ļ��Ĵ�С�ڶ�������һ���ڴ� pBuffer
			//	
			//	if (f.Read(pBuffer, nSize) > 0 )
			//	{
			//		BYTE* pBuf = pBuffer;			//������һ����ǰ�pBuffer���ͼ�����ݷŵ�����
			//		VARIANT	varBLOB;		
			//		SAFEARRAY *psa;
			//		SAFEARRAYBOUND rgsabound[1];
			//		if(pBuf)
			//		{    
			//			rgsabound[0].lLbound = 0;
			//			rgsabound[0].cElements = nSize;
			//									
			//			psa = SafeArrayCreate(VT_UI1, 1, rgsabound); // ���� SAFEARRAY ���� psa
			//			for (long i = 0; i < (long)nSize; i++)
			//			SafeArrayPutElement (psa, &i, pBuf++);	 // �� bBuf ָ��Ķ���������
			//														// ���浽 psa ��
			//			varBLOB.vt = VT_ARRAY | VT_UI1;	// �� varBLOB ������Ϊ BYTE ��������
			//			varBLOB.parray = psa;			// ���� psa �� varBLOB
			//		}   
			//			_variant_t var;
			//			var.vt=VT_DATE;
			//			var.date=int(m_SlideBoxData[nWorkPositionIndex].SlideInfo.dtDetectData); 

			//			_variant_t vart;
			//			vart.vt=VT_DATE;
			//			vart.date=DATE(m_SlideBoxData[nWorkPositionIndex].SlideInfo.dtDetectTime);

	  // 					m_pRs->raw_Close();
			//			sql.Format(_T("insert into BacilusDetect.dbo.PictureInfo values('%s','%s','%s',%d,%d,%d,%d,%f,%f,%f,CAST('%s' as varbinary(max)))"),
			//						   m_SlideBoxData[nWorkPositionIndex].SlideInfo.strSlideID,(LPCTSTR)(_bstr_t)var,(LPCTSTR)(_bstr_t)vart,nFOVIndex,GetTargetCountInFOV(nWorkPositionIndex, nFOVIndex),
			//						   GetJudgedTargetCountInFOV(nWorkPositionIndex, nFOVIndex),GetFOVCheckedStatus(nWorkPositionIndex, nFOVIndex),
			//						   m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].FOVPosition.x,m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].FOVPosition.y,
			//						   m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].FOVPosition.z,(LPCTSTR)(_bstr_t)varBLOB);
			//			/*m_pRs->raw_Close();
			//			sql.Format(_T("insert into BacilusDetect.dbo.Text values(newid (),1,CAST('%s' as varbinary(max)))"),(GetDirectory(nWorkPositionIndex)+str));*/
			//			m_pRs->Open((_bstr_t)sql,m_pCon.GetInterfacePtr(),adOpenKeyset,adLockOptimistic,adCmdText);
			//    }
			//}
			//catch(...)
			//{
			//	AfxMessageBox(_T("ͼƬ��Ϣ����ʧ��!"));
			//}	
		 // }

	m_pRs->raw_Close();
	m_pRs->Open("SELECT * FROM BacilusDetect.dbo.PictureInfo",_variant_t((IDispatch*)m_pCon),adOpenStatic,adLockOptimistic,adCmdText); 
	 for(int nFOVIndex=0;nFOVIndex<m_SlideBoxData.nFOVCount;nFOVIndex++)
	 {	     
		 try
		   {
				CFile f;
				CFileException e;
				CString str;
	
				str.Format(_T("\\%d.JPG"), nFOVIndex);
				f.Open((GetDirectory()+str), CFile::modeRead | CFile::typeBinary, &e); //��ָ����ͼ���ļ�
			
				int nSize = f.GetLength();          //�ȵõ� ͼ�� �ļ�����
					
				BYTE* pBuffer = new BYTE [nSize];   //���ļ��Ĵ�С�ڶ�������һ���ڴ� pBuffer
				
				if (f.Read(pBuffer, nSize) > 0 )
				{
					BYTE* pBuf = pBuffer;			//������һ����ǰ�pBuffer���ͼ�����ݷŵ�����
					VARIANT	varBLOB;		
					SAFEARRAY *psa;
					SAFEARRAYBOUND rgsabound[1];
					if(pBuf)
					{    
						rgsabound[0].lLbound = 0;
						rgsabound[0].cElements = nSize;
												
						psa = SafeArrayCreate(VT_UI1, 1, rgsabound); // ���� SAFEARRAY ���� psa
						for (long i = 0; i < (long)nSize; i++)
						SafeArrayPutElement (psa, &i, pBuf++);	 // �� bBuf ָ��Ķ���������
																	// ���浽 psa ��
						varBLOB.vt = VT_ARRAY | VT_UI1;	// �� varBLOB ������Ϊ BYTE ��������
						varBLOB.parray = psa;			// ���� psa �� varBLOB
					}   

					

					
					m_pRs->AddNew();//���һ���¼�¼
		   			m_pRs->Fields->GetItem(_variant_t("SlideID"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strSlideID;
						
					_variant_t var;
					var.vt=VT_DATE;
					var.date=int(m_SlideBoxData.SlideInfo.dtDetectData); 
					m_pRs->Fields->GetItem(_variant_t("DetectData"))->Value=(_bstr_t)var;
			
					_variant_t vart;
					vart.vt=VT_DATE;
					vart.date=DATE(m_SlideBoxData.SlideInfo.dtDetectTime); 
					m_pRs->Fields->GetItem(_variant_t("DetectTime"))->Value=(_bstr_t)vart;
					m_pRs->Fields->GetItem(_variant_t("PictureID"))->Value=(_bstr_t)nFOVIndex;
					m_pRs->Fields->GetItem(_variant_t("TargetCount"))->Value=(_bstr_t)GetTargetCountInFOV( nFOVIndex);
					m_pRs->Fields->GetItem(_variant_t("JudgedTargetCount"))->Value=(_bstr_t)GetJudgedTargetCountInFOV( nFOVIndex);
					m_pRs->Fields->GetItem(_variant_t("bChecked"))->Value=(_bstr_t)GetFOVCheckedStatus( nFOVIndex);
					
					
					m_pRs->Fields->GetItem("PictureContent")->AppendChunk(varBLOB);//��Ӵ������
					m_pRs->Fields->GetItem(_variant_t("FOVPositionX"))->Value=(_bstr_t)m_SlideBoxData.TargetInFOV[nFOVIndex].FOVPosition.x;
				    m_pRs->Fields->GetItem(_variant_t("FOVPositionY"))->Value=(_bstr_t)m_SlideBoxData.TargetInFOV[nFOVIndex].FOVPosition.y;
				    m_pRs->Fields->GetItem(_variant_t("FOVPositionZ"))->Value=(_bstr_t)m_SlideBoxData.TargetInFOV[nFOVIndex].FOVPosition.z;
					
					
					m_pRs->Update();
				
					
					delete [] pBuffer;
					pBuf=0; //��ָ��ָ��0, ��ϰ��
					f.Close();

					

					
				}            
				if(nFOVIndex==m_SlideBoxData.nFOVCount-1)
				{
					AfxMessageBox(_T("ͼƬ��Ϣ����ɹ�!"));
				}

				

		}

		catch(...)
		{
			AfxMessageBox(_T("ͼƬ��Ϣ����ʧ��!"));
		}	   		 
	} 

	 //ϸ����Ϣ��
	  for(int nFOVIndex=0;nFOVIndex<m_SlideBoxData.nFOVCount;nFOVIndex++)
	 {	     
		     
		 for(int TargetCountIndex=0; TargetCountIndex<GetTargetCountInFOV(nFOVIndex);TargetCountIndex++)  
		 {
			 try
		    {	
				m_pRs->raw_Close();
				//ϸ����Ϣ��
				m_pRs->Open("SELECT * FROM BacilusDetect.dbo.BacilusInfo",_variant_t((IDispatch*)m_pCon),adOpenStatic,adLockOptimistic,adCmdText); 
				m_pRs->AddNew();//���һ���¼�¼
		   		m_pRs->Fields->GetItem(_variant_t("SlideID"))->Value=(_bstr_t)m_SlideBoxData.SlideInfo.strSlideID;
						
				_variant_t var;
				var.vt=VT_DATE;
				var.date=int(m_SlideBoxData.SlideInfo.dtDetectData); 
				m_pRs->Fields->GetItem(_variant_t("DetectData"))->Value=(_bstr_t)var;
			
				_variant_t vart;
				vart.vt=VT_DATE;
				vart.date=DATE(m_SlideBoxData.SlideInfo.dtDetectTime); 
				m_pRs->Fields->GetItem(_variant_t("DetectTime"))->Value=(_bstr_t)vart;
				m_pRs->Fields->GetItem(_variant_t("PictureID"))->Value=(_bstr_t)nFOVIndex;
				m_pRs->Fields->GetItem(_variant_t("BacilusID"))->Value=(_bstr_t)TargetCountIndex;				
				//m_pRs->Fields->GetItem(_variant_t("RPositionX"))->Value=(_bstr_t)m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].rectTarget[TargetCountIndex].x;
				//m_pRs->Fields->GetItem(_variant_t("RPositionY"))->Value=(_bstr_t)m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].rectTarget[TargetCountIndex].y;
				//m_pRs->Fields->GetItem(_variant_t("RPositionWidth"))->Value=(_bstr_t)m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].rectTarget[TargetCountIndex].width;
				//m_pRs->Fields->GetItem(_variant_t("RPositionHeight"))->Value=(_bstr_t)m_SlideBoxData[nWorkPositionIndex].TargetInFOV[nFOVIndex].rectTarget[TargetCountIndex].height;
				m_pRs->Update();
			}

			catch(...)
			{
				AfxMessageBox(_T("��Ϣ����ʧ��!"));
			}	   		 
		 }
	  }
	  AfxMessageBox(_T("��Ϣ����ɹ�!"));

		}
        else return;
	 }


}


CString CBacilusDetectDoc::VariantToCString(VARIANT var)
{
	CString strValue;
	_variant_t var_t;
	_bstr_t bst_t;
	time_t cur_time;
	CTime time_value;
	COleCurrency var_currency;
	switch(var.vt)
	{
	
	case VT_EMPTY:
		strValue=_T("");
	    break;
	case VT_UI1:
		strValue.Format(_T("%d"),var.bVal);
		break;
	case VT_I2:
		strValue.Format(_T("%d"),var.iVal);
		break;
	case VT_I4:
		strValue.Format(_T("%d"),var.lVal);
		break;
	case VT_R4:
		strValue.Format(_T("%f"),var.fltVal);
		break;
	case VT_R8:
		strValue.Format(_T("%f"),var.dblVal);
		break;
	case VT_CY:
		var_currency=var;
		strValue=var_currency.Format(0);
		break;
	case VT_BSTR:
		var_t=var;
		bst_t=var_t;
		strValue.Format(_T("%s"),(const char*)bst_t);
		break;
	case VT_NULL:
		strValue=_T("");
		break;
    case VT_DATE:
		cur_time=(long)var.date;
		time_value=cur_time;
		strValue=time_value.Format(_T("%A,%B%d,%Y"));
		break;
	case VT_BOOL:
		strValue.Format(_T("%d"),var.boolVal);
		break;
	default:
		strValue=_T("");
		break;

	}
	return strValue;
}


