/*
 * TemplateMatchingDetector.cpp
 *
 *  Created on: 2015. szept. 29.
 *      Author: a.strack
 */

#include "TemplateMatchingDetector.h"

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

bool TemplateMatchingDetector::tryNextCandidate(cv::Mat source,
		cv::Mat& candidateMap, cv::Rect2i& sign) {
	double min, max;
	cv::Point min_loc, max_loc;
	cv::minMaxLoc(source, &min, &max, &min_loc, &max_loc);
	if(max<MIN_CANDIDATE_VOTES){
		return false;
	}

	//MAX LOC ellenõrzése
	cv::matchTemplate(ref, tpl, res, CV_TM_CCORR_NORMED);
	//MAX LOC környékén nullázni a candidate map-ot
}

} /* namespace slsr */
