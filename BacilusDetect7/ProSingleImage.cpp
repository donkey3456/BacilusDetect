// ProSingleImage.cpp: implementation of the CProSingleImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProSingleImage.h"
#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<math.h>
#include<vector> 

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const double AREATHR =60;



CProSingleImage::CProSingleImage(cv::Mat &img)
{
	//assert(img !=NULL || img.channels()== 3 );

	 
	std::vector<cv::Mat> channels; //建立一个vector变量 channels
	
	m_ImgSize = cv::Size(800,600); 
	m_pSrcImg.create(m_ImgSize.height,m_ImgSize.width,CV_8UC3);
	cv::resize(img,m_pSrcImg,m_ImgSize);

	m_pGrayImg.create(m_ImgSize.height,m_ImgSize.width,CV_8U);
	m_pHueImg.create(m_ImgSize.height,m_ImgSize.width,CV_8U);
	m_pSaturationImg.create(m_ImgSize.height,m_ImgSize.width,CV_8U);
	m_pValueImg.create(m_ImgSize.height,m_ImgSize.width,CV_8U);

	cv::Mat tmpimg(m_ImgSize,CV_8UC3);
	cv::cvtColor(m_pSrcImg, tmpimg, CV_BGR2HSV); //将原始的BGR图像变换为HSV图像
	cv::split(tmpimg, channels); //将tmping里HSV三通道的值分开保存在channels中
	m_pHueImg = channels[0];
	m_pSaturationImg = channels[1];
	m_pValueImg = channels[2]; //将channels里的三个值分别赋予“H、S、V”

	//cv::ReleaseImage(&tmpimg);
	
	m_pGrayImg.zeros(m_ImgSize,CV_8U);
	cv::cvtColor(m_pSrcImg, m_pGrayImg, CV_BGR2GRAY); //将原始图片变成灰度图（原始图片依然存在）
	m_pCoarseSegImg.create(m_ImgSize.height,m_ImgSize.width,CV_8U);
    m_pPostSegImg.create(m_ImgSize.height,m_ImgSize.width,CV_8U);
	m_pClassifiedSrcImg.create(m_ImgSize.height,m_ImgSize.width,CV_8U);
	m_pClassifiedBin3CImg.create(m_ImgSize.height,m_ImgSize.width,CV_8U);
	m_pCoarseSegImg.zeros(m_ImgSize,CV_8U);
	m_pPostSegImg.zeros(m_ImgSize,CV_8U);
	m_pClassifiedSrcImg.zeros(m_ImgSize,CV_8U);
	m_pClassifiedBin3CImg.zeros(m_ImgSize,CV_8U); //初始化四个图像数据

	//镜头杂质噪声掩膜图像
	m_pLensNoiseMaskImg.create(m_ImgSize,CV_8U);
	m_pLensNoiseMaskImg.zeros(m_ImgSize,CV_8U);

	m_maxHue = 0;
	m_nNumTB1 = 0;
	m_nNumTB2 = 0;

	m_rectTB1 = NULL;
	m_rectTB2 = NULL;

}

CProSingleImage::~CProSingleImage()
{
	//if(m_pHueImg.data)
	//{
	//	//cv::ReleaseImage(&m_pHueImg);
	//	m_pHueImg = NULL;
	//}
	//if(m_pSaturationImg.data)
	//{
	//	//cv::ReleaseImage(&m_pSaturationImg);
	//	m_pSaturationImg = NULL;
	//}
	//if(m_pGrayImg.data)
	//{
	//	//cv::ReleaseImage(&m_pGrayImg);
	//	m_pGrayImg = NULL;
	//}
	//if(m_pCoarseSegImg.data)
	//{
	//	//cv::ReleaseImage(&m_pCoarseSegImg);
	//	m_pCoarseSegImg = NULL;
	//}
	//if(m_pClassifiedSrcImg.data)
	//{
	//	//cv::ReleaseImage(&m_pClassifiedSrcImg);
	//	m_pClassifiedSrcImg = NULL;
	//}
	//if(m_pClassifiedBin3CImg.data)
	//{
	//	//cv::ReleaseImage(&m_pClassifiedBin3CImg);
	//	m_pClassifiedBin3CImg = NULL;
	//}
	//if(m_pPostSegImg.data)
	//{
	//	//cv::ReleaseImage(&m_pPostSegImg);
	//	m_pPostSegImg = NULL;
	//}
	//if(m_pSrcImg.data)
	//{
	//	//cv::ReleaseImage(&m_pSrcImg);
	//	m_pSrcImg = NULL;
	//}
	//if(m_pValueImg.data)
	//{
	//	//cv::ReleaseImage(&m_pValueImg);
	//	m_pValueImg = NULL;
	//}
	//if(m_pLensNoiseMaskImg.data)
	//{
	//	//cv::ReleaseImage(&m_pLensNoiseMaskImg);
	//	m_pLensNoiseMaskImg = NULL;
	//}
	//if(m_rectTB1)
	//{
	//	delete m_rectTB1;
	//	m_rectTB1 = NULL;
	//}
	//if(m_rectTB2)
	//{
	//	delete m_rectTB2;
	//	m_rectTB2 = NULL;
	//}
}

