#include <opencv2/opencv.hpp>
#include <string>
using namespace std;
using namespace cv;


int main()
{
	string imgname = "C:\\cv\\hw1\\lena.bmp";

	Mat img = imread(imgname, 0); //讀圖
	Mat imgUpDown, imgRightLeft, imgDiaMir1, imgDiaMir2;

	if (img.empty()) //如果讀圖失敗
	{
		return -1;
	}

	int i, j;
	img.copyTo(imgUpDown);
	for (i = 0; i <= (imgUpDown.rows / 2 - 1); i++)
	{
		for (j = 0; j <= imgUpDown.cols - 1; j++)
		{
			swap(imgUpDown.at<char>(i, j),
				imgUpDown.at<char>(imgUpDown.rows - 1 - i, j));
		}
	}

	img.copyTo(imgRightLeft);
	for (i = 0; i <= imgRightLeft.rows - 1; i++)
	{
		for (j = 0; j <= imgRightLeft.cols / 2 - 1; j++)
		{
			swap(imgRightLeft.at<char>(i, j),
				imgRightLeft.at<char>(i, imgRightLeft.cols - 1 - j));
		}
	}

	img.copyTo(imgDiaMir1);
	for (i = 0; i <= imgDiaMir1.rows - 1; i++)
	{
		for (j = i; j <= imgDiaMir1.cols - 1; j++)
		{
			swap(imgDiaMir1.at<char>(i, j),
				imgDiaMir1.at<char>(j, i));
		}
	}

	img.copyTo(imgDiaMir2);
	for (i = 0; i <= imgDiaMir2.rows - 1; i++)
	{
		for (j = 0; j <= imgDiaMir2.cols - 1 - i; j++)
		{
			swap(imgDiaMir2.at<char>(i, j),
				imgDiaMir2.at<char>(imgDiaMir2.cols - 1 - j, imgDiaMir2.rows - 1 - i));
		}
	}

	namedWindow("img", 1); //創建窗口
	namedWindow("result1", 1);
	namedWindow("result2", 1);

	imshow("img", img); //顯示圖
	imshow("result1", imgDiaMir1);
	imshow("result2", imgDiaMir2);

	imwrite("C:\\cv\\hw1\\imgUpDown.bmp", imgUpDown);
	imwrite("C:\\cv\\hw1\\imgRightLeft.bmp", imgRightLeft);
	imwrite("C:\\cv\\hw1\\imgDiaMir1.bmp", imgDiaMir1);
	imwrite("C:\\cv\\hw1\\imgDiaMir2.bmp", imgDiaMir2);

	waitKey();

	return 0;
}