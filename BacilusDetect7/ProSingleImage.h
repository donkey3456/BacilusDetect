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
	cv::Size 	m_ImgSize;              //图像大小
	//CBlobResult m_blobResult;       //当前图像blob分析结果
	double m_maxHue;				//当前图像块色调最大值
	int m_nNumTB1;                  //检测出的第一类目标数量（简单无分支）
	int m_nNumTB2;                  //检测出的第二类目标数量（重叠粘连）

public://图像成员变量
	cv::Mat m_pSrcImg;           //输入图像
	cv::Mat m_pHueImg;           //H通道图像
	cv::Mat m_pSaturationImg;    //S通道图像
	cv::Mat m_pValueImg;         //V通道图像
	cv::Mat m_pGrayImg;          //GRYA图像
	cv::Mat m_pLensNoiseMaskImg;  //镜头杂质噪声掩膜图像
	cv::Mat m_pCoarseSegImg;	   //粗分割结果图像(单通道)
	cv::Mat m_pPostSegImg;      //分割结果后处理结果(单通道）
	cv::Mat m_pClassifiedSrcImg;          //分类结果图像（在原始图像上标注，三通道）
	cv::Mat m_pClassifiedBin3CImg;        //分类结果图像（在二值图像上标注，三通道）
	
	std::vector<std::vector<cv::Point>> contour;  //分割后提取的轮廓
	
	cv::Rect *m_rectTB1;  //第一类目标的矩形框 (待改)
	cv::Rect *m_rectTB2;  //第二类目标的矩形框 (待改)

public:	//图像分割函数，这些函数采用不同的分割方法对输入图像进行分割。
	    //需要注意的是：目前的算法采用SegmentColorImageMixture4函数，该函数调用了SegmentColorImageHSVFixedAndGrayAdapt。
	
	
	
	void SegmentColorImageHSVFixedAndGrayAdapt(double huethr, double satthr, double satthr2,double valthr, int templateSize);
	void SegmentColorImageMixture4(double huethr=125, double satthr=15, double satthr2=150,double valthr=140);

public://基于决策树的目标分类函数，为了效率，采用If,then的方式写。
	void ClassifyObject();

public://设置及剔除镜头杂质函数。
	void SetLensNoiseMaskImg(cv::Mat &maskimg);
	void EliminateLensNoise(void);
	cv::Mat MakeLensNoiseMaskImage(cv::Mat &grayimg);

public:	//获取处理结果函数（结核杆菌的个数）
	UINT GetTB2Num();
	UINT GetTB1Num();

protected:
	//CBlobResult GetBlobResult();
	double CalcRedEdgeVar(cv::Mat &mask);
	void HueRotation(cv::Mat &srcImg, cv::Mat &dstImg, int sinta,int clockwiseOrNot);
	double CalcMaskedMean(cv::Mat &srcimg, cv::Mat &mask);
};

#endif // !defined(AFX_PROSINGLEIMAGE_H__9F8AE756_EE67_4A67_A76A_70748481B615__INCLUDED_)
