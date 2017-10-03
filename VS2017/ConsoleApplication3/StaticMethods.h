#pragma once
#include <numeric>
#include <opencv2/core.hpp>  
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>  
#include "BoolPtrImg.h"
#include <iostream>
#include <chrono>

class StaticMethods
{
public:
	static void ReplaceAll(std::string & str, const std::string & from, const std::string & to);
	static void InGray8(cv::Mat& mat);

	static void GetTreshold(cv::Mat &mat, int x, int y, uchar & tres_dw, uchar & tres_up, int margin_dw, int margin_up = -1);

	static BoolPtrImg BpiFromMat(cv::Mat &mat, int x, int y, int margin_dw, int margin_up = -1);
	static BoolPtrImg BpiFromMat(cv::Mat &mat, cv::Point2i pt, int margin_dw, int margin_up = -1);
	static BoolPtrImg BpiFromMat(cv::Mat &mat, uchar tres_dw, uchar tres_up);
	static BoolPtrImg BpiFromMat(cv::Mat &mat);



	static cv::Mat OutlineAsGradient(BoolPtrImg & fill, BoolPtrImg & outline);
	static void StaticMethods::WriteContoursToSVG(cv::Mat &mat, std::string filePath, int simplify = 0);

	template <typename T, typename Compare>
	static std::vector<std::size_t> sort_permutation(const std::vector<T>& vec, Compare& compare);

	template <typename T>
	static void apply_permutation_in_place(std::vector<T>& vec, const std::vector<std::size_t>& p);

	template<typename TimeT = std::chrono::milliseconds>
	struct measure
	{
		template<typename F, typename ...Args>
		static typename TimeT::rep execution(F&& func, Args&&... args)
		{
			auto start = std::chrono::steady_clock::now();
			std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
			auto duration = std::chrono::duration_cast< TimeT>
				(std::chrono::steady_clock::now() - start);
			return duration.count();
		}
	};


};

/* directly stolen from https://stackoverflow.com/a/17074810*/
template<typename T, typename Compare>
inline std::vector<std::size_t> StaticMethods::sort_permutation(const std::vector<T>& vec, Compare & compare)
{
	std::vector<std::size_t> p(vec.size());
	std::iota(p.begin(), p.end(), 0);
	std::sort(p.begin(), p.end(),
		[&](std::size_t i, std::size_t j) { return compare(vec[i], vec[j]); });
	return p;
}

template<typename T>
inline void StaticMethods::apply_permutation_in_place(std::vector<T>& vec, const std::vector<std::size_t>& p)
{
	std::vector<bool> done(vec.size());
	for (std::size_t i = 0; i < vec.size(); ++i)
	{
		if (done[i])
		{
			continue;
		}
		done[i] = true;
		std::size_t prev_j = i;
		std::size_t j = p[i];
		while (i != j)
		{
			std::swap(vec[prev_j], vec[j]);
			done[j] = true;
			prev_j = j;
			j = p[j];
		}
	}
}
