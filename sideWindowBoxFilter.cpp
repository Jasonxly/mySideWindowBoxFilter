//针对灰度图的均值滤波+CVPR 2019的SideWindowFilter
//其他种类的滤波直接换核即可
#include<stdio.h>
#include<iostream>
#include <stdlib.h>
#include<vector>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;
int cnt[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
vector <int> filter[8];
vector <int> mask[8];


void InitFilter(int radius) {
	int n = radius * 2 + 1;
	for (int i = 0; i < 8; i++) {
		cnt[i] = 0;
		filter[i].clear();
		mask[i].clear();
	}
	int InitFilter[9] = { 1,2,1,2,4,2,1,2,1 };
	for (int i = 0; i < 8; i++) {
		for (int x = 0; x < n; x++) {
			for (int y = 0; y < n; y++) {
				if (i == 0 && x <= radius && y <= radius) {
					mask[i].push_back(1);
				}
				else if (i == 1 && x <= radius && y >= radius) {
					mask[i].push_back(1);
				}
				else if (i == 2 && x >= radius && y <= radius) {
					mask[i].push_back(1);
				}
				else if (i == 3 && x >= radius && y >= radius) {
					mask[i].push_back(1);
				}
				else if (i == 4 && x <= radius) {
					mask[i].push_back(1);
				}
				else if (i == 5 && x >= radius) {
					mask[i].push_back(1);
				}
				else if (i == 6 && y >= radius) {
					mask[i].push_back(1);
				}
				else if (i == 7 && y <= radius) {
					mask[i].push_back(1);
				}
				else {
					mask[i].push_back(0);
				}
			}
		}
	}
	for (int i = 0; i < 8; i++) {
		int sum = 0;
		for (int j = 0; j < mask[i].size(); j++) {
			//cout << InitFilter[2] << "\t" <<mask[i][2]<<"\n"<< endl;
			filter[i].push_back(InitFilter[j] * mask[i][j]) ;
			//cout << filter[i][j] << "\n" << endl;
			sum += filter[i][j] ;
		} 
		cnt[i] = sum;
	}
}

Mat SideWindowFilter(Mat src, int radius = 1) {
	int row = src.rows;
	int col = src.cols;
	int channels = src.channels();
	InitFilter(radius);
	for (int i = 0; i < 8; i++) {
		printf("%d ", cnt[i]);
	}
	printf("\n");
	if (channels == 1) {
		Mat dst(row, col, CV_8UC1);
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				if (i < radius || i + radius >= row || j < radius || j + radius >= col) {
					dst.at<uchar>(i, j) = src.at<uchar>(i, j);
					continue;
				}
				int minn = 256;
				int pos = 0;
				for (int k = 0; k < 8; k++) {
					int val = 0;
					int id = 0;
					for (int x = -radius; x <= radius; x++) {
						for (int y = -radius; y <= radius; y++) {
							//if (x == 0 && y == 0) continue;
							val += src.at<uchar>(i + x, j + y) * filter[k][id++];
						}
					}
					val /= cnt[k];
					if (abs(val - src.at<uchar>(i, j)) < minn) {
						minn = abs(val - src.at<uchar>(i, j));
						pos = k;
					}
				}
				int val = 0;
				int id = 0;
				for (int x = -radius; x <= radius; x++) {
					for (int y = -radius; y <= radius; y++) {
						//if (x == 0 && y == 0) continue;
						val += src.at<uchar>(i + x, j + y) * filter[pos][id++];
					}
				}
				dst.at<uchar>(i, j) = val / cnt[pos];
			}
		}
		return dst;
	}
	Mat dst(row, col, CV_8UC3);
	for (int c = 0; c < 3; c++) {
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				if (i < radius || i + radius >= row || j < radius || j + radius >= col) {
					dst.at<Vec3b>(i, j)[c] = src.at<Vec3b>(i, j)[c];
					continue;
				}
				int minn = 256;
				int pos = 0;
				for (int k = 0; k < 8; k++) {
					int val = 0;
					int id = 0;
					for (int x = -radius; x <= radius; x++) {
						for (int y = -radius; y <= radius; y++) {
							//if (x == 0 && y == 0) continue;
							val += src.at<Vec3b>(i + x, j + y)[c] * filter[k][id++];
						}
					}
					val /= cnt[k];
					if (abs(val - src.at<Vec3b>(i, j)[c]) < minn) {
						minn = abs(val - src.at<Vec3b>(i, j)[c]);
						pos = k;
					}
				}
				int val = 0;
				int id = 0;
				for (int x = -radius; x <= radius; x++) {
					for (int y = -radius; y <= radius; y++) {
						//if (x == 0 && y == 0) continue;
						val += src.at<Vec3b>(i + x, j + y)[c] * filter[pos][id++];
					}
				}
				dst.at<Vec3b>(i, j)[c] = val / cnt[pos];
			}
		}
	}
	return dst;
}

void main() {
	string imageBasePath = "E:\\vsprojects\\sideWindowBoxFilter\\sideWindowBoxFilter\\";
	Mat inputImage = imread(imageBasePath + "1" + ".tif", 1);
	Mat srcGray;
	Mat outputImage;
	CV_Assert(inputImage.channels() == 1 || inputImage.channels() == 3);
	if (inputImage.channels() == 3)
	{
		//printf("%d\n",inputImage.channels());
		cvtColor(inputImage,srcGray,CV_RGB2GRAY);
	}
	else if (inputImage.channels() == 1)
	{
		srcGray = inputImage;
	}
	outputImage = SideWindowFilter(srcGray, 1);
	imshow("1",srcGray);
	imshow("2", outputImage);
	imwrite("src.tiff",srcGray);
	imwrite("sideWindowFilterImage.tiff", outputImage);

	waitKey();
}