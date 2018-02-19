//cv hw6

#include <opencv2\opencv.hpp>
#include <string>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

#define q 1
#define r 5
#define s 0

int h(int b, int c, int d, int e)
{
	if( b==c && (d!=b || e!=b) )
		return q;
	else if( b==c && (d==b && c==b) )
		return r;
	else if(b!=c)
		return s;
	else
		return -1;
}

int f(int a1, int a2, int a3, int a4)
{
	if(a1==r && a2==r && a3==r && a4==r)
		return 5;
	else
	{
		int n=0;
		if(a1==q)
			n++;
		if(a2==q)
			n++;
		if(a3==q)
			n++;
		if(a4==q)
			n++;
		return n;
	}
}

int main()
{
	string imgname = "D:\\course\\Computer Vision\\HW\\hw6\\lena.bmp";
 
	Mat img = imread(imgname,0);
	
	if(img.empty())
	{
		return -1;
	}

	Mat imgBinary;
	img.copyTo(imgBinary);
	for(int i=0;i<=imgBinary.rows-1;i++)
	{
		for(int j=0;j<=imgBinary.cols-1;j++)
		{
			if(imgBinary.at<uchar>(i,j)<=127)
				imgBinary.at<uchar>(i,j)=0;
			else
				imgBinary.at<uchar>(i,j)=255;
		}
	}

	//downsample
	Mat imgDownSample(66,66,CV_8UC1,Scalar(0));  //the boundary is zero
	for(int i=1; i<=imgDownSample.rows-2; i++)
	{
		for(int j=1; j<=imgDownSample.cols-2; j++)
		{
			imgDownSample.at<uchar>(i,j)=imgBinary.at<uchar>(8*(i-1),8*(j-1));
		}
	}

	//Mat YokoiNumber(64,64,CV_8UC1,Scalar(6));
	char filename[] = "YokoiNumber.txt";
	ofstream fout(filename);

	int yokoi[64][64]={6};
	int rows=64;
	int cols=64;
	for(int i=0; i<=rows-1; i++)
	{
		for(int j=0; j<=cols-1; j++)
		{
			if(imgDownSample.at<uchar>(i+1,j+1)==255)
			{
				int x[9];
				x[0]=imgDownSample.at<uchar>(i+1,j+1);
				x[1]=imgDownSample.at<uchar>(i+1,j+2);
				x[2]=imgDownSample.at<uchar>(i,j+1);
				x[3]=imgDownSample.at<uchar>(i+1,j);
				x[4]=imgDownSample.at<uchar>(i+2,j+1);
				x[5]=imgDownSample.at<uchar>(i+2,j+2);
				x[6]=imgDownSample.at<uchar>(i,j+2);
				x[7]=imgDownSample.at<uchar>(i,j);
				x[8]=imgDownSample.at<uchar>(i+2,j);
				int a1=h(x[0],x[1],x[6],x[2]);
				int a2=h(x[0],x[2],x[7],x[3]);
				int a3=h(x[0],x[3],x[8],x[4]);
				int a4=h(x[0],x[4],x[5],x[1]);
				yokoi[i][j]=f(a1,a2,a3,a4);
				//printf("%d ",yokoi[i][j]);
				fout << yokoi[i][j];
			}
			else
			{
				fout << " ";
			}
		}
		fout << "\n";
	}

	fout.close();

	namedWindow("img",1); 
	namedWindow("imgBinary",1);
	namedWindow("imgDownSample",1);

	imshow("img", img);
	imshow("imgBinary", imgBinary); 
	imshow("imgDownSample", imgDownSample); 

	imwrite("D:\\course\\Computer Vision\\HW\\hw6\\imgBinary.bmp",imgBinary);
	imwrite("D:\\course\\Computer Vision\\HW\\hw6\\imgDownSample.bmp",imgDownSample);

	waitKey();
 
	return 0;
}