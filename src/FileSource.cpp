/*
 * FileSource.cpp
 *
 *  Created on: 2015. szept. 23.
 *      Author: a.strack
 */

#include "FileSource.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <opencv2/imgcodecs.hpp>

namespace slsr {

FileSource::FileSource() {
	// TODO Auto-generated constructor stub

}

void FileSource::setSourcePath(std::string path, bool isDirectory) {
	const std::string imageExtension = "png";
	std::queue<std::string> newFilePaths;

	if(isDirectory) {
		boost::filesystem::directory_iterator directoryEnd;
		for( boost::filesystem::directory_iterator file( path ); file != directoryEnd; ++file )
		{
		    if( !boost::filesystem::is_regular_file( file->status() ) ) continue;
		    auto fileExtension = file->path().extension();
		    if(!boost::iequals(imageExtension, fileExtension)) continue;

		    newFilePaths.push(file->path().string());
		}
	}
	else{
		newFilePaths.push(path);
	}

	this->m_filePaths = newFilePaths;

}

bool FileSource::next() {
	if(m_filePaths.empty()){
		return false;
	}

	auto& path = m_filePaths.front();
	m_current = cv::imread(path.c_str());
	m_filePaths.pop();
	return true;
}

cv::Mat FileSource::getCurrent() {
	return m_current;
}

FileSource::~FileSource() {
	// TODO Auto-generated destructor stub
}

} /* namespace slsr */
