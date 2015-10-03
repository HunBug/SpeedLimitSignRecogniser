/*
 * RawPixelFeatureExtractor.h
 *
 *  Created on: 2015. okt. 3.
 *      Author: Ákos
 */

#ifndef RAWPIXELFEATUREEXTRACTOR_H_
#define RAWPIXELFEATUREEXTRACTOR_H_

#include "IFeatureExtractor.h"

namespace slsr {

class RawPixelFeatureExtractor: public IFeatureExtractor {
public:
	RawPixelFeatureExtractor();
	virtual ~RawPixelFeatureExtractor();
	virtual std::vector<double> extractFeatures(cv::Mat source) override;
};

} /* namespace slsr */

#endif /* RAWPIXELFEATUREEXTRACTOR_H_ */
