/*
 * TemplateMatchingDetector.h
 *
 *  Created on: 2015. szept. 29.
 *      Author: a.strack
 */

#ifndef TEMPLATEMATCHINGDETECTOR_H_
#define TEMPLATEMATCHINGDETECTOR_H_

#include "IDetector.h"
#include <string>

namespace slsr {

class TemplateMatchingDetector: public IDetector {
public:
	static const unsigned char CANDIDATES_TO_TRY = 15;
	static const constexpr double MIN_CANDIDATE_VOTES = 2000.0;
	static const constexpr double MIN_TEMPLATE_MATCH_VALUE = .32;

	TemplateMatchingDetector();
	virtual ~TemplateMatchingDetector();

	virtual std::vector<cv::Rect2i> getSigns(cv::Mat source,
			cv::Mat candidateMap) override;
	cv::Mat getDebugImage() {
		return _debug_image;
	}
private:
	static cv::Mat getSignTemplate();

	static std::vector<double> getTemplateScales();

	bool tryNextCandidate(cv::Mat source, cv::Mat candidateMap,
			cv::Rect2i& sign);

	cv::Mat _debug_image;
};

} /* namespace slsr */

#endif /* TEMPLATEMATCHINGDETECTOR_H_ */
