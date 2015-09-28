/*
 * RectangleCandidateFinder.cpp
 *
 *  Created on: 2015. szept. 28.
 *      Author: a.strack
 */

#include "RectangleCandidateFinder.h"
#include <cassert>
#include <algorithm>
#include <opencv2/imgproc.hpp>
#include "ImagingTools.h"

namespace slsr {

RectangleCandidateFinder::RectangleCandidateFinder(unsigned int width,
		unsigned int height) {
	this->m_rectangleAspectRation = (double) width / height;
	this->m_scales.push_back(height);
}

RectangleCandidateFinder::RectangleCandidateFinder(double rectangleAspectRation,
		std::vector<double> scales) :
		m_rectangleAspectRation(rectangleAspectRation), m_scales(scales) {
	// TODO Auto-generated constructor stub

}

RectangleCandidateFinder::~RectangleCandidateFinder() {
	// TODO Auto-generated destructor stub
}

std::vector<cv::Rect2i> RectangleCandidateFinder::getCandidates(
		cv::Mat source) {

	cv::Mat workImage;
	preprocess(source, workImage);

	cv::Mat gradientMapX;
	cv::Mat gradientMapY;
	calulateGradients(workImage, gradientMapX, gradientMapY);

	//For speed optimisation some values are precalculated
	cv::Mat absGradientMapX;
	cv::Mat absGradientMapY;
	convertScaleAbs(gradientMapX, absGradientMapX);
	convertScaleAbs(gradientMapY, absGradientMapY);
	for (auto scale : this->m_scales) {
		unsigned int width = (unsigned int) (this->m_rectangleAspectRation
				* scale);
		unsigned int height = (unsigned int) scale;
		cv::Mat voteMap = buildVoteMap(width, height, gradientMapX,
				gradientMapY, absGradientMapX, absGradientMapY);
	}

}

void RectangleCandidateFinder::preprocess(cv::Mat source,
		cv::OutputArray destination) {
	cv::Mat workImage;
//TODO maybe more verifying is needed
	if (source.channels() != 1) {
		cvtColor(source, workImage, cv::COLOR_BGR2GRAY);
	}

	destination = workImage;
}

void RectangleCandidateFinder::calulateGradients(cv::Mat source,
		cv::OutputArray gradientMapX, cv::OutputArray gradientMapY) {
//TODO try out Scharr too
	cv::Mat workGradientMapX;
	cv::Mat workGradientMapY;

	Sobel(source, workGradientMapX, CV_64F, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
	Sobel(source, workGradientMapY, CV_64F, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);

	gradientMapX = workGradientMapX;
	gradientMapY = workGradientMapY;
}

cv::Mat RectangleCandidateFinder::buildVoteMap(unsigned int width,
		unsigned int height, cv::Mat gradientMapX, cv::Mat gradientMapY,
		cv::Mat absGradientMapX, cv::Mat absGradientMapY) {
	cv::Mat workVotes;
	const unsigned int borderSize = std::max(width, height);
	ImagingTools::createImageBorder(gradientMapX, workVotes, borderSize);

	assert(gradientMapX.type() == cv::DataType<GradientType>::type);
	assert(gradientMapX.size() == gradientMapY.size());
	assert(gradientMapX.size() == absGradientMapX.size());
	assert(gradientMapX.size() == absGradientMapY.size());
	for (unsigned int y = 0;
			y < static_cast<unsigned int>(gradientMapX.size().height); ++y) {
		GradientType* pGradientX = gradientMapX.ptr<GradientType>(y);
		GradientType* pGradientY = gradientMapY.ptr<GradientType>(y);
		GradientType* pAbsGradientX = absGradientMapX.ptr<GradientType>(y);
		GradientType* pAbsGradientY = absGradientMapY.ptr<GradientType>(y);
		for (unsigned int x = 0;
				x < static_cast<unsigned int>(gradientMapX.size().width); ++x) {
			GradientType gradientX = pGradientX[x];
			GradientType gradientY = pGradientY[x];
			GradientType absGradientX = pAbsGradientX[x];
			GradientType absGradientY = pAbsGradientY[x];
			float approxMagnitude = absGradientX + absGradientY;
			if (approxMagnitude < MIN_GRADIENT_THRESHOLD) {
				//continue
			} else {
				giveVotesatPoint(workVotes,borderSize, x, y, width, height, gradientX, gradientY,
						absGradientX, absGradientY);
			}
		}
	}

	ImagingTools::removeImageBorder(workVotes, workVotes, borderSize);
	return workVotes;
}

void RectangleCandidateFinder::giveVotesatPoint(cv::Mat& votes,
		unsigned int borderSize, unsigned int x, unsigned int y,
		unsigned int width, unsigned int height, GradientType gradientX,
		GradientType gradientY, GradientType absGradientX,
		GradientType absGradientY) {

	bool isHorizontalEdge = absGradientX > absGradientY;
	//Can be calculated a precise magnitude if needed
	double votingMagnitude = absGradientX + absGradientY;
	//For precise voting position calculation
	//double directionRAD = atan2(gradientY, gradientX);
	//For easier debugging:
	//int directionDEG = (int) (180 + directionRAD / M_PI * 180);
	//votePointX = (int) round(distance * sin(directionRAD) + x);
	//votePointY = (int) round(distance * cos(directionRAD) + y);

	//TODO voting line with angle is needed?
	//TODO adding votes as a prepared cv::Mat line
	if (!isHorizontalEdge) {
		int votePointX;
		int votePointY;
		double votingDistance;
		votingDistance = static_cast<GradientType>(height) / 2;
		votePointX = x;
		votePointY = (gradientX > 0) ? y - (height / 2) : y + (height / 2);
		//Draw horizontal votes
		for (int distanceStep = 0; distanceStep < votingDistance;
				distanceStep++) {
			votes.at<GradientType>(votePointY + borderSize,
					votePointX + borderSize + distanceStep) += votingMagnitude;
			votes.at<GradientType>(votePointY + borderSize,
					votePointX + borderSize - distanceStep) += votingMagnitude;
			votes.at<GradientType>(votePointY + borderSize,
					votePointX + borderSize + votingDistance + distanceStep) -=
							votingMagnitude;
			votes.at<GradientType>(votePointY + borderSize,
					votePointX + borderSize - votingDistance - distanceStep) -=
							votingMagnitude;
		}
	} else {
		//Draw vertical votes
		int votePointX;
		int votePointY;
		double votingDistance;
		votingDistance = static_cast<GradientType>(width / 2);
		votePointX = (gradientY < 0) ? x - (width / 2) : x + (width / 2);
		votePointY = y;
		for (int distanceStep = 0; distanceStep < votingDistance;
				distanceStep++) {
			votes.at<GradientType>(votePointY + borderSize + distanceStep,
					votePointX + borderSize) += votingMagnitude;
			votes.at<GradientType>(votePointY + borderSize - distanceStep,
					votePointX + borderSize) += votingMagnitude;
			votes.at<GradientType>(
					votePointY + borderSize + votingDistance + distanceStep,
					votePointX + borderSize) -= votingMagnitude;
			votes.at<GradientType>(
					votePointY + borderSize - votingDistance - distanceStep,
					votePointX + borderSize) -= votingMagnitude;
		}
	}
}

} /* namespace slsr */
