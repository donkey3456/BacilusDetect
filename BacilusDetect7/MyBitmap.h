#pragma once

#include <opencv2/opencv.hpp>

class CMyBitmap
{
public:
	CMyBitmap(void);
	~CMyBitmap(void);

	CMyBitmap(const CMyBitmap& bitmap);
	CMyBitmap(const cv::Mat& mat);

	bool CopyFromMat(const cv::Mat& mat);
	bool CopyFromBitMap(const CMyBitmap& bitmap);

	CMyBitmap& operator=(const CMyBitmap& bitmap);
	CMyBitmap& operator=(const cv::Mat& mat);

	int GetWidth(){return m_nWidth;}
	int GetHeight(){return m_nHeight;}

	int GetStep(){return m_nStep;}
	uchar* GetData(){return m_pData;}

	bool IsEmpty();
	bool IsSizeSame(int nWidth,int nHeight,int nChannels); 

	int GetChannels(){return m_nChannels;}

	void Release();
#ifdef _WINDOWS
	void Draw(HDC hDC,RECT rect,int nFactor, int nXshift,int nYshift);
#endif

protected:
	void _create_rgb_bmp(int nWidth, int nHeight);
	void _create_gray_bmp(int nWidth, int nHeight);

protected:
	std::string m_strError;

	int m_nWidth;
	int m_nHeight;
	int m_nChannels;
	int m_nStep;

	unsigned char* m_pData;
	char* m_pBMPHeader;
	CRITICAL_SECTION m_csData;
	
};

