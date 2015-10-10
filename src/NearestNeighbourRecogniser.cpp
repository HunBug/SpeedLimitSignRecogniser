/*
 * NearestNeighbourRecogniser.cpp
 *
 *  Created on: 2015. okt. 3.
 *      Author: Ákos
 */

#include "NearestNeighbourRecogniser.h"
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "DebugTools.h"

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
	/*cv::Mat debugIm = sign.clone();
	 cv::rectangle(debugIm, numbersRoi, cv::Scalar(255));
	 cv::imwrite("numbersRect.png", debugIm);*/
	cv::Mat numbers = sign(numbersRoi);
//	cv::cvtColor(numbers, numbers, CV_BGR2GRAY);
	cv::resize(numbers, numbers, cv::Size(25, 15), cv::INTER_CUBIC);
	cv::normalize(numbers, numbers, 0, 255, cv::NORM_MINMAX);
	auto classifier = getClassifier();
	std::vector<float> result;
	classifier->findNearest(getFeatureExtractor()->extractFeatures(numbers),
			classifier->getDefaultK(), result);
	return boost::lexical_cast < std::string > (result[0]);
}

cv::Ptr<cv::ml::KNearest> NearestNeighbourRecogniser::getClassifier() {
	//Not well docuomented, sample can be found at:
	//http://stackoverflow.com/questions/28035484/opencv-3-knn-implementation
	//TODO this is only a Q&D prototype
	if (!m_classifier.is_initialized()) {
		auto classifier = cv::ml::KNearest::create();
		cv::Mat trainingResponses(getPossibleResults().size(), 1, CV_32F);
		std::cout << "ts: " << trainingResponses.size() << std::endl;
		boost::optional < cv::Mat > trainingSamples;
		for (size_t templateIndex = 0;
				templateIndex < getPossibleResults().size(); templateIndex++) {
			short currentTemplateValue = getPossibleResults()[templateIndex];
			std::cout << "curval " << currentTemplateValue << std::endl;
			trainingResponses.at<float>(templateIndex) =
					static_cast<float>(currentTemplateValue);
			std::string fileName = boost::lexical_cast < std::string
					> (currentTemplateValue) + ".png";
			cv::Mat templateImage = cv::imread(fileName, cv::IMREAD_GRAYSCALE);
			auto features = getFeature(templateImage);

			if (!trainingSamples.is_initialized()) {
				trainingSamples = cv::Mat(0, features.size(), CV_32F);
				std::cout << "tsa: " << trainingResponses.size() << std::endl;
			}

			auto featureRow = cv::Mat(features);
			std::cout << "fr: " << featureRow.size() << std::endl;
			cv::transpose(featureRow, featureRow);
			trainingSamples.get().push_back(featureRow);
		}

		auto trainingData = cv::ml::TrainData::create(trainingSamples.get(),
				cv::ml::SampleTypes::ROW_SAMPLE, trainingResponses);
		std::cout << trainingResponses << std::endl;
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
	//cv::imwrite("numbersThreshold.png", workImage);
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
	}
	bool isRoiFound = candidateIndices.size() >= 2;
	if (isRoiFound) {
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
			std::cout << std::endl << candidateIndices.size() << " "
					<< weakestIndex;
			auto candidateIndicesIndex = std::find(candidateIndices.begin(),
					candidateIndices.end(), weakestIndex);
			assert(candidateIndicesIndex != candidateIndices.end());
			candidateIndices.erase(candidateIndicesIndex);
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
	}
	return isRoiFound;
}

std::vector<float> NearestNeighbourRecogniser::getFeature(cv::Mat source) {
	if (source.channels() > 1) {
		cv::cvtColor(source, source, CV_8U);
	}
	cv::resize(source, source, cv::Size(25, 15), cv::INTER_CUBIC);
	cv::normalize(source, source, 0, 255, cv::NORM_MINMAX);
	cv::imwrite("templatesample.png", source);
	return getFeatureExtractor()->extractFeatures(source);
}

} /* namespace slsr */