//CBlobResult CProSingleImage::GetBlobResult()
//{
	//return m_blobResult;
//}






void CProSingleImage::ClassifyObject()
{
/********************************************************************************/
//第一步：首先获取每个连通块的面积、长宽比、圆形度、粗糙度等参数
/********************************************************************************/
//	int NumBlobs = m_blobResult.GetNumBlobs();
////	if(NumBlobs == 0)
////		return;
//
//	double * areas = new double[NumBlobs];
//	memset(areas,0,sizeof(double)*NumBlobs);
//
//	double * whratios = new double[NumBlobs];
//	memset(whratios,0,sizeof(double)*NumBlobs);
//
//	double * circs = new double[NumBlobs];
//	memset(circs,0,sizeof(double)*NumBlobs);
//
//	double * roughs = new double[NumBlobs];
//	memset(roughs,0,sizeof(double)*NumBlobs);
//
//	int i;
//	for (i=0; i<NumBlobs; i++)
//	{
//		CBlob Blob = m_blobResult.GetBlob(i);
//
//		//面积
//		areas[i] = Blob.Area();
//
//		//长宽比
//		CvBox2D box = Blob.GetMinAreaRect();
//		double height = box.size.height;
//		double width = box.size.width;
//		whratios[i] = (height > width)? height/width : width/height;
//		
//		//圆形度
//		CBlobGetCompactness getCompactness = CBlobGetCompactness();
//		circs[i] = getCompactness(Blob);
//
//		//粗糙度
//		CBlobGetRoughness getRoughness = CBlobGetRoughness();
//		roughs[i] = getRoughness(Blob);
//	}
	
	double * areas = new double[contour.size()];
	memset(areas,0,sizeof(double)*contour.size());

	double * whratios = new double[contour.size()];
	memset(whratios,0,sizeof(double)*contour.size());

	double * circs = new double[contour.size()];
	memset(circs,0,sizeof(double)*contour.size());

	double * roughs = new double[contour.size()];
	memset(roughs,0,sizeof(double)*contour.size());

	for (int i = 0; i < contour.size(); i++)  
    {  
        //得到每个轮廓的面积
		areas[i]= fabs(cv::contourArea(contour[i]));  
		
		//计算轮廓的长宽比  
		cv::Rect aRect = cv::boundingRect(contour[i]); 
		whratios[i] = (aRect.height > aRect.width)? (float)(aRect.height/aRect.width): (float)(aRect.width/aRect.height);
		//if((aRect.width/aRect.height)>1) whratios[i] = (float)aRect.width/(float)aRect.height;
		//else whratios[i] = (float)aRect.height/(float)aRect.width;

		//计算圆形度
		double contLenth = cv::arcLength(contour[i],true );
		circs[i] = (4 * 3.14159265358979323846 * areas[i]) / pow(contLenth,2); 
		
        //计算粗糙度
		std::vector<cv::Point> hull;
		cv::convexHull(contour[i],hull, true);
		double hullPerimeter = fabs(cv::arcLength(hull,true));
		roughs[i] = (float)(contLenth / hullPerimeter);

	}

/********************************************************************************/
//第二步：目标分类
/********************************************************************************/
	//int * objclass  = new int[NumBlobs];
	//memset(objclass,0,sizeof(int)*NumBlobs);
	int * objclass  = new int[contour.size()];
	memset(objclass,0,sizeof(int)*contour.size());
		
	for(int i=0; i < contour.size(); i++)
	{
		if(whratios[i] < 10)
		{
			if(whratios[i] >= 2.68852 )
			{
				if(roughs[i] >= 1.1752)
				{
					if(areas[i] > 150)
					{
						objclass[i]=2; //第2类目标
						m_nNumTB2++;
					}
					else
					{
						objclass[i]=3; //第3类目标
					}
				}
				else
				{
					objclass[i]=1; //第1类目标
					m_nNumTB1++;
				}
			}
			else
			{
				if(circs[i] >=0.204286)
				{
					if(areas[i]>=367.5)
					{
						objclass[i]=3; //第3类目标
					}
					else
					{
						if(whratios[i] >=2.3)
						{
							if(roughs[i] < 1.1752)
							{
								objclass[i]=1; //第1类目标
								m_nNumTB1++;
							}
							else
							{
								objclass[i]=3; //第3类目标
							}
						}
						else
						{
							if(circs[i] >=0.299157)
							{
								objclass[i]=3; //第3类目标
							}
							else
							{
								if(areas[i] > 150)
								{
									objclass[i]=2; //第2类目标
									m_nNumTB2++;
								}

							}
						}
					}
				}
				else
				{
					if(areas[i] >= 745.5)
					{
						if(circs[i] >=0.127547)
						{
							objclass[i]=3; //第3类目标
						}
						else
						{
							objclass[i]=2; //第2类目标
							m_nNumTB2++;
						}
					}
					else
					{
						if(areas[i] > 150)
						{
							objclass[i]=2; //第2类目标
							m_nNumTB2++;
						}
					}
				}
			}
		}

	}

/*******************************************************************************/
//第三步：标识目标
/********************************************************************************/
	std::vector<cv::Mat> PostSeg_channels;
	
	if(m_rectTB1 != NULL)
	{
		delete m_rectTB1;
		m_rectTB1 = NULL;
	}
	if(m_rectTB2 != NULL)
	{
		delete m_rectTB2;
		m_rectTB2 = NULL;
	}
	m_rectTB1 = new cv::Rect[contour.size()]; 
	m_rectTB2 = new cv::Rect[contour.size()];   
	int m=0;
	int n=0;
	
	

	//cvMerge(m_pPostSegImg,m_pPostSegImg,m_pPostSegImg,NULL,m_pClassifiedBin3CImg);
	//cvCopy(m_pSrcImg,m_pClassifiedSrcImg);
	PostSeg_channels.push_back(m_pPostSegImg);
	PostSeg_channels.push_back(m_pPostSegImg);
	PostSeg_channels.push_back(m_pPostSegImg);
	//PostSeg_channels[1] = m_pPostSegImg;
	//PostSeg_channels[2] = m_pPostSegImg;
	cv::merge(PostSeg_channels, m_pClassifiedBin3CImg);
	m_pSrcImg.copyTo(m_pClassifiedSrcImg);
	
	for (int i=0; i < contour.size(); i++)
	{
		/*CBlob Blob = m_blobResult.GetBlob(i);
        *int iMaxx=Blob.MaxX();
		int iMinx=Blob.MinX();
		int iMaxy=Blob.MaxY();
		int iMiny=Blob.MinY();
		
		rect.x=iMinx;
		rect.y=iMiny;
		rect.width = iMaxx-rect.x;
		rect.height= iMaxy-rect.y;*/

		cv::Rect aRect = cv::boundingRect(contour[i]); 
		if(objclass[i] == 1)
		{
			m_rectTB1[m] = aRect;
			m++;
 	      
			cv::rectangle( m_pClassifiedSrcImg,aRect.tl(), aRect.br(), cvScalar(0,255,0),1, 8, 0 );
			cv::rectangle( m_pClassifiedBin3CImg,aRect.tl(), aRect.br(), cvScalar(0,255,0),1, 8, 0 );
			/*cv::rectangle( m_pClassifiedSrcImg, cv::Point(Blob.MinX()-2,Blob.MinY()-2), cv::Point (Blob.MaxX()+2,Blob.MaxY()+2),CV_RGB(255, 0,0), 2, 8, 0);
			cv::rectangle( m_pClassifiedBin3CImg, cv::Point(Blob.MinX()-2,Blob.MinY()-2), cv::Point (Blob.MaxX()+2,Blob.MaxY()+2),CV_RGB(255,0,0), 2, 8, 0);*/
		}
		else if(objclass[i] == 2)
		{
			m_rectTB2[n] = aRect;
			n++;

			cv::rectangle( m_pClassifiedSrcImg,aRect.tl(), aRect.br(), cvScalar(0,255,0),1, 8, 0 );
			cv::rectangle( m_pClassifiedBin3CImg,aRect.tl(), aRect.br(), cvScalar(0,255,0),1, 8, 0 );
			/*cv::rectangle( m_pClassifiedSrcImg, cv::Point2f(Blob.MinX()-2,Blob.MinY()-2), cv::Point2f(Blob.MaxX()+2,Blob.MaxY()+2),CV_RGB(0, 255,0), 2, 8, 0);
			cv::rectangle( m_pClassifiedBin3CImg, cv::Point2f(Blob.MinX()-2,Blob.MinY()-2), cv::Point2f(Blob.MaxX()+2,Blob.MaxY()+2),CV_RGB(0,255,0), 2, 8, 0);*/
		}
		else
		{
	//		cvRectangle( m_pClassifiedSrcImg, cvPoint(Blob.MinX()-2,Blob.MinY()-2), cvPoint (Blob.MaxX()+2,Blob.MaxY()+2),CV_RGB(0,0,255), 2, 8, 0);
			cv::rectangle( m_pClassifiedBin3CImg,aRect.tl(), aRect.br(), cvScalar(0,255,0),1, 8, 0 );
		}
		
	}

// 	cv::namedWindow("result_1");
// 	cv::imshow("restult_1", m_pClassifiedSrcImg);
// 	cv::namedWindow("result_2");
// 	cv::imshow("restult_2", m_pClassifiedBin3CImg);
// 	
// 	
// 	cv::waitKey(0);
//  
/*******************************************************************************/
//第四步：释放内存
/********************************************************************************/
	delete [] areas;
	delete [] whratios ;
	delete [] circs ;
	delete [] roughs ;
	delete [] objclass ;
}

