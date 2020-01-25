#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "A_51_60.h"
#include <math.h>
#include <time.h>
#include <vector>

using namespace cv;
void A58(Mat img)
{
	int imgHeight = img.rows;
	int imgWeight = img.cols;
	int channel = img.channels();
	int val = 0;
	//�Ҷȶ�ֵ��
	Mat imgGray = Mat::zeros(imgHeight, imgWeight, CV_8UC1);
	Mat imgBin = Mat::zeros(imgHeight, imgWeight, CV_8UC1);

	for (int y = 0; y < imgHeight; ++y)
	{
		for (int x = 0; x < imgWeight; ++x)
		{
			imgGray.at<uchar>(y, x) = 0.114 * (float)img.at<Vec3b>(y, x)[2]
				+ 0.587 * (float)img.at<Vec3b>(y, x)[1]
				+ 0.299 * (float)img.at<Vec3b>(y, x)[0];
		}
	}

	const int grayScale = 256;//�Ҷ�ֵ

	int pixelCount[grayScale] = { 0 };//�Ҷ�ֱ��ͼ
	float pixelPro[grayScale] = { 0 };//�����Ҷ�ֵռ����ı���

	double w0, w1;//����/ǰ������ռ��
	double u0, u1;//ǰ��/����ƽ���Ҷ�ֵ
	double p0, p1;
	double g = 0;//��䷽��

	double max_g = 0;//�����䷽��
	double good_k = 0;//������ֵ
	int pixelSum = imgHeight * imgWeight;//������ֵ

	//ͳ��ͼƬ�и����Ҷ�ֵ�ĸ���
	for (int y = 0; y < imgHeight; ++y)
	{
		for (int x = 0; x < imgWeight; ++x)
		{
			int val = imgGray.at<uchar>(y, x);
			pixelCount[val]++;
		}
	}

	//ͳ��ͼƬ�и����Ҷ�ֵ��ռ�ı���
	for (int i = 0; i < grayScale; ++i)
	{
		pixelPro[i] = 1.0 * pixelCount[i] / pixelSum;
	}

	//k:�ݶ���ֵ(0-255)
	for (int k = 0; k < grayScale; ++k)
	{
		w0 = w1 = u0 = u1 = g = 0;
		p0 = p1 = 0;
		//ǰ������������ [0-k][k+1-255]
		for (int i = 0; i < grayScale; ++i)
		{
			//�����ǰ����ֵС����ֵk�����ڱ�������֮����ǰ��
			if (i <= k)
			{
				//���㱳������ռ��
				w0 += pixelPro[i];
				//���㵱ǰ�Ҷ�ֵ�����ĸ���:�Ҷ�ֵ*�Ҷ�ֵ�����ĸ���
				p0 += (i * pixelPro[i]);

			}
			else
			{
				//���㱳������ռ��
				w1 += pixelPro[i];
				p1 += (i * pixelPro[i]);
			}
		}
		//����ƽ���Ҷ�ֵ��p0/w0
		u0 = p0 / w0;
		u1 = p1 / w1;
		//�������ڷ���
		g = (float)(w0 * w1 * pow((u0 - u1), 2));
		if (g > max_g)
		{
			max_g = g;
			good_k = k;

		}
	}

	printf_s("good k;%f\n", good_k);

	//ȡ����õ�kֵ����kֵ��Ϊ��ֵ���ж�ֵ��
	for (int y = 0; y < imgHeight; ++y)
	{
		for (int x = 0; x < imgWeight; ++x)
		{
			if (imgGray.at<uchar>(y, x) > good_k)
				imgBin.at<uchar>(y, x) = 255;
			else
				imgBin.at<uchar>(y, x) = 0;
		}
	}

	int label = 0;
	uchar up = 0, left = 0;

	int** labelSet;

	//���ٿռ�:��=y
	labelSet = new int* [imgHeight];

	//���ٿռ䣺��=x
	for (int i = 0; i < imgHeight; ++i)
		labelSet[i] = new int[imgWeight];

	for (int i = 0; i < imgHeight; i++)
		for (int j = 0; j < imgWeight; j++)
			labelSet[i][j] = -1;

	int _y = 0, _x = 0;

	for (int y = 49; y < imgHeight; ++y)
	{
		for (int x = 0; x < imgWeight; ++x)
		{
			Mat temp = img.clone();
			Point p(x, y);
			circle(temp, p, 0, Scalar(0, 0, 255));
			cv::imshow("temp", temp);
			cv::waitKey(5);

			val = (int)imgBin.at<uchar>(y, x);
			//����ǰ�ɫ��255
			if (val == 255)
			{
				printf_s("��\n");
				if (y >= 1 && x >= 1)
				{
					printf_s("1\n");
					up = (int)imgBin.at<uchar>(y - 1, x);
					left = (int)imgBin.at<uchar>(y, x - 1);
				}
				else if (x == 0 && y >= 1)
				{
					printf_s("2\n");
					up = (int)imgBin.at<uchar>(y - 1, x);
					left = 0;
				}
				else if (x >= 1 && y == 0)
				{
					printf_s("3\n");
					up = 0;
					left = (int)imgBin.at<uchar>(y, x - 1);
				}
				else if (x == 0 && y == 0)
				{
					printf_s("4\n");
					up = 0;
					left = 0;
				}
				printf_s("y:%d,x:%d up:%d,left:%d\n", y, x, up, left);
				//����������ֵΪ0�������lable
				if (up == 0 && left == 0)
				{
					label++;
					_x = 0;
					//labelSet[_y][0] = label;
					imgBin.at<uchar>(y, x) = label;
					_y++;
					printf_s("label:%d\n", label);
					Point p(x, y);
					circle(img, p, 0, Scalar(0, 0, 255));
					cv::imshow("temp", temp);
					cv::waitKey(5);
				}
				//�������һ����Ϊ0��ѡ����С��labelΪ������label
				else if (up > 0 || left > 0)
				{
					int min = MIN(up, left);
					imgBin.at<uchar>(y, x) = min;
					//labelSet[_y][++_x] = min;
				}
			}
			//����Ƿǰ�ɫ�������һ������
			else
			{
				printf_s("��\n");
				continue;
			}
		}
	}

	cv::imshow("imgBin", imgBin);
	cv::imshow("img", img);
	cv::waitKey(0);
	cv::destroyAllWindows();
}