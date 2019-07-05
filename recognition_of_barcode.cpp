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

	sprintf_s(img_chenggong, "%s%s%s", "F:\\����������\\�ɹ�\\", time.c_str(), ".jpg");
	sprintf_s(img_shibai, "%s%s%s", "F:\\����������\\ʧ��\\", time.c_str(), ".jpg");

	ImageScanner scanner;
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

	//2����
	string str_barcode_chip;//CHIP ID��
	string str_barcode_sn;//SN��

	double rate = capture.get(CAP_PROP_FPS); // ��ȡ��Ƶ֡��

	Mat camera;
	for (int i = 0; i < rate * 3; ++i)//3��ѭ��ʶ��
	{
		capture.set(CAP_PROP_POS_FRAMES, i);//��ȡ��i֡
		capture >> camera;

		Mat imageGray;

		cvtColor(camera, imageGray, CV_RGB2GRAY);

		int width = imageGray.cols;
		int height = imageGray.rows;
		uchar *raw = (uchar *)imageGray.data;//raw�д�ŵ���ͼ��ĵ�ַ
		
		Image imageZbar(width, height, "Y800", raw, width * height);

		//ɨ������  
		scanner.scan(imageZbar);

		Image::SymbolIterator symbol = imageZbar.symbol_begin();

		str_barcode_chip = symbol->get_data();
		++symbol;
		str_barcode_sn = symbol->get_data();

		CString chip, sn;

		//ֻҪsn��chip��ȫ��ɨ���������˳�ѭ������ɹ��������������ѭ����ɨ��������ȷʵ��ɨ������
		if((!str_barcode_chip.empty()) && (!str_barcode_sn.empty()))
		{
			chip = str_barcode_chip.c_str();
			sn = str_barcode_sn.c_str();
			imshow("sss", camera);
			MessageBox(NULL, TEXT("SN:") + sn + TEXT("\nCHIP ID:") + chip, TEXT("������"), MB_DEFBUTTON1 | MB_DEFBUTTON2);
			imwrite(img_chenggong, camera);

			//ɨ��ɹ������õ���CHIP ID���SN�뱣�浽csv�ļ���
			char barcode_file[100];
			sprintf_s(barcode_file, "%s%s%s", "F:\\���뱣��\\barcode_csv\\barcode", time.c_str(), ".csv");
			ofstream outFile;
			outFile.open(barcode_file, ios::out);


			outFile << "chip:" << str_barcode_chip << "," << "sn:" << str_barcode_sn << endl;
			outFile.close();

			// ���ļ�  
			ifstream inFile(barcode_file, ios::in);

			return 0;//ɨ��ɹ����˳�
		}

	}

	//ѭ��ʶ����û�н�����Ǿ���ʧ����
	MessageBox(NULL, TEXT("ʶ��ʧ��"), TEXT("������"), MB_DEFBUTTON1 | MB_DEFBUTTON2);
	imwrite(img_shibai, camera);

	return 0;
}