//void CProSingleImage::ClassifyObject()
//{
///********************************************************************************/
//第一步：首先获取每个连通块的面积、长宽比、圆形度、粗糙度等参数
///********************************************************************************/
//	int NumBlobs = m_blobResult.GetNumBlobs();
//	if(NumBlobs == 0)
//		return;
//
//	double * areas = new double[NumBlobs];
//	memset(areas,0,sizeof(double)*NumBlobs);
//
//	double * whratios = new double[NumBlobs];
//	memset(whratios,0,sizeof(double)*NumBlobs);
//
//	double * circs = new double[NumBlobs];
//	memset(circs,0,sizeof(double)*NumBlobs);
//
//	double * roughs = new double[NumBlobs];
//	memset(roughs,0,sizeof(double)*NumBlobs);
//
//	int i;
//	for (i=0; i<NumBlobs; i++)
//	{
//		CBlob Blob = m_blobResult.GetBlob(i);
//
//		//面积
//		areas[i] = Blob.Area();
//
//		//长宽比
//		CvBox2D box = Blob.GetMinAreaRect();
//		double height = box.size.height;
//		double width = box.size.width;
//		whratios[i] = (height > width)? height/width : width/height;
//		
//		//圆形度
//		CBlobGetCompactness getCompactness = CBlobGetCompactness();
//		circs[i] = getCompactness(Blob);
//
//		//粗糙度
//		CBlobGetRoughness getRoughness = CBlobGetRoughness();
//		roughs[i] = getRoughness(Blob);
//	}
//
///********************************************************************************/
//第二步：目标分类
///********************************************************************************/
//	int * objclass  = new int[NumBlobs];
//	memset(objclass,0,sizeof(int)*NumBlobs);
//	m_nNumTB1=0;
//	m_nNumTB2=0;
//
//	for (i=0; i<NumBlobs; i++)
//	{
//		if(whratios[i] >=2.3)
//		{
//			if(roughs[i] >=1.1652)
//			{
//				//objclass[i]=2; //第2类目标
//				//m_nNumTB2++;
//				if(areas[i] > 167.5)
//				{
//					objclass[i]=2; //第2类目标
//					m_nNumTB2++;
//				}
//				else
//				{
//					objclass[i]=3; //第3类目标
//				}
//			}
//			else
//			{
//				objclass[i]=1; //第1类目标
//				m_nNumTB1++;
//			}
//		}
//		else
//		{
//			if(circs[i] >=0.204286)
//			{
//				objclass[i]=3; //第3类目标
//
//				//if(areas[i]>=367.5)
//				//{
//				//	objclass[i]=3; //第3类目标
//				//}
//				//else
//				//{
//				//	if(whratios[i] >=2.3)
//				//	{
//				//		objclass[i]=2; //第1类目标
//				//		m_nNumTB1++;
//				//	}
//				//	else
//				//	{
//				//		//if(circs[i] >=0.299157)
//				//		//{
//				//		//	objclass[i]=3; //第3类目标
//				//		//}
//				//		//else
//				//		//{
//				//		//	objclass[i]=2; //第2类目标
//				//		//	m_nNumTB2++;
//				//		//}
//				//		objclass[i]=3; //第3类目标
//				//	}
//				//}
//			}
//			else
//			{
//				if(areas[i] >= 745.5)
//				{
//					if(circs[i] >=0.127547)
//					{
//						objclass[i]=3; //第3类目标
//					}
//					else
//					{
//						objclass[i]=2; //第2类目标
//						m_nNumTB2++;
//					}
//				}
//				else if(areas[i] >= 367.5)
//				{
//					objclass[i]=2; //第2类目标
//					m_nNumTB2++;
//				}
//				//objclass[i]=3; //第3类目标
//			}
//		}
//
//	}
//
///*******************************************************************************/
//第三步：标识目标
///********************************************************************************/
//	if(m_rectTB1 != NULL)
//	{
//		delete m_rectTB1;
//		m_rectTB1 = NULL;
//	}
//	if(m_rectTB2 != NULL)
//	{
//		delete m_rectTB2;
//		m_rectTB2 = NULL;
//	}
//	m_rectTB1 = new CvRect[m_nNumTB1];
//	m_rectTB2 = new CvRect[m_nNumTB2];
//	int m=0;
//	int n=0;
//
//	cvMerge(m_pPostSegImg,m_pPostSegImg,m_pPostSegImg,NULL,m_pClassifiedBin3CImg);
//	cvCopy(m_pSrcImg,m_pClassifiedSrcImg);
//	for (i=0; i<NumBlobs; i++)
//	{
//		CBlob Blob = m_blobResult.GetBlob(i);
//		
//		int iMaxx=Blob.MaxX();
//		int iMinx=Blob.MinX();
//		int iMaxy=Blob.MaxY();
//		int iMiny=Blob.MinY();
//		CvRect rect;
//		rect.x=iMinx;
//		rect.y=iMiny;
//		rect.width = iMaxx-rect.x;
//		rect.height= iMaxy-rect.y;
//
//		if(objclass[i] == 1)
//		{
//			m_rectTB1[m]=rect;
//			m++;
//
//			cvRectangle( m_pClassifiedSrcImg, cvPoint(Blob.MinX()-2,Blob.MinY()-2), cvPoint (Blob.MaxX()+2,Blob.MaxY()+2),CV_RGB(255, 0,0), 2, 8, 0);
//			cvRectangle( m_pClassifiedBin3CImg, cvPoint(Blob.MinX()-2,Blob.MinY()-2), cvPoint (Blob.MaxX()+2,Blob.MaxY()+2),CV_RGB(255,0,0), 2, 8, 0);
//		}
//		else if(objclass[i] == 2)
//		{
//			m_rectTB2[n]=rect;
//			n++;
//
//			cvRectangle( m_pClassifiedSrcImg, cvPoint(Blob.MinX()-2,Blob.MinY()-2), cvPoint (Blob.MaxX()+2,Blob.MaxY()+2),CV_RGB(0, 255,0), 2, 8, 0);
//			cvRectangle( m_pClassifiedBin3CImg, cvPoint(Blob.MinX()-2,Blob.MinY()-2), cvPoint (Blob.MaxX()+2,Blob.MaxY()+2),CV_RGB(0,255,0), 2, 8, 0);
//		}
//		else
//		{
//	//		cvRectangle( m_pClassifiedSrcImg, cvPoint(Blob.MinX()-2,Blob.MinY()-2), cvPoint (Blob.MaxX()+2,Blob.MaxY()+2),CV_RGB(0,0,255), 2, 8, 0);
//			cvRectangle( m_pClassifiedBin3CImg, cvPoint(Blob.MinX()-2,Blob.MinY()-2), cvPoint (Blob.MaxX()+2,Blob.MaxY()+2),CV_RGB(0, 0,255), 2, 8 , 0);
//		}
//
//	}
//
///*******************************************************************************/
//第四步：释放内存
///********************************************************************************/
//	delete [] areas;
//	delete [] whratios ;
//	delete [] circs ;
//	delete [] roughs ;
//	delete [] objclass ;
//}

