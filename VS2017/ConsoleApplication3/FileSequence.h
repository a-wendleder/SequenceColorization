#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <experimental\filesystem>
#include <filesystem>
#include "BoolPtrImg.h"
#include <opencv2/core/core.hpp>           // cv::Mat
#include <opencv2/highgui/highgui.hpp>     // cv::namedWindow
#include <opencv2/imgcodecs/imgcodecs.hpp> // cv::imread()
#include <opencv2/imgproc/imgproc.hpp>     // cv::treshold() I think

class SequenceFile
{
public:
	std::experimental::filesystem::path path;
	std::string basename;
	std::string extension;
	int index;
	int padding;
	SequenceFile(std::experimental::filesystem::path path);
};
class FileSequence
{
public:
	std::experimental::filesystem::path path;
	std::string basename;
	std::string extension;
	std::vector<int> index;
	int padding;

	std::string padInt(int i, int p);
	int Size();
	std::string getFileAt(int i);

	FileSequence();
	FileSequence(SequenceFile seqFile);
	void Init(SequenceFile seqFile);
	bool Add(SequenceFile seqFile);

	FileSequence& operator=(const FileSequence & other);


	void Sort();
	void Print();

	std::vector<std::string> Paths();

	
	static FileSequence FileSequenceFromFolder(std::string path = "", int min_padding = 1);
};
class SequenceEditor {
private:
	std::vector<BoolPtrImg> bpis;
	std::vector<std::vector<BoolPtrImgProperties>> islands;
	FileSequence fileSequence;
	
	uchar treshold = 128;
	std::string cv_WindowName;
	int playhead = 0;

	void EvalInput();
public:
	SequenceEditor(FileSequence &fs, std::string windowName = "Sequence Viewer");
	
	bool IsCached() { return (bpis.size() > 0); };
	int Length() { return fileSequence.Size(); };

	void SetTreshold();
	void RemoveIslands(bool val);
	void CalculateIslands();

	std::vector<BoolPtrImg> Images() { return bpis; };
	std::vector<std::vector<BoolPtrImgProperties>> Islands() { return islands; };

	void View();
	void PlayheadNavigation(uchar k);
	void MoveHeadRelative(int i);
};