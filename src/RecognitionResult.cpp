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
		unsigned char speedLimit, bool isScool) :
		m_signFound(signFound), m_signPosition(signPosition), m_speedLimit(
				speedLimit), m_isSchool(isScool) {
}

RecognitionResult::~RecognitionResult() {
	// TODO Auto-generated destructor stub
}

RecognitionResult RecognitionResult::createNotFoundResult() {
	return RecognitionResult(false, cv::Rect(), 0, false);
}

RecognitionResult RecognitionResult::createFoundResult(cv::Rect signPosition,
		unsigned char speedLimit, bool isScool) {
	return RecognitionResult(true, signPosition, speedLimit, isScool);
}

} /* namespace slsr */
