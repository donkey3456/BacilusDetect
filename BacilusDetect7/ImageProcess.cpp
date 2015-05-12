// �Լ���д��ͼ������

#include "StdAfx.h"
#include "define.h"
#include "ImageProcess.h"

/* ע�⣺λͼ�е�R��G��B�����ı��������RGB����������COLORREF�е�R��G��B�����Ĵ����෴ */

// �����ֵ��ͼ��ǰ����������

int CalcTargetArea( cv::Mat image )
{
	int nArea = 0;
	for(int j = 0; j < image.rows; j++ )
	{
		unsigned char *np = (unsigned char *)image.ptr(j);
		for(int i = 0; i < image.cols; i++ )
		{
			unsigned char temp = (unsigned char) ( *np++ );
			if (temp > 0)
			{
				nArea++;
			}
		}
	}
	return nArea;
}

// ����Ҷ�ͼ���ֱ��ͼ


int CalcHist( cv::Mat image, int ihist[256], int & iMax, int & iMin )
{
	// ��ֱ��ͼ����
	memset(ihist, 0, sizeof(int)*256);
	int i, j;
	unsigned char *np; // ͼ��ָ��
	iMax = 0;
	iMin = 255;
	for( j = 0; j < image.rows; j++ )
	{
		np = (unsigned char *)image.ptr(j);
		for( i = 0; i < image.cols; i++ )
		{
			unsigned char temp = (unsigned char) ( *np++ );
			ihist[temp]++;
			if ( temp > iMax ) { iMax = temp; }
			if ( temp < iMin ) { iMin = temp; }
		}
	}

	int iMaxHist = -1;
	for ( i=0; i<256; i++ )
	{
		if ( ihist[i] > iMaxHist )
		{
			iMaxHist = ihist[i];
		}
	}
	return iMaxHist;
}


// ����Otsu�㷨����Ҷ�ͼ��Ķ�ֵ������ֵ

int otsu( cv::Mat image )
{
	unsigned char *np; // ͼ��ָ��
	int thresholdValue=256; // ��ֵ
	int ihist[256]; // ͼ��ֱ��ͼ��256����

	int i, j, k; // various counters
	int n, n1, n2;
	double m1, m2, sum, csum, fmax, sb;

	// ��ֱ��ͼ����
	memset(ihist, 0, sizeof(ihist));
	for( j = 0; j < image.rows; j++ )
	{
		np = (unsigned char *)image.ptr(j) ;
		for( i = 0; i < image.cols; i++ )
		{
			unsigned char temp = (unsigned char) ( *np++ );
			ihist[temp]++;
		}
	}

	// set up everything
	sum = csum = 0.0;
	n = 0;

	for (k = 0; k <= 255; k++) 
	{
		sum += (double) k * (double) ihist[k]; /**//* x*f(x) ������*/
		n += ihist[k]; /**//* f(x) ���� */
	}

	if (!n) 
	{
		return (256);
	}

	// do the otsu global thresholding method
	fmax = -1.0;
	n1 = 0;
	for (k = 0; k < 255; k++) 
	{
		n1 += ihist[k];
		if (!n1) 
		{ 
			continue;
		}
		n2 = n - n1;
		if (n2 == 0) 
		{ 
			break;
		}

		csum += (double) k *ihist[k];
		m1 = csum / n1;
		m2 = (sum - csum) / n2;
		sb = (double) n1 *(double) n2 * (m1 - m2) * (m1 - m2);
		/**//* bbg: note: can be optimized. */
		if (sb > fmax) 
		{
			fmax = sb;
			thresholdValue = k;
		}
	}

	return(thresholdValue);
}

//����BGRͼ���ƽ���Ҷ�ֵ
float CalImageAverageGray(const cv::Mat matImage)
{
	_ASSERTE(!matImage.empty());
	_ASSERTE(matImage.channels() == 3);

	cv::Mat matGrayImg = cv::Mat(matImage.size(),CV_8UC1);
	cv::cvtColor(matImage, matGrayImg, CV_BGR2GRAY);//ת���ɻҶ�ͼ��

	float fTemp;
	fTemp = cv::sum( matGrayImg )[0] / matGrayImg.total();   // �Ҷ�ͼ��ƽ��ֵ

	return fTemp;
}


