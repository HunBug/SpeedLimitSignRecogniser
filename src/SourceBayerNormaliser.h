/*
 * SourceBayerNormaliser.h
 *
 *  Created on: 2015. szept. 23.
 *      Author: a.strack
 */

#ifndef SOURCEBAYERNORMALISER_H_
#define SOURCEBAYERNORMALISER_H_

#include "ISourceNormaliser.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

namespace slsr {

class SourceBayerNormaliser: public ISourceNormaliser {
public:
	const static int BAYER_PATTERN = CV_BayerBG2BGR;

	SourceBayerNormaliser();
	virtual ~SourceBayerNormaliser();

	virtual cv::Mat normalise(cv::Mat source) override;
};

} /* namespace slsr */

#endif /* SOURCEBAYERNORMALISER_H_ */
