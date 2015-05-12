#include "StdAfx.h"
#include "MyBitmap.h"


CMyBitmap::CMyBitmap(void)
{
	InitializeCriticalSection(&m_csData);

	m_nWidth = 0;
	m_nHeight = 0;
	m_nChannels = 0;

	m_nStep = 0;

	m_pData = NULL;
	m_pBMPHeader = NULL;
}

CMyBitmap::CMyBitmap( const CMyBitmap& bitmap )
{
	InitializeCriticalSection(&m_csData);

	if (!CopyFromBitMap(bitmap))
	{
		m_nWidth = 0;
		m_nHeight = 0;
		m_nChannels = 0;

		m_pData = NULL;
		m_pBMPHeader = NULL;
	}
}

CMyBitmap::CMyBitmap( const cv::Mat& mat )
{
	InitializeCriticalSection(&m_csData);

	if (!CopyFromMat(mat))
	{
		m_nWidth = 0;
		m_nHeight = 0;
		m_nChannels = 0;

		m_pData = NULL;
		m_pBMPHeader = NULL;
	}
}


CMyBitmap::~CMyBitmap(void)
{
	DeleteCriticalSection(&m_csData);

	if (m_pData)
	{
		delete m_pData;
		m_pData = NULL;
	}

	if (m_pBMPHeader)
	{
		delete m_pBMPHeader;
		m_pBMPHeader = NULL;
	}
}

CMyBitmap& CMyBitmap::operator=( const CMyBitmap& bitmap )
{
	CopyFromBitMap(bitmap);
	return *this;
}

CMyBitmap& CMyBitmap::operator=( const cv::Mat& mat )
{
	CopyFromMat(mat);
	return *this;
}

bool CMyBitmap::CopyFromMat( const cv::Mat& mat )
{
	if (mat.type() != CV_8UC1 && mat.type() != CV_8UC3)
	{
		m_strError = "invalid input type";
		return false;
	}

	if (mat.empty())
	{
		m_strError = "empty mat";
		return false;
	}
	
	EnterCriticalSection(&m_csData);

	if (mat.type() == CV_8UC1)
	{
		_create_gray_bmp(mat.cols,mat.rows);
	}
	else
	{
		_create_rgb_bmp(mat.cols,mat.rows);

	}
// 	if (mat.isContinuous() && m_nStep == m_nWidth * m_nChannels)
// 	{
// 		const uchar* pData = mat.ptr(0);
// 		memcpy(m_pData,pData, m_nHeight * m_nStep);
// 	}
// 	else
// 	{
// 		for(int i = 0; i < mat.rows; i++)
// 		{
// 			const uchar* Mi = mat.ptr(i);
// 			memcpy(m_pData + i * m_nStep, Mi, mat.cols * m_nChannels);		
// 		}
// 	}
	for(int i = 0; i < mat.rows; i++)
	{
		const uchar* Mi = mat.ptr(mat.rows - 1 - i);
		memcpy(m_pData + i * m_nStep, Mi, mat.cols * m_nChannels);		
	}
	LeaveCriticalSection(&m_csData);

	return true;
}


bool CMyBitmap::CopyFromBitMap(const CMyBitmap& bitmap )
{
	if (bitmap.m_nWidth <= 0 || bitmap.m_nHeight <=0)
	{
		m_strError = "empty bitmap";
		return false;
	}
	
	if (bitmap.m_nChannels == 3)
	{
		_create_rgb_bmp(bitmap.m_nWidth,bitmap.m_nHeight);
	}
	else if (bitmap.m_nChannels == 1)
	{
		_create_gray_bmp(bitmap.m_nWidth,bitmap.m_nHeight);
	}
	else
	{
		m_strError = "invalid number of channels";
		return false;
	}
	memcpy(m_pData,bitmap.m_pData,m_nStep * m_nHeight);

	return true;
}


bool CMyBitmap::IsSizeSame( int nWidth,int nHeight,int nChannels )
{
	return ((nChannels == m_nChannels) && (nWidth == m_nWidth) && (nHeight == m_nHeight));
}

