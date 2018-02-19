//cv hw8

#include <string>
#include <time.h>
#include <math.h>
#include <opencv2\opencv.hpp>
#include <iostream>
#include <algorithm>

using namespace cv;
using namespace std;

double Normal( void )
{
	int a;
	double b = 0.;
	for( a = 0; a < 12; a++ )
	b += ( (double) rand() / RAND_MAX );
	return b - 6.;
}

double Uniform( void )
{
	double b = 0.;
	b=( (double) rand() / RAND_MAX );
	return b;
}

double VS(const Mat src)
{
	int N=src.rows*src.cols;
	double u=0;
	for(int i=0; i<=src.rows-1; i++)
	{
		for(int j=0; j<=src.cols-1; j++)
		{
			u=u+src.at<uchar>(i,j);
		}
	}
	u=u/N;
	double vS=0;
	for(int i=0; i<=src.rows-1; i++)
	{
		for(int j=0; j<=src.cols-1; j++)
		{
			vS=vS+pow( (src.at<uchar>(i,j)-u), 2 );
		}
	}
	vS=vS/N;
	return vS;
}

double VN(const Mat src, const Mat noise)
{
	int N=src.rows*src.cols;
	double uN=0;
	for(int i=0; i<=src.rows-1; i++)
	{
		for(int j=0; j<=src.cols-1; j++)
		{
			uN=uN+( noise.at<uchar>(i,j) -src.at<uchar>(i,j) );
		}
	}
	uN=uN/N;
	double vN=0;
	for(int i=0; i<=src.rows-1; i++)
	{
		for(int j=0; j<=src.cols-1; j++)
		{
			vN=vN+pow( (noise.at<uchar>(i,j) -src.at<uchar>(i,j) -uN), 2 );
		}
	}
	vN=vN/N;
	return vN;
}

double SNR(const Mat src, const Mat noise)
{
	double vS;
	vS=VS(src);
	double vN;
	vN=VN(src, noise);
	double snr;
	snr=20*log10(sqrt(vS)/sqrt(vN));
	return snr;
}

void GaussianNoise(Mat noise, double amplitude)
{
	for(int i=0; i<=noise.rows-1; i++)
	{
		for(int j=0; j<=noise.cols-1; j++)
		{		
			int Inim=noise.at<uchar>(i,j)+(int)( amplitude*Normal() );	
			if(Inim<=0)
				noise.at<uchar>(i,j)=0;
			else if(Inim>=255)
				noise.at<uchar>(i,j)=255;
			else
				noise.at<uchar>(i,j)=Inim;
		}
	}
}

void SaltPepperNoise(Mat noise, double threshold)
{
	for(int i=0; i<=noise.rows-1; i++)
	{
		for(int j=0; j<=noise.cols-1; j++)
		{		
			double u=Uniform();
			if(u<threshold)
				noise.at<uchar>(i,j)=0;
			else if(u>1-threshold)
				noise.at<uchar>(i,j)=255;
			else
				noise.at<uchar>(i,j)=noise.at<uchar>(i,j);
		}
	}
}

void BoxFilter(const Mat src, Mat res, int size)
{
	int sum=0;
	int n;
	for(int i=0; i<=res.rows-1; i++)
	{
		for(int j=0; j<=res.cols-1; j++)
		{		
			sum=0;
			n=0;
			for(int x=i-size/2; x<=i+size/2; x++)
			{
				if(x>=0 && x<=res.rows-1)
				{
					for(int y=j-size/2; y<=j+size/2; y++)
					{
						if(y>=0 && y<=res.cols-1)
						{
							sum=sum+src.at<uchar>(x,y);
							n=n+1;
						}
						else
							continue;
					}
				}
				else
					continue;
			}
			res.at<uchar>(i,j)=(int)(sum/n);
		}
	}
}

