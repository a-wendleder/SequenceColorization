#include <ctime>
#include <memory>
#include <iostream>
#include <stack>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "BoolPtrImg.h"
#include "StaticMethods.h"

bool BoolPtrImg::isBorderPixel4(int x, int y, bool out_def)
{
	if (GetPixel(x, y) == out_def) return false;
	if (x < width - 1)
		if (GetPixel(x + 1, y) == out_def) return true;
	if (x > 0)
		if (GetPixel(x - 1, y) == out_def) return true;
	if (y < height - 1)
		if (GetPixel(x, y + 1) == out_def) return true;
	if (y > 0)
		if (GetPixel(x, y - 1) == out_def) return true;
	return false;
}

bool BoolPtrImg::isBorderPixel8(int x, int y, bool out_def)
{
	if (GetPixel(x, y) == out_def) return false;
	if (x < width - 1)
	{
		if (GetPixel(x + 1, y) == out_def) return true;
		if (y < height - 1) {
			if (GetPixel(x + 1, y + 1) == out_def) return true;
			if (GetPixel(x, y + 1) == out_def) return true;
		}
		if (y > 0) {
			if (GetPixel(x + 1, y - 1) == out_def) return true;
			if (GetPixel(x, y - 1) == out_def) return true;
		}
	}
	if (x > 0)
	{
		if (GetPixel(x - 1, y) == out_def) return true;
		if (y < height - 1) {
			if (GetPixel(x - 1, y + 1) == out_def) return true;
		}
		if (y > 0) {
			if (GetPixel(x - 1, y - 1) == out_def) return true;
		}
	}
	return false;
}

bool BoolPtrImg::getFirstNeighbour(cv::Point2i & p, bool accept)
{
	if (p.y < height - 1)
		if (GetPixel(p.x, p.y + 1) == accept) {
			p.y += 1;
			return true;
		}
	if (p.y > 0)
		if (GetPixel(p.x, p.y - 1) == accept) {
			p.y -= 1;
			return true;
		}
	if (p.x < width - 1)
	{
		if (GetPixel(p.x + 1, p.y) == accept) {
			p.x += 1;
			return true;
		}
	}
	if (p.x > 0) {
		if (GetPixel(p.x - 1, p.y) == accept) {
			p.x -= 1;
			return true;
		}
		if (p.x < height - 1)
			if (GetPixel(p.x - 1, p.y + 1) == accept) {
				p.x -= 1; p.y += 1;
				return true;
			}
		if (p.x > 0)
			if (GetPixel(p.x - 1, p.y - 1) == accept) {
				p.x -= 1; p.y -= 1;
				return true;
			}
	}
	if (p.x < width - 1)
	{
		if (p.x < height - 1)
			if (GetPixel(p.x + 1, p.y + 1) == accept) {
				p.x += 1; p.y += 1;
				return true;
			}
		if (p.x > 0)
			if (GetPixel(p.x + 1, p.y - 1) == accept) {
				p.x += 1; p.y -= 1;
				return true;
			}
	}
	return false;
}

void BoolPtrImg::morphologicalOp(bool out_def, int size)
{
	BoolPtrImg eroded = this->Clone();

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (isBorderPixel4(x, y, out_def))
				eroded.DrawRect(x - size, y - size, x + size, y + size, out_def);
		}
	}
	// TODO magic memcopy, would be better to switch pointers (?), but then the eroded.ptr mem gets freed on deconstruction
	std::memcpy(ptr, eroded.ptr, sizeof(bool)*no_of_pixels);
}


int BoolPtrImg::GetNumberOfPixels()
{
	return this->no_of_pixels;
}

BoolPtrImg & BoolPtrImg::operator=(const BoolPtrImg & other)
{
	if (this == &other)
		return *this;

	this->width = other.width;
	this->height = other.height;
	this->no_of_pixels = other.no_of_pixels;
	//std::cout << "Assignment operator freeing and allocating new ptr." << std::endl;
	delete[] ptr;
	this->ptr = new bool[no_of_pixels];
	std::memcpy(ptr, other.ptr, sizeof(bool)*no_of_pixels);
	return *this;
}

BoolPtrImg::BoolPtrImg(const BoolPtrImg& other)
{
	this->width = other.width;
	this->height = other.height;
	this->no_of_pixels = other.no_of_pixels;
	//std::cout << "Copy constructor allocating ptr." << std::endl;
	this->ptr = new bool[no_of_pixels];

	std::memcpy(ptr, other.ptr, sizeof(bool)*no_of_pixels);
}


