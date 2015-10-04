/*
 * NearestNeighbourRecogniser.h
 *
 *  Created on: 2015. okt. 3.
 *      Author: Ákos
 */

#ifndef NEARESTNEIGHBOURRECOGNISER_H_
#define NEARESTNEIGHBOURRECOGNISER_H_

#include "IRecogniser.h"
#include <boost/optional.hpp>
#include <opencv2/ml.hpp>

namespace slsr {

class NearestNeighbourRecogniser: public IRecogniser {
public:
	NearestNeighbourRecogniser(
			std::shared_ptr<IFeatureExtractor> featureExtractor);
	virtual ~NearestNeighbourRecogniser();

	virtual std::string recognise(cv::Mat fullImage, cv::Rect signPosition)
			override;
private:
	boost::optional<cv::Ptr<cv::ml::KNearest>> m_classifier;

	cv::Ptr<cv::ml::KNearest> getClassifier();
	bool getNumbersRoi(cv::Mat source, cv::Rect& numbersRoi);
};

} /* namespace slsr */

#endif /* NEARESTNEIGHBOURRECOGNISER_H_ */
