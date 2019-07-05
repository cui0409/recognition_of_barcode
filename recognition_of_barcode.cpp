#pragma comment(lib,"zbra64\\ZBar\\lib\\libzbar64-0.lib")

#include "opencv\opencv\build\include\opencv2\opencv.hpp"
#include "opencv\opencv\build\include\opencv2\core\core.hpp"
#include "opencv\opencv\build\include\opencv2\highgui\highgui.hpp"
#include "opencv\opencv\build\include\opencv2\imgproc\imgproc.hpp"
#include "opencv\opencv\build\include\opencv2\imgproc\imgproc_c.h"

#include <atlstr.h>

#include "zbra64\ZBar\include\zbar.h"
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


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	VideoCapture capture(1);
	if (!capture.isOpened())
	{
		return -1;
	}

	string time = get_time();

	char img_chenggong[100];
	char img_shibai[100];

	sprintf_s(img_chenggong, "%s%s%s", "F:\\测试条码用\\成功\\", time.c_str(), ".jpg");
	sprintf_s(img_shibai, "%s%s%s", "F:\\测试条码用\\失败\\", time.c_str(), ".jpg");

	ImageScanner scanner;
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

	//2个码
	string str_barcode_chip;//CHIP ID码
	string str_barcode_sn;//SN码

	double rate = capture.get(CAP_PROP_FPS); // 获取视频帧率

	Mat camera;
	for (int i = 0; i < rate * 3; ++i)//3秒循环识别
	{
		capture.set(CAP_PROP_POS_FRAMES, i);//读取第i帧
		capture >> camera;

		Mat imageGray;

		cvtColor(camera, imageGray, CV_RGB2GRAY);

		int width = imageGray.cols;
		int height = imageGray.rows;
		uchar *raw = (uchar *)imageGray.data;//raw中存放的是图像的地址
		
		Image imageZbar(width, height, "Y800", raw, width * height);

		//扫描条码  
		scanner.scan(imageZbar);

		Image::SymbolIterator symbol = imageZbar.symbol_begin();

		str_barcode_chip = symbol->get_data();
		++symbol;
		str_barcode_sn = symbol->get_data();

		CString chip, sn;

		//只要sn和chip码全部扫出来，即退出循环保存成功结果，否则走完循环还扫不出来就确实是扫不出来
		if((!str_barcode_chip.empty()) && (!str_barcode_sn.empty()))
		{
			chip = str_barcode_chip.c_str();
			sn = str_barcode_sn.c_str();
			imshow("sss", camera);
			MessageBox(NULL, TEXT("SN:") + sn + TEXT("\nCHIP ID:") + chip, TEXT("条码结果"), MB_DEFBUTTON1 | MB_DEFBUTTON2);
			imwrite(img_chenggong, camera);

			//扫码成功，将得到的CHIP ID码和SN码保存到csv文件中
			char barcode_file[100];
			sprintf_s(barcode_file, "%s%s%s", "F:\\条码保存\\barcode_csv\\barcode", time.c_str(), ".csv");
			ofstream outFile;
			outFile.open(barcode_file, ios::out);


			outFile << "chip:" << str_barcode_chip << "," << "sn:" << str_barcode_sn << endl;
			outFile.close();

			// 读文件  
			ifstream inFile(barcode_file, ios::in);

			return 0;//扫码成功就退出
		}

	}

	//循环识别还是没有结果，那就是失败了
	MessageBox(NULL, TEXT("识别失败"), TEXT("条码结果"), MB_DEFBUTTON1 | MB_DEFBUTTON2);
	imwrite(img_shibai, camera);

	return 0;
}

