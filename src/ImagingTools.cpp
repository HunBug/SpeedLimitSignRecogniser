/*
 * ImagingTools.cpp
 *
 *  Created on: 2015. szept. 28.
 *      Author: a.strack
 */

#include "ImagingTools.h"
#include <opencv2/imgproc.hpp>

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

cv::Point multiScaleTemplateMatching(cv::Mat image, cv::Mat templateImage,
		std::vector<double> scales, double& resultScale, double& mathcValue) {
	double totalMaxTemplateValue = 0;
	cv::Point totalMaxTemplatePosition;
	for (auto templateScale : scales) {
		cv::Mat scaledTemplate;
		cv::resize(templateImage, scaledTemplate, cv::Size(), templateScale,
				templateScale, cv::INTER_CUBIC);
		cv::Mat result;
		cv::matchTemplate(image, scaledTemplate, result, CV_TM_CCOEFF_NORMED);
		double maxTemplateValue;
		double minTemplateValue;
		cv::Point minTemplatePosition;
		cv::Point maxTemplatePosition;
		cv::minMaxLoc(result, &minTemplateValue, &maxTemplateValue,
				&minTemplatePosition, &maxTemplatePosition);
		//Assuming increasing template scales
		if (totalMaxTemplateValue <= maxTemplateValue) {
			totalMaxTemplatePosition = maxTemplatePosition;
			totalMaxTemplateValue = maxTemplateValue;
			resultScale = templateScale;
		}
	}

	mathcValue = totalMaxTemplateValue;
	return totalMaxTemplatePosition;
}

bool shrinkRoiToImage(cv::Mat image, cv::Rect& roi) {
	//TODO clean this code
	bool isModified = false;
	int roiX = roi.x;
	int roiY = roi.y;
	int roiRight = roi.x + roi.width;
	int roiBottom = roi.y + roi.height;
	if (roiX < 0) {
		roiRight -= roiX;
		roiX -= roiX; //=0;
		isModified = true;
	}
	if (roiY < 0) {
		roiBottom -= roiY;
		roiY -= roiY; //=0;
		isModified = true;
	}
	if (roiRight >= image.size().width) {
		int correction = roiRight - image.size().width;
		roiRight -= correction;
		roiX -= correction; //=0;
		isModified = true;
	}
	if (roiBottom >= image.size().height) {
		int correction = roiBottom - image.size().height;
		roiBottom -= correction;
		roiY -= correction; //=0;
		isModified = true;
	}
	roi = cv::Rect(roiX, roiY, roiRight - roiX, roiBottom - roiY);
	return isModified;
}

}/* ImagingTools */
} /* namespace slsr */
