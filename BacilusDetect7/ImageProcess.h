// 自己编写的图像处理函数

#pragma once

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

typedef struct tagCoordinateFunction
{
	float fX_k;
	float fX_b;
	float fY_k;
	float fY_b;
} StructCoordinateFunction;

// 计算二值化图像前景区域的面积
int	CalcTargetArea(cv::Mat image);

// 计算灰度图像的直方图
int CalcHist(cv::Mat image, int ihist[256], int & iMax, int & iMin);

// 根据Otsu算法计算灰度图像的二值化的阈值
int otsu (cv::Mat image);

//计算BGR图像的平均灰度值
float CalImageAverageGray(const cv::Mat pcvImage); 

// 计算BGR图像标准方差
double CalImageStdDeviation(cv::Mat pcvImage);

// 画出坐标系
StructCoordinateFunction DrawCoordinate(cv::Mat pcvImg, float fXmin, float fXmax, float fYmin, float fYmax);

// 在坐标系中，画出一个点
void PlotPointInCoordinate(cv::Mat pcvImg, StructCoordinateFunction scf, float fX, float fY, CvScalar color, int type = 0);

// 画出子图的边界
void DrawSubImageBoundary(cv::Mat pcvImg, int iSubImgRow, int iSubImgCol, CvScalar color, int linewidth = 1);

// 删除文件夹
void  DeleteDirectory(CString strDirectory);

// 将图像数据在指定目录下保存为BMP图像（以扫描顺序编号命名）
int SaveImage2BMP(const CString strDirectory,const int ScanSequenceIndex, const cv::Mat image);

// 将图像数据在指定目录下保存为JPG图像（以扫描顺序编号命名）
CString SaveImage2JPG(const CString strDirectory,const int ScanSequenceIndex, const cv::Mat image);

// 将指定目录下的图像读到程序图像数据中
cv::Mat  LoadImage2Color(const CString strFileName);

// 将指定目录下的视野图像(JPG格式)读到程序图像数据中
cv::Mat  LoadFOVImage2Color(const CString strDirectory,const int ScanSequenceIndex);

