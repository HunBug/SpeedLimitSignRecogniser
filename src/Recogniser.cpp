/*
 * Recogniser.cpp
 *
 *  Created on: 2015. okt. 7.
 *      Author: a.strack
 */

#include "Recogniser.h"
#include <boost/lexical_cast.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "NoiseRemover.h"
#include "RectangleCandidateFinder.h"
#include "DemosaicingNormaliser.h"
#include "TemplateMatchingDetector.h"
#include "RawPixelFeatureExtractor.h"
#include "NearestNeighbourRecogniser.h"

namespace slsr {

#define FLOW_DEBUG std::cout<<"l:"<<__LINE__<<" "

Recogniser::Recogniser() {
	// TODO Auto-generated constructor stub

}

Recogniser::~Recogniser() {
	// TODO Auto-generated destructor stub
}

void Recogniser::start(FileSource& fileSource,
		std::shared_ptr<Evaluator> evaluator) {
	NoiseRemover noiseRemover;
	std::vector<double> scales { 30, 35, 40, 45, 50, 60, 70 };
	RectangleCandidateFinder rectangleCandidateFinder(40.0 / 50.0, scales);
	TemplateMatchingDetector templateMatchingDetector;
	std::shared_ptr < IFeatureExtractor
			> featureExtractor(new RawPixelFeatureExtractor());
	NearestNeighbourRecogniser nearestNeighbourRecogniser(featureExtractor);
	DemosaicingNormaliser demosaicing;

	//Do recognition
	while (fileSource.next()) {
		try {
			std::cout << "Processing: " << fileSource.getCurrentFileName()
					<< " path: " << fileSource.getCurrentPath();

			FLOW_DEBUG;
			cv::Mat source = fileSource.getCurrent();
			FLOW_DEBUG;
			cv::Mat normalised = demosaicing.normalise(source);
			FLOW_DEBUG;
			normalised = noiseRemover.normalise(normalised);
			FLOW_DEBUG;
			auto candidates = rectangleCandidateFinder.getCandidates(
					normalised);
			FLOW_DEBUG;
			auto signs = templateMatchingDetector.getSigns(normalised,
					candidates);
			FLOW_DEBUG;
			cv::Mat resultImage = source.clone();
			std::cout << " #Signs: " << signs.size();
			RecognitionResult result =
					RecognitionResult::createNotFoundResult();
			for (auto sign : signs) {
				FLOW_DEBUG;
				std::string recognised = nearestNeighbourRecogniser.recognise(
						normalised, sign);

				if (!recognised.empty()) {
					result = RecognitionResult::createFoundResult(sign,
							boost::lexical_cast<unsigned int>(recognised));
				}
				cv::Scalar textColor(0, 255, 0);
				if (recognised.empty()) {
					recognised = "NR";
					textColor = cv::Scalar(0, 0, 255);
				}
				std::cout << " Rcg: " << recognised;
				cv::putText(resultImage, recognised, cv::Point(sign.x, sign.y),
						cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 2, textColor);

			}
			cv::imwrite(fileSource.getCurrentFileName() + "_res.png",
					resultImage);
			if (evaluator) {
				evaluator->addMeasurement(result,
						fileSource.getCurrentFileName() + ".png");
			}
			std::cout << std::endl;

		} catch (...) {
			std::cout << "EXCEPTION" << std::endl;
		}
	}
	FLOW_DEBUG;
	if (evaluator) {
		FLOW_DEBUG;
		auto results = evaluator->summaryStatistics();
		std::cout << "TP: " << results.truePositive.size() << " TN: "
				<< results.trueNegative.size() << " FP: "
				<< results.falsePositive.size() << " FN: "
				<< results.falseNegative.size() << std::endl;
		std::cout << "F1 score: " << results.f1Score() << " (Recall: "
				<< results.recall() << "  Precision: " << results.precision()
				<< ")" << std::endl;
	}
}

} /* namespace slsr */
