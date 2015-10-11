/*
 * NearestNeighbourRecogniser.h
 *
 *  Created on: 2015. okt. 3.
 *      Author: Ákos
 */

#ifndef NEARESTNEIGHBOURRECOGNISER_H_
#define NEARESTNEIGHBOURRECOGNISER_H_

#include "IRecogniser.h"
#include <array>
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

	std::array<short, 6> getPossibleResults() {
		static std::array<short, 6> possibleResults { { 25, 30, 35, 45, 55, 65 } };
		return possibleResults;
	}
private:
	boost::optional<cv::Ptr<cv::ml::KNearest>> m_classifier;

	cv::Ptr<cv::ml::KNearest> getClassifier();
	cv::Mat getSchoolTemplate();
	bool getNumbersRoi(cv::Mat source, cv::Rect& numbersRoi);
	std::vector<float> getNumberFeatures(cv::Mat source);
	bool checkSchoolHeader(cv::Mat fullImage, cv::Rect mainSignPosition);
};

} /* namespace slsr */

#endif /* NEARESTNEIGHBOURRECOGNISER_H_ */
