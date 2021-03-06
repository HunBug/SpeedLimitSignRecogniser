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
		unsigned int borderSize, int matType = -1,
		const cv::Scalar& initialValue = 0);
void removeImageBorder(cv::Mat source, cv::OutputArray destination,
		unsigned int borderSize);
void normalizeToMax(cv::Mat source, cv::OutputArray destination);
cv::Point multiScaleTemplateMatching(cv::Mat image, cv::Mat templateImage,
		std::vector<double> scales, double& resultScale, double& mathcValue);
bool shrinkRoiToImage(cv::Mat image, cv::Rect& roi);

}/* ImagingTools */
} /* namespace slsr */

#endif /* IMAGINGTOOLS_H_ */
