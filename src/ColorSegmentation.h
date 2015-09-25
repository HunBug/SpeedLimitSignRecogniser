/*
 * ColorSegmentation.h
 *
 *  Created on: 2015. szept. 23.
 *      Author: a.strack
 */

#ifndef COLORSEGMENTATION_H_
#define COLORSEGMENTATION_H_

#include "ISegmentation.h"

namespace slsr {

class ColorSegmentation: public ISegmentation {
public:
	ColorSegmentation();
	virtual ~ColorSegmentation();

	virtual cv::Mat getMask(cv::Mat source) override;
};

} /* namespace slsr */

#endif /* COLORSEGMENTATION_H_ */
