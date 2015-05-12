// ProSingleImage.h: interface for the CProSingleImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROSINGLEIMAGE_H__9F8AE756_EE67_4A67_A76A_70748481B615__INCLUDED_)
#define AFX_PROSINGLEIMAGE_H__9F8AE756_EE67_4A67_A76A_70748481B615__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<WinDef.h>
class CProSingleImage  
{
public:
	CProSingleImage(cv::Mat &img);
	virtual ~CProSingleImage();

private:
	cv::Size 	m_ImgSize;              //ͼ���С
	//CBlobResult m_blobResult;       //��ǰͼ��blob�������
	double m_maxHue;				//��ǰͼ���ɫ�����ֵ
	int m_nNumTB1;                  //�����ĵ�һ��Ŀ�����������޷�֧��
	int m_nNumTB2;                  //�����ĵڶ���Ŀ���������ص�ճ����

public://ͼ���Ա����
	cv::Mat m_pSrcImg;           //����ͼ��
	cv::Mat m_pHueImg;           //Hͨ��ͼ��
	cv::Mat m_pSaturationImg;    //Sͨ��ͼ��
	cv::Mat m_pValueImg;         //Vͨ��ͼ��
	cv::Mat m_pGrayImg;          //GRYAͼ��
	cv::Mat m_pLensNoiseMaskImg;  //��ͷ����������Ĥͼ��
	cv::Mat m_pCoarseSegImg;	   //�ַָ���ͼ��(��ͨ��)
	cv::Mat m_pPostSegImg;      //�ָ���������(��ͨ����
	cv::Mat m_pClassifiedSrcImg;          //������ͼ����ԭʼͼ���ϱ�ע����ͨ����
	cv::Mat m_pClassifiedBin3CImg;        //������ͼ���ڶ�ֵͼ���ϱ�ע����ͨ����
	
	std::vector<std::vector<cv::Point>> contour;  //�ָ����ȡ������
	
	cv::Rect *m_rectTB1;  //��һ��Ŀ��ľ��ο� (����)
	cv::Rect *m_rectTB2;  //�ڶ���Ŀ��ľ��ο� (����)

public:	//ͼ��ָ������Щ�������ò�ͬ�ķָ��������ͼ����зָ
	    //��Ҫע����ǣ�Ŀǰ���㷨����SegmentColorImageMixture4�������ú���������SegmentColorImageHSVFixedAndGrayAdapt��
	
	
	
	void SegmentColorImageHSVFixedAndGrayAdapt(double huethr, double satthr, double satthr2,double valthr, int templateSize);
	void SegmentColorImageMixture4(double huethr=140, double satthr=5, double satthr2=150,double valthr=156);

public://���ھ�������Ŀ����ຯ����Ϊ��Ч�ʣ�����If,then�ķ�ʽд��
	void ClassifyObject();

public://���ü��޳���ͷ���ʺ�����
	void SetLensNoiseMaskImg(cv::Mat &maskimg);
	void EliminateLensNoise(void);
	cv::Mat MakeLensNoiseMaskImage(cv::Mat &grayimg);

public:	//��ȡ��������������˸˾��ĸ�����
	UINT GetTB2Num();
	UINT GetTB1Num();

protected:
	//CBlobResult GetBlobResult();
	double CalcRedEdgeVar(cv::Mat &mask);
	void HueRotation(cv::Mat &srcImg, cv::Mat &dstImg, int sinta,int clockwiseOrNot);
	double CalcMaskedMean(cv::Mat &srcimg, cv::Mat &mask);
};

#endif // !defined(AFX_PROSINGLEIMAGE_H__9F8AE756_EE67_4A67_A76A_70748481B615__INCLUDED_)
