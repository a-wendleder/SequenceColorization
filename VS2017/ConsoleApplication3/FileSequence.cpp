#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <experimental\filesystem>
#include <filesystem>
#include <iomanip>
#include <typeinfo>
#include <sstream>
#include <opencv2/core/core.hpp>           // cv::Mat
#include <opencv2/imgcodecs/imgcodecs.hpp> // cv::imread()
#include <opencv2/imgproc/imgproc.hpp>     // cv::treshold() I think
#include <opencv2/highgui/highgui.hpp>     // cv::namedWindow
#include <stdlib.h>
#include <stdio.h>

#include "StaticMethods.h"
#include "BoolPtrImg.h"
#include "FileSequence.h"

using namespace std;
namespace fs = std::experimental::filesystem;


SequenceFile::SequenceFile(std::experimental::filesystem::path path)
{
	this->path = path.parent_path();
	this->basename = path.stem().string();
	this->extension = path.extension().string();

	this->padding = 0;
	this->index = 0;

	string basename_orig = this->basename;

	for (int i = 1; i < basename_orig.length(); i++)
	{
		string tryPadding = basename_orig.substr(basename_orig.length() - i, i);
		int parsed = std::atoi(tryPadding.c_str());
		std::stringstream stream;
		stream.fill('0');
		stream.width(i);
		stream << parsed;
		if (stream.str() == tryPadding) {
			this->padding = i;
			this->index = parsed;
			this->basename = basename_orig.substr(0, basename_orig.length() - i);
		}
		else
		{
			return;
		}
	}
}


std::string FileSequence::padInt(int i, int p)
{
	std::stringstream stream;
	stream.fill('0');
	stream.width(p);
	stream << i;
	return stream.str();
}

int FileSequence::Size()
{
	return (int)index.size();
}

std::string FileSequence::getFileAt(int i)
{
	return path.string() + "/" + basename + this->padInt(this->index[i], this->padding) + extension;
}

FileSequence::FileSequence()
{
}

FileSequence::FileSequence(SequenceFile seqFile)
{
	Init(seqFile);
}

void FileSequence::Init(SequenceFile seqFile)
{
	basename = seqFile.basename;
	extension = seqFile.extension;
	index.clear();
	index.push_back(seqFile.index);
	padding = seqFile.padding;
	path = seqFile.path;
}

bool FileSequence::Add(SequenceFile seqFile)
{
	if (this->basename != seqFile.basename) return false;
	if (this->extension != seqFile.extension) return false;
	if (this->padding != seqFile.padding) return false;
	if (this->path != seqFile.path) return false;

	this->index.push_back(seqFile.index);

	return true;
}

FileSequence & FileSequence::operator=(const FileSequence & other)
{
	path = other.path;
	basename = other.basename;
	extension = other.extension;
	index = other.index;
	padding = other.padding;
	return *this;
}

void FileSequence::Sort()
{
	std::sort(index.begin(), index.end());
}

void FileSequence::Print()
{
	cout << basename;
	for (size_t i = 0; i < padding; i++) cout << "#";
	cout << extension << " [" << index.size() << "] ";
	int start = index[0];
	int end = index[0];
	cout << start;
	for (size_t i = 0; i < index.size(); i++)
	{
		end = index[i];
		if (i > 0) {
			if (index[i - 1] < index[i] - 1) {
				if (start < index[i - 1]) {
					cout << "-" << index[i - 1];
				}
				start = index[i];
				cout << ", " << start;
			}
		}
	}
	if (start < end)
		cout << "-" << end << endl;

	cout << endl;
}

std::vector<string> FileSequence::Paths()
{
	std::vector<string> paths;

	for (int i = 0; i < this->index.size(); i++)
	{
		std::stringstream stream;
		stream.fill('0');
		stream.width(this->padding);
		stream << this->index[i];
		string p = this->path.string() + "\\" + this->basename + stream.str() + this->extension;
		paths.push_back(p);
	}
	return paths;
}

