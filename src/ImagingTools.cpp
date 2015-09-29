/*
 * ImagingTools.cpp
 *
 *  Created on: 2015. szept. 28.
 *      Author: a.strack
 */

#include "ImagingTools.h"

namespace slsr {

namespace ImagingTools {

void createEmptyImageWithBorder(cv::Mat source, cv::OutputArray destination,
		unsigned int borderSize, int matType, const cv::Scalar& initialValue) {
	if (matType == -1) {
		matType = source.type();
	}
	cv::Mat(source.size().height + 2 * borderSize,
			source.size().width + 2 * borderSize, matType, initialValue).copyTo(
			destination);
}

void removeImageBorder(cv::Mat source, cv::OutputArray destination,
		unsigned int borderSize) {
	//TODO range verifications
	source(
			cv::Rect(borderSize, borderSize,
					source.size().width - 2 * borderSize,
					source.size().height - 2 * borderSize)).copyTo(destination);
}

void normalizeToMax(cv::Mat source, cv::OutputArray destination) {
	double min, max;
	cv::Point min_loc, max_loc;
	cv::minMaxLoc(source, &min, &max, &min_loc, &max_loc);
	source.convertTo(destination, CV_8UC1, 255 / max);
}

}/* ImagingTools */
} /* namespace slsr */
