/*
 * TemplateMatchingCandidateFinder.h
 *
 *  Created on: 2015. szept. 25.
 *      Author: a.strack
 */

#ifndef TEMPLATEMATCHINGCANDIDATEFINDER_H_
#define TEMPLATEMATCHINGCANDIDATEFINDER_H_

#include "ICandidateFinder.h"
#include <opencv2/imgproc.hpp>

namespace slsr {

class TemplateMatchingCandidateFinder: public ICandidateFinder {
public:
	const static int MATHOD_TYPE = cv::TM_CCORR_NORMED;
	const unsigned int MAX_PYRAMID_LEVEL = 3;

	TemplateMatchingCandidateFinder();
	virtual ~TemplateMatchingCandidateFinder();

	virtual std::vector<cv::Rect2i> getCandidates(cv::Mat source) override;
private:
	//This algorithm was copied from http://opencv-code.com/tutorials/fast-template-matching-with-image-pyramid/
	void fastMatchTemplate(cv::Mat& srca, cv::Mat& srcb, cv::Mat& dst, int maxlevel);
};

} /* namespace slsr */

#endif /* TEMPLATEMATCHINGCANDIDATEFINDER_H_ */
