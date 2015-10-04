/*
 * TemplateMatchingDetector.cpp
 *
 *  Created on: 2015. szept. 29.
 *      Author: a.strack
 */

#include "TemplateMatchingDetector.h"
#include <cassert>
#include <algorithm>
#include <limits>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

namespace slsr {

TemplateMatchingDetector::TemplateMatchingDetector() {
	// TODO Auto-generated constructor stub

}

TemplateMatchingDetector::~TemplateMatchingDetector() {
	// TODO Auto-generated destructor stub
}

std::vector<cv::Rect2i> TemplateMatchingDetector::getSigns(cv::Mat source,
		cv::Mat candidateMap) {
	std::vector < cv::Rect2i > signs;
	cv::Mat workCandidateMap = candidateMap;
	for (auto candidateIteration = 0; candidateIteration < CANDIDATES_TO_TRY;
			++candidateIteration) {
		cv::Rect2i sign;
		_debug_iterationprefix = boost::lexical_cast < std::string
				> (candidateIteration);
		bool signFound = tryNextCandidate(source, workCandidateMap, sign);
		if (signFound) {
			signs.push_back(sign);
		}
	}

	return signs;
}

cv::Mat TemplateMatchingDetector::getSignTemplate() {
	static cv::Mat instance;
	if (instance.empty()) {
		//TODO check if file exists
		instance = cv::imread("speedLimitText.png");
	}
	return instance;
}

std::vector<double> TemplateMatchingDetector::getTemplateScales() {
	//IMPORTANT use it in increasing order because the template
	//			matching algorithm assumes this! (Prefer bigger scale
	//			with same matching-value)
	static std::vector<double> scales { .30, .35, .4, .35, .5, .55, .75, .85,
			1.0 };
	return scales;
}

//TODO gradient space matching
bool TemplateMatchingDetector::tryNextCandidate(cv::Mat source,
		cv::Mat candidateMap, cv::Rect2i& sign) {
	double minVote;
	double maxVote;
	cv::Point minVote_loc, maxVote_loc;
	cv::minMaxLoc(candidateMap, &minVote, &maxVote, &minVote_loc, &maxVote_loc);
	if (maxVote < MIN_CANDIDATE_VOTES) {
		return false;
	}

	//TODO adjust this size
	cv::Size roiSize(getSignTemplate().size().width * 0.5,
			getSignTemplate().size().height * 0.5);
	int roiX = maxVote_loc.x - roiSize.width;
	int roiY = maxVote_loc.y - roiSize.height;
	int roiRight = maxVote_loc.x + roiSize.width;
	int roiBottom = maxVote_loc.y + roiSize.height;
	if (roiX < 0) {
		roiRight -= roiX;
		roiX -= roiX; //=0;
	}
	if (roiY < 0) {
		roiBottom -= roiY;
		roiY -= roiY; //=0;
	}
	if (roiRight >= source.size().width) {
		int correction = roiRight - source.size().width;
		roiRight -= correction;
		roiX -= correction; //=0;
	}
	if (roiBottom >= source.size().height) {
		int correction = roiBottom - source.size().height;
		roiBottom -= correction;
		roiY -= correction; //=0;
	}
	cv::Mat candidateRegion = source(
			cv::Rect(roiX, roiY, roiRight - roiX, roiBottom - roiY));

	double totalMaxTemplateValue = 0;
	cv::Point totalMaxTemplatePosition;
	cv::Mat debugMaxMatchingValues;
	double scale = 1.0;
	for (auto templateScale : getTemplateScales()) {
		cv::Mat signTemplate;
		cv::resize(getSignTemplate(), signTemplate, cv::Size(), templateScale,
				templateScale, cv::INTER_CUBIC);
		cv::Mat result;
		cv::matchTemplate(candidateRegion, signTemplate, result,
				CV_TM_CCOEFF_NORMED);
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
			debugMaxMatchingValues = result;
			scale = templateScale;
		}
	}
	cv::Point rectanlgeCornerShit(getSignTemplate().size().width * scale,
			getSignTemplate().size().height * scale);
	cv::rectangle(candidateMap, maxVote_loc - rectanlgeCornerShit,
			maxVote_loc + rectanlgeCornerShit, cv::Scalar(0), CV_FILLED);

	bool signFound = totalMaxTemplateValue > MIN_TEMPLATE_MATCH_VALUE;
	if (signFound) {
		sign = cv::Rect(roiX + totalMaxTemplatePosition.x,
				roiY + totalMaxTemplatePosition.y, rectanlgeCornerShit.x,
				rectanlgeCornerShit.y * 2.2);
	}

	//DEBUG
	/*cv::Point shift(roiX, roiY);
	 cv::Mat sc = source.clone();
	 cv::Scalar isFoundColor(0, 0, 255);
	 if (signFound) {
	 isFoundColor = cv::Scalar(0, 255, 0);
	 }
	 if (signFound) {
	 cv::rectangle(sc, sign, isFoundColor);
	 cv::circle(sc, maxVote_loc, 3, cv::Scalar(255));
	 imwrite(
	 boost::lexical_cast < std::string
	 > ((int) round(totalMaxTemplateValue * 10000))
	 + "result_i" + _debug_iterationprefix + ".png",
	 sc);
	 }*/
	//DEBUG END
	return signFound;
}

} /* namespace slsr */

