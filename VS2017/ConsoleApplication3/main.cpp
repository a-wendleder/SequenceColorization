#include <iostream>        
#include <chrono>               // std::cout
#include <array>
#include <stack>
#include <set>      
#include <algorithm>
#include <numeric>
#include <chrono>
#include <string>
#include <opencv2/core/core.hpp>           // cv::Mat
#include <opencv2/highgui/highgui.hpp>     // cv::namedWindow
#include <opencv2/imgcodecs/imgcodecs.hpp> // cv::imread()
#include <opencv2/imgproc/imgproc.hpp>     // cv::Canny()
#include <experimental/filesystem>
#include "BoolPtrImg.h"
#include "StaticMethods.h"
#include "FileSequence.h"


using namespace std;
using namespace cv;



BoolPtrImg floodfillGray(Mat& img, int x, int y, int threshold = 30) {
	StaticMethods::InGray8(img);

	int pix_value = img.at<uchar>(Point2i(x, y));
	int limit_dw = pix_value - threshold,
		limit_up = pix_value + threshold;

	cout << endl << "Starting flood fill"; std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();


	BoolPtrImg bpi(img.cols, img.rows);
	BoolPtrImg filled(img.cols, img.rows);
	stack<Point2i> pts_search;
	pts_search.push(Point2i(x, y));
	bpi.SetPixel(x, y);
	Point2i pt;

	while (pts_search.size() > 0) {

		pt = pts_search.top();
		pts_search.pop();
		if (img.at<uchar>(pt) >= limit_dw && img.at<uchar>(pt) <= limit_up)
		{
			filled.SetPixel(pt.x, pt.y);
			if (!bpi.SetPixelChecked(pt.x + 1, pt.y))
				pts_search.push(Point2i(pt.x + 1, pt.y));
			if (!bpi.SetPixelChecked(pt.x - 1, pt.y))
				pts_search.push(Point2i(pt.x - 1, pt.y));
			if (!bpi.SetPixelChecked(pt.x, pt.y - 1))
				pts_search.push(Point2i(pt.x, pt.y - 1));
			if (!bpi.SetPixelChecked(pt.x, pt.y + 1))
				pts_search.push(Point2i(pt.x, pt.y + 1));

		}
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	cout << endl << "Flood fill ended" << endl; std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;

	return filled;
} 


void a(); void b(Point p); void c(int x, int y); void d(int x, int y); void e(int x, int y); void f(int x, int y);
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == (CV_EVENT_LBUTTONUP))
	{
		e(x, y);
		cout << "Left mouse button position (" << x << ", " << y << ")" << endl;
	}
	if (event == (CV_EVENT_MBUTTONUP))
	{
		b(Point(x, y));
		cout << "Middle mouse button position (" << x << ", " << y << ")" << endl;
	}
	if (event == (CV_EVENT_RBUTTONUP))
	{
		a();
	}
}
Mat img;
void a()
{
	imshow("My Window", img);
}
void b(Point p)
{
	BoolPtrImg bpi = StaticMethods::BpiFromMat(img, p.x, p.y, 40, 40);
	imshow("My Window", bpi.GetMat());
	cout << "This is the bpi." << endl;
	waitKey(0);

	BoolPtrImg filled = bpi;
	bpi.OldFloodFill1(filled, p.x, p.y);
	imshow("My Window", filled.GetMat());
	cout << "This is filled." << endl;
	waitKey(0);

	std::cout << "Done.";


}
void c(int x, int y)
{
	BoolPtrImg bpi = StaticMethods::BpiFromMat(img, x, y, 60, 60);
	imshow("My Window", bpi.GetMat());
	cout << "This is the bpi." << endl;
	waitKey(0);

	BoolPtrImg filled = bpi;
	bpi.OldFloodFill1(filled, x, y);
	imshow("My Window", filled.GetMat());
	cout << "This is filled." << endl;
	waitKey(0);
	
	Mat mat = filled.GetMat();

	StaticMethods::WriteContoursToSVG(mat, "D:/170426 cpp imageswap/svg.svg", 3);

	waitKey(0);
}
void d(int x, int y)
{
	BoolPtrImg bpi = StaticMethods::BpiFromMat(img, x, y, 60, 60);
	imshow("My Window", bpi.GetMat());
	cout << "This is the bpi." << endl;
	waitKey(0);

	BoolPtrImg filled = bpi;
	bpi.OldFloodFill1(filled, x, y);
	BoolPtrImg cleanFill = filled;
	cleanFill.RemoveIslands(256);
	imshow("My Window", cleanFill.GetMat());
	cout << "This is clean filled." << endl;
	waitKey(0);


	BoolPtrImg InMask = cleanFill.Clone();
	InMask.morphologicalOp(true, 12);
	imshow("My Window", InMask.GetMat());
	cout << "This is larger inMask." << endl;
	waitKey(0);
	
	BoolPtrImg edge = cleanFill.GetEdge();

	BoolPtrImg outline = bpi.GetOutlineFromInnerEdge(edge);
	imshow("My Window", outline.GetMat());
	cout << "This is outline." << endl;
	waitKey(0);

	outline.Multiply(InMask);
	imshow("My Window", outline.GetMat());
	cout << "This is relevant outline." << endl;
	waitKey(0);


	Mat mat = StaticMethods::OutlineAsGradient(filled, outline);

	imshow("My Window", mat);
	cout << "This is crazy mat." << endl;
	waitKey(0);
	//imwrite("D:/170426 cpp imageswap/frame_edit.png", mat);


}
void e(int x, int y)
{
	Mat imgA = imread("D:/170426 cpp imageswap/seq/0001.png");
	uchar tres_dw = 0, tres_up = 0;
	StaticMethods::GetTreshold(imgA, x, y, tres_dw, tres_up, 24);
	BoolPtrImg bpiA = StaticMethods::BpiFromMat(imgA, tres_dw, tres_up);

	cout << "Plain." << endl;
	imshow("My Window", imgA);
	waitKey(0);

	cout << "This is the bpi." << endl;
	imshow("My Window", bpiA.GetMat());
	waitKey(0);

	BoolPtrImg filled = bpiA;
	bpiA.OldFloodFill1(filled, x, y);
	imshow("My Window", filled.GetMat());
	cout << "This is the A filled." << endl;
	waitKey(0);


	FileSequence inputSeq = FileSequence::FileSequenceFromFolder("D:/170426 cpp imageswap/seq");
	inputSeq.Sort();
	FileSequence outputSeq = inputSeq;
	outputSeq.path = std::experimental::filesystem::path("D:/170426 cpp imageswap/test");
	for (int j = 0; j < inputSeq.Size(); j++)
	{
		cout << (j+1) << "/" << inputSeq.Size() << " " << inputSeq.getFileAt(j) << endl;


		Mat imgB = imread(inputSeq.getFileAt(j));
		StaticMethods::InGray8(imgB);	
		BoolPtrImg bpiB = StaticMethods::BpiFromMat(imgB, tres_dw, tres_up);
		vector<BoolPtrImg> islands = bpiB.GetIslands(200);

		bpiA.SortMatchingIslands(islands);
		imshow("My Window", islands[0].GetMat());
		waitKey(1);
		bpiA = islands[0];
		string path = outputSeq.getFileAt(j);
		imwrite(path, bpiA.GetMat());

	}

	waitKey(0);
}


