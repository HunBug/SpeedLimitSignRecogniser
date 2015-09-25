/*
 * ColorSegmentation.cpp
 *
 *  Created on: 2015. szept. 23.
 *      Author: a.strack
 */

#include "ColorSegmentation.h"
#include <vector>
#include <opencv2/imgproc.hpp>

namespace slsr {

ColorSegmentation::ColorSegmentation() {
	// TODO Auto-generated constructor stub

}

ColorSegmentation::~ColorSegmentation() {
	// TODO Auto-generated destructor stub
}

cv::Mat ColorSegmentation::getMask(cv::Mat source) {
	cv::Mat hsvSource;
	cv::cvtColor(source, hsvSource, CV_BGR2HSV);
	std::vector<cv::Mat> hsvChannels;
	cv::split(hsvSource, hsvChannels);


}

} /* namespace slsr */
