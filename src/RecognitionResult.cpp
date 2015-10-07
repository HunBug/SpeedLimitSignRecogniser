/*
 * RecognitionResult.cpp
 *
 *  Created on: 2015. okt. 5.
 *      Author: a.strack
 */

#include "RecognitionResult.h"

namespace slsr {

RecognitionResult::RecognitionResult() :
		m_signFound(false) {
}

RecognitionResult::RecognitionResult(bool signFound, cv::Rect signPosition,
		unsigned char speedLimit) :
		m_signFound(signFound), m_signPosition(signPosition), m_speedLimit(
				speedLimit) {
}

RecognitionResult::~RecognitionResult() {
	// TODO Auto-generated destructor stub
}

RecognitionResult RecognitionResult::createNotFoundResult() {
	return RecognitionResult(false, cv::Rect(), 0);
}

RecognitionResult RecognitionResult::createFoundResult(cv::Rect signPosition,
		unsigned char speedLimit) {
	return RecognitionResult(true, signPosition, speedLimit);
}

} /* namespace slsr */