BoolPtrImg::BoolPtrImg(cv::Mat & mat, uchar treshold)
{
	this->width = mat.cols;
	this->height = mat.rows;
	this->no_of_pixels = width * height;
	//std::cout << "Normal constructor allocating ptr." << std::endl;
	this->ptr = new bool[no_of_pixels];

	std::cout << width << " w h " << height << std::endl;

	StaticMethods::InGray8(mat);
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (mat.at<uchar>(y, x) >= treshold)
				SetPixel(x, y);
			else
				SetPixel(x, y, false);
		}
	}
}

BoolPtrImg::BoolPtrImg(int width, int height, bool init, bool init_val) {
	this->width = width;
	this->height = height;
	this->no_of_pixels = width * height;
	//std::cout << "Normal constructor allocating ptr." << std::endl;
	this->ptr = new bool[no_of_pixels];

	if (init) {
		for (int i = 0; i < this->no_of_pixels; i++)
		{
			ptr[i] = init_val;
		}
	}
}

BoolPtrImg::~BoolPtrImg() {
	//std::cout << "Deallocating BoolPtrImg mem." << std::endl ;
	delete[] ptr;
}
bool BoolPtrImg::Contains(int x, int y) {
	if (x < 0) return false;
	if (y < 0) return false;
	if (x >= width) return false;
	if (y >= height) return false;
	return true;
}
void BoolPtrImg::SetPixel(int x, int y, bool v) {
	ptr[y * width + x] = v;
}
bool BoolPtrImg::SetPixelChecked(int x, int y) {
	if (!Contains(x, y))
		return true;
	if (!GetPixel(x, y))
	{
		SetPixel(x, y);
		return false;
	}
	return true;
}
bool BoolPtrImg::GetPixel(int x, int y) {
	return ptr[y * width + x];
}

cv::Size BoolPtrImg::GetSize()
{
	return cv::Size(this->width, this->height);
}

long BoolPtrImg::Size()
{
	long s = 0;
	for (size_t i = 0; i < this->no_of_pixels; i++)
		if (ptr[i])
			s++;
	return s;
}

bool BoolPtrImg::CompareDimensions(BoolPtrImg & compare)
{
	if (this->width != compare.width || this->height != compare.height)
		return false;
	return true;
}

std::vector<int> BoolPtrImg::GetBoundsXYXY()
{
	int min_x = 0;
	int min_y = 0;
	int max_x = width;
	int max_y = height;
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			if (GetPixel(x, y)) { min_x = x; goto found_min_x; }
found_min_x:
	for (int x = width - 1; x > 0; x--)
		for (int y = 0; y < height; y++)
			if (GetPixel(x, y)) { max_x = x; goto found_max_x; }
found_max_x:
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			if (GetPixel(x, y)) { min_y = y; goto found_min_y; }
found_min_y:
	for (int y = height - 1; y > 0; y--)
		for (int x = 0; x < width; x++)
			if (GetPixel(x, y)) { max_y = y; goto found_max_y; }
found_max_y:

	std::vector<int> bounds { min_x, min_y, max_x, max_y };
	return bounds;
}


int BoolPtrImg::GetProximity(int x, int y, bool v)
{
	BoolPtrImg checked(width, height);

	std::stack<cv::Point2i> pts_search_active; // y tho, how about: long = y*width+x;
	std::stack<cv::Point2i> pts_search_next;

	pts_search_active.push(cv::Point2i(x, y));
	checked.SetPixel(x, y);

	int proximity = 0;
	while (pts_search_active.size() > 0) {
		x = pts_search_active.top().x;
		y = pts_search_active.top().y;
		pts_search_active.pop();

		if (v != GetPixel(x, y))
		{
			if (!checked.SetPixelChecked(x + 1, y))
				pts_search_next.push(cv::Point2i(x + 1, y));
			if (!checked.SetPixelChecked(x - 1, y))
				pts_search_next.push(cv::Point2i(x - 1, y));
			if (!checked.SetPixelChecked(x, y + 1))
				pts_search_next.push(cv::Point2i(x, y + 1));
			if (!checked.SetPixelChecked(x, y - 1))
				pts_search_next.push(cv::Point2i(x, y - 1));
		}
		else
		{
			return proximity;
		}

		if (pts_search_active.size() == 0) {
			proximity++;
			pts_search_active = pts_search_next;
			pts_search_next.empty();
		}
	}
	return proximity;
}

