//cv hw10

#include <string>
#include <opencv2\opencv.hpp>
//#include <iostream>

using namespace cv;
using namespace std;

struct Kernel
{
    int kCols; //结构元素的行宽
    int kRows; //列高
    int anchorX; //结构原点位置水平坐标
    int anchorY; //结构原点位置垂直坐标
	Mat values;
	Kernel(int cols, int rows, int ancx, int ancy, Mat val)
		:kCols(cols), kRows(rows), anchorX(ancx), anchorY(ancy), values(val.clone())
	{
	}
};

double ZeroCrossPixel( const Mat src, const Kernel Mask, int sI, int sJ )
{
	float ZeroCross=0;
	for (int MaskI = 0; MaskI <= Mask.kRows-1; MaskI++)
	{
		for (int MaskJ = 0; MaskJ <= Mask.kCols-1; MaskJ++)
		{
			int sX=sI+(MaskI-Mask.anchorX);
			int sY=sJ+(MaskJ-Mask.anchorY);
			if (sX>=0 && sX<=src.rows-1 &&
				sY>=0 && sY<=src.cols-1)
			{
				ZeroCross=ZeroCross+(int)src.at<uchar>(sX,sY) * (float)Mask.values.at<float>(MaskI, MaskJ);
			}
			else
				return 0;
		}
	}
	return ZeroCross;
}

void Laplace1(const Mat src, Mat res, int threshold)
{
	//kernal
	float m[]={0,1,0,
			   1,-4,1,
			   0,1,0 };
	Mat M=Mat(3,3,CV_32F,m).clone();
	Kernel Mask(3, 3, 1, 1, M);

	float ZeroCross;
	for (int sI = 0; sI <= src.rows-1; sI++)
	{
		for (int sJ = 0; sJ <= src.cols-1; sJ++)
		{
			ZeroCross=ZeroCrossPixel(src, Mask, sI, sJ);
			if(ZeroCross>=threshold)
				res.at<uchar>(sI,sJ)=0;
			else
				res.at<uchar>(sI,sJ)=255;
		}
	}
}

void Laplace2(const Mat src, Mat res, int threshold)
{
	//kernal
	float m[]={1,1,1,
			   1,-8,1,
			   1,1,1 };
	Mat M=Mat(3,3,CV_32F,m).clone();
	Kernel Mask(3, 3, 1, 1, M);

	float ZeroCross;
	for (int sI = 0; sI <= src.rows-1; sI++)
	{
		for (int sJ = 0; sJ <= src.cols-1; sJ++)
		{
			ZeroCross=ZeroCrossPixel(src, Mask, sI, sJ);
			if( (ZeroCross/3.0) >= threshold )
				res.at<uchar>(sI,sJ)=0;
			else
				res.at<uchar>(sI,sJ)=255;
		}
	}
}

void MvLaplace(const Mat src, Mat res, int threshold)
{
	//kernal
	float m[]={2,-1,2,
			   -1,-4,-1,
			   2,-1,2 };
	Mat M=Mat(3,3,CV_32F,m).clone();
	Kernel Mask(3, 3, 1, 1, M);

	float ZeroCross;
	for (int sI = 0; sI <= src.rows-1; sI++)
	{
		for (int sJ = 0; sJ <= src.cols-1; sJ++)
		{
			ZeroCross=ZeroCrossPixel(src, Mask, sI, sJ);
			if( (ZeroCross/3.0) >= threshold )
				res.at<uchar>(sI,sJ)=0;
			else
				res.at<uchar>(sI,sJ)=255;
		}
	}
}

void LaplaceGaussian(const Mat src, Mat res, int threshold)
{
	//kernal
	float m[]={0,  0,  0,  -1, -1, -2, -1, -1, 0,  0,  0,
			   0,  0,  -2, -4, -8, -9, -8, -4, -2, 0,  0,
			   0,  -2, -7, -15,-22,-23,-22,-15,-7, -2, 0,
			   -1, -4, -15,-24,-14,-1, -14,-24,-15,-4,-1,
			   -1, -8, -22,-14,52, 103,52, -14,-22,-8,-1,
			   -2, -9, -23,-1, 103,178,103,-1, -23,-9,-2,
			   -1, -8, -22,-14,52, 103,52, -14,-22,-8,-1,
			   -1, -4, -15,-24,-14,-1, -14,-24,-15,-4,-1,
			   0,  -2, -7, -15,-22,-23,-22,-15,-7, -2, 0,
			   0,  0,  -2, -4, -8, -9, -8, -4, -2, 0,  0,
			   0,  0,  0,  -1, -1, -2, -1, -1, 0,  0,  0 };
	Mat M=Mat(11,11,CV_32F,m).clone();
	Kernel Mask(11, 11, 5, 5, M);

	float ZeroCross;
	for (int sI = 0; sI <= src.rows-1; sI++)
	{
		for (int sJ = 0; sJ <= src.cols-1; sJ++)
		{
			ZeroCross=ZeroCrossPixel(src, Mask, sI, sJ);
			if( ZeroCross >= threshold )
				res.at<uchar>(sI,sJ)=0;
			else
				res.at<uchar>(sI,sJ)=255;
		}
	}
}

