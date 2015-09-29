/*
 * TemplateMatchingDetector.h
 *
 *  Created on: 2015. szept. 29.
 *      Author: a.strack
 */

#ifndef TEMPLATEMATCHINGDETECTOR_H_
#define TEMPLATEMATCHINGDETECTOR_H_

#include "IDetector.h"

namespace slsr {

class TemplateMatchingDetector: public IDetector {
public:
	static const unsigned char MAX_CANDIDATES_TO_TRY = 20;
	static const constexpr double MIN_CANDIDATE_VOTES = 2000.0;

	static cv::Mat& getSignTemplate() {
		static cv::Mat instance;
		if(instance.empty()){
			instance = cv::imread("signHeader.png");
		}
		return instance;
	}

	TemplateMatchingDetector();
	virtual ~TemplateMatchingDetector();

	virtual std::vector<cv::Rect2i> getSigns(cv::Mat source,
			cv::Mat candidateMap) override;
private:
	bool tryNextCandidate(cv::Mat source, cv::Mat& candidateMap,
			cv::Rect2i& sign);
};

} /* namespace slsr */

#endif /* TEMPLATEMATCHINGDETECTOR_H_ */
