/*
 * IRecogniser.h
 *
 *  Created on: 2015. okt. 3.
 *      Author: Ákos
 */

#ifndef IRECOGNISER_H_
#define IRECOGNISER_H_

#include <memory>
#include <string>
#include <opencv2/core.hpp>
#include "IFeatureExtractor.h"

namespace slsr {

class IRecogniser {
public:
	IRecogniser(std::shared_ptr<IFeatureExtractor> featureExtractor) :
			m_featureExtractor(featureExtractor) {
	}
	virtual ~IRecogniser() {
	}
	virtual std::string recognise(cv::Mat fullImage, cv::Rect signPosition) =0;

	std::shared_ptr<IFeatureExtractor> getFeatureExtractor() const {
		return m_featureExtractor;
	}

protected:
	std::shared_ptr<IFeatureExtractor> m_featureExtractor;
};

} /* namespace slsr */

#endif /* IRECOGNISER_H_ */
