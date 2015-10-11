/*
 * Recogniser.cpp
 *
 *  Created on: 2015. okt. 7.
 *      Author: a.strack
 */

#include "Recogniser.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/timer/timer.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "NoiseRemover.h"
#include "RectangleCandidateFinder.h"
#include "DemosaicingNormaliser.h"
#include "TemplateMatchingDetector.h"
#include "RawPixelFeatureExtractor.h"
#include "NearestNeighbourRecogniser.h"
#include "DebugTools.h"

namespace slsr {

Recogniser::Recogniser() :
		m_loadResultIfAvailable(false) {
	// TODO Auto-generated constructor stub

}

Recogniser::~Recogniser() {
	// TODO Auto-generated destructor stub
}

void Recogniser::start(FileSource& fileSource,
		std::shared_ptr<Evaluator> evaluator) {
	NoiseRemover noiseRemover;
	//std::vector<double> scales { 30, 35, 40, 45, 50, 60, 70 };
	std::vector<double> scales { 60 };
	RectangleCandidateFinder rectangleCandidateFinder(40.0 / 50.0, scales);
	TemplateMatchingDetector templateMatchingDetector;
	std::shared_ptr < IFeatureExtractor
			> featureExtractor(new RawPixelFeatureExtractor());
	NearestNeighbourRecogniser nearestNeighbourRecogniser(featureExtractor);
	DemosaicingNormaliser demosaicing;

	//Do recognition
	while (fileSource.next()) {
		boost::timer::auto_cpu_timer measureRecognition(
				"Image processing: %t sec CPU, %w sec real");
		try {
			std::cout << "Processing: " << fileSource.getCurrentFileName()
					<< " path: " << fileSource.getCurrentPath() << "  "
					<< fileSource.ramainingSize() << "(" << fileSource.size()
					<< ")" << std::endl;
			boost::optional < RecognitionResult > result;
			if (isLoadResultIfAvailable()) {
				result = loadResultIfAvailable(fileSource.getCurrentFileName());
			}
			if (!result.is_initialized()) {
				result = RecognitionResult::createNotFoundResult();
				PRINT_LINE_DEBUG;
				cv::Mat source = fileSource.getCurrent();
				PRINT_LINE_DEBUG;
				cv::Mat normalised = demosaicing.normalise(source);
				PRINT_LINE_DEBUG;
				normalised = noiseRemover.normalise(normalised);
				PRINT_LINE_DEBUG;
				auto candidates = rectangleCandidateFinder.getCandidates(
						normalised);
				PRINT_LINE_DEBUG;
				auto signs = templateMatchingDetector.getSigns(normalised,
						candidates);
				PRINT_LINE_DEBUG;
				cv::Mat resultImage = source.clone();
				std::cout << " #Signs: " << signs.size();

				for (auto sign : signs) {
					PRINT_LINE_DEBUG;
					std::string recognised =
							nearestNeighbourRecogniser.recognise(normalised,
									sign);
					std::cout << "re: " << recognised << "  si: " << sign
							<< std::endl;
					PRINT_LINE_DEBUG;
					bool schoolSignFound;
					if (!recognised.empty()) {
						PRINT_LINE_DEBUG;
						schoolSignFound =
								nearestNeighbourRecogniser.checkSchoolHeader(
										normalised, sign);
						result = RecognitionResult::createFoundResult(sign,
								boost::lexical_cast<unsigned int>(recognised),
								schoolSignFound);
					}
					PRINT_LINE_DEBUG;
					cv::Scalar textColor(0, 255, 0);
					if (recognised.empty()) {
						recognised = "NR";
						textColor = cv::Scalar(0, 0, 255);
					} else if (schoolSignFound) {
						recognised = recognised + "S";
					}
					std::cout << " Rcg: " << recognised;
					cv::putText(resultImage, recognised,
							cv::Point(sign.x, sign.y),
							cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 2, textColor);

				}
				PRINT_LINE_DEBUG;
				saveResults(fileSource.getCurrentFileName(), resultImage,
						result.get());
			}
			PRINT_LINE_DEBUG;
			if (evaluator) {
				evaluator->addMeasurement(result.get(),
						fileSource.getCurrentFileName() + ".png");
			}
			std::cout << std::endl;

		} catch (boost::exception& ex) {
			// error handling
			std::cout << boost::diagnostic_information(ex);
		}
	}
	PRINT_LINE_DEBUG;
	if (evaluator) {
		PRINT_LINE_DEBUG;
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

boost::optional<RecognitionResult> Recogniser::loadResultIfAvailable(
		std::string imagePath) {
	boost::optional < RecognitionResult > result;
	std::string dataFileName = imagePath + ".res";
	if (boost::filesystem::exists(dataFileName)) {
		std::ifstream streamReader(dataFileName);
		boost::archive::text_iarchive textArchiver(streamReader);
		RecognitionResult readData;
		textArchiver >> readData;
		result = readData;
		std::cout << "Result loaded for: " << imagePath << std::endl;
	}
	return result;
}

void Recogniser::saveResults(std::string imagePath, cv::Mat resultImage,
		RecognitionResult recoknitionResult) {
	cv::imwrite(imagePath + "_res.png", resultImage);
	{
		std::ofstream streamWriter(imagePath + ".res");
		boost::archive::text_oarchive textArchiver(streamWriter);
		textArchiver << recoknitionResult;
	}
}

} /* namespace slsr */
