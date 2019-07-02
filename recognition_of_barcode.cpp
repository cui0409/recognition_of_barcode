#include "stdafx.h"
#include "recognition_of_barcode.h"

#pragma comment(lib,"E:\\zbra64\\ZBar\\lib\\libzbar64-0.lib")

#include "E:\opencv\opencv\build\include\opencv2\opencv.hpp"
#include "E:\opencv\opencv\build\include\opencv2\core\core.hpp"
#include "E:\opencv\opencv\build\include\opencv2\highgui\highgui.hpp"
#include "E:\opencv\opencv\build\include\opencv2\imgproc\imgproc.hpp"
#include "E:\opencv\opencv\build\include\opencv2\imgproc\imgproc_c.h"

#include <atlstr.h>

#include "E:\zbra64\ZBar\include\zbar.h"
#include <iostream>
#include <fstream>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string>

using namespace std;
using namespace zbar;
using namespace cv;

string get_time()
{
	SYSTEMTIME  st, lt;
	//GetSystemTime(&lt);
	GetLocalTime(&lt);

	char szResult[30] = "\0";

	sprintf_s(szResult, 30, "%d-%d-%d-%d-%d-%d-%d", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);

	return szResult;
}


//灰度处理
Mat getGray(Mat image, bool show) 
{
	Mat cimage;
	cvtColor(image, cimage, CV_RGBA2GRAY);
	if (show)
		imshow("灰度图", cimage);
	return cimage;
}

//高斯滤波处理
Mat getGass(Mat image, bool show) {
	Mat cimage;
	GaussianBlur(image, cimage, Size(3, 3), 0);
	if (show)
	imshow("高斯滤波图", cimage);
	return cimage;
}

//sobel x-y差处理
Mat getSobel(Mat image, bool show) 
{
	Mat cimageX16s, cimageY16s, imageSobelX, imageSobelY, out;
	Sobel(image, cimageX16s, CV_16S, 1, 0, 3, 1, 0, 4);
	Sobel(image, cimageY16s, CV_16S, 0, 1, 3, 1, 0, 4);
	convertScaleAbs(cimageX16s, imageSobelX, 1, 0);
	convertScaleAbs(cimageY16s, imageSobelY, 1, 0);
	out = imageSobelX - imageSobelY;
	if (show)
	    imshow("Sobelx-y差 图", out);
	return out;
			
}

//均值滤波处理
Mat getBlur(Mat image, bool show) 
{
	Mat cimage;
	blur(image, cimage, Size(3, 3));
	if (show)
	    imshow("均值滤波图", cimage);
	return cimage;		
}

//二值化处理
Mat getThold(Mat image, bool show) 
{
	Mat cimage;
	threshold(image, cimage, 112, 255, CV_THRESH_BINARY);
	if (show)
	imshow("二值化图", cimage);
	return cimage;
}

//闭运算处理
Mat getBys(Mat image, bool show)
{
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	morphologyEx(image, image, MORPH_CLOSE, element);
	if (show)
	imshow("闭运算图", image);
	return image;
}

//腐蚀
Mat getErode(Mat image, bool show) 
{
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	erode(image, image, element);
	if (show)
	imshow("腐蚀图", image);
	return image;
}

//膨胀
Mat getDilate(Mat image, bool show) 
{
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	for (int i = 0; i < 3; i++)
	dilate(image, image, element);
	if (show)
	imshow("膨胀图", image);
	return image;
}