void BoolPtrImg::Add(BoolPtrImg &bpi)
{
	if (!CompareDimensions(bpi))
		throw std::invalid_argument("Add function: Bpi size doesn't match.");
	for (size_t i = 0; i < this->no_of_pixels; i++)
		if (bpi.ptr[i])
			ptr[i] = true;

}
void BoolPtrImg::Fill(bool v)
{
	for (size_t i = 0; i < this->no_of_pixels; i++)
			ptr[i] = v;
}
void BoolPtrImg::Subtract(BoolPtrImg & bpi)
{
	if (!CompareDimensions(bpi))
		throw std::invalid_argument("Add function: Bpi size doesn't match.");
	for (size_t i = 0; i < this->no_of_pixels; i++)
		if (bpi.ptr[i])
			ptr[i] = false;
}

void BoolPtrImg::Multiply(BoolPtrImg & bpi)
{
	if (!CompareDimensions(bpi))
		throw std::invalid_argument("Add function: Bpi size doesn't match.");
	for (size_t i = 0; i < this->no_of_pixels; i++)
		if (!bpi.ptr[i])
			ptr[i] = false;
}

void BoolPtrImg::Invert()
{
	for (size_t i = 0; i < this->no_of_pixels; i++)
		ptr[i] = !ptr[i];
}

void BoolPtrImg::DrawRect(int min_x, int min_y, int max_x, int max_y, bool v)
{
	if (min_x < 0) min_x = 0;
	if (min_y < 0) min_y = 0;
	if (max_x >= width) max_x = width - 1;
	if (max_y >= height) max_y = height - 1;

	for (int x = min_x; x <= max_x; x++)
		for (int y = min_y; y <= max_y; y++)
			SetPixel(x, y, v);
}

void BoolPtrImg::OldFloodFill1(BoolPtrImg & dst_filled, int x, int y)
{
	// Create an empty image, to store which pixel are going to be / were already checked and don't have to be added to the pts_search-list.
	BoolPtrImg checked(width, height);

	// Clear the destionation image (which will hold the filled shape), by filling it with black/false.
	dst_filled.Fill(false);

	// The stack will hold the coordinates of potential fill-pixels, which are going to be checked.
	std::stack<cv::Point2i> pts_search;

	// Add the starting coordinates to the search-list.
	pts_search.push(cv::Point2i(x, y));

	// Color in the starting coordinates, so we don't check them again.
	checked.SetPixel(x, y);

	// Get the value of the starting pixel. All neighbouring pixels with the same value (v) will be added to the filled shape.
	bool v = GetPixel(x, y);

	// If there are pixels remaining in the search coordinates list (pts_search), we will grab the first and consider its neighbours.
	while (pts_search.size() > 0) {

		// Get the coordinates of the top item, to examine, then remove it.
		x = pts_search.top().x;
		y = pts_search.top().y;
		pts_search.pop();

		// Check if the coordinates have the desired value.
		if (v == GetPixel(x, y))
		{
			// Color them in, in the filled shape output image.
			dst_filled.SetPixel(x, y);

			// Look up the neighbouring pixels in the 'checked' image. If they have not been checked, add them to the pts_search list and mark them as in the checked image.
			if (x < width - 1)
				if (!checked.GetPixel(x + 1, y)) {
					checked.SetPixel(x + 1, y);
					pts_search.push(cv::Point2i(x + 1, y));
				}
			if (x > 0)
				if (!checked.GetPixel(x - 1, y)) {
					checked.SetPixel(x - 1, y);
					pts_search.push(cv::Point2i(x - 1, y));
				}
			if (y < height - 1)
				if (!checked.GetPixel(x, y + 1)) {
					checked.SetPixel(x, y + 1);
					pts_search.push(cv::Point2i(x, y + 1));
				}
			if (y > 0)
				if (!checked.GetPixel(x, y - 1)) {
					checked.SetPixel(x, y - 1);
					pts_search.push(cv::Point2i(x, y - 1));
				}
		}
	}
}

