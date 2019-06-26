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

	capture.open(1, CAP_DSHOW);	//������ͷ�豸�ɼ�����ͼƬ
	if (!capture.isOpened())
	{
		return -1;
	}
	//��ȡһ֡ͼ��
	capture.read(camera);

	ImageScanner scanner;
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	//Mat image = imread("F:\\E6.jpg");//ʶ�𱾵�ͼƬ����
	namedWindow("tiaoma");
	//imshow("tiaoma",image);
	imshow("tiaoma", camera);

	Mat imageGray;
	cvtColor(camera, imageGray, CV_RGB2GRAY);
	int width = imageGray.cols;
	int height = imageGray.rows;
	uchar *raw = (uchar *)imageGray.data;//raw�д�ŵ���ͼ��ĵ�ַ
	Image imageZbar(width, height, "Y800", raw, width * height);

	//ɨ������  
	scanner.scan(imageZbar); 

	//2����
	CString str_barcode_chip;//CHIP ID��
	CString str_barcode_sn;//SN��

	Image::SymbolIterator symbol = imageZbar.symbol_begin();
	if (imageZbar.symbol_begin() == imageZbar.symbol_end())
	{
		MessageBox(NULL, TEXT("ʶ��ʧ��"), TEXT("������"), MB_DEFBUTTON1 | MB_DEFBUTTON2);
		return -1;
	}
	else
	{
		str_barcode_chip = symbol->get_data().c_str();

		++symbol;

		str_barcode_sn = symbol->get_data().c_str();
	}

	MessageBox(NULL, TEXT("SN:") + str_barcode_sn + TEXT("\nCHIP ID:") + str_barcode_chip, TEXT("������"), MB_DEFBUTTON1 | MB_DEFBUTTON2);

	waitKey();
	imageZbar.set_data(NULL, 0);
	return 0;


	//Mat srcImage = imread("F:\\������1.jpg");
	//if (srcImage.empty())
	//{
	//	cout << "image file read error" << endl;

	//	return -1;
	//}
	////ͼ��ת��Ϊ�Ҷ�ͼ��
	//Mat grayImage;
	//if (srcImage.channels() == 3)
	//{
	//	cvtColor(srcImage, grayImage, CV_RGB2GRAY);
	//}
	//else
	//{
	//	grayImage = srcImage.clone();
	//}
	////����ͼ����ݶȷ�ֵ
	//Mat gradientXImage;
	//Mat gradientYImage;
	//Mat gradientImage;
	//Scharr(grayImage, gradientXImage, CV_32F, 1, 0);
	//Scharr(grayImage, gradientYImage, CV_32F, 0, 1);
	////��Ϊ������Ҫ������������ҪX����ˮƽ,���Ը���Ĺ�עX������ݶȷ�ֵ,��ʡ�Ե�Y������ݶȷ�ֵ
	//subtract(gradientXImage, gradientYImage, gradientImage);
	////��һ��Ϊ��λͼ��
	//convertScaleAbs(gradientImage, gradientImage);
	////�����õ����ݶ�ͼ����ʲô����
	////imshow(windowNameString,gradientImage);
	////��ͼƬ������Ӧ��ģ����,ʹһЩ�������
	//Mat blurImage;
	//blur(gradientImage, blurImage, Size(9, 9));
	////ģ�����Ժ������ֵ��,�õ�����Ӧ�ĺڰ׶�ֵ��ͼ��,��ֵ������ֵ���Ը���ʵ���������
	//Mat thresholdImage;
	//threshold(blurImage, thresholdImage, 210, 255, THRESH_BINARY);
	////������ֵ��ͼ��
	////imshow(windowNameString,thresholdImage);
	////��ֵ���Ժ��ͼ��,������֮��ĺڰ�û����������,��Ҫ������̬ѧ����,������϶,�൱��С�͵ĺڶ�,ѡ�������
	////��Ϊ�ǳ���֮��ķ�϶,������Ҫѡ���ȴ��ڳ���
	//Mat kernel = getStructuringElement(MORPH_RECT, Size(21, 7));
	//Mat morphImage;
	//morphologyEx(thresholdImage, morphImage, MORPH_CLOSE, kernel);
	////������̬ѧ�����Ժ��ͼ��
	////imshow(windowNameString,morphImage);
	////����Ҫ������������������һ��,����ѡ�����͸�ʴ,����Ϊ�˱���ͼ�δ�С��������,Ӧ��ʹ����ͬ���������͸�ʴ
	////�ȸ�ʴ,��������������ĵط��������������,Ȼ�����ͻ���,��������,������������ʵ�����ѡ��
	//erode(morphImage, morphImage, getStructuringElement(MORPH_RECT, Size(3, 3)), Point(-1, -1), 4);
	//dilate(morphImage, morphImage, getStructuringElement(MORPH_RECT, Size(3, 3)), Point(-1, -1), 4);
	////������̬ѧ�����Ժ��ͼ��
	////imshow(windowNameString,morphImage);
	//vector<vector<Point2i>>contours;
	//vector<float>contourArea;
	////��������Ŀ���������в���,Ŀ����Ϊ�˼���ͼ�����
	//findContours(morphImage, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//namedWindow("aaa");
	//imshow("aaa", morphImage);
	////����������������Ҵ��
	//for (int i = 0; i < contours.size(); i++)
	//{
	//	contourArea.push_back(cv::contourArea(contours[i]));
	//}
	////�ҳ������������
	//double maxValue; Point maxLoc;
	//minMaxLoc(contourArea, NULL, &maxValue, NULL, &maxLoc);
	////�������������������С���������
	//RotatedRect minRect = minAreaRect(contours[maxLoc.x]);
	////Ϊ�˷�ֹ�Ҵ�,Ҫ���������ε�ƫб�ǶȲ��ܳ���
	////�������,�Ǿ���û�ҵ�
	//Mat resultImage;
	//if (minRect.angle<2.0)
	//{
	//	//�ҵ��˾��εĽǶ�,��������һ����ת����,���Ի�Ҫ���»��һ�������С����
	//	Rect myRect = boundingRect(contours[maxLoc.x]);
	//	//�����������Դͼ���л�����
	//	//rectangle(srcImage,myRect,Scalar(0,255,255),3,LINE_AA);
	//	//������ʾЧ��,�ҵĶԲ���
	//	//imshow(windowNameString,srcImage);
	//	//��ɨ���ͼ��ü�����,������Ϊ��Ӧ�Ľ��,����һЩX����ı߽�,���Զ�rect����һ��������
	//	myRect.x = myRect.x - (myRect.width / 20);
	//	myRect.width = myRect.width*1.1;
	//	resultImage = Mat(srcImage, myRect);
	//}
	//namedWindow("sss");
	//imshow("sss", resultImage);


	//int width = resultImage.cols;
	//int height = resultImage.rows;
	//uchar *raw = (uchar *)resultImage.data;//raw�д�ŵ���ͼ��ĵ�ַ
	//Image imageZbar(width, height, "Y800", raw, width * height);

	//ImageScanner scanner;
	//scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	////ɨ������  
	//scanner.scan(imageZbar); 

	////2����
	//CString str_barcode_chip;//CHIP ID��
	//CString str_barcode_sn;//SN��

	//Image::SymbolIterator symbol = imageZbar.symbol_begin();
	//if (imageZbar.symbol_begin() == imageZbar.symbol_end())
	//{
	//	MessageBox(NULL, TEXT("ʶ��ʧ��"), TEXT("������"), MB_DEFBUTTON1 | MB_DEFBUTTON2);
	//	return -1;
	//}
	//else
	//{
	//	str_barcode_chip = symbol->get_data().c_str();

	//	++symbol;

	//	str_barcode_sn = symbol->get_data().c_str();
	//}

	//MessageBox(NULL, TEXT("SN:") + str_barcode_sn + TEXT("\nCHIP ID:") + str_barcode_chip, TEXT("������"), MB_DEFBUTTON1 | MB_DEFBUTTON2);

}