// ��������ϵ
StructCoordinateFunction DrawCoordinate(cv::Mat matImg, float fXmin, float fXmax, float fYmin, float fYmax)
{
	matImg = cv::Scalar(255, 255, 255, 0);
	
	cv::rectangle( matImg, cv::Point(0,0), cv::Point(matImg.rows - 1, matImg.cols - 1), CV_RGB(0,0,0) );

	float fDeltaX = (fXmax - fXmin) / 10;
	float fDeltaY = (fYmax - fYmin) / 10;
	fXmin -= fDeltaX;
	fXmax += fDeltaX;
	fYmin -= fDeltaY;
	fYmax += fDeltaY;
	
	//  ����XY������
	int iStickCount = 12;
	CvPoint pt1, pt2;
	int YStick = matImg.rows / iStickCount;
	int XStick = matImg.cols / iStickCount;
	char  text[100];
// 	cv::Font font;
// 	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5f, 0.5f); 
	int i;
	for ( i=1; i<iStickCount; i++ )
	{	
		pt1 =  cv::Point( (matImg.rows-1)-YStick*i, 0);
		pt2 =  cv::Point( (matImg.rows-1)-YStick*i, matImg.cols-1);
		cv::line( matImg, pt1,pt2, CV_RGB(128,128,128) );     
// 		pt1 =  cvPoint(0,				      (matImg->height-1)-YStick*i );
// 		pt2 =  cvPoint(matImg->width-1,       (matImg->height-1)-YStick*i );
// 		cvLine( matImg, pt1,pt2, CV_RGB(128,128,128) );                          // Y��Grid
		sprintf(text, "%g", fYmin + i*fDeltaY);
		pt1.x += 2;
		pt1.y -= 3;
		cv::putText( matImg, text, pt1, cv::FONT_HERSHEY_SIMPLEX, 0.5f, CV_RGB(128,128,128) );              // Y��Stick
	}	
	for ( i=1; i<iStickCount; i++ )
	{	
		pt1 =  cv::Point(matImg.rows-1, i*XStick);
		pt2 =  cv::Point(0            , i*XStick);
		cv::line( matImg, pt1,pt2, CV_RGB(128,128,128) );                          // X��Grid
		sprintf(text, "%g", fXmin + i*fDeltaX);
		pt1.x += 2;
		pt1.y -= 4;
		cv::putText( matImg, text, pt1, cv::FONT_HERSHEY_SIMPLEX, 0.5f, CV_RGB(128,128,128) );              // X��Stick
	}

	StructCoordinateFunction scf;
	scf.fX_k = scf.fX_b = scf.fY_k = scf.fY_b = 0;

	if ( (fXmax - fXmin) != 0 )
	{
		scf.fX_k = matImg.cols / (fXmax - fXmin);
		scf.fX_b = - scf.fX_k * fXmin;
	}
	if ( (fYmax - fYmin) != 0 )
	{
		scf.fY_k = -matImg.rows / (fYmax - fYmin);
		scf.fY_b = - scf.fY_k * fYmax;
	}
	return scf;
}

// ������ϵ�У�����һ����
void PlotPointInCoordinate(cv::Mat matImg, StructCoordinateFunction scf, float fX, float fY, CvScalar color, int type)
{
	cv::Point pt;
	pt.x = scf.fX_k * fX + scf.fX_b;		
	pt.y = scf.fY_k * fY + scf.fY_b;	
	char  text[100];
	BYTE* ptr = NULL;
	int i;
	if ( (pt.x>=0) && (pt.x < matImg.rows) && (pt.y >= 0) && (pt.y < matImg.cols) )
	{
		switch ( type )
		{
		case 0:
			ptr = (BYTE*) ( matImg.ptr(pt.x) );
			for ( i=0; i < matImg.channels(); i++ )
			{
				*( ptr + matImg.channels() * pt.y + i ) = color.val[i];
			}			
			break;

		case 1:
			sprintf(text, "*");
			cv::putText( matImg, text, pt, cv::FONT_HERSHEY_SIMPLEX, 0.5f, CV_RGB(128,128,128) );
			break;

		default:
			break;
		}
	}	
}

