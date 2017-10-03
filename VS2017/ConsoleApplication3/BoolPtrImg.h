#pragma once
#include <iostream>
#include <opencv2\core.hpp>
#include <opencv2\core.hpp>

class BoolPtrImg;  // forward declaration

class BoolPtrImgProperties {
private:
	cv::Point2i medianPixel;
	cv::Point2f centerOfMass;
	long size;
public:
	cv::Point2i GetMedianPixel() { return medianPixel; };
	cv::Point2f GetCenterOfMass() { return centerOfMass;  };
	long GetSize() { return size; };
	uint id;

	float key;

	BoolPtrImgProperties(BoolPtrImg & bpi);
	float Match(BoolPtrImgProperties & other, int w, int h);

};

class BoolPtrImg
{
private:
	int width;
	int height;
	int no_of_pixels;

	bool isBorderPixel4(int x, int y, bool out_def = false);
	bool isBorderPixel8(int x, int y, bool out_def = false );
	bool getFirstNeighbour(cv::Point2i &p, bool accept = true);

public:
	bool * ptr;
	int GetNumberOfPixels();

	int GetWidth() { return this->width; };
	int GetHeight() { return this->height; };

	BoolPtrImg& operator=(const BoolPtrImg& other);
	BoolPtrImg(const BoolPtrImg& other);
	BoolPtrImg(cv::Mat & mat, uchar treshold = 255);
	BoolPtrImg(int width, int height, bool init = true, bool init_val = false);
	~BoolPtrImg();

	bool Contains(int x, int y);
	void SetPixel(int x, int y, bool v = true);
	bool SetPixelChecked(int x, int y);
	bool GetPixel(int x, int y);
	cv::Size GetSize();
	long Size();

	bool CompareDimensions(BoolPtrImg & compare);
	std::vector<int> GetBoundsXYXY();

	int GetProximity(int x, int y, bool v = true);


	void Add(BoolPtrImg &bpi);
	void Fill(bool v = true);
	void Subtract(BoolPtrImg &bpi);
	void Multiply(BoolPtrImg &bpi);
	void Invert();
	void DrawRect(int min_x, int min_y, int max_x, int max_y, bool v = true);
	void OldFloodFill1(BoolPtrImg & dst_filled, int x, int y);
	void FloodFill(BoolPtrImg & dst_filled, int x, int y);
	BoolPtrImg GetEdge();
	void CleanEdge();
	BoolPtrImg GetOutlineFromInnerEdge(BoolPtrImg & edge);
	void RemoveIslands(long min_size = 0, bool type = true);

	void SortMatchingIslands(std::vector<BoolPtrImg> & bpis);

	std::vector<cv::Point2i> GetPointVector();
	std::vector<BoolPtrImg> GetIslands(long min_size = 0, bool type = true);

	void Erode();
	void Dilate();
	void morphologicalOp(bool out_def, int size = 0);	
	BoolPtrImg Clone();
	cv::Mat GetMat();
};