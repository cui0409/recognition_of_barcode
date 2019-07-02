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


//�Ҷȴ���
Mat getGray(Mat image, bool show) 
{
	Mat cimage;
	cvtColor(image, cimage, CV_RGBA2GRAY);
	if (show)
		imshow("�Ҷ�ͼ", cimage);
	return cimage;
}

//��˹�˲�����
Mat getGass(Mat image, bool show) {
	Mat cimage;
	GaussianBlur(image, cimage, Size(3, 3), 0);
	if (show)
	imshow("��˹�˲�ͼ", cimage);
	return cimage;
}

//sobel x-y���
Mat getSobel(Mat image, bool show) 
{
	Mat cimageX16s, cimageY16s, imageSobelX, imageSobelY, out;
	Sobel(image, cimageX16s, CV_16S, 1, 0, 3, 1, 0, 4);
	Sobel(image, cimageY16s, CV_16S, 0, 1, 3, 1, 0, 4);
	convertScaleAbs(cimageX16s, imageSobelX, 1, 0);
	convertScaleAbs(cimageY16s, imageSobelY, 1, 0);
	out = imageSobelX - imageSobelY;
	if (show)
	    imshow("Sobelx-y�� ͼ", out);
	return out;
			
}

//��ֵ�˲�����
Mat getBlur(Mat image, bool show) 
{
	Mat cimage;
	blur(image, cimage, Size(3, 3));
	if (show)
	    imshow("��ֵ�˲�ͼ", cimage);
	return cimage;		
}

//��ֵ������
Mat getThold(Mat image, bool show) 
{
	Mat cimage;
	threshold(image, cimage, 112, 255, CV_THRESH_BINARY);
	if (show)
	imshow("��ֵ��ͼ", cimage);
	return cimage;
}

//�����㴦��
Mat getBys(Mat image, bool show)
{
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	morphologyEx(image, image, MORPH_CLOSE, element);
	if (show)
	imshow("������ͼ", image);
	return image;
}

//��ʴ
Mat getErode(Mat image, bool show) 
{
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	erode(image, image, element);
	if (show)
	imshow("��ʴͼ", image);
	return image;
}

//����
Mat getDilate(Mat image, bool show) 
{
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	for (int i = 0; i < 3; i++)
	dilate(image, image, element);
	if (show)
	imshow("����ͼ", image);
	return image;
}

