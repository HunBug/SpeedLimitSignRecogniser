/*
 * NearestNeighbourRecogniser.cpp
 *
 *  Created on: 2015. okt. 3.
 *      Author: Ákos
 */

#include "NearestNeighbourRecogniser.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>

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
	cv::normalize(sign, sign, 0, 255, cv::NORM_MINMAX);
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
	std::cout << "e: " << CV_32F << " c: " << trainingSamples.type()
			<< std::endl;
	std::cout << "t: " << trainingSamples.rows << " R: "
			<< trainingResponses.rows << std::endl;
	auto trainingData = cv::ml::TrainData::create(trainingSamples,
			cv::ml::SampleTypes::ROW_SAMPLE, trainingResponses);

	classifier->setIsClassifier(true);
	classifier->setAlgorithmType(cv::ml::KNearest::Types::BRUTE_FORCE);
	classifier->setDefaultK(1);

	classifier->train(trainingData);
	cv::Mat result;
	classifier->findNearest(getFeatureExtractor()->extractFeatures(sign),
			classifier->getDefaultK(), result);

	//Just checking the settings
	std::cout << "Training data: " << std::endl << "getNSamples\t"
			<< trainingData->getNSamples() << std::endl << "getSamples\n"
			<< trainingData->getSamples() << std::endl << std::endl;

	std::cout << "Classifier :" << std::endl << "classifier->getDefaultK(): "
			<< classifier->getDefaultK() << std::endl
			<< "classifier->getIsClassifier()   : "
			<< classifier->getIsClassifier() << std::endl
			<< "classifier->getAlgorithmType(): "
			<< classifier->getAlgorithmType() << std::endl << std::endl;

	//confirming sample order
	std::cout << "matSample: " << std::endl << result << std::endl << std::endl;

	return "";
}

} /* namespace slsr */
