/*
 * NearestNeighbourRecogniser.h
 *
 *  Created on: 2015. okt. 3.
 *      Author: Ákos
 */

#ifndef NEARESTNEIGHBOURRECOGNISER_H_
#define NEARESTNEIGHBOURRECOGNISER_H_

#include "IRecogniser.h"

namespace slsr {

class NearestNeighbourRecogniser: public IRecogniser {
public:
	NearestNeighbourRecogniser(
			std::shared_ptr<IFeatureExtractor> featureExtractor);
	virtual ~NearestNeighbourRecogniser();

	virtual std::string recognise(cv::Mat fullImage, cv::Rect signPosition)
			override;
};

} /* namespace slsr */

#endif /* NEARESTNEIGHBOURRECOGNISER_H_ */
