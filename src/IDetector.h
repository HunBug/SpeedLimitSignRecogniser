/*
 * IDetector.h
 *
 *  Created on: 2015. szept. 29.
 *      Author: a.strack
 */

#ifndef IDETECTOR_H_
#define IDETECTOR_H_

#include <opencv2/core.hpp>

namespace slsr {

class IDetector {
public:
	IDetector() {
	}
	virtual ~IDetector() {
	}

	virtual std::vector<cv::Rect2i> getSigns(cv::Mat source,
			cv::Mat candidateMap) =0;
};

} /* namespace slsr */

#endif /* IDETECTOR_H_ */