// ������ͼ�ı߽�
void DrawSubImageBoundary(cv::Mat matImg, int iSubImgRow, int iSubImgCol, CvScalar color, int linewidth)
{
	int i;
	int iSubImgWidth = matImg.cols / iSubImgCol;
	int iSubImgHeight = matImg.rows / iSubImgRow;
	CvPoint pt1, pt2;
	pt1.y = 0;
	pt2.y = matImg.rows - 1;
	for ( i=1; i<iSubImgCol; i++ )
	{
		pt1.x = i * iSubImgWidth;
		pt2.x = pt1.x;
		cv::line( matImg, pt1,pt2, color, linewidth );
	}

	pt1.x = 0;
	pt2.x = matImg.cols - 1;
	for ( i=1; i<iSubImgRow; i++ )
	{
		pt1.y = i * iSubImgHeight;
		pt2.y = pt1.y;
		cv::line( matImg, pt1,pt2, color, linewidth );
	}
}

// ɾ���ļ���
void  DeleteDirectory(CString strDirectory)
{
	int sizeOfString = (strDirectory.GetLength() + 1);
	LPTSTR pszSource = new TCHAR[sizeOfString+2];
	_tcscpy_s(pszSource, sizeOfString, strDirectory);
	pszSource[sizeOfString] = '\0';
	pszSource[sizeOfString+1] = '\0';
	
	SHFILEOPSTRUCT FileOp;
	FileOp.hwnd = NULL;
	FileOp.wFunc = FO_DELETE;
	FileOp.pFrom = (LPCTSTR) pszSource;
	FileOp.pTo = NULL;
	FileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;
	FileOp.fAnyOperationsAborted = TRUE;
	FileOp.hNameMappings = NULL;
	FileOp.lpszProgressTitle = NULL;

	SHFileOperation(&FileOp);
	delete [] pszSource;
}

// ��ͼ��������ָ��Ŀ¼�±���ΪBMPͼ����ɨ��˳����������
int SaveImage2BMP(const CString strDirectory, const int ScanSequenceIndex, const cv::Mat image)
{
	int result;

	CString str;
	str.Format(_T("\\%d.bmp"), ScanSequenceIndex);
	CString strFileName = strDirectory + str;
	int sizeOfString = (strFileName.GetLength() + 1);
	char* filename = new char[sizeOfString];
	int i = 0;
	for ( i=0; i<strFileName.GetLength(); i++ )
	{
		filename[i] = strFileName.GetAt(i);
	}
	filename[i] = '\0';

	result = cv::imwrite(filename,image);

	delete [] filename;
	return result;
}

// ��ͼ��������ָ��Ŀ¼�±���ΪJPGͼ����ɨ��˳����������
CString SaveImage2JPG(const CString strDirectory,const int ScanSequenceIndex, const cv::Mat image)
{
	CString str;
	str.Format(_T("\\%d.JPG"), ScanSequenceIndex);
	CString strFileName = strDirectory + str;
	int sizeOfString = (strFileName.GetLength() + 1);
	char* filename = new char[sizeOfString];
	int i = 0;
	for ( i=0; i<strFileName.GetLength(); i++ )
	{
		filename[i] = strFileName.GetAt(i);
	}
	filename[i] = '\0';

	cv::imwrite(filename,image);

	delete [] filename;
	return strFileName;
}

// ��ָ��Ŀ¼�µ�ͼ���������ͼ��������
cv::Mat  LoadImage2Color(const CString strFileName)
{
	int sizeOfString = (strFileName.GetLength() + 1);
	char filename[1000];
	_ASSERT(sizeOfString<1000);

	int i = 0;
	for ( i=0; i<strFileName.GetLength(); i++ )
	{
		filename[i] = strFileName.GetAt(i);
	}
	filename[i] = '\0';

	return cv::imread(filename);
}

// ��ָ��Ŀ¼�µ���Ұͼ��(JPG��ʽ)��������ͼ��������
cv::Mat  LoadFOVImage2Color(const CString strDirectory,const int ScanSequenceIndex)
{
	CString str;
	str.Format(_T("\\%d.JPG"), ScanSequenceIndex);
	CString strFileName = strDirectory + str;
	
	return LoadImage2Color(strFileName);
}

