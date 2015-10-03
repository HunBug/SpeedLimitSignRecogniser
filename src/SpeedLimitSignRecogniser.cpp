//============================================================================
// Name        : SpeedLimitSignRecogniser.cpp
// Author      : Ákos Strack
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

//TODO: replace absolute paths in the eclipse project configurations
#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "NoiseRemover.h"
#include "RectangleCandidateFinder.h"
#include "FileSource.h"
#include "TemplateMatchingDetector.h"
#include "RawPixelFeatureExtractor.h"
#include "NearestNeighbourRecogniser.h"

namespace po = boost::program_options;
using namespace slsr;

int main(int argc, char* argv[]) {
	//Handle command line arguments
	std::string path;
	bool isDirectory;
	po::options_description parameterDescription(
			"Allowed options\nOnly one file or directory path can be set.");
	parameterDescription.add_options()("help", "Show help.")("f",
			po::value < std::string > (&path), "file")("d",
			po::value < std::string > (&path), "file");
	po::variables_map parametersMap;
	po::store(po::parse_command_line(argc, argv, parameterDescription),
			parametersMap);
	po::notify(parametersMap);
	if (parametersMap.count("help")) {
		std::cout << parameterDescription << std::endl;
		return 0;
	}
	if (parametersMap.count("f") + parametersMap.count("d") != 1) {
		std::cout << parameterDescription << std::endl;
		return 1;
	}
	isDirectory = (parametersMap.count("d") == 1);

	//Initialise recognition flow
	FileSource fs;
	fs.setSourcePath(path, isDirectory);
	NoiseRemover nr;
	std::vector<double> scales { 30, 35, 40, 45, 50, 60, 70 };
	RectangleCandidateFinder rcf(40.0 / 50.0, scales);
	TemplateMatchingDetector tmd;

	cv::Mat sample = cv::imread("sample2.png", cv::IMREAD_GRAYSCALE);
	cv::resize(sample, sample, cv::Size(25, 15), cv::INTER_CUBIC);
	std::shared_ptr < IFeatureExtractor > fe(new RawPixelFeatureExtractor());
	NearestNeighbourRecogniser nnr(fe);
	nnr.recognise(sample, cv::Rect(cv::Point(0, 0), sample.size()));

	return 0;
	//Do recognition
	fs.next();
	cv::Mat source = fs.getCurrent();
	cv::Mat normalised = nr.normalise(source);
	//TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//		RETURN RECTANGLE CENTER AND SIZE TOO
	//		AND USE IT IN THE NUMBER DETECTOR TOO
	//		not just the temlapte matching position
	auto candidates = rcf.getCandidates(normalised);
	tmd.getSigns(normalised, candidates);

	return 0;
}
