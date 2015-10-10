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

private:
	bool m_loadResultIfAvailable;
	boost::optional<RecognitionResult> loadResultIfAvailable(
			std::string imageName);
	void saveResults(std::string imagePath, cv::Mat resultImage,
			RecognitionResult recoknitionResult);
};

} /* namespace slsr */

#endif /* RECOGNISER_H_ */