double CProSingleImage::CalcRedEdgeVar(cv::Mat &mask)
{
	int i,j;
	int width = m_pGrayImg.cols;
	int height = m_pGrayImg.rows;
	double sum=0;
	double mean=0;
	double var=0;
	double gray=0;
	int count=0;
	uchar* data_tmpimg;
    uchar* data_mask;
	
	cv::Mat tmpimg;
	tmpimg.create(m_pGrayImg.rows,m_pGrayImg.cols,CV_8U);
	cv::medianBlur(m_pGrayImg,tmpimg,3);
	cv::GaussianBlur(tmpimg,tmpimg,cv::Size(3,3),0,0);
	cv::GaussianBlur(tmpimg,tmpimg,cv::Size(3,3),0,0);

	for(i = 0 ; i< height; i++)
		 data_tmpimg = tmpimg.ptr<uchar>(i);
	     data_mask = mask.ptr<uchar>(i);
				for(j =0 ; j< width; j++)
					if(data_mask[j] > 0)
					{
						sum += data_tmpimg[j];  
						count++;
					}
		
	mean = sum/count;

	for( i = 0 ; i< height; i++)

		for(j =0 ; j< width; j++)
		{
			if( data_mask[j] > 0)
			{
				var += (data_tmpimg[j] - mean)*(data_tmpimg[j] - mean);
			}
		}
	var = var/count;

	tmpimg = NULL;	

	return var;
}





