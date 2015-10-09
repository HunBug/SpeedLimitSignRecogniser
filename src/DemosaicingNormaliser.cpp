/*
 * DemosaicingNormaliser.cpp
 *
 *  Created on: 2015. okt. 9.
 *      Author: a.strack
 */

#include "DemosaicingNormaliser.h"
#include <exception>
#include <boost/throw_exception.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace slsr {

DemosaicingNormaliser::DemosaicingNormaliser() {
	// TODO Auto-generated constructor stub
}

DemosaicingNormaliser::~DemosaicingNormaliser() {
	// TODO Auto-generated destructor stub
}

cv::Mat DemosaicingNormaliser::normalise(cv::Mat source) {
	if (source.channels() != 1) {
		cv::cvtColor(source, source, CV_BGR2GRAY);
	}
	if (source.type() != CV_8UC1) {
		BOOST_THROW_EXCEPTION(
				std::logic_error(
						"Only on unsigned char images can be demosaiced."));
	}
	const unsigned int DEINTERLACE = 0;
	cv::Mat green = cv::Mat::zeros(source.size(), source.type());
	cv::Mat red = cv::Mat::zeros(source.size(), source.type());
	cv::Mat blue = cv::Mat::zeros(source.size(), source.type());
	for (unsigned int y = 0;
			y < static_cast<unsigned int>(source.size().height) - 1; ++y) {
		//unsigned char* pRow = source.ptr<unsigned char>(y);
		for (unsigned int x = 0;
				x < static_cast<unsigned int>(source.size().width) - 1; ++x) {
			//unsigned char grayValue = pRow[x];
			if (1 == y % 2) {
				if (0 == x % 2) {
					green.at<unsigned char>(y, x) = source.at<unsigned char>(y,
							x + DEINTERLACE);
					red.at<unsigned char>(y, x) = source.at<unsigned char>(y,
							x + 1 - DEINTERLACE);
					blue.at<unsigned char>(y, x) = source.at<unsigned char>(
							y + 1, x + DEINTERLACE);
				} else {
					green.at<unsigned char>(y, x) = source.at<unsigned char>(y,
							x + 1 - DEINTERLACE);
					red.at<unsigned char>(y, x) = source.at<unsigned char>(y,
							x + DEINTERLACE);
					blue.at<unsigned char>(y, x) = source.at<unsigned char>(
							y + 1, x + 1 - DEINTERLACE);
				}
			} else {
				if (0 == x % 2) {
					green.at<unsigned char>(y, x) = source.at<unsigned char>(y,
							x + 1);
					red.at<unsigned char>(y, x) = source.at<unsigned char>(
							y + 1, x + 1);
					blue.at<unsigned char>(y, x) = source.at<unsigned char>(y,
							x);
				} else {
					green.at<unsigned char>(y, x) = source.at<unsigned char>(y,
							x);
					red.at<unsigned char>(y, x) = source.at<unsigned char>(
							y + 1, x);
					blue.at<unsigned char>(y, x) = source.at<unsigned char>(y,
							x + 1);
				}
			}
		}
	}
	cv::imwrite("green.png", green);
	cv::imwrite("red.png", red);
	cv::imwrite("vlue.png", blue);
	return green;
}

} /* namespace slsr */
