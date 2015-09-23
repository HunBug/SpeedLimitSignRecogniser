/*
 * SourceSizeNormaliser.h
 *
 *  Created on: 2015. szept. 23.
 *      Author: a.strack
 */

#ifndef SOURCESIZENORMALISER_H_
#define SOURCESIZENORMALISER_H_

#include "ISourceNormaliser.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

namespace slsr {

class SourceSizeNormaliser: public ISourceNormaliser {
public:
	const static unsigned short NORMALISED_HEIGHT = 480;
	const static int INTERPOLATION_TYPE = cv::INTER_LINEAR;

	SourceSizeNormaliser();

	virtual cv::Mat normalise(cv::Mat source) override;
	virtual ~SourceSizeNormaliser();
};

} /* namespace slsr */

#endif /* SOURCESIZENORMALISER_H_ */
