/*
 * TemplateMatchingCandidateFinder.cpp
 *
 *  Created on: 2015. szept. 25.
 *      Author: a.strack
 */

#include "TemplateMatchingCandidateFinder.h"
#include <opencv2/imgcodecs.hpp>

namespace slsr {

TemplateMatchingCandidateFinder::TemplateMatchingCandidateFinder() {
	// TODO Auto-generated constructor stub

}

TemplateMatchingCandidateFinder::~TemplateMatchingCandidateFinder() {
	// TODO Auto-generated destructor stub
}

cv::Mat TemplateMatchingCandidateFinder::getCandidates(
		cv::Mat source) {
	//TODO read from resource or get an absolute path (from startup path)
	cv::Mat signHeader = cv::imread("signHeader.png");
	cv::Mat result;

	fastMatchTemplate(source,signHeader,result,MAX_PYRAMID_LEVEL);
	return result;
}

void TemplateMatchingCandidateFinder::fastMatchTemplate(cv::Mat srca,
		cv::Mat srcb, cv::Mat dst, int maxlevel) {
	/*std::vector<cv::Mat> refs, tpls, results;

	// Build Gaussian pyramid
	cv::buildPyramid(srca, refs, maxlevel);
	cv::buildPyramid(srcb, tpls, maxlevel);

	cv::Mat ref, tpl, res;

	// Process each level
	for (int level = maxlevel; level >= 0; level--) {
		ref = refs[level];
		tpl = tpls[level];
		res = cv::Mat::zeros(ref.size() + cv::Size(1, 1) - tpl.size(),
				CV_32FC1);

		if (level == maxlevel) {
			// On the smallest level, just perform regular template matching
			cv::matchTemplate(ref, tpl, res, CV_TM_CCORR_NORMED);
		} else {
			// On the next layers, template matching is performed on pre-defined
			// ROI areas.  We define the ROI using the template matching result
			// from the previous layer.

			cv::Mat mask;
			cv::pyrUp(results.back(), mask);

			cv::Mat mask8u;
			mask.convertTo(mask8u, CV_8U);

			// Find matches from previous layer
			std::vector<std::vector<cv::Point> > contours;
			cv::findContours(mask8u, contours, CV_RETR_EXTERNAL,
					CV_CHAIN_APPROX_NONE);

			// Use the contours to define region of interest and
			// perform template matching on the areas
			for (int i = 0; i < contours.size(); i++) {
				cv::Rect r = cv::boundingRect(contours[i]);
				cv::matchTemplate(ref(r + (tpl.size() - cv::Size(1, 1))), tpl,
						res(r), CV_TM_CCORR_NORMED);
			}
		}

		// Only keep good matches
		cv::threshold(res, res, 0.94, 1., CV_THRESH_TOZERO);
		results.push_back(res);
	}

	res.copyTo(dst);*/
}

} /* namespace slsr */
