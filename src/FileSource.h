/*
 * FileSource.h
 *
 *  Created on: 2015. szept. 23.
 *      Author: a.strack
 */

#ifndef FILESOURCE_H_
#define FILESOURCE_H_

#include <string>
#include <queue>
#include <opencv2/core.hpp>

namespace slsr {

class FileSource {
public:
	FileSource();

	void setSourcePath(std::string path, bool isDirectory=false);

	bool next();
	cv::Mat getCurrent();

	virtual ~FileSource();
private:
	cv::Mat m_current;
	std::queue<std::string> m_filePaths;
};

} /* namespace slsr */

#endif /* FILESOURCE_H_ */
