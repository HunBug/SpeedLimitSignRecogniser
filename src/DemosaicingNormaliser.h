/*
 * DemosaicingNormaliser.h
 *
 *  Created on: 2015. okt. 9.
 *      Author: a.strack
 */

#ifndef DEMOSAICINGNORMALISER_H_
#define DEMOSAICINGNORMALISER_H_

#include "ISourceNormaliser.h"

namespace slsr {

class DemosaicingNormaliser: public ISourceNormaliser {
public:
	DemosaicingNormaliser();
	virtual ~DemosaicingNormaliser();
	virtual cv::Mat normalise(cv::Mat source) override;
};

} /* namespace slsr */

#endif /* DEMOSAICINGNORMALISER_H_ */
