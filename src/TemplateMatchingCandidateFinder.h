/*
 * TemplateMatchingCandidateFinder.h
 *
 *  Created on: 2015. szept. 25.
 *      Author: a.strack
 */

#ifndef TEMPLATEMATCHINGCANDIDATEFINDER_H_
#define TEMPLATEMATCHINGCANDIDATEFINDER_H_

#include "ICandidateFinder.h"

namespace slsr {

class TemplateMatchingCandidateFinder: public ICandidateFinder {
public:
	TemplateMatchingCandidateFinder();
	virtual ~TemplateMatchingCandidateFinder();

	virtual std::vector<cv::Rect2i> getCandidates(cv::Mat source) override;
};

} /* namespace slsr */

#endif /* TEMPLATEMATCHINGCANDIDATEFINDER_H_ */
