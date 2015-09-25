/*
 * ISegmentation.h
 *
 *  Created on: 2015. szept. 23.
 *      Author: a.strack
 */

#ifndef ISEGMENTATION_H_
#define ISEGMENTATION_H_

#include <opencv2/core.hpp>

namespace slsr {

class ISegmentation {
public:
	ISegmentation() {};
	virtual ~ISegmentation(){};

	virtual cv::Mat getMask(cv::Mat source) = 0;
};

} /* namespace slsr */

#endif /* ISEGMENTATION_H_ */
