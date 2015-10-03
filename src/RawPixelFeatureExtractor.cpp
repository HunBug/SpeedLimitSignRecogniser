/*
 * RawPixelFeatureExtractor.cpp
 *
 *  Created on: 2015. okt. 3.
 *      Author: Ákos
 */

#include "RawPixelFeatureExtractor.h"
#include <opencv2/imgproc.hpp>

namespace slsr {

RawPixelFeatureExtractor::RawPixelFeatureExtractor() :
		m_convertToGrayPixles(true) {
	// TODO Auto-generated constructor stub

}

RawPixelFeatureExtractor::~RawPixelFeatureExtractor() {
	// TODO Auto-generated destructor stub
}

std::vector<double> RawPixelFeatureExtractor::extractFeatures(cv::Mat source) {
	cv::Mat workImage = source;
	std::vector<double> features;
	if (m_convertToGrayPixles) {
		cv::cvtColor(workImage, workImage, CV_BGR2GRAY);
	}
	workImage.convertTo(workImage, cv::DataType<double>::type, 1, 0);
	workImage.copyTo(features);
	return features;
}

} /* namespace slsr */
