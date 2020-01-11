#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "Q_1_10.h"

using namespace cv;

void A7(void)
{
	printf_s("��ͼƬ���չ̶���С����ָ�����ڵ�����ֵȡ�������������ص�ƽ��ֵ");
	//ע��������Ҫʹ��ż���ߴ��ͼƬ��128x128�ػ���Χ�Ͳ���Ӧ�����㱶����ϵ
	Mat imgSrc = imread("C:\\Users\\Administrator\\Desktop\\img3.jpg");
	int imgHeight = imgSrc.rows;
	int imgWidth = imgSrc.cols;
	int channel = imgSrc.channels();
	int r = 8;//�ػ���Χ
	int step = 8;//����
	Mat imgOut = Mat::zeros(imgHeight, imgWidth, CV_8UC3);

	for (int y = 0; y < imgHeight; y += step)
	{
		for (int x = 0; x < imgWidth; x += step)
		{
			for (int c = 0; c < channel; c++)
			{
				//ͳ����r��Χ�ڵ���������֮��
				int val = 0;
				for (int dy = 0; dy < r; dy++)
				{
					for (int dx = 0; dx < r; ++dx)
					{
						val += (double)imgSrc.at<Vec3b>(y + dy, x + dx)[c];
					}
				}
				//����ƽ��ֵ
				val = val / (r * r);
				printf_s("%d", val);
				//��r��Χ����ͳһ��ֵ
				for (int dy = 0; dy < r; dy++)
				{
					for (int dx = 0; dx < r; ++dx)
					{
						imgOut.at<Vec3b>(y + dy, x + dx)[c] = (uchar)val;
					}
				}
			}
		}
	}

	imshow("src", imgSrc);
	imshow("out",imgOut);

	waitKey(0);
	destroyAllWindows();

}

