/*
 * ICandidateFinder.h
 *
 *  Created on: 2015. szept. 25.
 *      Author: a.strack
 */

#ifndef ICANDIDATEFINDER_H_
#define ICANDIDATEFINDER_H_

#include <vector>
#include <opencv2/core.hpp>

namespace slsr {

class ICandidateFinder {
public:
	ICandidateFinder(){};
	virtual ~ICandidateFinder(){};

	virtual cv::Mat getCandidates(cv::Mat source) = 0;
};

} /* namespace slsr */

#endif /* ICANDIDATEFINDER_H_ */
