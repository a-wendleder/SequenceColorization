#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>           // cv::Mat
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>     // cv::imread()
#include <opencv2/imgproc/imgproc.hpp>  
#include "BoolPtrImg.h"
#include "StaticMethods.h"
#include <fstream>

void StaticMethods::ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}


void StaticMethods::InGray8(cv::Mat & mat) {
	CV_Assert(mat.depth() == CV_8U);
	int channels = mat.channels();
	if (channels > 1) {
		cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
	}
}
void StaticMethods::GetTreshold(cv::Mat & mat, int x, int y, uchar & tres_dw, uchar & tres_up, int margin_dw, int margin_up)
{
	if (margin_up < 0)
		margin_up = margin_dw;
	InGray8(mat);
	int pix_value = mat.at<uchar>(x, y);
	int tresh_dw = pix_value - margin_dw,
		tresh_up = pix_value + margin_up;
	uchar t_dw = tresh_dw,
		t_up = tresh_up;
	;
	if (tresh_dw < 0)
		t_dw = 0;
	if (tresh_up > 255)
		t_up = 255;
	tres_dw = t_dw;
	tres_up = t_up;
}


BoolPtrImg StaticMethods::BpiFromMat(cv::Mat &mat, cv::Point2i pt, int margin_dw, int margin_up)
{
	return BpiFromMat(mat, pt.x, pt.y, margin_dw, margin_up);
}
BoolPtrImg StaticMethods::BpiFromMat(cv::Mat &mat, int x, int y, int margin_dw, int margin_up)
{
	uchar t_dw, t_up;
	GetTreshold(mat, x, y, t_dw, t_up, margin_dw, margin_up);
	return BpiFromMat(mat, t_dw, t_up);
}
BoolPtrImg StaticMethods::BpiFromMat(cv::Mat &mat, uchar tresh_dw, uchar tresh_up)
{
	BoolPtrImg bpi(mat.cols, mat.rows, false);
	for (int x = 0; x < mat.cols; x++)
	{
		for (int y = 0; y < mat.rows; y++)
		{
			uchar v = mat.at<uchar>(y, x);

			if (v >= tresh_dw && v <= tresh_up)
			{
				bpi.SetPixel(x, y);
			}
			else
				bpi.SetPixel(x, y, false);
		}
	}
	return bpi;
}

BoolPtrImg StaticMethods::BpiFromMat(cv::Mat &mat)
{
	BoolPtrImg bpi(mat.cols, mat.rows, false);
	for (int x = 0; x < mat.cols; x++)
	{
		for (int y = 0; y < mat.rows; y++)
		{
			if (mat.at<uchar>(y, x) == 255)
				bpi.SetPixel(x, y);
			else
				bpi.SetPixel(x, y, false);
		}
	}
	return bpi;
}

cv::Mat StaticMethods::OutlineAsGradient(BoolPtrImg & fill, BoolPtrImg & outline)
{
	if (!fill.CompareDimensions(outline))
		throw std::invalid_argument("Add function: Bpi size doesn't match.");
	BoolPtrImg outerFill = fill.Clone();
	outerFill.Add(outline);
	outerFill.Invert();
	

	cv::Mat outlineGrad = outline.GetMat();

	std::vector<cv::Point2i> outlinePoints = outline.GetPointVector();

	std::cout << "Found outline points: " << outlinePoints.size() << std::endl;

	cv::namedWindow("ProcessViewer");
	cv::Mat show(outlineGrad);
	cv::Mat res(outlineGrad);

	int inner = 0;
	int outer = 0;
	float sum = 0;
	int result = 0;
	for (size_t i = 0; i < outlinePoints.size(); i++)
	{

		inner = fill.GetProximity(outlinePoints[i].x, outlinePoints[i].y);
		outer = outerFill.GetProximity(outlinePoints[i].x, outlinePoints[i].y);
		sum = (float)(inner + outer);
		if (sum != 0)
			result = (int)((float)outer / sum * 255);
		outlineGrad.at<uchar>(outlinePoints[i]) = result;
		if (i % 240 == 0)
		{
			std::cout << "Updated viewer " << i << "/" << outlinePoints.size() << std::endl;
			show = outlineGrad.clone();
			cv::putText(show, "Calculating " + std::to_string(i) + "/" + std::to_string(outlinePoints.size()), cv::Point2i(0, 120), cv::FONT_HERSHEY_COMPLEX, 3, cv::Scalar(255), 8, 8, false);
			cv::imshow("ProcessViewer", show);
			cv::waitKey(1);
		}
	}
	cv::add(outlineGrad, fill.GetMat(), res);
	cv::destroyWindow("ProcessViewer");
	return res;
}

void StaticMethods::WriteContoursToSVG(cv::Mat & mat, std::string filePath, int simplify)
{
	std::vector<std::vector<cv::Point>> points;
	cv::findContours(mat, points, cv::RETR_TREE, cv::CHAIN_APPROX_TC89_L1);
	;
	std::cout << "Storing " << points[0].size();
	std::cout << " points here: " << filePath;

	std::ofstream myfile;
	myfile.open(filePath);
	myfile << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << std::endl;
	myfile << "<!--Generator: Arun Crappy Fill Tool-->" << std::endl;
	myfile << "<!DOCTYPE svg PUBLIC \" -//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << std::endl;

	if (points[0].size() > simplify)
		for (size_t i = 0; i < simplify; i++)
			points[0].pop_back();

	myfile << "<svg version = \"1.1\" id = \"mySvg\" xmlns = \"http://www.w3.org/2000/svg\" xmlns:xlink = \"http://www.w3.org/1999/xlink\" x = \"0px\" y = \"0px\" ";
	myfile << "width=\"" << mat.cols << "px\" height=\"" << mat.rows << "px\" ";
	myfile << "viewBox=\"0 0 " << mat.cols << " " << mat.rows << "\" ";
	myfile << "enable-background=\"new 0 0 " << mat.cols << " " << mat.rows << "\" xml:space = \"preserve\" > ";
	myfile << std::endl;

	myfile << "<path d = \"M";
	// TODO even distribution
	simplify++;
	for (size_t i = 0; i < points[0].size(); i++)
		if (i%simplify == 0)
			myfile << points[0][i].x << " " << points[0][i].y << " ";
	myfile << "Z\" />" << std::endl;
	myfile << "</svg>";

	myfile.close();
}

