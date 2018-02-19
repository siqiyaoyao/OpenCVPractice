#include <string>
#include <opencv2\opencv.hpp>
#include <vector>
using namespace std;
using namespace cv;

//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
/*
int main()
{

	//从文件中读取图像
	Mat img;
	img = imread("C:\\123.png");
	imshow("123", img);
	waitKey();
	return 0;
}

*/

struct Node
{
	//struct Node *parent;
	int root;
	int nodeLabel;
	int nodeCount;
	int top, down, left, right;
};

int label[512][512];
int main()
{
	string imgname = "C:\\cv\\hw2\\lena.bmp";

	Mat img = imread(imgname, 0); //讀圖
	Mat imgBinary;
	int histogram[256];

	if (img.empty()) //如果讀圖失敗
	{
		return -1;
	}

	int i, j;

	img.copyTo(imgBinary);
	for (i = 0; i <= imgBinary.rows - 1; i++)
	{
		for (j = 0; j <= imgBinary.cols - 1; j++)
		{
			if (imgBinary.at<uchar>(i, j) <= 127)
				imgBinary.at<uchar>(i, j) = 0;
			else
				imgBinary.at<uchar>(i, j) = 255;
		}
	}

	//histogram初始化
	memset(histogram, 0, sizeof(histogram));
	//for (int i = 0; i <= 255; i++)
	//{
	//	histogram[i]=0;
	//}

	//統計histogram
	Mat_<uchar>::iterator iteK = img.begin<uchar>(),
		iteKEnd = img.end<uchar>();
	while (iteK != iteKEnd)
	{
		histogram[(*iteK)]++;
		iteK++;
	}
	//for(i=0;i<=img.rows-1;i++)
	//{
	//	for(j=0;j<=img.cols-1;j++)
	//	{
	//		histogram[ img.at<uchar>(i,j) ]++;
	//	}
	//}

	//畫histogram
	Mat imgHistogram(300, 300, CV_8UC1, Scalar(0));
	Point a, b;
	a.x = 20;  a.y = 280;
	b.x = 275; b.y = 280;
	line(imgHistogram, a, b, CV_RGB(255, 255, 255), 1, 8, 0);
	a.x = 20; a.y = 280;
	b.x = 20; b.y = 0;
	line(imgHistogram, a, b, CV_RGB(255, 255, 255), 1, 8, 0);
	for (int k = 0; k <= 255; k++)
	{
		a.x = 20 + k; a.y = 280;
		b.x = 20 + k; b.y = 280 - (histogram[k] / 10);
		line(imgHistogram, a, b, CV_RGB(255, 255, 255), 1, 8, 0);
	}


	//找connected-component
	Mat imgLabel;
	Mat imgConCom;

	imgBinary.copyTo(imgLabel);

	for (int i = 0; i < 512; i++)
	{
		for (int j = 0; j < 512; j++)
		{
			label[i][j] = 0;
		}
	}
	//memset(label,0,sizeof(label));

	vector<struct Node> linked;
	//struct Node *linked[];
	Node newNode;
	//newNode.parent=NULL;
	newNode.root = 0;
	newNode.nodeLabel = 0;
	newNode.nodeCount = 0;
	linked.push_back(newNode);

	int labelNumber = 1;

	//first pass
	for (int i = 0; i <= imgLabel.rows - 1; i++)
	{
		for (int j = 0; j <= imgLabel.cols - 1; j++)
		{
			if (imgLabel.at<uchar>(i, j) != 0)
			{
				if ((i == 0 && j == 0)
					|| (i == 0 && label[i][j - 1] == 0)
					|| (label[i - 1][j] == 0 && j == 0)
					|| (label[i - 1][j] == 0 && label[i][j - 1] == 0)) //如果上和左是空的
				{
					//把labelNumber加到linked[labelNumber]   
					//newNode.parent =NULL;
					newNode.root = 0;
					newNode.nodeLabel = labelNumber;
					newNode.nodeCount = 1;

					newNode.top = i;
					newNode.down = i;
					newNode.left = j;
					newNode.right = j;

					linked.push_back(newNode);
					//linked[labelNumber]=newNode;
					label[i][j] = labelNumber;
					labelNumber++;
				}
				else //鄰居已經有label
				{
					if (i == 0 && j != 0)
					{
						label[i][j] = label[i][j - 1];
						linked[label[i][j]].nodeCount++;
					}
					else if (i != 0 && j == 0)
					{
						label[i][j] = label[i - 1][j];
						linked[label[i][j]].nodeCount++;
					}
					else
					{
						if (label[i - 1][j] == 0)
						{
							label[i][j] = label[i][j - 1];
							linked[label[i][j]].nodeCount++;
						}
						else if (label[i][j - 1] == 0)
						{
							label[i][j] = label[i - 1][j];
							linked[label[i][j]].nodeCount++;
						}
						else
						{
							label[i][j] = label[i - 1][j];
							linked[label[i][j]].nodeCount++;

							//Node *p=&(linked[ label[i][j-1] ]);
							//Node *q=&(linked[ label[i-1][j] ]);
							int p = label[i][j - 1];
							int q = label[i - 1][j];

							while (linked[p].root>0)
							{
								p = linked[p].root;
							}
							while (linked[q].root>0)
							{
								q = linked[q].root;
							}

							if (linked[p].nodeLabel > linked[q].nodeLabel)
								linked[q].root = p;
							else if (linked[p].nodeLabel < linked[q].nodeLabel)
								linked[p].root = q;
							else
							{
								linked[p].root = 0;
								linked[q].root = 0;
							}
							//if(linked[3].parent<&linked[0] && linked[3].parent!=0 )
							//	printf("%x %x\n",linked[3].parent,p);
						}

						if (i<linked[label[i][j]].top)
							linked[label[i][j]].top = i;
						if (i>linked[label[i][j]].down)
							linked[label[i][j]].down = i;
						if (j<linked[label[i][j]].left)
							linked[label[i][j]].left = j;
						if (j>linked[label[i][j]].right)
							linked[label[i][j]].right = j;
					}
				}
			}
		}
	}

	//second pass

	size_t sizeLinked = linked.size();
	for (int k = 1; k < sizeLinked; k++)
	{
		if (linked[k].root != 0)
		{
			linked[linked[k].root].nodeCount = linked[linked[k].root].nodeCount + linked[k].nodeCount;

			if (linked[k].top<linked[linked[k].root].top)
				linked[linked[k].root].top = linked[k].top;
			if (linked[k].down>linked[linked[k].root].down)
				linked[linked[k].root].down = linked[k].down;
			if (linked[k].left<linked[linked[k].root].left)
				linked[linked[k].root].left = linked[k].left;
			if (linked[k].right>linked[linked[k].root].right)
				linked[linked[k].root].right = linked[k].right;
		}
		//if(linked[k].parent!=NULL)
		//	linked[k].parent->nodeCount=linked[k].parent->nodeCount + linked[k].nodeCount;
	}

	cvtColor(imgLabel, imgConCom, CV_GRAY2RGB);
	for (int k = 1; k < sizeLinked; k++)
	{
		if (linked[k].root == 0 && linked[k].nodeCount >= 500)
		{
			cout << linked[k].nodeCount << endl;
			Point a, b, c,left,right,up,down;
			a.x = linked[k].left;
			a.y = linked[k].top;
			b.x = linked[k].right;
			b.y = linked[k].down;
			c = a + b;
			c.x = c.x / 2;
			c.y = c.y / 2;
			left = c;
			right = c;
			left.x = c.x - 5;
			right.x = c.x + 5;
			up = c;
			down = c;
			up.y = c.y - 5;
			down.y = c.y + 5;
			//circle(imgConCom, c, 6, CV_RGB(0, 255, 0), 2);
			line(imgConCom, left, right, CV_RGB(0, 255, 0), 2);
			line(imgConCom, up, down, CV_RGB(0, 255, 0), 2);

			rectangle(imgConCom, a, b, CV_RGB(255, 120, 120), 2);
		}
		//if(linked[k].parent==NULL && linked[k].nodeCount>=500)
		//	cout << linked[k].nodeCount << endl;
	}

	namedWindow("img", 1); //創建窗口
	namedWindow("binary", 1);
	namedWindow("histogram", 1);
	namedWindow("conCom", 1);

	imshow("img", img); //顯示圖
	imshow("binary", imgBinary);
	imshow("histogram", imgHistogram);
	imshow("conCom", imgConCom);

	imwrite("C:\\cv\\hw2\\imgBinary.bmp", imgBinary);
	imwrite("C:\\cv\\hw2\\imgHistogram.bmp", imgHistogram);
	imwrite("C:\\cv\\hw2\\imgConCom.bmp", imgConCom);

	waitKey();

	return 0;
}