/*
 * IFeatureExtractor.h
 *
 *  Created on: 2015. okt. 3.
 *      Author: Ákos
 */

#ifndef IFEATUREEXTRACTOR_H_
#define IFEATUREEXTRACTOR_H_

#include <vector>
#include <opencv2/core.hpp>

namespace slsr {

class IFeatureExtractor {
public:
	IFeatureExtractor() {
	}
	virtual ~IFeatureExtractor() {
	}
	virtual std::vector<float> extractFeatures(cv::Mat source) = 0;
};

} /* namespace slsr */

#endif /* IFEATUREEXTRACTOR_H_ */
