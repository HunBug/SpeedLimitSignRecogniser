/*
 * NearestNeighbourRecogniser.cpp
 *
 *  Created on: 2015. okt. 3.
 *      Author: Ákos
 */

#include "NearestNeighbourRecogniser.h"
#include <boost/lexical_cast.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace slsr {

NearestNeighbourRecogniser::NearestNeighbourRecogniser(
		std::shared_ptr<IFeatureExtractor> featureExtractor) :
		IRecogniser(featureExtractor) {
	// TODO Auto-generated constructor stub

}

NearestNeighbourRecogniser::~NearestNeighbourRecogniser() {
	// TODO Auto-generated destructor stub
}

std::string NearestNeighbourRecogniser::recognise(cv::Mat fullImage,
		cv::Rect signPosition) {
	cv::Mat sign = fullImage(signPosition);
	cv::cvtColor(sign, sign, CV_BGR2GRAY);
	cv::Rect numbersRoi;
	bool roiFound = getNumbersRoi(sign, numbersRoi);
	if (!roiFound) {
		return "";
	}

	cv::rectangle(sign, numbersRoi, cv::Scalar(255));
	cv::imwrite("numbers.png", sign);
	cv::normalize(sign, sign, 0, 255, cv::NORM_MINMAX);
	auto classifier = getClassifier();
	cv::Mat result;
	classifier->findNearest(getFeatureExtractor()->extractFeatures(sign),
			classifier->getDefaultK(), result);
	return boost::lexical_cast < std::string > (result.at<float>(1));
}

cv::Ptr<cv::ml::KNearest> NearestNeighbourRecogniser::getClassifier() {
	//TODO this is only a Q&D prototype
	if (!m_classifier.is_initialized()) {
		auto classifier = cv::ml::KNearest::create();
		cv::Mat s25 = cv::imread("25.png", cv::IMREAD_GRAYSCALE);
		cv::resize(s25, s25, cv::Size(25, 15), cv::INTER_CUBIC);
		cv::normalize(s25, s25, 0, 255, cv::NORM_MINMAX);
		cv::Mat s30 = cv::imread("30.png", cv::IMREAD_GRAYSCALE);
		cv::resize(s30, s30, cv::Size(25, 15), cv::INTER_CUBIC);
		cv::normalize(s30, s30, 0, 255, cv::NORM_MINMAX);
		cv::Mat s35 = cv::imread("35.png", cv::IMREAD_GRAYSCALE);
		cv::resize(s35, s35, cv::Size(25, 15), cv::INTER_CUBIC);
		cv::normalize(s35, s35, 0, 255, cv::NORM_MINMAX);
		auto feature25 = getFeatureExtractor()->extractFeatures(s25);
		auto feature30 = getFeatureExtractor()->extractFeatures(s30);
		auto feature35 = getFeatureExtractor()->extractFeatures(s35);
		cv::Mat trainingResponses = (cv::Mat_<float>(3, 1) << 25, 30, 35);
		cv::Mat trainingSamples(0, feature25.size(), CV_32F);
		auto a = cv::Mat(feature25);
		cv::transpose(a, a);
		trainingSamples.push_back(a);
		a = cv::Mat(feature30);
		cv::transpose(a, a);
		trainingSamples.push_back(a);
		a = cv::Mat(feature35);
		cv::transpose(a, a);
		trainingSamples.push_back(a);
		auto trainingData = cv::ml::TrainData::create(trainingSamples,
				cv::ml::SampleTypes::ROW_SAMPLE, trainingResponses);

		classifier->setIsClassifier(true);
		classifier->setAlgorithmType(cv::ml::KNearest::Types::BRUTE_FORCE);
		classifier->setDefaultK(1);

		classifier->train(trainingData);
		m_classifier = classifier;
	}
	return m_classifier.get();
}

bool NearestNeighbourRecogniser::getNumbersRoi(cv::Mat source,
		cv::Rect& numbersRoi) {
	cv::Mat workImage;
	cv::threshold(source, workImage, 1, 255, cv::THRESH_OTSU);
	workImage = 255 - workImage;
	//Changes workimage!
	std::vector < std::vector<cv::Point> > contours;
	cv::findContours(workImage, contours, CV_RETR_EXTERNAL,
			CV_CHAIN_APPROX_NONE);
	//Find a blob pair that should be the numbers
	std::vector<int> candidateIndices;
	for (int contourIndex = 0; contourIndex < contours.size(); contourIndex++) {
		auto& contour = contours[contourIndex];
		bool areaOk = cv::contourArea(contour) > 30;
		auto boundingBox = cv::boundingRect(contour);
		bool widthOk = boundingBox.width > 10;
		bool heightOk = boundingBox.height > 10;
		if (areaOk && widthOk && heightOk) {
			candidateIndices.push_back(contourIndex);
		}
		//cv::drawContours(workImage, contours, contourIndex, cv::Scalar(0),-1);
	}
	bool isRoiFound = candidateIndices.size() >= 2;
	while (candidateIndices.size() > 2) {
		//find the weakest
		int weakestIndex = 0;
		int mostTop = source.size().height;
		for (auto index : candidateIndices) {
			auto boundingBox = cv::boundingRect(contours[index]);
			auto top = boundingBox.y;
			if (top < mostTop) {
				mostTop = top;
				weakestIndex = index;
			}
		}

		candidateIndices.erase(candidateIndices.begin() + weakestIndex);
	}
	assert(candidateIndices.size() == 2);
	auto boundingBox1 = cv::boundingRect(contours[candidateIndices[0]]);
	auto boundingBox2 = cv::boundingRect(contours[candidateIndices[1]]);
	numbersRoi.x = std::min(boundingBox1.x, boundingBox2.x);
	numbersRoi.y = std::min(boundingBox1.y, boundingBox2.y);
	numbersRoi.width = std::max(boundingBox1.x + boundingBox1.width,
			boundingBox2.x + boundingBox2.width) - numbersRoi.x;
	numbersRoi.height = std::max(boundingBox1.y + boundingBox1.height,
			boundingBox2.y + boundingBox2.height) - numbersRoi.y;
	return isRoiFound;
}

} /* namespace slsr */
