/*
 * DebugTools.h
 *
 *  Created on: 2015. okt. 10.
 *      Author: Ákos
 */

#ifndef DEBUGTOOLS_H_
#define DEBUGTOOLS_H_

#define PRINT_LINE_DEBUG std::cout<<"l:"<<__LINE__<<" "

class DebugTools {
public:
	DebugTools();
	virtual ~DebugTools();
};

#endif /* DEBUGTOOLS_H_ */
