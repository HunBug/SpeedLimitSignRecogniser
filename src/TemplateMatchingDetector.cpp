/*
 * TemplateMatchingDetector.cpp
 *
 *  Created on: 2015. szept. 29.
 *      Author: a.strack
 */

#include "TemplateMatchingDetector.h"
#include <algorithm>
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
	std::vector<cv::Rect2i> signs;
	cv::Mat workCandidateMap = candidateMap;
	for (auto candidateIteration = 0;
			candidateIteration < MAX_CANDIDATES_TO_TRY; ++candidateIteration) {
		cv::Rect2i sign;
		bool signFound = tryNextCandidate(source, workCandidateMap, sign);
		if (signFound) {
			signs.push_back(sign);
		} else {
			break;
		}
	}

	return signs;
}

cv::Mat& TemplateMatchingDetector::getSignTemplate() {
	static cv::Mat instance;
	if (instance.empty()) {
		instance = cv::imread("signHeader.png");
		cv::resize(instance, instance, cv::Size(100, 125));
	}
	return instance;
}

std::vector<double> TemplateMatchingDetector::getTemplateScales() {
	{
		static std::vector<double> scales { 1.0, .9, .8, 0.7, .6, .5, .4, .3,
				.25, .2 };
		return scales;
	}
}

//TODO gradient space matching
bool TemplateMatchingDetector::tryNextCandidate(cv::Mat source,
		cv::Mat& candidateMap, cv::Rect2i& sign) {
	double minVote;
	double maxVote;
	cv::Point min_loc, max_loc;
	cv::minMaxLoc(candidateMap, &minVote, &maxVote, &min_loc, &max_loc);
	if (maxVote < MIN_CANDIDATE_VOTES) {
		return false;
	}

	cv::Size roiSize = getSignTemplate().size();
	int roiX = std::max(0, max_loc.x - roiSize.width);
	int roiY = std::max(0, max_loc.y - roiSize.height);
	int roiRight = std::min(source.size().width, max_loc.x + roiSize.width);
	int roiBottom = std::min(source.size().height, max_loc.y + roiSize.height);
	cv::Mat candidateRegion = source(
			cv::Rect(roiX, roiY, roiRight - roiX, roiBottom - roiY));
	//MAX LOC ellenõrzése
	cv::Mat templateMatchingResult;
	for (auto templateScale : getTemplateScales()) {
		cv::Mat signTemplate;
		cv::resize(getSignTemplate(), signTemplate, cv::Size(), templateScale,
				templateScale, cv::INTER_LINEAR);
		cv::Mat result;
		cv::matchTemplate(candidateRegion, signTemplate, result,
				CV_TM_CCORR_NORMED);
		if (templateMatchingResult.empty()) {
			templateMatchingResult = result;
		} else {
			//Assume that templateMarching always the smaller
			cv::Mat newTemplateMatchingResult(result.size(), result.type(),
					0.0);
			newTemplateMatchingResult(
					cv::Rect(0, 0, templateMatchingResult.size().width,
							templateMatchingResult.size().height)) =
					templateMatchingResult;
			cv::max(result, newTemplateMatchingResult, newTemplateMatchingResult);
			templateMatchingResult = newTemplateMatchingResult;
		}
	}

	cv::minMaxLoc(templateMatchingResult, &minVote, &maxVote, &min_loc,
			&max_loc);
	cv::Point rectanlgeSize(15, 20);
	cv::Point shift(roiX, roiY);
	cv::rectangle(source, shift + max_loc - rectanlgeSize,
			shift + max_loc + rectanlgeSize, cv::Scalar(255));
	imwrite("sign.png", source);
//MAX LOC környékén nullázni a candidate map-ot
	return true;
}

} /* namespace slsr */

