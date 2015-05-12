// �Լ���д��ͼ������

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

// �����ֵ��ͼ��ǰ����������
int	CalcTargetArea(cv::Mat image);

// ����Ҷ�ͼ���ֱ��ͼ
int CalcHist(cv::Mat image, int ihist[256], int & iMax, int & iMin);

// ����Otsu�㷨����Ҷ�ͼ��Ķ�ֵ������ֵ
int otsu (cv::Mat image);

//����BGRͼ���ƽ���Ҷ�ֵ
float CalImageAverageGray(const cv::Mat pcvImage); 

// ����BGRͼ���׼����
double CalImageStdDeviation(cv::Mat pcvImage);

// ��������ϵ
StructCoordinateFunction DrawCoordinate(cv::Mat pcvImg, float fXmin, float fXmax, float fYmin, float fYmax);

// ������ϵ�У�����һ����
void PlotPointInCoordinate(cv::Mat pcvImg, StructCoordinateFunction scf, float fX, float fY, CvScalar color, int type = 0);

// ������ͼ�ı߽�
void DrawSubImageBoundary(cv::Mat pcvImg, int iSubImgRow, int iSubImgCol, CvScalar color, int linewidth = 1);

// ɾ���ļ���
void  DeleteDirectory(CString strDirectory);

// ��ͼ��������ָ��Ŀ¼�±���ΪBMPͼ����ɨ��˳����������
int SaveImage2BMP(const CString strDirectory,const int ScanSequenceIndex, const cv::Mat image);

// ��ͼ��������ָ��Ŀ¼�±���ΪJPGͼ����ɨ��˳����������
CString SaveImage2JPG(const CString strDirectory,const int ScanSequenceIndex, const cv::Mat image);

// ��ָ��Ŀ¼�µ�ͼ���������ͼ��������
cv::Mat  LoadImage2Color(const CString strFileName);

// ��ָ��Ŀ¼�µ���Ұͼ��(JPG��ʽ)��������ͼ��������
cv::Mat  LoadFOVImage2Color(const CString strDirectory,const int ScanSequenceIndex);

