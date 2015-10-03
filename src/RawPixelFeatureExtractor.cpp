/*
 * RawPixelFeatureExtractor.cpp
 *
 *  Created on: 2015. okt. 3.
 *      Author: Ákos
 */

#include "RawPixelFeatureExtractor.h"
#include <iostream>
#include <opencv2/imgproc.hpp>

namespace slsr {

RawPixelFeatureExtractor::RawPixelFeatureExtractor() :
		m_convertToGrayPixles(true) {
	// TODO Auto-generated constructor stub

}

RawPixelFeatureExtractor::~RawPixelFeatureExtractor() {
	// TODO Auto-generated destructor stub
}

std::vector<float> RawPixelFeatureExtractor::extractFeatures(cv::Mat source) {
	cv::Mat workImage = source;
	std::vector<float> features;
	if (m_convertToGrayPixles && source.channels() > 1) {
		cv::cvtColor(workImage, workImage, CV_BGR2GRAY);
	}
	workImage.convertTo(workImage, cv::DataType<float>::type, 1, 0);
	workImage = workImage.reshape(0, 1);
	workImage.copyTo(features);
	return features;
}

} /* namespace slsr */
