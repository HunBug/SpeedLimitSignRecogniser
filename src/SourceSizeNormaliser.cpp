/*
 * SourceSizeNormaliser.cpp
 *
 *  Created on: 2015. szept. 23.
 *      Author: a.strack
 */

#include "SourceSizeNormaliser.h"

namespace slsr {

SourceSizeNormaliser::SourceSizeNormaliser() {
	// TODO Auto-generated constructor stub

}

cv::Mat SourceSizeNormaliser::normalise(cv::Mat source) {
	cv::Mat result;
	double ratio = NORMALISED_HEIGHT / source.size().height;
	cv::resize(source,result,cv::Size(), ratio, ratio, INTERPOLATION_TYPE);
	return result;
}

SourceSizeNormaliser::~SourceSizeNormaliser() {
	// TODO Auto-generated destructor stub
}

} /* namespace slsr */
