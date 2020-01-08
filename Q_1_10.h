#pragma once

#ifndef  _Q_1_10_
#define _Q_1_10_

#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

class Q_1_10
{

private:

	int imgWidth = 0;
	int imgHeight = 0;
public:
	void A1(void)
	{
		//ͼ�ο��

		printf_s("//��ȡͼ��Ȼ��{RGB}ͨ���滻��{BGR}ͨ��\n");

		Mat imgSrc = imread("C:\\Users\\Administrator\\Desktop\\nanasai1.jpg");
		imgWidth = imgSrc.cols;
		imgHeight = imgSrc.rows;

		Mat imgOut = Mat::zeros(imgHeight, imgWidth, CV_8UC3);
		//����ͼƬ�������ң����ϵ���
		for (int y = 0; y < imgHeight; y++) 
		{
			for (int x = 0; x < imgWidth; x++)
			{
				//Mat.at:���ڻ�ȡͼ�����ĳ���ֵ��ı�ĳ���ֵ��[0][1][2]����ͨ��
				//typedef Vec<uchar,3> Vec3b;0-255
				// R -> B
				imgOut.at<Vec3b>(y, x)[0] = imgSrc.at<Vec3b>(y, x)[2];
				// G -> G
				imgOut.at<Vec3b>(y, x)[1] = imgSrc.at<Vec3b>(y, x)[1];
				// B -> R
				imgOut.at<Vec3b>(y, x)[2] = imgSrc.at<Vec3b>(y, x)[0];
			}
		}
		
		printf_s("imgWidth:%d\n", imgWidth);
		printf_s("imgHeight:%d\n", imgHeight);

		imshow("imgSrc(RGB)", imgSrc);
		imshow("imgOut(BGR)", imgOut);
		waitKey(0);
		destroyWindow("imgSrc");
	}

};

#endif // ! _Q_1_10_

