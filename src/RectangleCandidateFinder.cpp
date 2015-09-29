/*
 * RectangleCandidateFinder.cpp
 *
 *  Created on: 2015. szept. 28.
 *      Author: a.strack
 */

#include "RectangleCandidateFinder.h"
#include <cassert>
#include <algorithm>
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include "ImagingTools.h"

//debug
#include <string>
#include <boost/lexical_cast.hpp>

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
	cv::Mat voteMap(workImage.size(), cv::DataType<GradientType>::type, 0);
	for (auto scale : this->m_scales) {
		unsigned int width = (unsigned int) (this->m_rectangleAspectRation
				* scale);
		unsigned int height = (unsigned int) scale;
		cv::Mat currentVoteMap = buildVoteMap(width, height, gradientMapX,
				gradientMapY, absGradientMapX, absGradientMapY);
		voteMap += currentVoteMap;
	}

	//TODO remove
	double min, max;
	cv::Point min_loc, max_loc;
	cv::minMaxLoc(voteMap, &min, &max, &min_loc, &max_loc);
	cv::Point cornerVector(30/*signWidht*// 2, 40/*signHeight*// 2);
	voteMap.convertTo(voteMap, CV_8UC1, 255 / max);
	cv::imwrite("voteMap.png", voteMap);
	cv::addWeighted(workImage, 0.75, voteMap, 0.25, 0.0, workImage);
	cv::rectangle(workImage, max_loc - cornerVector, max_loc + cornerVector,
			cv::Scalar(255));
	cv::imwrite("votes.png", workImage);

	return std::vector<cv::Rect2i>();
}

void RectangleCandidateFinder::preprocess(cv::Mat source,
		cv::OutputArray destination) {
	cv::Mat workImage;
//TODO maybe more verifying is needed
	if (source.channels() != 1) {
		cvtColor(source, workImage, cv::COLOR_BGR2GRAY);
	} else {
		workImage = source;
	}

	workImage.copyTo(destination);
}

void RectangleCandidateFinder::calulateGradients(cv::Mat source,
		cv::OutputArray gradientMapX, cv::OutputArray gradientMapY) {
//TODO try out Scharr too
	cv::Mat workGradientMapX;
	cv::Mat workGradientMapY;

	Sobel(source, workGradientMapX, cv::DataType<GradientType>::type, 1, 0, 3,
			1, 0, cv::BORDER_DEFAULT);
	Sobel(source, workGradientMapY, cv::DataType<GradientType>::type, 0, 1, 3,
			1, 0, cv::BORDER_DEFAULT);

	workGradientMapX.copyTo(gradientMapX);
	workGradientMapY.copyTo(gradientMapY);
}