void BoolPtrImg::FloodFill(BoolPtrImg & dst_filled, int x, int y)
{
	BoolPtrImg checked(width, height);
	dst_filled.Fill(false);

	checked.SetPixel(x, y);
	bool v = GetPixel(x, y);

	// Two pointers are used instead of the stack<Point2i>.
	int * x_values = new int[no_of_pixels];
	int * y_values = new int[no_of_pixels];

	// A read and write counter will be used to simulate the stack behaviour.
	// No items will be pop()ed, the read head will increment.
	size_t read_head = 0;
	size_t write_head = 0;

	x_values[write_head] = x;
	y_values[write_head] = y;
	write_head++;

	while (read_head < write_head) {

		x = x_values[read_head];
		y = y_values[read_head];
		read_head++;

		if (v == GetPixel(x, y))
		{

			dst_filled.SetPixel(x, y);
			if (x < width - 1)
				if (!checked.GetPixel(x + 1, y)) {
					checked.SetPixel(x + 1, y);
					x_values[write_head] = x + 1;
					y_values[write_head] = y;
					write_head++;
				}
			if (x > 0)
				if (!checked.GetPixel(x - 1, y)) {
					checked.SetPixel(x - 1, y);
					x_values[write_head] = x - 1;
					y_values[write_head] = y;
					write_head++;
				}
			if (y < height - 1)
				if (!checked.GetPixel(x, y + 1)) {
					checked.SetPixel(x, y + 1);
					x_values[write_head] = x;
					y_values[write_head] = y + 1;
					write_head++;
				}
			if (y > 0)
				if (!checked.GetPixel(x, y - 1)) {
					checked.SetPixel(x, y - 1);
					x_values[write_head] = x;
					y_values[write_head] = y - 1;
					write_head++;
				}
		}
	}
	delete[] x_values;
	delete[] y_values;
}

BoolPtrImg BoolPtrImg::GetEdge()
{
	BoolPtrImg Edge(width, height);
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (isBorderPixel8(x, y))
				Edge.SetPixel(x, y, true);
		}
	}
	return Edge;
}
void BoolPtrImg::CleanEdge()
{
	int connections = 0;
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (GetPixel(x, y)) {
				connections = 0;
				if (x > 0)
					if (GetPixel(x - 1, y))
						connections++;
				if (y > 0)
					if (GetPixel(x, y - 1))
						connections++;
				if (x < width - 1)
					if (GetPixel(x + 1, y))
						connections++;
				if (y < height - 1)
					if (GetPixel(x, y + 1))
						connections++;
				if (connections > 1)
				{

				}
			}
		}
	}
}

BoolPtrImg BoolPtrImg::GetOutlineFromInnerEdge(BoolPtrImg & edge)
{
	BoolPtrImg reverseEdge(width, height); //default black (false) 
	BoolPtrImg temp(width, height);
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
		{
			if (edge.GetPixel(x, y)) {
				if (!GetPixel(x + 1, y))
					if (!reverseEdge.GetPixel(x + 1, y)) {
						OldFloodFill1(temp, x + 1, y);
						reverseEdge.Add(temp);
					}
				if (!GetPixel(x - 1, y))
					if (!reverseEdge.GetPixel(x - 1, y)) {
						OldFloodFill1(temp, x - 1, y);
						reverseEdge.Add(temp);
					}
				if (!GetPixel(x, y + 1))
					if (!reverseEdge.GetPixel(x, y + 1)) {
						OldFloodFill1(temp, x, y + 1);
						reverseEdge.Add(temp);
					}
				if (!GetPixel(x, y - 1))
					if (!reverseEdge.GetPixel(x, y - 1)) {
						OldFloodFill1(temp, x, y - 1);
						reverseEdge.Add(temp);
					}
			}
		}

	return reverseEdge;
}

void BoolPtrImg::RemoveIslands(long max_size, bool type)
{
	int removed = 0;
	int untouched = 0;

	BoolPtrImg checked(width, height);
	BoolPtrImg island(width, height);

	std::cout << "Removing islands ..." << std::endl;
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			if (!checked.GetPixel(x, y))
			{
				if (this->GetPixel(x, y) == type)
				{
					this->FloodFill(island, x, y);

					if (island.Size() <= max_size) {
						removed++;
						if (type)
							this->Subtract(island);
						else
							this->Add(island);
					}
					else
						untouched++;
					checked.Add(island);
				}
				else
					checked.SetPixel(x, y);
			}
	std::cout << "Removed " << removed << " islands." << std::endl;
	std::cout << "Left " << untouched << " islands untouched." << std::endl;

}

