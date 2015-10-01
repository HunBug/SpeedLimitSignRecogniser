/*
 * TemplateMatchingDetector.cpp
 *
 *  Created on: 2015. szept. 29.
 *      Author: a.strack
 */

#include "TemplateMatchingDetector.h"
#include <cassert>
#include <algorithm>
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
	for (auto candidateIteration = 0;
			candidateIteration < MAX_CANDIDATES_TO_TRY; ++candidateIteration) {
		cv::Rect2i sign;
		_debug_iterationprefix = boost::lexical_cast < std::string
				> (candidateIteration);
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
		//TODO check if file exists
		instance = cv::imread("speedLimitText.png");
		cv::resize(instance, instance, cv::Size(100, 125));
	}
	return instance;
}

std::vector<double> TemplateMatchingDetector::getTemplateScales() {
	//static std::vector<double> scales { 1.0, .9, .8, 0.7, .6, .5, .4, .3, .25,
	//		.2 };
	static std::vector<double> scales { 0.20 };
	return scales;
}

//TODO gradient space matching
bool TemplateMatchingDetector::tryNextCandidate(cv::Mat source,
		cv::Mat& candidateMap, cv::Rect2i& sign) {
	double minVote;
	double maxVote;
	cv::Point minVote_loc, maxVote_loc;
	cv::minMaxLoc(candidateMap, &minVote, &maxVote, &minVote_loc, &maxVote_loc);
	if (maxVote < MIN_CANDIDATE_VOTES) {
		return false;
	}

	cv::Mat currentTemplate = getSignTemplate();
	//TODO Cache templates
	//cv::resize(currentTemplate, currentTemplate, cv::Size(), 0.5, 0.5,
	//		CV_INTER_LANCZOS4);
	//TODO adjust this size
	cv::Size roiSize(currentTemplate.size().width * 0.3,
			currentTemplate.size().height * 0.3);
	int roiX = std::max(0, maxVote_loc.x - roiSize.width);
	int roiY = std::max(0, maxVote_loc.y - roiSize.height);
	int roiRight = std::min(source.size().width, maxVote_loc.x + roiSize.width);
	int roiBottom = std::min(source.size().height,
			maxVote_loc.y + roiSize.height);
	cv::Mat candidateRegion = source(
			cv::Rect(roiX, roiY, roiRight - roiX, roiBottom - roiY));
	//MAX LOC ellenõrzése
	cv::Mat templateMatchingResult;
	for (auto templateScale : getTemplateScales()) {
		cv::Mat signTemplate;
		cv::resize(currentTemplate, signTemplate, cv::Size(), templateScale,
				templateScale, cv::INTER_LANCZOS4);
		cv::Mat result;
		cv::matchTemplate(candidateRegion, signTemplate, result,
				CV_TM_CCORR_NORMED);
		if (templateMatchingResult.empty()) {
			templateMatchingResult = result;
		} else {
			//Assume that templateMarching always the smaller
			assert(result.size().width >= templateMatchingResult.size().width);
			assert(
					result.size().height
							>= templateMatchingResult.size().height);
			cv::Mat newTemplateMatchingResult(result.size(), result.type(),
					0.0);
			newTemplateMatchingResult(
					cv::Rect(0, 0, templateMatchingResult.size().width,
							templateMatchingResult.size().height)) =
					templateMatchingResult;
			cv::max(result, newTemplateMatchingResult,
					newTemplateMatchingResult);
			templateMatchingResult = newTemplateMatchingResult;
		}
		imwrite(_debug_iterationprefix + "candidateTemplate.png", signTemplate);
	}

	cv::Point minTemplate_loc, maxTemplate_loc;
	double minTemplate, maxTemplate;
	cv::minMaxLoc(templateMatchingResult, &minTemplate, &maxTemplate,
			&minTemplate_loc, &maxTemplate_loc);
	cv::Point rectanlgeSize = roiSize;
	cv::Point shift(roiX, roiY);
	cv::Mat sc = source.clone();
	cv::rectangle(sc, shift + maxTemplate_loc,
			shift + maxTemplate_loc + rectanlgeSize, cv::Scalar(255));
	cv::rectangle(candidateMap, maxVote_loc, maxVote_loc + rectanlgeSize,
			cv::Scalar(0), CV_FILLED);

	imwrite(_debug_iterationprefix + "candidateRegion.png", candidateRegion);
	imwrite(_debug_iterationprefix + "candidateMap.png", candidateMap / 255);
	imwrite(
			_debug_iterationprefix + "templateMap" + boost::lexical_cast
					< std::string > ((int) round(maxTemplate * 10000)) + ".png",
			templateMatchingResult * 255);
	imwrite(_debug_iterationprefix + "sign.png", sc);
//MAX LOC környékén nullázni a candidate map-ot
	return true;
}

} /* namespace slsr */

