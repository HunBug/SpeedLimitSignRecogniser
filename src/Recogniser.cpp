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

#include "TemplateMatchingDetector.h"
#include "RawPixelFeatureExtractor.h"
#include "NearestNeighbourRecogniser.h"

namespace slsr {

Recogniser::Recogniser() {
	// TODO Auto-generated constructor stub

}

Recogniser::~Recogniser() {
	// TODO Auto-generated destructor stub
}

void Recogniser::start(FileSource& fileSource,
		std::shared_ptr<Evaluator> evaluator) {
	NoiseRemover nr;
	std::vector<double> scales { 30, 35, 40, 45, 50, 60, 70 };
	RectangleCandidateFinder rcf(40.0 / 50.0, scales);
	TemplateMatchingDetector tmd;
	std::shared_ptr < IFeatureExtractor > fe(new RawPixelFeatureExtractor());
	NearestNeighbourRecogniser nnr(fe);

	//Do recognition
	while (fileSource.next()) {
		try {
			std::cout << "Processing: " << fileSource.getCurrentFileName()
					<< " path: " << fileSource.getCurrentPath();

			cv::Mat source = fileSource.getCurrent();
			cv::Mat normalised = nr.normalise(source);
			auto candidates = rcf.getCandidates(normalised);
			auto signs = tmd.getSigns(normalised, candidates);
			cv::Mat resultImage = source.clone();
			std::cout << " #Signs: " << signs.size();
			for (auto sign : signs) {
				std::string recognised = nnr.recognise(normalised, sign);
				RecognitionResult result =
						RecognitionResult::createNotFoundResult();
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
				if (evaluator) {
					evaluator->addMeasurement(result,
							fileSource.getCurrentFileName() + ".png");
				}
			}
			cv::imwrite(fileSource.getCurrentFileName() + "_res.png",
					resultImage);
			std::cout << std::endl;

		} catch (...) {
			std::cout << "EXCEPTION" << std::endl;
		}
	}

	if (evaluator) {
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