void BoolPtrImg::SortMatchingIslands(std::vector<BoolPtrImg>& bpis)
{
	BoolPtrImgProperties compareProp(*this);
	std::vector<BoolPtrImgProperties> props;
	for (size_t i = 0; i < bpis.size(); i++) {
		BoolPtrImgProperties addProp = BoolPtrImgProperties(bpis[i]);
		addProp.Match(compareProp, width, height);
		props.push_back(addProp);
	}
	auto p = StaticMethods::sort_permutation(props, [](BoolPtrImgProperties const& a, BoolPtrImgProperties const& b) { return (a.key > b.key); });

	StaticMethods::apply_permutation_in_place(props, p);

	std::cout << "props keys " << std::endl;
	for (size_t i = 0; i < props.size(); i++)
	{
		std::cout << props[i].key << " ";
	}
	std::cout << std::endl;

	StaticMethods::apply_permutation_in_place(bpis, p);
}

std::vector<cv::Point2i> BoolPtrImg::GetPointVector()
{
	std::vector<cv::Point2i> edgePoints;

	cv::Point2i search(-1, -1);
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			if (GetPixel(x, y))
				edgePoints.push_back(cv::Point2i(x, y));
	return edgePoints;
}
std::vector<BoolPtrImg> BoolPtrImg::GetIslands(long min_size, bool type)
{
	int added = 0;
	int rejected = 0;

	std::vector<BoolPtrImg> islands;
	BoolPtrImg island(width, height);
	BoolPtrImg checked(width, height);

	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			if (!checked.GetPixel(x, y))
			{
				if (this->GetPixel(x, y) == type)
				{
					this->OldFloodFill1(island, x, y);
					if (island.Size() >= min_size) {
						islands.push_back(island);
						added++;
					}
					else
						rejected++;
					checked.Add(island);
				}
				else
					checked.SetPixel(x, y);
			}

	std::cout << std::endl << "Added " << added << std::endl;
	std::cout << std::endl << "Rejected " << rejected << std::endl;
	return islands;
}

void BoolPtrImg::Erode()
{
	morphologicalOp(false);
}
void BoolPtrImg::Dilate()
{
	morphologicalOp(true);
}
BoolPtrImg BoolPtrImg::Clone()
{
	BoolPtrImg clone(this->width, this->height);
	for (int i = 0; i < this->no_of_pixels; i++)
	{
		if (ptr[i])
			clone.ptr[i] = true;
	}
	return clone;
}
cv::Mat	BoolPtrImg::GetMat() {
	cv::Mat mat(this->height, this->width, CV_8UC1, cv::Scalar(0));
	for (int x = 0; x < this->width; x++)
	{
		for (int y = 0; y < this->height; y++)
		{
			if (GetPixel(x, y))
				mat.at<uchar>(y, x) = 255;
		}
	}
	return mat;
}

BoolPtrImgProperties::BoolPtrImgProperties(BoolPtrImg & bpi)
{
	long size = 0;
	double sum_x = 0;
	double sum_y = 0;

	int width = bpi.GetWidth();
	int height = bpi.GetHeight();

	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			if (bpi.GetPixel(x, y)) {
				sum_x += x;
				sum_y += y;
				size++;
			}
	sum_x /= size;
	sum_y /= size;

	cv::Point2f centerOfMass(static_cast<float>(sum_x), static_cast<float>(sum_y));

	int close_x = width;
	int close_y = height;
	double distance_close = pow((pow(width, 2) + pow(height, 2)), 0.5);
	double distance_eval;
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			if (bpi.GetPixel(x, y)) {
				distance_eval = pow((pow(x - sum_x, 2) + pow(y - sum_y, 2)), 0.5);
				if (distance_close > distance_eval) {
					distance_close = distance_eval;
					close_x = x;
					close_y = y;
				}
			}
	cv::Point2i medianPixel(close_x, close_y);


	this->medianPixel = medianPixel;
	this->centerOfMass = centerOfMass;
	this->size = size;
}

float BoolPtrImgProperties::Match(BoolPtrImgProperties & other, int w, int h)
{

	float m_size = (float)size / (float)other.size;
	if (m_size > 1) m_size = (float)1 / m_size;

	double m_centerOfMass_distance = pow(pow((centerOfMass.x - other.centerOfMass.x), 2) + pow((centerOfMass.y - other.centerOfMass.y), 2), 0.5);
	double diagonal = pow(pow(w, 2) + pow(h, 2), 0.5);
	double m_centerOfMass = m_centerOfMass_distance / diagonal;

	float match = (float)(2 * m_size + m_centerOfMass)/3;

	key = match;

	std::cout << centerOfMass.x << " " << centerOfMass.y << " - " << other.centerOfMass.x << " " << other.centerOfMass.y << " = "  << m_centerOfMass_distance <<  " > " << m_centerOfMass << "  < match " << std::endl;

	return match;
}
