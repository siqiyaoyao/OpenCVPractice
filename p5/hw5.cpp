//cv hw5

//#include <string>
#include <iostream>
#include <opencv2\opencv.hpp>

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

void grayPixelDil( const Mat src, Mat dst, const Kernel ker, int sI, int sJ )
{
	uchar grayMax=src.at<uchar>(sI,sJ);
	for (int kI = 0; kI < ker.kRows; kI++)
	{
		for (int kJ = 0; kJ < ker.kCols; kJ++)
		{
			int sX=sI-(kI-ker.anchorX);
			int sY=sJ-(kJ-ker.anchorY);
			if (sX>=0 && sX<=src.rows-1 &&
				sY>=0 && sY<=src.cols-1)
			{
				if (ker.values.at<uchar>(kI,kJ)==255)
				{
					grayMax=(uchar)max( (int)grayMax, (int)src.at<uchar>(sX,sY) );
					dst.at<uchar>(sI,sJ)=grayMax;
				}
			}			
		}
	}
}

void grayDilation( const Mat src, Mat dst, const Kernel ker )
{
	if(ker.values.empty())
	{
		printf("error");
		return;
	}

	for (int sI = 0; sI < src.rows; sI++)
	{
		for (int sJ = 0; sJ < src.cols; sJ++)
		{
			//dilation
			grayPixelDil(src, dst, ker, sI, sJ);		
		}
	}
}

void grayPixelEro( const Mat src, Mat dst, const Kernel ker, int sI, int sJ )
{
	uchar grayMin=src.at<uchar>(sI,sJ);
	for (int kI = 0; kI < ker.kRows; kI++)
	{
		for (int kJ = 0; kJ < ker.kCols; kJ++)
		{
			int sX=sI+(kI-ker.anchorX);
			int sY=sJ+(kJ-ker.anchorY);
			if (sX>=0 && sX<=src.rows-1 &&
				sY>=0 && sY<=src.cols-1)
			{
				if (ker.values.at<uchar>(kI,kJ)==255)
				{
					grayMin=(uchar)min((int)grayMin,(int)src.at<uchar>(sX,sY));
					dst.at<uchar>(sI,sJ)=grayMin;
				}
			}			
		}
	}
}

void grayErosion( const Mat src, Mat dst, const Kernel ker )
{
	if(ker.values.empty())
	{
		printf("error");
		return;
	}

	for (int sI = 0; sI < src.rows; sI++)
	{
		for (int sJ = 0; sJ < src.cols; sJ++)
		{						
			//erosion
			grayPixelEro(src, dst, ker, sI, sJ);
		}
	}
}

int main()
{
	string imgname = "D:\\course\\CV\\HW\\hw5\\lena.bmp";
 
	Mat img = imread(imgname,0); 
	if(img.empty())
	{
		return -1;
	}

	uchar kValArr[]={0,  255,255,255,0,
					 255,255,255,255,255,
					 255,255,255,255,255,
					 255,255,255,255,255,
					 0,  255,255,255,0  };
	Mat kVal=Mat(5,5,CV_8U,kValArr).clone();
	Kernel ker(5, 5, 2, 2, kVal);

	Mat imgGrayDilation( img.cols, img.rows, CV_8U, Scalar(0) );
	grayDilation( img, imgGrayDilation, ker );
	
	Mat imgGrayErosion(img.cols, img.rows, CV_8U, Scalar(0) );
	grayErosion( img, imgGrayErosion, ker );
	
	Mat imgGrayOpening(img.cols, img.rows, CV_8U, Scalar(0) );
	//grayErosion( img, imgGrayErosion, ker );
	grayDilation( imgGrayErosion, imgGrayOpening, ker );

	Mat imgGrayClosing(img.cols, img.rows, CV_8U, Scalar(0) );
	//grayDilation( img, imgGrayDilation, ker );
	grayErosion( imgGrayDilation, imgGrayClosing, ker );

	namedWindow("img",1); //創建窗口
	namedWindow("dilation",1);
	namedWindow("erosion",1);
	namedWindow("opening",1);
	namedWindow("closing",1);

	imshow("img", img); //顯示圖
	imshow("dilation", imgGrayDilation);
	imshow("erosion", imgGrayErosion);
	imshow("opening", imgGrayOpening);
	imshow("closing", imgGrayClosing);

	imwrite("D:\\course\\CV\\HW\\hw5\\imgGrayDilation.bmp",imgGrayDilation);
	imwrite("D:\\course\\CV\\HW\\hw5\\imgGrayErosion.bmp",imgGrayErosion);
	imwrite("D:\\course\\CV\\HW\\hw5\\imgGrayOpening.bmp",imgGrayOpening);
	imwrite("D:\\course\\CV\\HW\\hw5\\imgGrayClosing.bmp",imgGrayClosing);

	waitKey();

	return 0;
}