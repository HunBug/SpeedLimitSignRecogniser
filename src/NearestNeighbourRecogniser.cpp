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
#include "ImagingTools.h"

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
	//Increase search ROI;
	const int INCREASE_SIZE = 3;
	signPosition.x -= INCREASE_SIZE;
	signPosition.y -= INCREASE_SIZE;
	signPosition.width += 2 * INCREASE_SIZE;
	signPosition.height += 2 * INCREASE_SIZE;
	ImagingTools::shrinkRoiToImage(fullImage, signPosition);
	cv::Mat sign = fullImage(signPosition);
	_debug_image = fullImage.clone();
	_debug_signPosition = signPosition;
	cv::Scalar signColor(255, 0, 0);
	cv::rectangle(_debug_image, signPosition, signColor, 1.0);
	cv::cvtColor(sign, sign, CV_BGR2GRAY);
	cv::Rect numbersRoi;
	bool roiFound = getNumbersRoi(sign, numbersRoi);
	cv::Scalar numberRoiColor =
			roiFound ? cv::Scalar(128, 255, 0) : cv::Scalar(128, 0, 255);
	cv::Rect debugNumberRoi = numbersRoi;
	debugNumberRoi.x += signPosition.x;
	debugNumberRoi.y += signPosition.y;
	cv::rectangle(_debug_image, debugNumberRoi, numberRoiColor, 1.0);
	std::string resultText;
	if (roiFound) {
		cv::Mat numbers = sign(numbersRoi);
		auto numberFeatures = getNumberFeatures(numbers);
		auto classifier = getClassifier();
		std::vector<float> result;
		classifier->findNearest(numberFeatures, classifier->getDefaultK(),
				result);
		resultText = boost::lexical_cast < std::string > (result[0]);
	} else {
		resultText = "";
	}
	return resultText;
}

cv::Mat NearestNeighbourRecogniser::getSchoolTemplate() {
	static cv::Mat school;
	if (school.empty()) {
		school = cv::imread("school.png");
	}
	return school;
}

bool NearestNeighbourRecogniser::checkSchoolHeader(cv::Mat fullImage,
		cv::Rect mainSignPosition) {
	cv::Rect schoolSearchRoi;
	schoolSearchRoi.x = mainSignPosition.x - mainSignPosition.width / 2;
	schoolSearchRoi.y = mainSignPosition.y - mainSignPosition.height / 2;
	schoolSearchRoi.width = 2 * mainSignPosition.width;
	schoolSearchRoi.height = mainSignPosition.height;
	ImagingTools::shrinkRoiToImage(fullImage, schoolSearchRoi);

	//TODO revisit scales;
	static std::vector<double> scales { .30, .35, .4, .35, .5, .55, .75, .85,
			1.0 };
	cv::Mat result;
	double resultScale;
	double matchValue;
	ImagingTools::multiScaleTemplateMatching(fullImage(schoolSearchRoi),
			getSchoolTemplate(), scales, resultScale, matchValue);
	bool schoolSignFound = matchValue > MIN_SCHOOL_TEMPLATE_THRESHOLD;
	std::cout << "School ize:" << matchValue << std::endl;
	return schoolSignFound;
}

cv::Ptr<cv::ml::KNearest> NearestNeighbourRecogniser::getClassifier() {
	//Not well docuomented, sample can be found at:
	//http://stackoverflow.com/questions/28035484/opencv-3-knn-implementation
	if (!m_classifier.is_initialized()) {
		auto classifier = cv::ml::KNearest::create();
		cv::Mat trainingResponses(getPossibleResults().size(), 1, CV_32F);
		cv::Mat trainingSamples;
		for (size_t templateIndex = 0;
				templateIndex < getPossibleResults().size(); templateIndex++) {
			short currentTemplateValue = getPossibleResults()[templateIndex];
			trainingResponses.at<float>(templateIndex) =
					static_cast<float>(currentTemplateValue);
			std::string fileName = boost::lexical_cast < std::string
					> (currentTemplateValue) + ".png";
			cv::Mat templateImage = cv::imread(fileName, cv::IMREAD_GRAYSCALE);
			auto features = getNumberFeatures(templateImage);

			if (trainingSamples.empty()) {
				trainingSamples = cv::Mat(0, features.size(), CV_32F);
			}

			auto featureRow = cv::Mat(features);
			cv::transpose(featureRow, featureRow);
			trainingSamples.push_back(featureRow);
		}

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
	//cv::imwrite("numbersThreshold.png", workImage);
	workImage = 255 - workImage;
	cv::Mat debugWorkImage;
	cv::cvtColor(workImage, debugWorkImage, CV_GRAY2BGR);
	debugWorkImage.copyTo(
			_debug_image(
					cv::Rect(0, 0, workImage.size().width,
							workImage.size().height)));

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
		} else {
			drawContours(_debug_image(_debug_signPosition), contours,
					contourIndex, cv::Scalar(0, 0, 128), 1, 8);
		}
	}
	bool isRoiFound = candidateIndices.size() >= 2;
	if (isRoiFound) {
		//TODO better criterion on filtering. Focusing on false positive border matches!
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
			drawContours(_debug_image(_debug_signPosition), contours,
					weakestIndex, cv::Scalar(0, 128, 128), 1, 8);
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
		drawContours(_debug_image(_debug_signPosition), contours,
				candidateIndices[0], cv::Scalar(0, 128, 0), 1, 8);
		drawContours(_debug_image(_debug_signPosition), contours,
				candidateIndices[1], cv::Scalar(0, 128, 0), 1, 8);
	}
	return isRoiFound;
}

std::vector<float> NearestNeighbourRecogniser::getNumberFeatures(
		cv::Mat source) {
	if (source.channels() > 1) {
		cv::cvtColor(source, source, CV_8U);
	}
	cv::resize(source, source, cv::Size(25, 15), cv::INTER_CUBIC);
	cv::normalize(source, source, 0, 255, cv::NORM_MINMAX);
	return getFeatureExtractor()->extractFeatures(source);
}

} /* namespace slsr */