//////////////////////////////////////////////////////////////////////////
// This function is used to setup the data and the header for rgb bitmap
//////////////////////////////////////////////////////////////////////////
void CMyBitmap::_create_rgb_bmp( int nWidth, int nHeight )
{
	//check size
	if (IsSizeSame(nWidth,nHeight,3))
	{
		return;
	}

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nChannels = 3;

	//the number of byte of each row must be multiples of 4
	//so that windows can draw it
	m_nStep = m_nWidth * m_nChannels / 4 * 4;

	//create new header
	if (m_pBMPHeader)
	{
		delete[] m_pBMPHeader;
	}
	m_pBMPHeader = new char[sizeof(BITMAPINFOHEADER)];



	// write header
	memset(m_pBMPHeader,0,sizeof(BITMAPINFOHEADER));
	((BITMAPINFOHEADER*)m_pBMPHeader)->biBitCount = 24;
	((BITMAPINFOHEADER*)m_pBMPHeader)->biHeight = m_nHeight;
	((BITMAPINFOHEADER*)m_pBMPHeader)->biWidth = m_nWidth;
	((BITMAPINFOHEADER*)m_pBMPHeader)->biSize = sizeof(BITMAPINFOHEADER);
	((BITMAPINFOHEADER*)m_pBMPHeader)->biPlanes = 1;
	((BITMAPINFOHEADER*)m_pBMPHeader)->biCompression = BI_RGB;
	((BITMAPINFOHEADER*)m_pBMPHeader)->biSizeImage = m_nStep * m_nHeight;

	//write image
	if (m_pData)
	{
		delete m_pData;
	}

	m_pData = new unsigned char[m_nHeight * m_nStep];
}

//////////////////////////////////////////////////////////////////////////
// This function is used to setup the data and the header for gray bitmap
//////////////////////////////////////////////////////////////////////////
void CMyBitmap::_create_gray_bmp( int nWidth, int nHeight )
{
	//check image size
	if (IsSizeSame(nWidth,nHeight,1))
	{
		return;
	}

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nChannels = 1;

	//the number of byte of each row must be multiples of 4
	//so that windows can draw it
	m_nStep = m_nWidth * m_nChannels / 4 * 4;

	//create new header
	if (m_pBMPHeader)
	{
		delete[] m_pBMPHeader;
	}
	m_pBMPHeader = new char[sizeof(BITMAPINFOHEADER) + 256 * 4];

	memset(m_pBMPHeader,0,sizeof(BITMAPINFOHEADER));
	((BITMAPINFOHEADER*)m_pBMPHeader)->biBitCount = 8;
	((BITMAPINFOHEADER*)m_pBMPHeader)->biClrUsed = 256;
	((BITMAPINFOHEADER*)m_pBMPHeader)->biHeight = nHeight;
	((BITMAPINFOHEADER*)m_pBMPHeader)->biWidth = nWidth;
	((BITMAPINFOHEADER*)m_pBMPHeader)->biSize = sizeof(BITMAPINFOHEADER);
	((BITMAPINFOHEADER*)m_pBMPHeader)->biPlanes = 1;
	((BITMAPINFOHEADER*)m_pBMPHeader)->biCompression = BI_RGB;
	((BITMAPINFOHEADER*)m_pBMPHeader)->biSizeImage = nWidth * nHeight;
	((BITMAPINFOHEADER*)m_pBMPHeader)->biClrImportant = 0;

	//write palette
	unsigned char* pColorP = (unsigned char*)m_pBMPHeader + sizeof(BITMAPINFOHEADER);
	int i = 0;

	for (; i < 256;i++)
	{
		*(pColorP + i * 4) = i;
		*(pColorP + i * 4 + 1) = i;
		*(pColorP + i * 4 + 2) = i;
		*(pColorP + i * 4 + 3) = 0;
	}

	// write image
	if (m_pData)
	{
		delete []m_pData;
	}

	m_pData = new unsigned char[m_nStep * m_nHeight];

}

bool CMyBitmap::IsEmpty()
{
	return (m_nWidth <= 0 || m_nHeight <=0);
}

#ifdef _WINDOWS
void CMyBitmap::Draw( HDC hDC,RECT rect,int nFactor, int nXshift,int nYshift )
{
	SetStretchBltMode(hDC,COLORONCOLOR);
	StretchDIBits(hDC, 0, 0, abs(rect.right - rect.left),abs(rect.top - rect.bottom),
		m_nWidth / 2 - m_nWidth / 2 / nFactor + nXshift,
		m_nHeight / 2 - m_nHeight / 2 /nFactor + nYshift,
		m_nWidth / nFactor,	m_nHeight / nFactor, m_pData, (BITMAPINFO*)m_pBMPHeader,
		DIB_RGB_COLORS,SRCCOPY);
}


#endif

void CMyBitmap::Release()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nChannels = 0;

	m_nStep = 0;
	if (m_pData)
	{
		delete []m_pData;
		m_pData = NULL;
	}
	if (m_pBMPHeader)
	{
		delete []m_pBMPHeader;
		m_pBMPHeader = NULL;
	}
}
