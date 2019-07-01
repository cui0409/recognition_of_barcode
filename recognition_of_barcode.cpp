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
using namespace std;
using namespace zbar;
using namespace cv;

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

void Dis_code(Mat image) 
{
	 ImageScanner scanner;
	 scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

	 Mat imageGray;
	 cvtColor(image, imageGray, CV_RGB2GRAY);

	 int width = imageGray.cols;
	 int height = imageGray.rows;
	 uchar *raw = (uchar *)imageGray.data;//raw中存放的是图像的地址
	 Image imageZbar(width, height, "Y800", raw, width * height);

	 //扫描条码  
	 scanner.scan(imageZbar); 

	 //2个码
	 CString str_barcode_chip;//CHIP ID码
	 CString str_barcode_sn;//SN码

	 Image::SymbolIterator symbol = imageZbar.symbol_begin();
	 if (imageZbar.symbol_begin() == imageZbar.symbol_end())
	 {
	 	MessageBox(NULL, TEXT("识别失败"), TEXT("条码结果"), MB_DEFBUTTON1 | MB_DEFBUTTON2);
	 	return ;
	 }
	 else
	 {
	 	str_barcode_chip = symbol->get_data().c_str();

	 	++symbol;

	 	str_barcode_sn = symbol->get_data().c_str();
	 }

	 MessageBox(NULL, TEXT("SN:") + str_barcode_sn + TEXT("\nCHIP ID:") + str_barcode_chip, TEXT("条码结果"), MB_DEFBUTTON1 | MB_DEFBUTTON2);


	 //得到的CHIP ID码和SN码保存到csv文件中
	 // 写文件  
	 ofstream outFile;
	 outFile.open("F:\\条码保存\\data.csv", ios::out); // 打开模式可省略 
	 outFile << str_barcode_chip << ',' << 21 << ',' << str_barcode_sn << endl;
	 outFile.close();

	 // 读文件  
	 ifstream inFile("F:\\条码保存\\data.csv", ios::in);

	 //while (getline(inFile, lineStr))
	 //{

		// vector<string> lineArray;
		// // 按照逗号分隔  
		// while (getline(ss, str, ','))
		//	 lineArray.push_back(str);
		// strArray.push_back(lineArray);
	 //}

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	VideoCapture capture(1);
	Mat camera;

	if (!capture.isOpened())
	{
		return -1;
	}
	//读取一帧图像
	capture.read(camera);
	imshow("sss", camera);
	
	string time = get_time();
	char image_loca[100];
	sprintf_s(image_loca, "%s%s%s", "F:\\测试条码用\\", time.c_str(), ".jpg");
	imwrite(image_loca, camera);
	
	//Mat srcimage = imread("F:\\3d.jpg");
	//imshow("原图", srcimage);
	//Mat image;
	//image = getGray(srcimage, true);//获取灰度图
	//image = getGass(image, true);//高斯平滑滤波
	//image = getSobel(image, true);//Sobel x—y梯度差
	//image = getBlur(image, true);//均值滤波除高频噪声
	//image = getThold(image, true);//二值化
	//image = getBys(image, true);//闭运算
	//image = getErode(image, true);//腐蚀
	//image = getDilate(image, true);//膨胀

	//image = getRect(image, srcimage, true);//获取ROI
	//imshow("处理结果图", image);
	Dis_code(camera);

	waitKey();

	return -1;
}
