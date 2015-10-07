/*
 * Recogniser.h
 *
 *  Created on: 2015. okt. 7.
 *      Author: a.strack
 */

#ifndef RECOGNISER_H_
#define RECOGNISER_H_

#include <memory>
#include "FileSource.h"
#include "Evaluator.h"

namespace slsr {

class Recogniser {
public:
	Recogniser();
	virtual ~Recogniser();

	void start(FileSource &source, std::shared_ptr<Evaluator> evaluator = { });
};

} /* namespace slsr */

#endif /* RECOGNISER_H_ */
