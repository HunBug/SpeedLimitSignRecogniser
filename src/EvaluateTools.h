/*
 * EvaluateTools.h
 *
 *  Created on: 2015. okt. 5.
 *      Author: a.strack
 */

#ifndef EVALUATETOOLS_H_
#define EVALUATETOOLS_H_

#include <string>
#include <map>
#include <opencv2/core.hpp>
#include "RecognitionResult.h"

namespace slsr {

class EvaluateTools {
public:
	struct EvaluateResult {
		bool isSignExpected;
		bool isSignFound;
		unsigned char limitExpected;
		unsigned char limitFound;
		cv::Rect signPositionExpected;
		cv::Rect signPositionFound;
	};

	static const unsigned int POSITION_TOLERANCE = 30;

	EvaluateTools(std::string annotationFilePath);
	virtual ~EvaluateTools();
	bool evaluate(const RecognitionResult result,
			std::string sourceFileName) const;
	bool evaluate(const RecognitionResult& result,
			const RecognitionResult& evaluteResult) const;
	void addMeasurement(const RecognitionResult result, std::string fileName);
	void printStatistics();
private:
	std::map<std::string, std::vector<RecognitionResult>> m_expectedResults;
	std::map<std::string, std::vector<RecognitionResult>> m_measuredResults;

	static std::map<std::string, std::vector<RecognitionResult>> parseFile(
			std::string filePath);
	static bool isInToleranceRange(unsigned int a, unsigned int b) {
		return abs(a - b) <= POSITION_TOLERANCE;
	}
};

} /* namespace slsr */

#endif /* EVALUATETOOLS_H_ */
