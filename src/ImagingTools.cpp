/*
 * ImagingTools.cpp
 *
 *  Created on: 2015. szept. 28.
 *      Author: a.strack
 */

#include "ImagingTools.h"

namespace slsr {

namespace ImagingTools {

void createImageBorder(cv::Mat source, cv::OutputArray destination,
		unsigned int borderSize, int matType, const cv::Scalar& initialValue) {
	if (matType == -1) {
		matType = source.type();
	}
	destination = cv::Mat(source.size().height + 2 * borderSize,
			source.size().width + 2 * borderSize, matType, initialValue);
}

void removeImageBorder(cv::Mat source, cv::OutputArray destination,
		unsigned int borderSize) {
	destination = source(
			cv::Rect(borderSize, borderSize, source.size().width - borderSize,
					source.size().height - borderSize));
}

}/* ImagingTools */
} /* namespace slsr */