cv::Mat RectangleCandidateFinder::buildVoteMap(unsigned int width,
		unsigned int height, cv::Mat gradientMapX, cv::Mat gradientMapY,
		cv::Mat absGradientMapX, cv::Mat absGradientMapY) {
	const unsigned int borderSize = std::max(width, height);

	cv::Mat upVotes;
	cv::Mat downVotes;
	cv::Mat rightVotes;
	cv::Mat leftVotes;
	ImagingTools::createEmptyImageWithBorder(gradientMapX, upVotes, borderSize);
	ImagingTools::createEmptyImageWithBorder(gradientMapX, downVotes,
			borderSize);
	ImagingTools::createEmptyImageWithBorder(gradientMapX, rightVotes,
			borderSize);
	ImagingTools::createEmptyImageWithBorder(gradientMapX, leftVotes,
			borderSize);
	//assert(gradientMapX.type() == cv::DataType<GradientType>::type);
	assert(gradientMapX.size() == gradientMapY.size());
	assert(gradientMapX.size() == absGradientMapX.size());
	assert(gradientMapX.size() == absGradientMapY.size());
	for (unsigned int y = 0;
			y < static_cast<unsigned int>(gradientMapX.size().height); ++y) {
		GradientType* pGradientX = gradientMapX.ptr<GradientType>(y);
		GradientType* pGradientY = gradientMapY.ptr<GradientType>(y);
		unsigned char* pAbsGradientX = absGradientMapX.ptr<unsigned char>(y);
		unsigned char* pAbsGradientY = absGradientMapY.ptr<unsigned char>(y);
		for (unsigned int x = 0;
				x < static_cast<unsigned int>(gradientMapX.size().width); ++x) {
			GradientType gradientX = pGradientX[x];
			GradientType gradientY = pGradientY[x];
			unsigned char absGradientX = pAbsGradientX[x];
			unsigned char absGradientY = pAbsGradientY[x];
			float approxMagnitude = absGradientX + absGradientY;
			if (approxMagnitude < MIN_GRADIENT_THRESHOLD) {
				//continue
			} else {
				giveVotesatPoint(leftVotes, downVotes, rightVotes, upVotes,
						borderSize, x, y, width, height, gradientX, gradientY,
						absGradientX, absGradientY);
			}
		}
	}

	ImagingTools::removeImageBorder(downVotes, downVotes, borderSize);
	ImagingTools::removeImageBorder(leftVotes, leftVotes, borderSize);
	ImagingTools::removeImageBorder(upVotes, upVotes, borderSize);
	ImagingTools::removeImageBorder(rightVotes, rightVotes, borderSize);

	cv::Mat downMask = createMask(downVotes);
	cv::Mat upMask = createMask(upVotes);
	cv::Mat leftMask = createMask(leftVotes);
	cv::Mat rightMask = createMask(rightVotes);

	cv::Mat mask = downMask & upMask & leftMask & rightMask;
	cv::Mat votes = downVotes + leftVotes + upVotes + rightVotes;
	cv::imwrite(
			(boost::lexical_cast<std::string>(width) + "votesdownMask.png").c_str(),
			downMask);
	cv::imwrite(
			(boost::lexical_cast<std::string>(width) + "votesupMask.png").c_str(),
			upMask);
	cv::imwrite(
			(boost::lexical_cast<std::string>(width) + "votesrightMask.png").c_str(),
			rightMask);
	cv::imwrite(
			(boost::lexical_cast<std::string>(width) + "votesleftMask.png").c_str(),
			leftMask);
	cv::imwrite(
			(boost::lexical_cast<std::string>(width) + "votesMask.png").c_str(),
			mask);
	mask.convertTo(mask, CV_8UC1);
	cv::Mat maskedVotes;
	votes.copyTo(maskedVotes, mask);

	cv::Mat combined;
	ImagingTools::normalizeToMax(maskedVotes, combined);
	cv::imwrite(
			(boost::lexical_cast<std::string>(width) + "votesCombined.png").c_str(),
			combined);

	cv::Mat maskedVotesAll = leftVotes+rightVotes+upVotes+downVotes;
	ImagingTools::normalizeToMax(maskedVotesAll, combined);
	cv::imwrite(
				(boost::lexical_cast<std::string>(width) + "votesCombinedAll.png").c_str(),
				combined);
	return maskedVotes;
}

void RectangleCandidateFinder::giveVotesatPoint(cv::Mat& leftVotes,
		cv::Mat& downVotes, cv::Mat& rightVotes, cv::Mat& upVotes,
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
		double votingDistance = static_cast<GradientType>(height) / 2;
		int votePointX = x;
		bool isUp = (gradientX > 0);
		int votePointY = isUp ? y - (height / 2) : y + (height / 2);
		auto& votes = isUp ? upVotes : downVotes;
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
		double votingDistance = static_cast<GradientType>(width / 2);
		bool isLeft = gradientY < 0;
		int votePointX = isLeft ? x - (width / 2) : x + (width / 2);
		int votePointY = y;
		auto& votes = isLeft ? leftVotes : rightVotes;
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

cv::Mat RectangleCandidateFinder::createMask(cv::Mat source) {
	cv::Mat mask;
	cv::threshold(source, mask, MIN_VOTE_THRESHOLD, 255,
			cv::ThresholdTypes::THRESH_BINARY);
	int erosion_size = 10;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,
			cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
			cv::Point(erosion_size, erosion_size));
	cv::dilate(mask, mask, element);
	return mask;
}

} /* namespace slsr */
