/*
 * ISourceNormaliser.h
 *
 *  Created on: 2015. szept. 23.
 *      Author: a.strack
 */

#ifndef ISOURCENORMALISER_H_
#define ISOURCENORMALISER_H_

#include <opencv2/core.hpp>

namespace slsr {

class ISourceNormaliser {
public:
	ISourceNormaliser() {};

	virtual cv::Mat normalise(cv::Mat source) =0;

	virtual ~ISourceNormaliser() {};
};

} /* namespace slsr */

#endif /* ISOURCENORMALISER_H_ */