void CProSingleImage::HueRotation(cv::Mat &srcImg, cv::Mat &dstImg, int sinta, int clockwiseOrNot)
{
	if(srcImg.channels() !=1 || dstImg.channels() !=1)
		return;
	if(sinta > 180)
		return;
	int hVal = 0;
	int dstVal = 0;
	int i,j;
	uchar* data_srcImg;
	uchar* data_dstImg;
	if(clockwiseOrNot == 0)
	{
		for(i=0; i< srcImg.rows; i++)
		{
		    data_srcImg = srcImg.ptr<uchar>(i);
			for(j=0; j< srcImg.cols; j++)
			{
				hVal = data_srcImg[j];
				if(hVal >=0 && hVal < sinta) 
					dstVal = 180 + hVal - sinta;
				else if(hVal >= sinta)
					dstVal = hVal - sinta;
				data_dstImg = dstImg.data + i*dstImg.step + j*dstImg.elemSize();  
				*data_dstImg = dstVal;
				//SetPixVal8(dstImg,j,i,dstVal);
			}
		}
			
	}
	else if(clockwiseOrNot == 1)
	{
		for(i=0; i< srcImg.rows; i++)
		{
			data_srcImg = srcImg.ptr<uchar>(i);
			for(j=0; j< srcImg.cols; j++)
			{
				hVal = data_srcImg[j];
				if(hVal >=0 && hVal < 180-sinta) 
					dstVal = hVal + sinta;
				else if(hVal >= 180-sinta)
					dstVal = hVal + sinta - 180;
				data_dstImg = dstImg.data + i*dstImg.step + j*dstImg.elemSize();  
				*data_dstImg = dstVal;
				//SetPixVal8(dstImg,j,i,dstVal);
			}
		}
	}
}

