/*
 * EvaluateTools.cpp
 *
 *  Created on: 2015. okt. 5.
 *      Author: a.strack
 */

#include "EvaluateTools.h"
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/exception/exception.hpp>
#include <boost/lexical_cast.hpp>

namespace slsr {

EvaluateTools::EvaluateTools(std::string annotationFilePath) {
	m_expectedResults = parseFile(annotationFilePath);
}

EvaluateTools::~EvaluateTools() {
	// TODO Auto-generated destructor stub
}

void EvaluateTools::addMeasurement(const RecognitionResult result,
		std::string fileName) {
	m_measuredResults[fileName].push_back(result);
}

void EvaluateTools::printStatistics() {
}

bool EvaluateTools::evaluate(const RecognitionResult result,
		std::string sourceFileName) const {
	auto fileExpectedResult = m_expectedResults.at(sourceFileName);
	bool isEqual = false;
	for (auto expectedResult : fileExpectedResult) {
		isEqual |= evaluate(result, expectedResult);
	}
	return isEqual;
}

bool EvaluateTools::evaluate(const RecognitionResult& result,
		const RecognitionResult& evaluteResult) const {
	bool isEqual = result.isSignFound() ^ evaluteResult.isSignFound();
	if (isEqual) {
		isEqual &= result.getSpeedLimit() == evaluteResult.getSpeedLimit();
		isEqual &= isInToleranceRange(result.getSignPosition().x,
				evaluteResult.getSignPosition().x);
		isEqual &= isInToleranceRange(result.getSignPosition().y,
				evaluteResult.getSignPosition().y);
		isEqual &= isInToleranceRange(result.getSignPosition().width,
				evaluteResult.getSignPosition().width);
		isEqual &= isInToleranceRange(result.getSignPosition().height,
				evaluteResult.getSignPosition().height);
	}
	return isEqual;
}

std::map<std::string, std::vector<RecognitionResult>> EvaluateTools::parseFile(
		std::string filePath) {
	std::map<std::string, std::vector<RecognitionResult>> parsedResults;
	std::ifstream file(filePath);
	if (!file.is_open()) {
		BOOST_THROW_EXCEPTION(
				std::ios::failure("Annotation file can not be opened."));
	}
	std::string line;
	bool isFirstLine = true;
	while (std::getline(file, line)) {
		if (isFirstLine) {
			isFirstLine = false;
			continue;
		}
		std::vector < std::string > lineParts;
		boost::split(lineParts, line, boost::is_any_of(";"));
		if (lineParts.size() != 10) {
			BOOST_THROW_EXCEPTION(
					std::ios::failure(
							"Annotation file is not in the correct format."));
		}

		std::string imageFileName = lineParts[0];
		std::string annotation = lineParts[1];
		std::string cornerLeftX = lineParts[2];
		std::string cornerTopY = lineParts[3];
		std::string cornerRightX = lineParts[4];
		std::string cornerBottomY = lineParts[5];
		if (imageFileName.find("speedLimit") != std::string::npos) {
			cv::Rect position;
			unsigned char speedLimit;

			speedLimit = boost::lexical_cast<unsigned char>(
					annotation.substr(annotation.length() - 2));
			position.x = boost::lexical_cast<int>(cornerLeftX);
			position.y = boost::lexical_cast<int>(cornerTopY);
			position.width = boost::lexical_cast<int>(cornerRightX)
					- position.x;
			position.height = boost::lexical_cast<int>(cornerBottomY)
					- position.y;

			parsedResults[imageFileName].push_back(
					RecognitionResult::createFoundResult(position, speedLimit));
		}
	}

	return parsedResults;
}

} /* namespace slsr */
