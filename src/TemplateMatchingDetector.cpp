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
#include "ImagingTools.h"

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
	//TODO revisit scales;
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
	cv::Rect roi(maxVote_loc.x - roiSize.width, maxVote_loc.y - roiSize.height,
			2 * roiSize.width, 2 * roiSize.height);
	ImagingTools::shrinkRoiToImage(source, roi);
	cv::Mat candidateRegion = source(roi);

	double foundScale;
	double templateMatchValue;
	cv::Point signPosition = ImagingTools::multiScaleTemplateMatching(
			candidateRegion, getSignTemplate(), getTemplateScales(), foundScale,
			templateMatchValue);
	cv::Point rectanlgeCornerShit(getSignTemplate().size().width * foundScale,
			getSignTemplate().size().height * foundScale);
	cv::rectangle(candidateMap, maxVote_loc - rectanlgeCornerShit,
			maxVote_loc + rectanlgeCornerShit, cv::Scalar(0), CV_FILLED);

	bool signFound = templateMatchValue > MIN_TEMPLATE_MATCH_VALUE;
	if (signFound) {
		sign = cv::Rect(roi.x + signPosition.x, roi.y + signPosition.y,
				rectanlgeCornerShit.x, rectanlgeCornerShit.y * 2.2);
	}
	return signFound;
}

} /* namespace slsr */