int main(int argc)
{/*
	typedef std::chrono::high_resolution_clock Time;
	typedef std::chrono::milliseconds ms;
	typedef std::chrono::duration<float> fsec;
	auto t0 = Time::now();
	auto t1 = Time::now();
	fsec fs = t1 - t0;
	ms d = std::chrono::duration_cast<ms>(fs);
	std::cout << fs.count() << "s\n";
	std::cout << d.count() << "ms\n";*/


	FileSequence fs = FileSequence::FileSequenceFromFolder("D:/170426 cpp imageswap/seq/");
	//FileSequence::FileSequenceFromFolder();

	SequenceEditor se(fs);

	img = imread("D:/170426 cpp imageswap/seq/0001.png");
	BoolPtrImg bpi(img, 170);
	BoolPtrImg filled = bpi;
	//filled.Fill(false);

	/*
	auto t0 = Time::now();
	for (size_t i = 0; i < 100; i++)
		bpi.OldFloodFill1(filled, 10, 10);
	auto t1 = Time::now();
	fsec fs = t1 - t0;
	ms d = std::chrono::duration_cast<ms>(fs);
	std::cout << d.count() << "ms\n";

	imshow("My Window", filled.GetMat());

	waitKey();
	filled = bpi;

	cout << endl;
	t0 = Time::now();
	for (size_t i = 0; i < 100; i++)
		bpi.FloodFill(filled, 10, 10);
	t1 = Time::now();
	fs = t1 - t0;
	d = std::chrono::duration_cast<ms>(fs);
	std::cout << d.count() << "ms\n";


	imshow("My Window", filled.GetMat());

	waitKey();
	std::string a;
	cin >> a;*/



    /*BoolPtrImg bpi = StaticMethods::BpiFromMat(img);

	namedWindow("My Window", WINDOW_GUI_EXPANDED);
	setMouseCallback("My Window", CallBackFunc, NULL);
	a();*/
	

	waitKey(0);

	return 0;
}