//获取ROI
Mat getRect(Mat image, Mat simage, bool show) 
{
	vector<vector<Point>> contours;
	vector<Vec4i> hiera;
	//Mat cimage;
	findContours(image, contours, hiera, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	vector<float>contourArea;
	for (int i = 0; i < contours.size(); i++)
	{
	    contourArea.push_back(cv::contourArea(contours[i]));
	}
	 //找出面积最大的轮廓
	double maxValue; Point maxLoc;
	minMaxLoc(contourArea, NULL, &maxValue, NULL, &maxLoc);
	//计算面积最大的轮廓的最小的外包矩形
	 RotatedRect minRect = minAreaRect(contours[maxLoc.x]);
	//为了防止找错,要检查这个矩形的偏斜角度不能超标
	//如果超标,那就是没找到
	 Rect myRect;
	if (minRect.angle<2.0)
	{
	    //找到了矩形的角度,但是这是一个旋转矩形,所以还要重新获得一个外包最小矩形
	        myRect = boundingRect(contours[maxLoc.x]);
	    //把这个矩形在源图像中画出来
	        rectangle(image,myRect,Scalar(0,255,255),3,LINE_AA);
	        //看看显示效果,找的对不对
	        //imshow("rect", image);
	        //将扫描的图像裁剪下来,并保存为相应的结果,保留一些X方向的边界,所以对rect进行一定的扩张
	        myRect.x = myRect.x - (myRect.width / 20);
	    myRect.width = myRect.width*1.1;
	    Mat resultImage = Mat(image, myRect);
		//imshow("rect_result", resultImage);
	 }
	
	 for (int i = 0; i<contours.size(); i++)
	 {
	     Rect rect = boundingRect((Mat)contours[i]);
	     //cimage = simage(rect);
	         rectangle(simage, rect, Scalar(0), 2);
		if (show)
				 ;
	         //imshow("转变图", simage);
	 }

	 //裁剪转变图
	 myRect.x = myRect.x - (myRect.width / 20);
	 myRect.width = myRect.width*1.1;
	 simage = Mat(simage, myRect);
	 imshow("result", simage);

	 return simage;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	//VideoCapture capture(1);
	//Mat camera;

	//if (!capture.isOpened())
	//{
	//	return -1;
	//}
	////读取一帧图像


	//double rate = capture.get(CAP_PROP_FPS); // 获取视频帧率
	//capture.set(CAP_PROP_POS_FRAMES, rate);//读取第 帧,第一秒
	//capture >> camera;
	//imshow("sss", camera);



	//string time = get_time();

	//char img_chenggong[100];
	//char img_shibai[100];

	//sprintf_s(img_chenggong, "%s%s%s", "F:\\测试条码用\\成功\\", time.c_str(), ".jpg");
	//sprintf_s(img_shibai, "%s%s%s", "F:\\测试条码用\\失败\\", time.c_str(), ".jpg");


	////Mat srcimage = imread("F:\\3d.jpg");
	////imshow("原图", srcimage);
	////Mat image;
	////image = getGray(srcimage, true);//获取灰度图
	////image = getGass(image, true);//高斯平滑滤波
	////image = getSobel(image, true);//Sobel x—y梯度差
	////image = getBlur(image, true);//均值滤波除高频噪声
	////image = getThold(image, true);//二值化
	////image = getBys(image, true);//闭运算
	////image = getErode(image, true);//腐蚀
	////image = getDilate(image, true);//膨胀

	////image = getRect(image, srcimage, true);//获取ROI
	////imshow("处理结果图", image);

	//ImageScanner scanner;
	//scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

	//Mat imageGray;
	//cvtColor(camera, imageGray, CV_RGB2GRAY);

	//int width = imageGray.cols;
	//int height = imageGray.rows;
	//uchar *raw = (uchar *)imageGray.data;//raw中存放的是图像的地址
	//Image imageZbar(width, height, "Y800", raw, width * height);

	////扫描条码  
	//scanner.scan(imageZbar);

	////2个码
	//CString str_barcode_chip;//CHIP ID码
	//CString str_barcode_sn;//SN码

	//Image::SymbolIterator symbol = imageZbar.symbol_begin();
	//if (imageZbar.symbol_begin() == imageZbar.symbol_end())
	//{
	//	MessageBox(NULL, TEXT("识别失败"), TEXT("条码结果"), MB_DEFBUTTON1 | MB_DEFBUTTON2);
	//	imwrite(img_shibai, camera);
	//	return -1;
	//}
	//else
	//{
	//	str_barcode_chip = symbol->get_data().c_str();

	//	++symbol;

	//	str_barcode_sn = symbol->get_data().c_str();
	//}

	//MessageBox(NULL, TEXT("SN:") + str_barcode_sn + TEXT("\nCHIP ID:") + str_barcode_chip, TEXT("条码结果"), MB_DEFBUTTON1 | MB_DEFBUTTON2);
	//imwrite(img_chenggong, camera);

	////得到的CHIP ID码和SN码保存到csv文件中
	//// 写文件  
	//ofstream outFile;
	//outFile.open("F:\\条码保存\\data.csv", ios::out); // 打开模式可省略 
	//outFile << str_barcode_chip << ',' << 21 << ',' << str_barcode_sn << endl;
	//outFile.close();

	//// 读文件  
	//ifstream inFile("F:\\条码保存\\data.csv", ios::in);

	//waitKey();

	////Mat pic;
	////pic = imread("F:\\测试条码用\\2019-6-29-10-14-43-180.jpg");
	////imshow("sss", pic);
	////Dis_code(pic);
	////waitKey();

	//capture.release();
	//return 0;









	//VideoCapture capture(1);
	//Mat camera;

	//if (!capture.isOpened())
	//{
	//	return -1;
	//}
	////读取一帧图像
	//double rate = capture.get(CAP_PROP_FPS); // 获取视频帧率
	//capture.set(CAP_PROP_POS_FRAMES, rate);//读取第 帧,第一秒
	//capture >> camera;
	//imshow("sss", camera);

	string time = get_time();

	char img_chenggong[100];
	char img_shibai[100];
	sprintf_s(img_chenggong, "%s%s%s", "F:\\测试条码用\\成功\\", time.c_str(), ".jpg");
	sprintf_s(img_shibai, "%s%s%s", "F:\\测试条码用\\失败\\", time.c_str(), ".jpg");

	Mat image, imageGray, imageGuussian;
	Mat imageSobelX, imageSobelY, imageSobelOut;
	imageGray = imread("F:\\2019-6-29-10-10-12-667.jpg", 0);
	imageGray.copyTo(image);
	imshow("Source Image", image);
	GaussianBlur(imageGray, imageGuussian, Size(3, 3), 0);
	//水平和垂直方向灰度图像的梯度和,使用Sobel算子    
	Mat imageX16S, imageY16S;
	Sobel(imageGuussian, imageX16S, CV_16S, 1, 0, 3, 1, 0, 4);
	Sobel(imageGuussian, imageY16S, CV_16S, 0, 1, 3, 1, 0, 4);
	convertScaleAbs(imageX16S, imageSobelX, 1, 0);
	convertScaleAbs(imageY16S, imageSobelY, 1, 0);
	imageSobelOut = imageSobelX + imageSobelY;
	imshow("XY方向梯度和", imageSobelOut);


	Mat srcImg = imageSobelOut;
	//宽高扩充，非必须，特定的宽高可以提高傅里叶运算效率  
	Mat padded;
	int opWidth = getOptimalDFTSize(srcImg.rows);
	int opHeight = getOptimalDFTSize(srcImg.cols);
	copyMakeBorder(srcImg, padded, 0, opWidth - srcImg.rows, 0, opHeight - srcImg.cols, BORDER_CONSTANT, Scalar::all(0));//扩充src的边缘，将图像变大
	imshow("padded", padded);

	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat comImg;
	//通道融合，融合成一个2通道的图像  
	merge(planes, 2, comImg);
	dft(comImg, comImg);
	split(comImg, planes);
	magnitude(planes[0], planes[1], planes[0]);
	Mat magMat = planes[0];
	magMat += Scalar::all(1);
	log(magMat, magMat);     //对数变换，方便显示 
	magMat = magMat(Rect(0, 0, magMat.cols & -2, magMat.rows & -2));
	//以下把傅里叶频谱图的四个角落移动到图像中心
	int cx = magMat.cols / 2;
	int cy = magMat.rows / 2;
	Mat q0(magMat, Rect(0, 0, cx, cy));
	Mat q1(magMat, Rect(0, cy, cx, cy));
	Mat q2(magMat, Rect(cx, cy, cx, cy));
	Mat q3(magMat, Rect(cx, 0, cx, cy));
	Mat tmp;
	q0.copyTo(tmp);
	q2.copyTo(q0);
	tmp.copyTo(q2);
	q1.copyTo(tmp);
	q3.copyTo(q1);
	tmp.copyTo(q3);
	normalize(magMat, magMat, 0, 1, CV_MINMAX);
	Mat magImg(magMat.size(), CV_8UC1);
	magMat.convertTo(magImg, CV_8UC1, 255, 0);
	imshow("傅里叶频谱", magImg);
	//HoughLines查找傅里叶频谱的直线，该直线跟原图的一维码方向相互垂直
	threshold(magImg, magImg, 180, 255, CV_THRESH_BINARY);
	imshow("二值化", magImg);
	vector<Vec2f> lines;
	float pi180 = (float)CV_PI / 180;
	Mat linImg(magImg.size(), CV_8UC3);
	HoughLines(magImg, lines, 1, pi180, 100, 0, 0);
	int numLines = lines.size();
	float theta;
	for (int l = 0; l<numLines; l++)
	{
		float rho = lines[l][0];
		theta = lines[l][1];
		float aa = (theta / CV_PI) * 180;
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(linImg, pt1, pt2, Scalar(255, 0, 0), 3, 8, 0);
	}
	imshow("Hough直线", linImg);
	//校正角度计算
	//float angelD = 180 * theta / CV_PI - 90;
	float angelD = 0;
	Point center(image.cols / 2, image.rows / 2);
	Mat rotMat = getRotationMatrix2D(center, angelD, 1.0);
	Mat imageSource = Mat::ones(image.size(), CV_8UC3);
	warpAffine(image, imageSource, rotMat, image.size(), 1, 0, Scalar(255, 255, 255));//仿射变换校正图像  
	imshow("角度校正", imageSource);

	//Zbar一维码识别
	ImageScanner scanner;
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	int width1 = imageSource.cols;
	int height1 = imageSource.rows;
	uchar *raw = (uchar *)imageSource.data;
	Image imageZbar(width1, height1, "Y800", raw, width1 * height1);

	//扫描条码  
	scanner.scan(imageZbar);

	//2个码
	string str_barcode_chip;//CHIP ID码
	string str_barcode_sn;//SN码

	Image::SymbolIterator symbol = imageZbar.symbol_begin();
	if (imageZbar.symbol_begin() == imageZbar.symbol_end())
	{
		MessageBox(NULL, TEXT("识别失败"), TEXT("条码结果"), MB_DEFBUTTON1 | MB_DEFBUTTON2);
		//imwrite(img_shibai, camera);
		imwrite(img_shibai, imageGray);
		return -1;
	}
	else
	{
		str_barcode_chip = symbol->get_data();

		++symbol;

		str_barcode_sn = symbol->get_data();
	}

	CString chip,sn;
	chip = str_barcode_chip.c_str();
	sn = str_barcode_sn.c_str();

	MessageBox(NULL, TEXT("SN:") + sn + TEXT("\nCHIP ID:") + chip, TEXT("条码结果"), MB_DEFBUTTON1 | MB_DEFBUTTON2);
	//imwrite(img_chenggong, camera);
	imwrite(img_chenggong, imageGray);

	//得到的CHIP ID码和SN码保存到csv文件中
	// 写文件  
	char barcode_file[100];
	sprintf_s(barcode_file, "%s%s%s", "F:\\条码保存\\barcode", time.c_str(), ".csv");
	ofstream outFile;
	outFile.open(barcode_file, ios::out); 


	outFile << "chip:" << str_barcode_chip << "," << "sn:"<< str_barcode_sn << endl;
	outFile.close();

	// 读文件  
	ifstream inFile(barcode_file, ios::in);

	waitKey();

	//capture.release();
}

