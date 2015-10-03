/*
 * IRecogniser.h
 *
 *  Created on: 2015. okt. 3.
 *      Author: Ákos
 */

#ifndef IRECOGNISER_H_
#define IRECOGNISER_H_

#include <string>
#include <opencv2/core.hpp>

namespace slsr {

class IRecogniser {
public:
	IRecogniser() {
	}
	virtual ~IRecogniser() {
	}
	virtual std::string recognise(cv::Mat fullImage, cv::Rect signPosition) =0;
};

} /* namespace slsr */

#endif /* IRECOGNISER_H_ */
