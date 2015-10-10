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

	void setSourcePath(std::string path, bool isDirectory = false);

	bool next();
	cv::Mat getCurrent();

	virtual ~FileSource();

	std::string getCurrentFileName() const;
	const std::string& getCurrentPath() const {
		return m_currentPath;
	}
	size_t ramainingSize() {
		return m_filePaths.size();
	}
	size_t size() {
		return m_initialSize;
	}

private:
	cv::Mat m_current;
	std::string m_currentPath;
	std::queue<std::string> m_filePaths;
	size_t m_initialSize;
};

} /* namespace slsr */

#endif /* FILESOURCE_H_ */
