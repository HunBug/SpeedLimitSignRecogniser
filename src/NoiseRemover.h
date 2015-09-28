/*
 * NoiseRemover.h
 *
 *  Created on: 2015. szept. 28.
 *      Author: a.strack
 */

#ifndef NOISEREMOVER_H_
#define NOISEREMOVER_H_

#include "ISourceNormaliser.h"

namespace slsr {

class NoiseRemover: public ISourceNormaliser {
public:
	static const unsigned int GAUSS_KERNEL_SIZE = 5;

	NoiseRemover();
	virtual ~NoiseRemover();

	virtual cv::Mat normalise(cv::Mat source) override;
};

} /* namespace slsr */

#endif /* NOISEREMOVER_H_ */
