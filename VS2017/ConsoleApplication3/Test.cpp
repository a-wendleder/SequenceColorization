#include <iostream>
# include "opencv2/core/core.hpp"
# include "opencv2/features2d.hpp"
#include "opencv2/features2d/features2d.hpp"
# include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/calib3d/calib3d.hpp"

# include "opencv2/xfeatures2d.hpp"
# include "opencv2/xfeatures2d/nonfree.hpp"


#include <string>


using namespace cv;
using namespace std;

using namespace cv::xfeatures2d;

/**
* @function main
* @brief Main function
*/
int fmain()
{
	Mat img1 = imread("D:/170426 cpp imageswap/graf1.png", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img2 = imread("D:/170426 cpp imageswap/graf3.png", CV_LOAD_IMAGE_GRAYSCALE);
	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 400;

	Ptr<FeatureDetector> detector = FastFeatureDetector::create(15);
	vector<KeyPoint> keypoints1, keypoints2;
	detector->detect(img1, keypoints1);
	detector->detect(img2, keypoints2);

	Ptr<SURF> extractor = SURF::create();
	Mat descriptors1, descriptors2;
	extractor->compute(img1, keypoints1, descriptors1);
	extractor->compute(img2, keypoints2, descriptors2);


	BFMatcher matcher;
	vector<DMatch> matches;
	matcher.match(descriptors1, descriptors2, matches);

	namedWindow("matches", 1);
	Mat img_matches;
	drawMatches(img1, keypoints1, img2, keypoints2, matches, img_matches);
	imshow("matches", img_matches);
	waitKey(0);

	waitKey(0);

	string inp;
	cin >> inp;
	return 0;
}