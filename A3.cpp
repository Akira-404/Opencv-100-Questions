#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "Q_1_10.h"

using namespace cv;

void A3(int th)
{
	printf_s("��ͼ����ж�ֵ��");
	Mat imgSrc = imread("C:\\Users\\Administrator\\Desktop\\img1.jpg");
	int imgHeight = imgSrc.rows;
	int imgWeight = imgSrc.cols;
	Mat imgOut = Mat::zeros(imgHeight, imgWeight, CV_8UC1);
	//�Ƚ��ԻҶȻ�
	for (int y = 0; y < imgHeight; ++y)
	{
		for (int x = 0; x < imgWeight; ++x)
		{
			imgOut.at<uchar>(y, x) = 0.114 * (float)imgSrc.at<Vec3b>(y, x)[2]
				+ 0.587 * (float)imgSrc.at<Vec3b>(y, x)[1]
				+ 0.299 * (float)imgSrc.at<Vec3b>(y, x)[0];
		}
	}
	for (int y = 0; y < imgHeight; ++y)
	{
		for (int x = 0; x < imgWeight; ++x)
		{
			if (imgOut.at<uchar>(y, x) > th)
			{
				imgOut.at<uchar>(y, x) = 255;//��ɫ
			}
			else
			{
				imgOut.at<uchar>(y, x) = 0;//��ɫ
			}

		}
	}

	imshow("imgSrc", imgSrc);
	imshow("imgOut", imgOut);
	waitKey(0);
	destroyAllWindows();
}