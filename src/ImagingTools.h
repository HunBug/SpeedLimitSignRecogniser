/*
 * ImagingTools.h
 *
 *  Created on: 2015. szept. 28.
 *      Author: a.strack
 */

#ifndef IMAGINGTOOLS_H_
#define IMAGINGTOOLS_H_

#include <opencv2/imgproc.hpp>

namespace slsr {
namespace ImagingTools {

void createEmptyImageWithBorder(cv::Mat source, cv::OutputArray destination,
		unsigned int borderSize, int matType = -1, const cv::Scalar& initialValue = 0);
void removeImageBorder(cv::Mat source, cv::OutputArray destination,
		unsigned int borderSize);

}/* ImagingTools */
} /* namespace slsr */

#endif /* IMAGINGTOOLS_H_ */
