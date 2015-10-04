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
	std::shared_ptr < IFeatureExtractor > fe(new RawPixelFeatureExtractor());
	NearestNeighbourRecogniser nnr(fe);

	//Do recognition
	fs.next();
	cv::Mat source = fs.getCurrent();
	cv::Mat normalised = nr.normalise(source);
	auto candidates = rcf.getCandidates(normalised);
	auto signs = tmd.getSigns(normalised, candidates);
	for (auto sign : signs) {
		std::string recognised = nnr.recognise(normalised, sign);
		if (!recognised.empty()) {
			std::cout << "result: " << recognised << std::endl;
		}
	}

	return 0;
}