//��ȡROI
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
	 //�ҳ������������
	double maxValue; Point maxLoc;
	minMaxLoc(contourArea, NULL, &maxValue, NULL, &maxLoc);
	//�������������������С���������
	 RotatedRect minRect = minAreaRect(contours[maxLoc.x]);
	//Ϊ�˷�ֹ�Ҵ�,Ҫ���������ε�ƫб�ǶȲ��ܳ���
	//�������,�Ǿ���û�ҵ�
	 Rect myRect;
	if (minRect.angle<2.0)
	{
	    //�ҵ��˾��εĽǶ�,��������һ����ת����,���Ի�Ҫ���»��һ�������С����
	        myRect = boundingRect(contours[maxLoc.x]);
	    //�����������Դͼ���л�����
	        rectangle(image,myRect,Scalar(0,255,255),3,LINE_AA);
	        //������ʾЧ��,�ҵĶԲ���
	        //imshow("rect", image);
	        //��ɨ���ͼ��ü�����,������Ϊ��Ӧ�Ľ��,����һЩX����ı߽�,���Զ�rect����һ��������
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
	         //imshow("ת��ͼ", simage);
	 }

	 //�ü�ת��ͼ
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
	////��ȡһ֡ͼ��


	//double rate = capture.get(CAP_PROP_FPS); // ��ȡ��Ƶ֡��
	//capture.set(CAP_PROP_POS_FRAMES, rate);//��ȡ�� ֡,��һ��
	//capture >> camera;
	//imshow("sss", camera);



	//string time = get_time();

	//char img_chenggong[100];
	//char img_shibai[100];

	//sprintf_s(img_chenggong, "%s%s%s", "F:\\����������\\�ɹ�\\", time.c_str(), ".jpg");
	//sprintf_s(img_shibai, "%s%s%s", "F:\\����������\\ʧ��\\", time.c_str(), ".jpg");


	////Mat srcimage = imread("F:\\3d.jpg");
	////imshow("ԭͼ", srcimage);
	////Mat image;
	////image = getGray(srcimage, true);//��ȡ�Ҷ�ͼ
	////image = getGass(image, true);//��˹ƽ���˲�
	////image = getSobel(image, true);//Sobel x��y�ݶȲ�
	////image = getBlur(image, true);//��ֵ�˲�����Ƶ����
	////image = getThold(image, true);//��ֵ��
	////image = getBys(image, true);//������
	////image = getErode(image, true);//��ʴ
	////image = getDilate(image, true);//����

	////image = getRect(image, srcimage, true);//��ȡROI
	////imshow("������ͼ", image);

	//ImageScanner scanner;
	//scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

	//Mat imageGray;
	//cvtColor(camera, imageGray, CV_RGB2GRAY);

	//int width = imageGray.cols;
	//int height = imageGray.rows;
	//uchar *raw = (uchar *)imageGray.data;//raw�д�ŵ���ͼ��ĵ�ַ
	//Image imageZbar(width, height, "Y800", raw, width * height);

	////ɨ������  
	//scanner.scan(imageZbar);

	////2����
	//CString str_barcode_chip;//CHIP ID��
	//CString str_barcode_sn;//SN��

	//Image::SymbolIterator symbol = imageZbar.symbol_begin();
	//if (imageZbar.symbol_begin() == imageZbar.symbol_end())
	//{
	//	MessageBox(NULL, TEXT("ʶ��ʧ��"), TEXT("������"), MB_DEFBUTTON1 | MB_DEFBUTTON2);
	//	imwrite(img_shibai, camera);
	//	return -1;
	//}
	//else
	//{
	//	str_barcode_chip = symbol->get_data().c_str();

	//	++symbol;

	//	str_barcode_sn = symbol->get_data().c_str();
	//}

	//MessageBox(NULL, TEXT("SN:") + str_barcode_sn + TEXT("\nCHIP ID:") + str_barcode_chip, TEXT("������"), MB_DEFBUTTON1 | MB_DEFBUTTON2);
	//imwrite(img_chenggong, camera);

	////�õ���CHIP ID���SN�뱣�浽csv�ļ���
	//// д�ļ�  
	//ofstream outFile;
	//outFile.open("F:\\���뱣��\\data.csv", ios::out); // ��ģʽ��ʡ�� 
	//outFile << str_barcode_chip << ',' << 21 << ',' << str_barcode_sn << endl;
	//outFile.close();

	//// ���ļ�  
	//ifstream inFile("F:\\���뱣��\\data.csv", ios::in);

	//waitKey();

	////Mat pic;
	////pic = imread("F:\\����������\\2019-6-29-10-14-43-180.jpg");
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
	////��ȡһ֡ͼ��
	//double rate = capture.get(CAP_PROP_FPS); // ��ȡ��Ƶ֡��
	//capture.set(CAP_PROP_POS_FRAMES, rate);//��ȡ�� ֡,��һ��
	//capture >> camera;
	//imshow("sss", camera);

	string time = get_time();

	char img_chenggong[100];
	char img_shibai[100];
	sprintf_s(img_chenggong, "%s%s%s", "F:\\����������\\�ɹ�\\", time.c_str(), ".jpg");
	sprintf_s(img_shibai, "%s%s%s", "F:\\����������\\ʧ��\\", time.c_str(), ".jpg");

	Mat image, imageGray, imageGuussian;
	Mat imageSobelX, imageSobelY, imageSobelOut;
	imageGray = imread("F:\\2019-6-29-10-10-12-667.jpg", 0);
	imageGray.copyTo(image);
	imshow("Source Image", image);
	GaussianBlur(imageGray, imageGuussian, Size(3, 3), 0);
	//ˮƽ�ʹ�ֱ����Ҷ�ͼ����ݶȺ�,ʹ��Sobel����    
	Mat imageX16S, imageY16S;
	Sobel(imageGuussian, imageX16S, CV_16S, 1, 0, 3, 1, 0, 4);
	Sobel(imageGuussian, imageY16S, CV_16S, 0, 1, 3, 1, 0, 4);
	convertScaleAbs(imageX16S, imageSobelX, 1, 0);
	convertScaleAbs(imageY16S, imageSobelY, 1, 0);
	imageSobelOut = imageSobelX + imageSobelY;
	imshow("XY�����ݶȺ�", imageSobelOut);


	Mat srcImg = imageSobelOut;
	//������䣬�Ǳ��룬�ض��Ŀ�߿�����߸���Ҷ����Ч��  
	Mat padded;
	int opWidth = getOptimalDFTSize(srcImg.rows);
	int opHeight = getOptimalDFTSize(srcImg.cols);
	copyMakeBorder(srcImg, padded, 0, opWidth - srcImg.rows, 0, opHeight - srcImg.cols, BORDER_CONSTANT, Scalar::all(0));//����src�ı�Ե����ͼ����
	imshow("padded", padded);

	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat comImg;
	//ͨ���ںϣ��ںϳ�һ��2ͨ����ͼ��  
	merge(planes, 2, comImg);
	dft(comImg, comImg);
	split(comImg, planes);
	magnitude(planes[0], planes[1], planes[0]);
	Mat magMat = planes[0];
	magMat += Scalar::all(1);
	log(magMat, magMat);     //�����任��������ʾ 
	magMat = magMat(Rect(0, 0, magMat.cols & -2, magMat.rows & -2));
	//���°Ѹ���ҶƵ��ͼ���ĸ������ƶ���ͼ������
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
	imshow("����ҶƵ��", magImg);
	//HoughLines���Ҹ���ҶƵ�׵�ֱ�ߣ���ֱ�߸�ԭͼ��һά�뷽���໥��ֱ
	threshold(magImg, magImg, 180, 255, CV_THRESH_BINARY);
	imshow("��ֵ��", magImg);
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
	imshow("Houghֱ��", linImg);
	//У���Ƕȼ���
	//float angelD = 180 * theta / CV_PI - 90;
	float angelD = 0;
	Point center(image.cols / 2, image.rows / 2);
	Mat rotMat = getRotationMatrix2D(center, angelD, 1.0);
	Mat imageSource = Mat::ones(image.size(), CV_8UC3);
	warpAffine(image, imageSource, rotMat, image.size(), 1, 0, Scalar(255, 255, 255));//����任У��ͼ��  
	imshow("�Ƕ�У��", imageSource);

	//Zbarһά��ʶ��
	ImageScanner scanner;
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	int width1 = imageSource.cols;
	int height1 = imageSource.rows;
	uchar *raw = (uchar *)imageSource.data;
	Image imageZbar(width1, height1, "Y800", raw, width1 * height1);

	//ɨ������  
	scanner.scan(imageZbar);

	//2����
	string str_barcode_chip;//CHIP ID��
	string str_barcode_sn;//SN��

	Image::SymbolIterator symbol = imageZbar.symbol_begin();
	if (imageZbar.symbol_begin() == imageZbar.symbol_end())
	{
		MessageBox(NULL, TEXT("ʶ��ʧ��"), TEXT("������"), MB_DEFBUTTON1 | MB_DEFBUTTON2);
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

	MessageBox(NULL, TEXT("SN:") + sn + TEXT("\nCHIP ID:") + chip, TEXT("������"), MB_DEFBUTTON1 | MB_DEFBUTTON2);
	//imwrite(img_chenggong, camera);
	imwrite(img_chenggong, imageGray);

	//�õ���CHIP ID���SN�뱣�浽csv�ļ���
	// д�ļ�  
	char barcode_file[100];
	sprintf_s(barcode_file, "%s%s%s", "F:\\���뱣��\\barcode", time.c_str(), ".csv");
	ofstream outFile;
	outFile.open(barcode_file, ios::out); 


	outFile << "chip:" << str_barcode_chip << "," << "sn:"<< str_barcode_sn << endl;
	outFile.close();

	// ���ļ�  
	ifstream inFile(barcode_file, ios::in);

	waitKey();

	//capture.release();
}