UINT CProSingleImage::GetTB1Num()
{
	return m_nNumTB1;
}

UINT CProSingleImage::GetTB2Num()
{
	return m_nNumTB2;
}

// //采用固定阈值法进行图像分割
void CProSingleImage::SegmentColorImageHSVFixedAndGrayAdapt(double huethr, double satthr, double satthr2,double valthr, int templateSize)
{
	cv::Mat tmpimg1;
	tmpimg1.create(m_ImgSize.height,m_ImgSize.width,CV_8U);
	cv::Mat tmpimg2; 
	tmpimg2.create(m_ImgSize.height,m_ImgSize.width,CV_8U);
	tmpimg1.zeros(m_ImgSize,CV_8U);
	tmpimg2.zeros(m_ImgSize,CV_8U);

	/********************************************************************************/
	//第一步：在HSV颜色空间采用固定阈值法进行图像初始分割
	/********************************************************************************/
	cv::threshold(m_pHueImg,tmpimg1,huethr,255,CV_THRESH_BINARY);
	cv::threshold(m_pSaturationImg,tmpimg2,satthr,255,CV_THRESH_BINARY);
	cv::bitwise_and(tmpimg1,tmpimg2,tmpimg1);
	cv::threshold(m_pSaturationImg,tmpimg2,satthr2,255,CV_THRESH_BINARY_INV);
	cv::bitwise_and(tmpimg1,tmpimg2,tmpimg1);
	cv::threshold(m_pValueImg,tmpimg2,valthr,255,CV_THRESH_BINARY);
	cv::bitwise_and(tmpimg1,tmpimg2,tmpimg1);

	
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
	cv::dilate(tmpimg1,tmpimg1,element);
    //cvDilate(tmpimg1,tmpimg1);
	cv::erode(tmpimg1,tmpimg1,element);
//	cvErode(tmpimg1,tmpimg1);

	/********************************************************************************/
	//第二步：进行自适应阈值分割
	/********************************************************************************/
	cv::adaptiveThreshold(m_pGrayImg,tmpimg2,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV ,templateSize,5);
	medianBlur(tmpimg2,tmpimg2,3); // 中值滤波
	
	/********************************************************************************/
	//第三步：两次分割结果进行“与”操作
	/********************************************************************************/
	cv::bitwise_and(tmpimg1,tmpimg2,m_pCoarseSegImg);//粗分割结果存放在m_pCoarseSegImg

	/********************************************************************************/
	//第四步：剔除镜头杂质
	/********************************************************************************/
	EliminateLensNoise();

	/********************************************************************************/
	//第五步：释放内存
	/********************************************************************************/
	//cv::ReleaseImage(&tmpimg1);
	//cv::ReleaseImage(&tmpimg2);
}