FileSequence FileSequence::FileSequenceFromFolder(std::string path, int min_padding)
{
	if (fs::exists(fs::path(path))) { goto inputChecked; }

	std::cout << "Type directory name. Press Enter." << endl;
	std::getline(std::cin, path, '\n');
	while (!fs::exists(fs::path(path))) {
		if (path != "")
			std::cout << "\"" << path << "\" doesn't exist. Try another." << endl;
		std::getline(std::cin, path, '\n');
	}

inputChecked:

	vector<FileSequence> FileSequencesFromFolder;
	std::vector<fs::path> dirs;

	for (auto & p : fs::directory_iterator(path))
	{
		fs::path pa = p.path();
		if (pa.has_extension())
		{
			SequenceFile seqFile(pa);
			if (seqFile.padding >= min_padding) {
				for (size_t i = 0; i < FileSequencesFromFolder.size(); i++)
				{
					if (FileSequencesFromFolder[i].Add(seqFile)) {
						goto sortedAndShelved;
					}
				}
				FileSequencesFromFolder.push_back(FileSequence(seqFile));
			}
		sortedAndShelved:
			// Nothing else happens here.
			true;
		}
	}
	if (FileSequencesFromFolder.size() == 0) {
		cout << "No File Sequence Found" << endl;
		return FileSequenceFromFolder();
	}
	for (size_t i = 0; i < FileSequencesFromFolder.size(); i++)
	{
		cout << (i + 1) << "     ";
		FileSequencesFromFolder[i].Print();
	}

	cout << "Choose #" << endl;
	string in_no;
	cin >> in_no;
	int in_int = std::atoi(in_no.c_str());


	while (!(in_int - 1 < FileSequencesFromFolder.size() && in_int > 0)) {
		cout << "Not in range. Choose correct #." << endl;
		cin >> in_no;
		in_int = std::atoi(in_no.c_str());
	}

	return FileSequencesFromFolder[in_int - 1];
}






SequenceEditor::SequenceEditor(FileSequence & fs, std::string windowName)
{
	fileSequence = fs;
	cv_WindowName = windowName;
	cv::namedWindow(cv_WindowName, cv::WINDOW_KEEPRATIO);
	SetTreshold();
	View();
}

void SequenceEditor::SetTreshold()
{
	std::vector<cv::Mat> mats;
	cv::Mat m = cv::	imread("D:/170426 cpp imageswap/seq/0001.png");
	cout << m.cols << "  cols" << endl;

	// load sequence into cache
	vector<string> paths = fileSequence.Paths();

	cv::Mat mat;
	for (int i = 0; i < paths.size(); i++) {
		mat = cv::imread(paths[i]);
		cout << paths[i] << endl;
		mats.push_back(mat);
	}

	uint k = 0;

	while (k != 13)
	{
		PlayheadNavigation(k);
		switch (k) {
		case 104:
			this->treshold--;
			break;
		case 106:
			this->treshold++;
			break;
		}
		mat = mats.at(playhead);
		cv::cvtColor(mat, mat, CV_BGR2GRAY);
		cv::threshold(mat, mat, this->treshold, 255, 0);
		cout << playhead << " " << static_cast<unsigned>((this->treshold)) <<" " << k << endl;
		
		cv::imshow(cv_WindowName, mat);

		k = cv::waitKey();
	}

	for (int i = 0; i < mats.size(); i++) {
		this->bpis.push_back(BoolPtrImg(mats[i], treshold));
	}
}

void SequenceEditor::RemoveIslands(bool val)
{
	cout << "Remove islands " << val << endl;

	uint k = 0;
	long s = 0;
	BoolPtrImg bpi = bpis[playhead];
	while (k != 13)
	{
		PlayheadNavigation(k);
		switch (k) {
		case 104:
			s--;
			break;
		case 106:
			s++;
			break;
		}
		bpi = bpis[playhead];
		bpi.RemoveIslands(s, val);
		cout << "Max island size: " << s << endl;
		cv::imshow(cv_WindowName, bpi.GetMat());

		k = cv::waitKey();
	}
	cout << "Removing islands... ";
	for (int i = 0; i < bpis.size(); i++)
		bpis[i].RemoveIslands(s, val);
	cout << "finished." << endl;
}

void SequenceEditor::View()
{
	cout << "Evaluate bpis";
	uint k = 0;
	while (k != 13)
	{
		PlayheadNavigation(k);
		switch (k) {
		case 113:
			RemoveIslands(false);
			break;
		case 119:
			RemoveIslands(true);
			break;
		}
		cout << static_cast<unsigned>((k)) << endl;

		cv::imshow(cv_WindowName, bpis[playhead].GetMat());

		k = cv::waitKey();
	}
}

void SequenceEditor::PlayheadNavigation(uchar k)
{
	switch (k) {
	case 98:
		MoveHeadRelative(-1);
		break;
	case 110:
		MoveHeadRelative(1);
		break;
	}
}

void SequenceEditor::MoveHeadRelative(int i)
{
	playhead += i + Length();
	playhead %= Length();
}