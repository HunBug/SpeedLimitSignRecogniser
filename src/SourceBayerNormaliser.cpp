/*
 * SourceBayerNormaliser.cpp
 *
 *  Created on: 2015. szept. 23.
 *      Author: a.strack
 */

#include "SourceBayerNormaliser.h"

namespace slsr {

SourceBayerNormaliser::SourceBayerNormaliser() {
	// TODO Auto-generated constructor stub

}

SourceBayerNormaliser::~SourceBayerNormaliser() {
	// TODO Auto-generated destructor stub
}

cv::Mat SourceBayerNormaliser::normalise(cv::Mat source) {
	cv::Mat result;
	cv::cvtColor(source, result, BAYER_PATTERN);
	return result;
}

} /* namespace slsr */