void MedianFilter(const Mat src, Mat res, int size)
{
	for(int i=0; i<=res.rows-1; i++)
	{
		for(int j=0; j<=res.cols-1; j++)
		{		
			vector<int> pixelValue;			
			for(int x=i-size/2; x<=i+size/2; x++)
			{
				if(x>=0 && x<=res.rows-1)
				{
					for(int y=j-size/2; y<=j+size/2; y++)
					{
						if(y>=0 && y<=res.cols-1)
						{
							pixelValue.push_back( src.at<uchar>(x,y) );	
						}
						else
							continue;
					}
				}
				else
					continue;
			}
			sort(pixelValue.begin(),pixelValue.end());
			//qsort(pixelValue,n,sizeof(pixelValue[0]),cmp);
			int N=pixelValue.size();
			res.at<uchar>(i,j)=pixelValue[(N+1)/2-1];
		}
	}
}

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
	srand((unsigned)time(NULL));

	string imgname = "D:\\course\\Computer Vision\\HW\\hw8\\lena.bmp"; 	
	Mat img = imread(imgname,0); 
	if(img.empty())
	{
		return -1;
	}
	namedWindow("img",1); 
	imshow("img", img);
	waitKey();

	Mat imgGaussian10;
	img.copyTo(imgGaussian10);
	GaussianNoise(imgGaussian10,10);
	namedWindow("imgGaussian10",1);
	imshow("imgGaussian10", imgGaussian10);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgGaussian10.bmp",imgGaussian10);
	double snrGaussian10;
	snrGaussian10=SNR(img, imgGaussian10);
	cout << snrGaussian10 <<endl;
	waitKey();

	Mat imgGaussian30;
	img.copyTo(imgGaussian30);
	GaussianNoise(imgGaussian30,30);
	namedWindow("imgGaussian30",1);
	imshow("imgGaussian30", imgGaussian30);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgGaussian30.bmp",imgGaussian30);
	double snrGaussian30;
	snrGaussian30=SNR(img, imgGaussian30);
	cout << snrGaussian30 <<endl;
	waitKey();

	Mat imgSaltPepper005;
	img.copyTo(imgSaltPepper005);
	SaltPepperNoise(imgSaltPepper005,0.05);
	namedWindow("imgSaltPepper005",1);
	imshow("imgSaltPepper005", imgSaltPepper005);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgSaltPepper005.bmp",imgSaltPepper005);
	double snrSaltPepper005;
	snrSaltPepper005=SNR(img, imgSaltPepper005);
	cout << snrSaltPepper005 <<endl;
	waitKey();

	Mat imgSaltPepper01;
	img.copyTo(imgSaltPepper01);
	SaltPepperNoise(imgSaltPepper01,0.1);
	namedWindow("imgSaltPepper01",1);
	imshow("imgSaltPepper01", imgSaltPepper01);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgSaltPepper01.bmp",imgSaltPepper01);
	double snrSaltPepper01;
	snrSaltPepper01=SNR(img, imgSaltPepper01);
	cout << snrSaltPepper01 <<endl;
	waitKey();

	//box filter Gaussian
	Mat imgBox3Gaussian10;
	imgGaussian10.copyTo(imgBox3Gaussian10);
	BoxFilter(imgGaussian10,imgBox3Gaussian10,3);
	namedWindow("imgBox3Gaussian10",1);
	imshow("imgBox3Gaussian10", imgBox3Gaussian10);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgBox3Gaussian10.bmp",imgBox3Gaussian10);
	double snrBox3Gaussian10;
	snrBox3Gaussian10=SNR(img, imgBox3Gaussian10);
	cout << snrBox3Gaussian10 <<endl;
	waitKey();

	Mat imgBox3Gaussian30;
	imgGaussian30.copyTo(imgBox3Gaussian30);
	BoxFilter(imgGaussian30,imgBox3Gaussian30,3);
	namedWindow("imgBox3Gaussian30",1);
	imshow("imgBox3Gaussian30", imgBox3Gaussian30);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgBox3Gaussian30.bmp",imgBox3Gaussian30);
	double snrBox3Gaussian30;
	snrBox3Gaussian30=SNR(img, imgBox3Gaussian30);
	cout << snrBox3Gaussian30 <<endl;
	waitKey();

	Mat imgBox5Gaussian10;
	imgGaussian10.copyTo(imgBox5Gaussian10);
	BoxFilter(imgGaussian10,imgBox5Gaussian10,5);
	namedWindow("imgBox5Gaussian10",1);
	imshow("imgBox5Gaussian10", imgBox5Gaussian10);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgBox5Gaussian10.bmp",imgBox5Gaussian10);
	double snrBox5Gaussian10;
	snrBox5Gaussian10=SNR(img, imgBox5Gaussian10);
	cout << snrBox5Gaussian10 <<endl;
	waitKey();

	Mat imgBox5Gaussian30;
	imgGaussian30.copyTo(imgBox5Gaussian30);
	BoxFilter(imgGaussian30,imgBox5Gaussian30,5);
	namedWindow("imgBox5Gaussian30",1);
	imshow("imgBox5Gaussian30", imgBox5Gaussian30);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgBox5Gaussian30.bmp",imgBox5Gaussian30);
	double snrBox5Gaussian30;
	snrBox5Gaussian30=SNR(img, imgBox5Gaussian30);
	cout << snrBox5Gaussian30 <<endl;
	waitKey();

	//box filter SaltPepper
	Mat imgBox3SaltPepper005;
	imgSaltPepper005.copyTo(imgBox3SaltPepper005);
	BoxFilter(imgSaltPepper005,imgBox3SaltPepper005,3);
	namedWindow("imgBox3SaltPepper005",1);
	imshow("imgBox3SaltPepper005", imgBox3SaltPepper005);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgBox3SaltPepper005.bmp",imgBox3SaltPepper005);
	double snrBox3SaltPepper005;
	snrBox3SaltPepper005=SNR(img, imgBox3SaltPepper005);
	cout << snrBox3SaltPepper005 <<endl;
	waitKey();

	Mat imgBox3SaltPepper01;
	imgSaltPepper01.copyTo(imgBox3SaltPepper01);
	BoxFilter(imgSaltPepper01,imgBox3SaltPepper01,3);
	namedWindow("imgBox3SaltPepper01",1);
	imshow("imgBox3SaltPepper01", imgBox3SaltPepper01);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgBox3SaltPepper01.bmp",imgBox3SaltPepper01);
	double snrBox3SaltPepper01;
	snrBox3SaltPepper01=SNR(img, imgBox3SaltPepper01);
	cout << snrBox3SaltPepper01 <<endl;
	waitKey();

	Mat imgBox5SaltPepper005;
	imgSaltPepper005.copyTo(imgBox5SaltPepper005);
	BoxFilter(imgSaltPepper005,imgBox5SaltPepper005,5);
	namedWindow("imgBox5SaltPepper005",1);
	imshow("imgBox5SaltPepper005", imgBox5SaltPepper005);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgBox5SaltPepper005.bmp",imgBox5SaltPepper005);
	double snrBox5SaltPepper005;
	snrBox5SaltPepper005=SNR(img, imgBox5SaltPepper005);
	cout << snrBox5SaltPepper005 <<endl;
	waitKey();

	Mat imgBox5SaltPepper01;
	imgSaltPepper01.copyTo(imgBox5SaltPepper01);
	BoxFilter(imgSaltPepper01,imgBox5SaltPepper01,5);
	namedWindow("imgBox5SaltPepper01",1);
	imshow("imgBox5SaltPepper01", imgBox5SaltPepper01);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgBox5SaltPepper01.bmp",imgBox5SaltPepper01);
	double snrBox5SaltPepper01;
	snrBox5SaltPepper01=SNR(img, imgBox5SaltPepper01);
	cout << snrBox5SaltPepper01 <<endl;
	waitKey();

	//median filter Gaussian
	Mat imgMedian3Gaussian10;
	imgGaussian10.copyTo(imgMedian3Gaussian10);
	MedianFilter(imgGaussian10,imgMedian3Gaussian10,3);
	namedWindow("imgMedian3Gaussian10",1);
	imshow("imgMedian3Gaussian10", imgMedian3Gaussian10);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgMedian3Gaussian10.bmp",imgMedian3Gaussian10);
	double snrMedian3Gaussian10;
	snrMedian3Gaussian10=SNR(img, imgMedian3Gaussian10);
	cout << snrMedian3Gaussian10 <<endl;
	waitKey();

	Mat imgMedian3Gaussian30;
	imgGaussian30.copyTo(imgMedian3Gaussian30);
	MedianFilter(imgGaussian30,imgMedian3Gaussian30,3);
	namedWindow("imgMedian3Gaussian30",1);
	imshow("imgMedian3Gaussian30", imgMedian3Gaussian30);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgMedian3Gaussian30.bmp",imgMedian3Gaussian30);
	double snrMedian3Gaussian30;
	snrMedian3Gaussian30=SNR(img, imgMedian3Gaussian30);
	cout << snrMedian3Gaussian30 <<endl;
	waitKey();

	Mat imgMedian5Gaussian10;
	imgGaussian10.copyTo(imgMedian5Gaussian10);
	MedianFilter(imgGaussian10,imgMedian5Gaussian10,5);
	namedWindow("imgMedian5Gaussian10",1);
	imshow("imgMedian5Gaussian10", imgMedian5Gaussian10);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgMedian5Gaussian10.bmp",imgMedian5Gaussian10);
	double snrMedian5Gaussian10;
	snrMedian5Gaussian10=SNR(img, imgMedian5Gaussian10);
	cout << snrMedian5Gaussian10 <<endl;
	waitKey();

	Mat imgMedian5Gaussian30;
	imgGaussian30.copyTo(imgMedian5Gaussian30);
	MedianFilter(imgGaussian30,imgMedian5Gaussian30,5);
	namedWindow("imgMedian5Gaussian30",1);
	imshow("imgMedian5Gaussian30", imgMedian5Gaussian30);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgMedian5Gaussian30.bmp",imgMedian5Gaussian30);
	double snrMedian5Gaussian30;
	snrMedian5Gaussian30=SNR(img, imgMedian5Gaussian30);
	cout << snrMedian5Gaussian30 <<endl;
	waitKey();

	//median filter SaltPepper
	Mat imgMedian3SaltPepper005;
	imgSaltPepper005.copyTo(imgMedian3SaltPepper005);
	MedianFilter(imgSaltPepper005,imgMedian3SaltPepper005,3);
	namedWindow("imgMedian3SaltPepper005",1);
	imshow("imgMedian3SaltPepper005", imgMedian3SaltPepper005);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgMedian3SaltPepper005.bmp",imgMedian3SaltPepper005);
	double snrMedian3SaltPepper005;
	snrMedian3SaltPepper005=SNR(img, imgMedian3SaltPepper005);
	cout << snrMedian3SaltPepper005 <<endl;
	waitKey();

	Mat imgMedian3SaltPepper01;
	imgSaltPepper01.copyTo(imgMedian3SaltPepper01);
	MedianFilter(imgSaltPepper01,imgMedian3SaltPepper01,3);
	namedWindow("imgMedian3SaltPepper01",1);
	imshow("imgMedian3SaltPepper01", imgMedian3SaltPepper01);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgMedian3SaltPepper01.bmp",imgMedian3SaltPepper01);
	double snrMedian3SaltPepper01;
	snrMedian3SaltPepper01=SNR(img, imgMedian3SaltPepper01);
	cout << snrMedian3SaltPepper01 <<endl;
	waitKey();

	Mat imgMedian5SaltPepper005;
	imgSaltPepper005.copyTo(imgMedian5SaltPepper005);
	MedianFilter(imgSaltPepper005,imgMedian5SaltPepper005,5);
	namedWindow("imgMedian5SaltPepper005",1);
	imshow("imgMedian5SaltPepper005", imgMedian5SaltPepper005);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgMedian5SaltPepper005.bmp",imgMedian5SaltPepper005);
	double snrMedian5SaltPepper005;
	snrMedian5SaltPepper005=SNR(img, imgMedian5SaltPepper005);
	cout << snrMedian5SaltPepper005 <<endl;
	waitKey();

	Mat imgMedian5SaltPepper01;
	imgSaltPepper01.copyTo(imgMedian5SaltPepper01);
	MedianFilter(imgSaltPepper01,imgMedian5SaltPepper01,5);
	namedWindow("imgMedian5SaltPepper01",1);
	imshow("imgMedian5SaltPepper01", imgMedian5SaltPepper01);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgMedian5SaltPepper01.bmp",imgMedian5SaltPepper01);
	double snrMedian5SaltPepper01;
	snrMedian5SaltPepper01=SNR(img, imgMedian5SaltPepper01);
	cout << snrMedian5SaltPepper01 <<endl;
	waitKey();

	//kernal
	uchar kValArr[]={0,  255,255,255,0,
					 255,255,255,255,255,
					 255,255,255,255,255,
					 255,255,255,255,255,
					 0,  255,255,255,0  };
	Mat kVal=Mat(5,5,CV_8U,kValArr).clone();
	Kernel ker(5, 5, 2, 2, kVal);

	//open close Gaussian	
	Mat imgErosionGaussian10(imgGaussian10.cols, imgGaussian10.rows, CV_8U, Scalar(0) );
	grayErosion( imgGaussian10, imgErosionGaussian10, ker );
	Mat imgOpenGaussian10(imgGaussian10.cols, imgGaussian10.rows, CV_8U, Scalar(0) );
	grayDilation( imgErosionGaussian10, imgOpenGaussian10, ker );
	Mat imgOpenDilationGaussian10(imgGaussian10.cols, imgGaussian10.rows, CV_8U, Scalar(0) );
	grayDilation( imgOpenGaussian10, imgOpenDilationGaussian10, ker );
	Mat imgOpenCloseGaussian10(imgGaussian10.cols, imgGaussian10.rows, CV_8U, Scalar(0) );
	grayErosion( imgOpenDilationGaussian10, imgOpenCloseGaussian10, ker );
	namedWindow("imgOpenCloseGaussian10",1);
	imshow("imgOpenCloseGaussian10", imgOpenCloseGaussian10);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgOpenCloseGaussian10.bmp",imgOpenCloseGaussian10);
	double snrOpenCloseGaussian10;
	snrOpenCloseGaussian10=SNR(img, imgOpenCloseGaussian10);
	cout << snrOpenCloseGaussian10 <<endl;
	waitKey();

	Mat imgErosionGaussian30(imgGaussian30.cols, imgGaussian30.rows, CV_8U, Scalar(0) );
	grayErosion( imgGaussian30, imgErosionGaussian30, ker );
	Mat imgOpenGaussian30(imgGaussian30.cols, imgGaussian30.rows, CV_8U, Scalar(0) );
	grayDilation( imgErosionGaussian30, imgOpenGaussian30, ker );
	Mat imgOpenDilationGaussian30(imgGaussian30.cols, imgGaussian30.rows, CV_8U, Scalar(0) );
	grayDilation( imgOpenGaussian30, imgOpenDilationGaussian30, ker );
	Mat imgOpenCloseGaussian30(imgGaussian30.cols, imgGaussian30.rows, CV_8U, Scalar(0) );
	grayErosion( imgOpenDilationGaussian30, imgOpenCloseGaussian30, ker );
	namedWindow("imgOpenCloseGaussian30",1);
	imshow("imgOpenCloseGaussian30", imgOpenCloseGaussian30);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgOpenCloseGaussian30.bmp",imgOpenCloseGaussian30);
	double snrOpenCloseGaussian30;
	snrOpenCloseGaussian30=SNR(img, imgOpenCloseGaussian30);
	cout << snrOpenCloseGaussian30 <<endl;
	waitKey();

	//open close SaltPepper
	Mat imgErosionSaltPepper005(imgSaltPepper005.cols, imgSaltPepper005.rows, CV_8U, Scalar(0) );
	grayErosion( imgSaltPepper005, imgErosionSaltPepper005, ker );
	Mat imgOpenSaltPepper005(imgSaltPepper005.cols, imgSaltPepper005.rows, CV_8U, Scalar(0) );
	grayDilation( imgErosionSaltPepper005, imgOpenSaltPepper005, ker );
	Mat imgOpenDilationSaltPepper005(imgSaltPepper005.cols, imgSaltPepper005.rows, CV_8U, Scalar(0) );
	grayDilation( imgOpenSaltPepper005, imgOpenDilationSaltPepper005, ker );
	Mat imgOpenCloseSaltPepper005(imgSaltPepper005.cols, imgSaltPepper005.rows, CV_8U, Scalar(0) );
	grayErosion( imgOpenDilationSaltPepper005, imgOpenCloseSaltPepper005, ker );
	namedWindow("imgOpenCloseSaltPepper005",1);
	imshow("imgOpenCloseSaltPepper005", imgOpenCloseSaltPepper005);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgOpenCloseSaltPepper005.bmp",imgOpenCloseSaltPepper005);
	double snrOpenCloseSaltPepper005;
	snrOpenCloseSaltPepper005=SNR(img, imgOpenCloseSaltPepper005);
	cout << snrOpenCloseSaltPepper005 <<endl;
	waitKey();

	Mat imgErosionSaltPepper01(imgSaltPepper01.cols, imgSaltPepper01.rows, CV_8U, Scalar(0) );
	grayErosion( imgSaltPepper01, imgErosionSaltPepper01, ker );
	Mat imgOpenSaltPepper01(imgSaltPepper01.cols, imgSaltPepper01.rows, CV_8U, Scalar(0) );
	grayDilation( imgErosionSaltPepper01, imgOpenSaltPepper01, ker );
	Mat imgOpenDilationSaltPepper01(imgSaltPepper01.cols, imgSaltPepper01.rows, CV_8U, Scalar(0) );
	grayDilation( imgOpenSaltPepper01, imgOpenDilationSaltPepper01, ker );
	Mat imgOpenCloseSaltPepper01(imgSaltPepper01.cols, imgSaltPepper01.rows, CV_8U, Scalar(0) );
	grayErosion( imgOpenDilationSaltPepper01, imgOpenCloseSaltPepper01, ker );
	namedWindow("imgOpenCloseSaltPepper01",1);
	imshow("imgOpenCloseSaltPepper01", imgOpenCloseSaltPepper01);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgOpenCloseSaltPepper01.bmp",imgOpenCloseSaltPepper01);
	double snrOpenCloseSaltPepper01;
	snrOpenCloseSaltPepper01=SNR(img, imgOpenCloseSaltPepper01);
	cout << snrOpenCloseSaltPepper01 <<endl;
	waitKey();

	//close open Gaussian
	Mat imgDilationGaussian10(imgGaussian10.cols, imgGaussian10.rows, CV_8U, Scalar(0) );
	grayDilation( imgGaussian10, imgDilationGaussian10, ker );
	Mat imgCloseGaussian10(imgGaussian10.cols, imgGaussian10.rows, CV_8U, Scalar(0) );
	grayErosion( imgDilationGaussian10, imgCloseGaussian10, ker );
	Mat imgCloseErosionGaussian10(imgGaussian10.cols, imgGaussian10.rows, CV_8U, Scalar(0) );
	grayErosion( imgCloseGaussian10, imgCloseErosionGaussian10, ker );
	Mat imgCloseOpenGaussian10(imgGaussian10.cols, imgGaussian10.rows, CV_8U, Scalar(0) );
	grayDilation( imgCloseErosionGaussian10, imgCloseOpenGaussian10, ker );
	namedWindow("imgCloseOpenGaussian10",1);
	imshow("imgCloseOpenGaussian10", imgCloseOpenGaussian10);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgCloseOpenGaussian10.bmp",imgCloseOpenGaussian10);
	double snrCloseOpenGaussian10;
	snrCloseOpenGaussian10=SNR(img, imgCloseOpenGaussian10);
	cout << snrCloseOpenGaussian10 <<endl;
	waitKey();

	Mat imgDilationGaussian30(imgGaussian30.cols, imgGaussian30.rows, CV_8U, Scalar(0) );
	grayDilation( imgGaussian30, imgDilationGaussian30, ker );
	Mat imgCloseGaussian30(imgGaussian30.cols, imgGaussian30.rows, CV_8U, Scalar(0) );
	grayErosion( imgDilationGaussian30, imgCloseGaussian30, ker );
	Mat imgCloseErosionGaussian30(imgGaussian30.cols, imgGaussian30.rows, CV_8U, Scalar(0) );
	grayErosion( imgCloseGaussian30, imgCloseErosionGaussian30, ker );
	Mat imgCloseOpenGaussian30(imgGaussian30.cols, imgGaussian30.rows, CV_8U, Scalar(0) );
	grayDilation( imgCloseErosionGaussian30, imgCloseOpenGaussian30, ker );
	namedWindow("imgCloseOpenGaussian30",1);
	imshow("imgCloseOpenGaussian30", imgCloseOpenGaussian30);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgCloseOpenGaussian30.bmp",imgCloseOpenGaussian30);
	double snrCloseOpenGaussian30;
	snrCloseOpenGaussian30=SNR(img, imgCloseOpenGaussian30);
	cout << snrCloseOpenGaussian30 <<endl;
	waitKey();

	//close open SaltPepper
	Mat imgDilationSaltPepper005(imgSaltPepper005.cols, imgSaltPepper005.rows, CV_8U, Scalar(0) );
	grayDilation( imgSaltPepper005, imgDilationSaltPepper005, ker );
	Mat imgCloseSaltPepper005(imgSaltPepper005.cols, imgSaltPepper005.rows, CV_8U, Scalar(0) );
	grayErosion( imgDilationSaltPepper005, imgCloseSaltPepper005, ker );
	Mat imgCloseErosionSaltPepper005(imgSaltPepper005.cols, imgSaltPepper005.rows, CV_8U, Scalar(0) );
	grayErosion( imgCloseSaltPepper005, imgCloseErosionSaltPepper005, ker );
	Mat imgCloseOpenSaltPepper005(imgSaltPepper005.cols, imgSaltPepper005.rows, CV_8U, Scalar(0) );
	grayDilation( imgCloseErosionSaltPepper005, imgCloseOpenSaltPepper005, ker );
	namedWindow("imgCloseOpenSaltPepper005",1);
	imshow("imgCloseOpenSaltPepper005", imgCloseOpenSaltPepper005);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgCloseOpenSaltPepper005.bmp",imgCloseOpenSaltPepper005);
	double snrCloseOpenSaltPepper005;
	snrCloseOpenSaltPepper005=SNR(img, imgCloseOpenSaltPepper005);
	cout << snrCloseOpenSaltPepper005 <<endl;
	waitKey();

	Mat imgDilationSaltPepper01(imgSaltPepper01.cols, imgSaltPepper01.rows, CV_8U, Scalar(0) );
	grayDilation( imgSaltPepper01, imgDilationSaltPepper01, ker );
	Mat imgCloseSaltPepper01(imgSaltPepper01.cols, imgSaltPepper01.rows, CV_8U, Scalar(0) );
	grayErosion( imgDilationSaltPepper01, imgCloseSaltPepper01, ker );
	Mat imgCloseErosionSaltPepper01(imgSaltPepper01.cols, imgSaltPepper01.rows, CV_8U, Scalar(0) );
	grayErosion( imgCloseSaltPepper01, imgCloseErosionSaltPepper01, ker );
	Mat imgCloseOpenSaltPepper01(imgSaltPepper01.cols, imgSaltPepper01.rows, CV_8U, Scalar(0) );
	grayDilation( imgCloseErosionSaltPepper01, imgCloseOpenSaltPepper01, ker );
	namedWindow("imgCloseOpenSaltPepper01",1);
	imshow("imgCloseOpenSaltPepper01", imgCloseOpenSaltPepper01);
	imwrite("D:\\course\\Computer Vision\\HW\\hw8\\imgCloseOpenSaltPepper01.bmp",imgCloseOpenSaltPepper01);
	double snrCloseOpenSaltPepper01;
	snrCloseOpenSaltPepper01=SNR(img, imgCloseOpenSaltPepper01);
	cout << snrCloseOpenSaltPepper01 <<endl;
	waitKey();

	return 0;
}