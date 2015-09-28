/*
 * NoiseRemover.cpp
 *
 *  Created on: 2015. szept. 28.
 *      Author: a.strack
 */

#include "NoiseRemover.h"
#include <opencv2/imgproc.hpp>

namespace slsr {

NoiseRemover::NoiseRemover() {
	// TODO Auto-generated constructor stub

}

NoiseRemover::~NoiseRemover() {
	// TODO Auto-generated destructor stub
}

cv::Mat NoiseRemover::normalise(cv::Mat source) {
	cv::Mat result;
	cv::GaussianBlur(source, result,
			cv::Size(GAUSS_KERNEL_SIZE, GAUSS_KERNEL_SIZE), 0, 0,
			cv::BORDER_DEFAULT);
	return result;
}

} /* namespace slsr */
