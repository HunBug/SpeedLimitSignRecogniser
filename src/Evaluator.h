/*
 * EvaluateTools.h
 *
 *  Created on: 2015. okt. 5.
 *      Author: a.strack
 */

#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#include <string>
#include <map>
#include <opencv2/core.hpp>
#include "RecognitionResult.h"

namespace slsr {

//TODO School is not taken into account yet
class Evaluator {
public:
	struct ResultElement {
		std::string fileName;
		RecognitionResult expected;
		RecognitionResult measured;
	};
	struct Result {
		std::vector<ResultElement> truePositive;
		std::vector<ResultElement> trueNegative;
		std::vector<ResultElement> falsePositive;
		std::vector<ResultElement> falseNegative;
		double f1Score() {
			if (0 == (precision() + recall())) {
				return 0;
			}
			return 2.0 * (precision() * recall()) / (precision() + recall());
		}
		double recall() {
			if (0 == (truePositive.size() + falseNegative.size())) {
				return 0;
			}
			return ((double) truePositive.size())
					/ (truePositive.size() + falseNegative.size());
		}
		double precision() {
			if (0 == (truePositive.size() + falsePositive.size())) {
				return 0;
			}
			return ((double) truePositive.size())
					/ (truePositive.size() + falsePositive.size());
		}
		size_t totalMeasurement() {
			return truePositive.size() + trueNegative.size()
					+ falsePositive.size() + falseNegative.size();
		}
	};
	static const unsigned int POSITION_TOLERANCE = 30;

	Evaluator(std::string annotationFilePath);
	virtual ~Evaluator();
	/**
	 * Evaluate a measured result on a stored expected result. Expected result is found by the source file name
	 * @param result is the result of the recognition algorithm
	 * @param sourceFileName the file name of the image. This is the key of connection with the expected result
	 * @param expectedResult returns the expectedResult. If there isn't any matching expected result then the value of this return value is undefinded
	 * @return true if everything is the same in the result as in the expected result.
	 */
	bool evaluate(const RecognitionResult result, std::string sourceFileName,
			RecognitionResult& expectedResult) const;
	bool evaluate(const RecognitionResult& result,
			const RecognitionResult& evaluteResult) const;
	void addMeasurement(const RecognitionResult result, std::string fileName);
	Result summaryStatistics();
private:
	std::map<std::string, std::vector<RecognitionResult>> m_expectedResults;
	std::map<std::string, std::vector<RecognitionResult>> m_measuredResults;

	static std::map<std::string, std::vector<RecognitionResult>> parseFile(
			std::string filePath);
	static bool isInToleranceRange(unsigned int a, unsigned int b) {
		return abs(a - b) <= POSITION_TOLERANCE;
	}
	static bool isContains(std::string text, std::string pattern,
			bool ignoreCase = false);
}
;

} /* namespace slsr */

#endif /* EVALUATOR_H_ */
