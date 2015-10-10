/*
 * RectangleCandidateFinder.h
 *
 *  Created on: 2015. szept. 28.
 *      Author: a.strack
 */

#ifndef RECTANGLECANDIDATEFINDER_H_
#define RECTANGLECANDIDATEFINDER_H_

#include "ICandidateFinder.h"

namespace slsr {

class RectangleCandidateFinder: public ICandidateFinder {
public:
	typedef float GradientType;
	static constexpr GradientType MIN_GRADIENT_THRESHOLD = 100;
	static constexpr GradientType MIN_VOTE_THRESHOLD = 4750;

	RectangleCandidateFinder(unsigned int width, unsigned int height);
	RectangleCandidateFinder(double rectangleSpectRation,
			std::vector<double> scales);

	virtual ~RectangleCandidateFinder();
	virtual cv::Mat getCandidates(cv::Mat source) override;
private:
	double m_rectangleAspectRation;
	std::vector<double> m_scales;

	void preprocess(cv::Mat source, cv::OutputArray destination);
	void calulateGradients(cv::Mat source, cv::OutputArray gradientMapX,
			cv::OutputArray gradientMapY);
	cv::Mat buildVoteMap(unsigned int width, unsigned int height,
			cv::Mat gradientMapX, cv::Mat gradientMapY, cv::Mat absGradientMapX,
			cv::Mat absGradientMapY);
	void giveVotesatPoint(cv::Mat leftVotes, cv::Mat downVotes,
			cv::Mat rightVotes, cv::Mat upVotes, unsigned int borderSize,
			unsigned int x, unsigned int y, unsigned int width,
			unsigned int height, GradientType gradientX, GradientType gradientY,
			GradientType absGradientX, GradientType absGradientY,
			bool gradientReverseDirection);
	cv::Mat createMask(cv::Mat source);
};

} /* namespace slsr */

#endif /* RECTANGLECANDIDATEFINDER_H_ */
