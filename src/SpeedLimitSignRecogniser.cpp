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
#include "FileSource.h"
#include "Evaluator.h"
#include "Recogniser.h"

namespace po = boost::program_options;
using namespace slsr;

int main(int argc, char* argv[]) {
	//Handle command line arguments
	std::string path;
	std::string annotationFile;
	bool isDirectory;
	po::options_description parameterDescription(
			"Allowed options\nOnly one file or directory path can be set.");
	parameterDescription.add_options()("help", "Show help.")("f",
			po::value < std::string > (&path), "file")("d",
			po::value < std::string > (&path), "directory")("a",
			po::value < std::string > (&annotationFile),
			"annotation file for performance statistics");
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
	std::shared_ptr < Evaluator > evaluator;
	if (parametersMap.count("a") == 1) {
		std::cout << "Reading annotation file..." << std::endl;
		evaluator.reset(new Evaluator(annotationFile));
	}

	FileSource fileSource;
	fileSource.setSourcePath(path, isDirectory);

	Recogniser recogniser;
	recogniser.start(fileSource, evaluator);

	return 0;
}
