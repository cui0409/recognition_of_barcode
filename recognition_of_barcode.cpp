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
using namespace std;
using namespace zbar;
using namespace cv;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	VideoCapture capture;
	Mat camera;

	capture.open(1, CAP_DSHOW);	//打开摄像头设备采集条码图片
	if (!capture.isOpened())
	{
		return -1;
	}
	//读取一帧图像
	capture.read(camera);

	ImageScanner scanner;
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	//Mat image = imread("F:\\E6.jpg");//识别本地图片条码
	namedWindow("tiaoma");
	//imshow("tiaoma",image);
	imshow("tiaoma", camera);

	Mat imageGray;
	cvtColor(camera, imageGray, CV_RGB2GRAY);
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
		return -1;
	}
	else
	{
		str_barcode_chip = symbol->get_data().c_str();

		++symbol;

		str_barcode_sn = symbol->get_data().c_str();
	}

	MessageBox(NULL, TEXT("SN:") + str_barcode_sn + TEXT("\nCHIP ID:") + str_barcode_chip, TEXT("条码结果"), MB_DEFBUTTON1 | MB_DEFBUTTON2);

	waitKey();
	imageZbar.set_data(NULL, 0);
	return 0;


	//Mat srcImage = imread("F:\\机顶盒1.jpg");
	//if (srcImage.empty())
	//{
	//	cout << "image file read error" << endl;

	//	return -1;
	//}
	////图像转换为灰度图像
	//Mat grayImage;
	//if (srcImage.channels() == 3)
	//{
	//	cvtColor(srcImage, grayImage, CV_RGB2GRAY);
	//}
	//else
	//{
	//	grayImage = srcImage.clone();
	//}
	////建立图像的梯度幅值
	//Mat gradientXImage;
	//Mat gradientYImage;
	//Mat gradientImage;
	//Scharr(grayImage, gradientXImage, CV_32F, 1, 0);
	//Scharr(grayImage, gradientYImage, CV_32F, 0, 1);
	////因为我们需要的条形码在需要X方向水平,所以更多的关注X方向的梯度幅值,而省略掉Y方向的梯度幅值
	//subtract(gradientXImage, gradientYImage, gradientImage);
	////归一化为八位图像
	//convertScaleAbs(gradientImage, gradientImage);
	////看看得到的梯度图像是什么样子
	////imshow(windowNameString,gradientImage);
	////对图片进行相应的模糊化,使一些噪点消除
	//Mat blurImage;
	//blur(gradientImage, blurImage, Size(9, 9));
	////模糊化以后进行阈值化,得到到对应的黑白二值化图像,二值化的阈值可以根据实际情况调整
	//Mat thresholdImage;
	//threshold(blurImage, thresholdImage, 210, 255, THRESH_BINARY);
	////看看二值化图像
	////imshow(windowNameString,thresholdImage);
	////二值化以后的图像,条形码之间的黑白没有连接起来,就要进行形态学运算,消除缝隙,相当于小型的黑洞,选择闭运算
	////因为是长条之间的缝隙,所以需要选择宽度大于长度
	//Mat kernel = getStructuringElement(MORPH_RECT, Size(21, 7));
	//Mat morphImage;
	//morphologyEx(thresholdImage, morphImage, MORPH_CLOSE, kernel);
	////看看形态学操作以后的图像
	////imshow(windowNameString,morphImage);
	////现在要让条形码区域连接在一起,所以选择膨胀腐蚀,而且为了保持图形大小基本不变,应该使用相同次数的膨胀腐蚀
	////先腐蚀,让其他区域的亮的地方变少最好是消除,然后膨胀回来,消除干扰,迭代次数根据实际情况选择
	//erode(morphImage, morphImage, getStructuringElement(MORPH_RECT, Size(3, 3)), Point(-1, -1), 4);
	//dilate(morphImage, morphImage, getStructuringElement(MORPH_RECT, Size(3, 3)), Point(-1, -1), 4);
	////看看形态学操作以后的图像
	////imshow(windowNameString,morphImage);
	//vector<vector<Point2i>>contours;
	//vector<float>contourArea;
	////接下来对目标轮廓进行查找,目标是为了计算图像面积
	//findContours(morphImage, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//namedWindow("aaa");
	//imshow("aaa", morphImage);
	////计算轮廓的面积并且存放
	//for (int i = 0; i < contours.size(); i++)
	//{
	//	contourArea.push_back(cv::contourArea(contours[i]));
	//}
	////找出面积最大的轮廓
	//double maxValue; Point maxLoc;
	//minMaxLoc(contourArea, NULL, &maxValue, NULL, &maxLoc);
	////计算面积最大的轮廓的最小的外包矩形
	//RotatedRect minRect = minAreaRect(contours[maxLoc.x]);
	////为了防止找错,要检查这个矩形的偏斜角度不能超标
	////如果超标,那就是没找到
	//Mat resultImage;
	//if (minRect.angle<2.0)
	//{
	//	//找到了矩形的角度,但是这是一个旋转矩形,所以还要重新获得一个外包最小矩形
	//	Rect myRect = boundingRect(contours[maxLoc.x]);
	//	//把这个矩形在源图像中画出来
	//	//rectangle(srcImage,myRect,Scalar(0,255,255),3,LINE_AA);
	//	//看看显示效果,找的对不对
	//	//imshow(windowNameString,srcImage);
	//	//将扫描的图像裁剪下来,并保存为相应的结果,保留一些X方向的边界,所以对rect进行一定的扩张
	//	myRect.x = myRect.x - (myRect.width / 20);
	//	myRect.width = myRect.width*1.1;
	//	resultImage = Mat(srcImage, myRect);
	//}
	//namedWindow("sss");
	//imshow("sss", resultImage);


	//int width = resultImage.cols;
	//int height = resultImage.rows;
	//uchar *raw = (uchar *)resultImage.data;//raw中存放的是图像的地址
	//Image imageZbar(width, height, "Y800", raw, width * height);

	//ImageScanner scanner;
	//scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	////扫描条码  
	//scanner.scan(imageZbar); 

	////2个码
	//CString str_barcode_chip;//CHIP ID码
	//CString str_barcode_sn;//SN码

	//Image::SymbolIterator symbol = imageZbar.symbol_begin();
	//if (imageZbar.symbol_begin() == imageZbar.symbol_end())
	//{
	//	MessageBox(NULL, TEXT("识别失败"), TEXT("条码结果"), MB_DEFBUTTON1 | MB_DEFBUTTON2);
	//	return -1;
	//}
	//else
	//{
	//	str_barcode_chip = symbol->get_data().c_str();

	//	++symbol;

	//	str_barcode_sn = symbol->get_data().c_str();
	//}

	//MessageBox(NULL, TEXT("SN:") + str_barcode_sn + TEXT("\nCHIP ID:") + str_barcode_chip, TEXT("条码结果"), MB_DEFBUTTON1 | MB_DEFBUTTON2);

}