void DifferenceGaussian(const Mat src, Mat res, int threshold)
{
	//kernal
	float m[]={-1,  -3,  -4,  -6, -7,  -8,  -7,  -6,  -4,  -3, -1,
			   -3,  -5,  -8, -11, -13, -13, -13, -11,  -8, -5, -3,
			   -4,  -8, -12, -16, -17, -17, -17, -16, -12, -8, -4,
			   -6, -11, -16, -16,   0,  15,   0, -16, -16,-11, -6,
			   -7, -13, -17,  0,   85, 160,  85,   0, -17,-13, -7,
			   -8, -13, -17,  15, 160, 283, 160,  15, -17,-13, -8,
			   -7, -13, -17,  0,   85, 160,  85,   0, -17,-13, -7,
			   -6, -11, -16, -16,   0,  15,   0, -16, -16,-11, -6,
			   -4,  -8, -12, -16, -17, -17, -17, -16, -12, -8, -4,
			   -3,  -5,  -8, -11, -13, -13, -13, -11,  -8, -5, -3,
			   -1,  -3,  -4,  -6, -7,  -8,  -7,  -6,  -4,  -3, -1 };
	Mat M=Mat(11,11,CV_32F,m).clone();
	Kernel Mask(11, 11, 5, 5, M);

	float ZeroCross;
	for (int sI = 0; sI <= src.rows-1; sI++)
	{
		for (int sJ = 0; sJ <= src.cols-1; sJ++)
		{
			ZeroCross=ZeroCrossPixel(src, Mask, sI, sJ);
			if( -ZeroCross >= threshold )
				res.at<uchar>(sI,sJ)=0;
			else
				res.at<uchar>(sI,sJ)=255;
		}
	}
}

int main()
{
	string imgname = "D:\\course\\Computer Vision\\HW\\hw9\\lena.bmp"; 	
	Mat img = imread(imgname,0); 
	if(img.empty())
	{
		return -1;
	}
	namedWindow("img",1); 
	imshow("img", img);
	waitKey();

	//Laplace1
	Mat imgLaplace1;
	img.copyTo(imgLaplace1);
	Laplace1(img,imgLaplace1,15);
	namedWindow("imgLaplace1",1);
	imshow("imgLaplace1", imgLaplace1);
	imwrite("D:\\course\\Computer Vision\\HW\\hw10\\imgLaplace1.bmp",imgLaplace1);
	waitKey();

	//Laplace2
	Mat imgLaplace2;
	img.copyTo(imgLaplace2);
	Laplace2(img,imgLaplace2,15);
	namedWindow("imgLaplace2",1);
	imshow("imgLaplace2", imgLaplace2);
	imwrite("D:\\course\\Computer Vision\\HW\\hw10\\imgLaplace2.bmp",imgLaplace2);
	waitKey();

	//Minimum variance Laplacian
	Mat imgMvLaplace;
	img.copyTo(imgMvLaplace);
	MvLaplace(img,imgMvLaplace,20);
	namedWindow("imgMvLaplace",1);
	imshow("imgMvLaplace", imgMvLaplace);
	imwrite("D:\\course\\Computer Vision\\HW\\hw10\\imgMvLaplace.bmp",imgMvLaplace);
	waitKey();

	//Laplace of Gaussian
	Mat imgLaplaceGaussian;
	img.copyTo(imgLaplaceGaussian);
	LaplaceGaussian(img,imgLaplaceGaussian,3000);
	namedWindow("imgLaplaceGaussian",1);
	imshow("imgLaplaceGaussian", imgLaplaceGaussian);
	imwrite("D:\\course\\Computer Vision\\HW\\hw10\\imgLaplaceGaussian.bmp",imgLaplaceGaussian);
	waitKey();

	//Difference of Gaussian
	Mat imgDifferenceGaussian;
	img.copyTo(imgDifferenceGaussian);
	DifferenceGaussian(img,imgDifferenceGaussian,1);
	namedWindow("imgDifferenceGaussian",1);
	imshow("imgDifferenceGaussian", imgDifferenceGaussian);
	imwrite("D:\\course\\Computer Vision\\HW\\hw10\\imgDifferenceGaussian.bmp",imgDifferenceGaussian);
	waitKey();

	return 0;
}
