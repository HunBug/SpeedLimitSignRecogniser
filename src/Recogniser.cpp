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
	std::cout << "Start - Debug: " << isEnableDebug() << " Separate: "
			<< isSeparateResults() << " Load: " << isLoadResultIfAvailable()
			<< std::endl;
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
		try {
			std::cout << "Processing: " << fileSource.getCurrentFileName()
					<< "  " << fileSource.ramainingSize() << "("
					<< fileSource.size() << ")" << std::endl;
			boost::optional < RecognitionResult > result;
			if (isLoadResultIfAvailable()) {
				result = loadResultIfAvailable(fileSource.getCurrentFileName());
			}
			if (!result.is_initialized()) {
				result = RecognitionResult::createNotFoundResult();
				cv::Mat source = fileSource.getCurrent();
				cv::Mat normalised = demosaicing.normalise(source);
				normalised = noiseRemover.normalise(normalised);
				PRINT_LINE_DEBUG;
				auto candidates = rectangleCandidateFinder.getCandidates(
						normalised);
				PRINT_LINE_DEBUG;
				auto signs = templateMatchingDetector.getSigns(normalised,
						candidates);
				if (isEnableDebug()) {
					cv::imwrite(debugTMName(fileSource.getCurrentFileName()),
							templateMatchingDetector.getDebugImage());
				}
				PRINT_LINE_DEBUG;
				cv::Mat resultImage = source.clone();
				std::cout << " #Signs: " << signs.size();

				for (auto sign : signs) {
					PRINT_LINE_DEBUG;
					std::string recognised =
							nearestNeighbourRecogniser.recognise(normalised,
									sign);
					if (isEnableDebug()) {
						cv::imwrite(
								debugkNName(fileSource.getCurrentFileName()),
								nearestNeighbourRecogniser.getDebugImage());
					}
					PRINT_LINE_DEBUG;
					bool schoolSignFound = false;
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
			if (evaluator) {
				evaluator->addMeasurement(result.get(),
						fileSource.getCurrentFileName() + ".png");
			}
			std::cout << "Processing end." << std::endl;

		} catch (boost::exception& ex) {
			// error handling
			PRINT_LINE_DEBUG;
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

		debugMoveResults(results.falsePositive, "DebugFalsePositive");
		debugMoveResults(results.falseNegative, "DebugFalseNegative");
		debugMoveResults(results.truePositive, "DebugTruePositive");
		debugMoveResults(results.trueNegative, "DebugTrueNegative");

		if (isSeparateResults()) {
			separateResults(results.falsePositive, "ResultFalsePositive");
			separateResults(results.falseNegative, "ResultFalseNegative");
			separateResults(results.truePositive, "ResultTruePositive");
			separateResults(results.trueNegative, "ResultTrueNegative");
		}
	}
}

boost::optional<RecognitionResult> Recogniser::loadResultIfAvailable(
		std::string imagePath) {
	boost::optional < RecognitionResult > result;
	std::string dataFileName = "Results/" + imagePath + ".res";
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

std::string Recogniser::resultDataFile(std::string sourceName) {
	auto fileName = getFilenamrWithoutExtension(sourceName);
	return "Results/" + fileName + ".res";
}

std::string Recogniser::resultImageFile(std::string sourceName) {
	auto fileName = getFilenamrWithoutExtension(sourceName);
	return "Results/" + fileName + "_res.png";
}

void Recogniser::saveResults(std::string imagePath, cv::Mat resultImage,
		RecognitionResult recoknitionResult) {
	boost::filesystem::create_directory("Results");
	cv::imwrite(resultImageFile(imagePath + ".png"), resultImage);
	{
		std::ofstream streamWriter(resultDataFile(imagePath + ".png"));
		boost::archive::text_oarchive textArchiver(streamWriter);
		textArchiver << recoknitionResult;
	}
}

void Recogniser::separateResults(
		const std::vector<Evaluator::ResultElement>& results,
		std::string destinationFolder) {
	std::cout << "Moving " + destinationFolder + " result files..."
			<< std::endl;
	boost::filesystem::create_directory(destinationFolder);
	for (auto result : results) {
		auto image = resultImageFile(result.fileName);
		auto data = resultDataFile(result.fileName);
		if (boost::filesystem::exists(image)) {
			boost::filesystem::rename(image,
					destinationFolder + "/" + result.fileName + ".png");
		}
		if (boost::filesystem::exists(data)) {
			boost::filesystem::rename(data,
					destinationFolder + "/" + result.fileName + ".res");
		}
	}
}

std::string Recogniser::debugTMName(std::string sourceName) {
	auto fileName = sourceName; // getFilenamrWithoutExtension(sourceName);
	return "debug_" + fileName + "_10templateMatchingDetector" + ".png";
}

std::string Recogniser::debugkNName(std::string sourceName) {
	auto fileName = sourceName; //getFilenamrWithoutExtension(sourceName);
	return "debug_" + fileName + "_20nearestNeighbourRecogniser" + ".png";
}

void Recogniser::debugMove(std::string sourceFile, std::string directoryName) {
	boost::filesystem::create_directory(directoryName);
	if (boost::filesystem::exists(sourceFile)) {
		boost::filesystem::rename(sourceFile, directoryName + "/" + sourceFile);
	}
}

std::string Recogniser::getFilenamrWithoutExtension(std::string path) {
	boost::filesystem::path imageStem(path);
	auto fileName = imageStem.filename().string();
	return fileName.substr(0, fileName.size() - 4);
}

void Recogniser::debugMoveResults(
		const std::vector<Evaluator::ResultElement>& results,
		std::string destinationFolder) {
	std::cout << "Moving " + destinationFolder + " debug files..." << std::endl;
	for (auto result : results) {
		debugMove(debugTMName(getFilenamrWithoutExtension(result.fileName)),
				destinationFolder);
		debugMove(debugkNName(getFilenamrWithoutExtension(result.fileName)),
				destinationFolder);
	}
}

} /* namespace slsr */
