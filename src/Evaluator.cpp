/*
 * EvaluateTools.cpp
 *
 *  Created on: 2015. okt. 5.
 *      Author: a.strack
 */

#include "Evaluator.h"

#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/exception/exception.hpp>
#include <boost/lexical_cast.hpp>
#include <utility>

namespace slsr {

Evaluator::Evaluator(std::string annotationFilePath) {
	m_expectedResults = parseFile(annotationFilePath);
}

Evaluator::~Evaluator() {
	// TODO Auto-generated destructor stub
}

void Evaluator::addMeasurement(const RecognitionResult result,
		std::string fileName) {
	m_measuredResults[fileName].push_back(result);
}

Evaluator::Result Evaluator::summaryStatistics() {
	Evaluator::Result evaluateStatistics;
	for (auto& imageMeasuredResults : m_measuredResults) {
		for (auto& measuredResult : imageMeasuredResults.second) {
			//Dummy value, there is no default constructor
			RecognitionResult expectedResult =
					RecognitionResult::createNotFoundResult();
			auto evaluation = evaluate(measuredResult,
					imageMeasuredResults.first, expectedResult);
			Evaluator::ResultElement resultElement;
			resultElement.fileName = imageMeasuredResults.first;
			resultElement.measured = measuredResult;
			resultElement.expected = expectedResult;
			if (evaluation) {
				if (measuredResult.isSignFound()) {
					evaluateStatistics.truePositive.push_back(resultElement);
				} else {
					evaluateStatistics.trueNegative.push_back(resultElement);
				}
			} else {
				//TODO handle this case in a better way
				if (measuredResult.isSignFound()
						&& expectedResult.isSignFound()) {
					evaluateStatistics.falseNegative.push_back(resultElement);
				} else if (!measuredResult.isSignFound()
						&& expectedResult.isSignFound()) {
					evaluateStatistics.falseNegative.push_back(resultElement);

				} else if (measuredResult.isSignFound()
						&& !expectedResult.isSignFound()) {
					evaluateStatistics.falsePositive.push_back(resultElement);

				} else if (!measuredResult.isSignFound()
						&& !expectedResult.isSignFound()) {
					BOOST_THROW_EXCEPTION(
							std::logic_error(
									"Program error. This state should be handled in 'true evaluation'."));
				} else {
					BOOST_THROW_EXCEPTION(
							std::logic_error(
									"Unhandled result evaluation state"));
				}
			}
		}
	}

	return evaluateStatistics;
}

bool Evaluator::evaluate(const RecognitionResult result,
		std::string sourceFileName, RecognitionResult& expectedResult) const {
	auto fileExpectedResult = m_expectedResults.at(sourceFileName);
	bool isEqual = false;
	for (auto possibleExpectedResult : fileExpectedResult) {
		auto evaluateResult = evaluate(result, possibleExpectedResult);
		isEqual |= evaluateResult;
		expectedResult = possibleExpectedResult;
		if (evaluateResult) {
			break;
		}
	}
	return isEqual;
}

bool Evaluator::evaluate(const RecognitionResult& result,
		const RecognitionResult& evaluteResult) const {
	bool isEqual = result.isSignFound() == evaluteResult.isSignFound();
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

std::map<std::string, std::vector<RecognitionResult>> Evaluator::parseFile(
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
		if (lineParts.size() != 11) {
			BOOST_THROW_EXCEPTION(
					std::ios::failure(
							"Annotation file is not in the correct format."));
		}

		std::string imageFileName = lineParts.at(0);
		std::string annotation = lineParts.at(1);
		std::string cornerLeftX = lineParts.at(2);
		std::string cornerTopY = lineParts.at(3);
		std::string cornerRightX = lineParts.at(4);
		std::string cornerBottomY = lineParts.at(5);
		if (imageFileName.find("speedLimit") != std::string::npos) {
			cv::Rect position;
			unsigned int speedLimit;

			speedLimit = boost::lexical_cast<unsigned int>(
					annotation.substr(annotation.length() - 2));
			position.x = boost::lexical_cast<int>(cornerLeftX);
			position.y = boost::lexical_cast<int>(cornerTopY);
			position.width = boost::lexical_cast<int>(cornerRightX)
					- position.x;
			position.height = boost::lexical_cast<int>(cornerBottomY)
					- position.y;

			parsedResults[imageFileName].push_back(
					RecognitionResult::createFoundResult(position,
							static_cast<unsigned char>(speedLimit)));
		}
	}

	return parsedResults;
}

} /* namespace slsr */
