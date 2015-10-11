/*
 * Recogniser.h
 *
 *  Created on: 2015. okt. 7.
 *      Author: a.strack
 */

#ifndef RECOGNISER_H_
#define RECOGNISER_H_

#include <memory>
#include <boost/optional.hpp>
#include "FileSource.h"
#include "Evaluator.h"
#include "RecognitionResult.h"

namespace slsr {

class Recogniser {
public:
	Recogniser();
	virtual ~Recogniser();

	void start(FileSource &source, std::shared_ptr<Evaluator> evaluator = { });

	bool isLoadResultIfAvailable() const {
		return m_loadResultIfAvailable;
	}

	void setLoadResultIfAvailable(bool loadResultIfAvailable) {
		m_loadResultIfAvailable = loadResultIfAvailable;
	}

	bool isEnableDebug() const {
		return m_enableDebug;
	}

	void setEnableDebug(bool enableDebug) {
		m_enableDebug = enableDebug;
	}

	bool isSeparateResults() const {
		return m_separateResults;
	}

	void setSeparateResults(bool separateResults) {
		m_separateResults = separateResults;
	}

private:
	bool m_loadResultIfAvailable;
	bool m_separateResults;
	bool m_enableDebug;
	boost::optional<RecognitionResult> loadResultIfAvailable(
			std::string imageName);
	void saveResults(std::string imagePath, cv::Mat resultImage,
			RecognitionResult recoknitionResult);
	std::string resultDataFile(std::string sourceName);
	std::string resultImageFile(std::string sourceName);
	void separateResults(const std::vector<Evaluator::ResultElement>& results,
			std::string destinationFolder);

	std::string debugTMName(std::string sourceName);
	std::string debugkNName(std::string sourceName);
	void debugMove(std::string sourceFile, std::string directoryName);
	void debugMoveResults(const std::vector<Evaluator::ResultElement>& results,
			std::string destinationFolder);
};

} /* namespace slsr */

#endif /* RECOGNISER_H_ */