//计算输入图像（必须是单通道图像）的像素均值，如果mask不为NULL,则计算仅仅mask覆盖区域
double CProSingleImage::CalcMaskedMean(cv::Mat &srcimg, cv::Mat &mask)
{
	if(srcimg.data == NULL || srcimg.channels() != 1)
		return 0 ;
	else
	{
		int sum,count;
		double mean;
		sum = 0;
		count = 0;
		mean = 0;
		int i,j;
		if(!mask.data)
		{
			for(i = 0 ; i< srcimg.rows; i++)
			{
				uchar* data_scr = srcimg.ptr<uchar>(i);
			    uchar* data_mask = mask.ptr<uchar>(i);
				for(j =0 ; j< srcimg.cols; j++)
					if(data_mask[j] > 0)
					{
						sum += data_scr[j];  
						count++;
					}
			}
		}
		else
		{
			for(i = 0 ; i< srcimg.rows; i++)
			{
				uchar* data_scr = srcimg.ptr<uchar>(i);
			    uchar* data_mask = mask.ptr<uchar>(i);
				for(j =0 ; j< srcimg.cols; j++)
					if(data_mask[j] > 0)
					{
						sum += data_scr[j];  
						count++;
					}
			}
		}
		if(count != 0)
		{
			mean = (double)sum/(double)count;
		}
		return mean;
	}
}


