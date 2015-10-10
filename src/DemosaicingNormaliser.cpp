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

//TODO Quick&Dirty implementation, but it works
cv::Mat DemosaicingNormaliser::normalise(cv::Mat source) {
	if (source.channels() != 1) {
		cv::cvtColor(source, source, CV_BGR2GRAY);
	}
	if (source.type() != CV_8UC1) {
		BOOST_THROW_EXCEPTION(
				std::logic_error(
						"Only on unsigned char images can be demosaiced."));
	}
	//Chanel names are not relevant!!!!!!
	cv::Mat green = cv::Mat::zeros(source.size(), source.type());
	cv::Mat red = cv::Mat::zeros(source.size(), source.type());
	cv::Mat blue = cv::Mat::zeros(source.size(), source.type());
	cv::Mat white = cv::Mat::zeros(source.size(), source.type());
	for (unsigned int y = 1;
			y < static_cast<unsigned int>(source.size().height) - 1; ++y) {
		//unsigned char* pRow = source.ptr<unsigned char>(y);
		for (unsigned int x = 1;
				x < static_cast<unsigned int>(source.size().width) - 1; ++x) {
			//unsigned char grayValue = pRow[x];
			if (0 == y % 2) {
				if (0 == x % 2) {
					green.at<unsigned char>(y, x) = source.at<unsigned char>(y,
							x);
					red.at<unsigned char>(y, x) = source.at<unsigned char>(y,
							x + 1);
					blue.at<unsigned char>(y, x) = source.at<unsigned char>(
							y + 1, x);
					white.at<unsigned char>(y, x) = source.at<unsigned char>(
							y + 1, x + 1);
				} else {
					green.at<unsigned char>(y, x) = source.at<unsigned char>(y,
							x - 1);
					red.at<unsigned char>(y, x) = source.at<unsigned char>(y,
							x);
					blue.at<unsigned char>(y, x) = source.at<unsigned char>(
							y + 1, x - 1);
					white.at<unsigned char>(y, x) = source.at<unsigned char>(
							y + 1, x);
				}
			} else {
				if (0 == x % 2) {
					green.at<unsigned char>(y, x) = source.at<unsigned char>(
							y - 1, x);
					red.at<unsigned char>(y, x) = source.at<unsigned char>(
							y - 1, x + 1);
					blue.at<unsigned char>(y, x) = source.at<unsigned char>(y,
							x);
					white.at<unsigned char>(y, x) = source.at<unsigned char>(y,
							x + 1);
				} else {
					green.at<unsigned char>(y, x) = source.at<unsigned char>(
							y - 1, x - 1);
					red.at<unsigned char>(y, x) = source.at<unsigned char>(
							y - 1, x);
					blue.at<unsigned char>(y, x) = source.at<unsigned char>(y,
							x - 1);
					white.at<unsigned char>(y, x) = source.at<unsigned char>(y,
							x);
				}
			}
		}
	}
	cv::equalizeHist(green, green);
	cv::equalizeHist(red, red);
	cv::equalizeHist(blue, blue);
	cv::equalizeHist(white, white);
	cv::resize(green, green, cv::Size(), 0.5, 0.5, cv::INTER_NEAREST);
	cv::resize(green, green, cv::Size(), 2.0, 2.0, cv::INTER_LANCZOS4);
	cv::resize(red, red, cv::Size(), 0.5, 0.5, cv::INTER_NEAREST);
	cv::resize(red, red, cv::Size(), 2.0, 2.0, cv::INTER_LANCZOS4);
	cv::resize(blue, blue, cv::Size(), 0.5, 0.5, cv::INTER_NEAREST);
	cv::resize(blue, blue, cv::Size(), 2.0, 2.0, cv::INTER_LANCZOS4);
	cv::resize(white, white, cv::Size(), 0.5, 0.5, cv::INTER_NEAREST);
	cv::resize(white, white, cv::Size(), 2.0, 2.0, cv::INTER_LANCZOS4);

	//cv::imwrite("green.png", green);
	//cv::imwrite("red.png", red);
	//cv::imwrite("blue.png", blue);
	//cv::imwrite("white.png", white);

	cv::addWeighted(red, 0.5, green, 0.5, 1, red);
	cv::addWeighted(blue, 0.5, white, 0.5, 1, blue);
	cv::addWeighted(blue, 0.5, red, 0.5, 1, red);

	cv::cvtColor(red, white, CV_GRAY2BGR);
	//cv::imwrite("result.png", red);
	return white;
}

} /* namespace slsr */