void CProSingleImage::SegmentColorImageMixture4(double huethr, double satthr, double satthr2,double valthr)
{
	int i;

   //归一化面积参数，以800*600为基准，该面积参数用来过滤小的杂质
	double areathr = AREATHR*(m_ImgSize.width * m_ImgSize.height) /(800*600);

	//*************************************************************************/
	//第一步： 采用固定阈值进行分割，分割结果为m_pCoarseSegImg
	//************************************************************************/
	m_pCoarseSegImg.zeros(m_ImgSize,CV_8U);
	//cvZero(m_pCoarseSegImg);
	SegmentColorImageHSVFixedAndGrayAdapt(huethr,satthr,satthr2,valthr,27);//粗分割

	//*************************************************************************/
	//第二步： 将分割结果添加到m_blobResult中
	//************************************************************************/
	//cv::rectangle(m_pCoarseSegImg, cv::Point(0,0), cv::Point(m_pCoarseSegImg.cols-1, m_pCoarseSegImg.rows-1), cv::Scalar(0), 1);
	/*CBlobResult BlobResult(m_pCoarseSegImg, NULL, 0,false);
	BlobResult.Filter( BlobResult, B_INCLUDE, CBlobGetExterior(), B_NOT_EQUAL, 1);	
	BlobResult.Filter( BlobResult, B_INCLUDE, CBlobGetMean(), B_GREATER, 1);
	BlobResult.Filter( BlobResult, B_INCLUDE, CBlobGetArea(), B_GREATER, areathr);
	int NumBlobs = BlobResult.GetNumBlobs();

	for(i=0; i< NumBlobs; i++)
	{
		CBlob Blob = BlobResult.GetBlob(i);
		m_blobResult.AddBlob(Blob);
		Blob.FillBlob(m_pPostSegImg,CV_RGB(255,255,255));
	}	*/	


	//CvMemStorage* storage = cvCreateMemStorage(0);
	//CvSeq* contour=0;
	cv::Mat src;
	m_pCoarseSegImg.copyTo(src); //将分割结果m_pCoarseSegImg放到src中
	
	/*cv::Mat dst;*/ //destination image
	//dst.create(m_ImgSize.height,m_ImgSize.width,CV_8U);
	//std::vector<cv::Point> contour; 
	std::vector<cv::Vec4i> hierarchy; 
	//cv::Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));//随机生成颜色
	//cv::Mat dst2;
	
	//cvFindContours(src,storage,&contour,sizeof(CvContour),CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE);
	cv::findContours(src,contour,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	
	//cvZero(dst);
	//dst.zeros(m_ImgSize,CV_8U);

	//画所有轮廓(填充)
	//cvDrawContours(dst,contour,CV_RGB(255,255,255),CV_RGB(0,255,0),2,CV_FILLED,8,cvPoint(0,0));
	/*cv::drawContours(dst,contour,idx,cv::Scalar(255),1,8,hierarchy,2);*/
	
	//cvNamedWindow("contour",1);
	//cvShowImage("contour",dst);

	for (int i = 0; i<contour.size(); i++)  
    {  
  
        double tmparea = fabs(contourArea(contour[i])); 

		if(tmparea < areathr) contour.erase(contour.begin() + i); 

	}

	//cv::drawContours(m_pPostSegImg,contour,-1,cv::Scalar(255),CV_FILLED,8);
	cv::drawContours(m_pPostSegImg,contour,-1,cv::Scalar(255),2);
	                         
}
void CProSingleImage::SetLensNoiseMaskImg(cv::Mat &maskimg)
{
//	assert(maskimg !=NULL);
	if(maskimg.data)
	{
		cv::resize(maskimg,m_pLensNoiseMaskImg,m_ImgSize);  
	}
}

void CProSingleImage::EliminateLensNoise(void)
{
	cv::Mat masknot;
	masknot.create(m_pLensNoiseMaskImg.rows,m_pLensNoiseMaskImg.cols,CV_8U);
	cv::bitwise_not(m_pLensNoiseMaskImg,masknot);
	cv::bitwise_and(masknot,m_pCoarseSegImg,m_pCoarseSegImg);
	//cv::ReleaseImage(&masknot);
}  


cv::Mat CProSingleImage::MakeLensNoiseMaskImage(cv::Mat &grayimg)
{
    cv::Mat maskimg;
	maskimg.create(grayimg.rows, grayimg.cols, CV_8U);
	maskimg.zeros(grayimg.rows, grayimg.cols,CV_8U);
	//cvAdaptiveThreshold(grayimg,maskimg,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV ,31,5);
	cv::adaptiveThreshold(grayimg,maskimg,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV,31,5);
	return maskimg;
}