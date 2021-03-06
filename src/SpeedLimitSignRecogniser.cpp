//============================================================================
// Name        : SpeedLimitSignRecogniser.cpp
// Author      : �kos Strack
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

//TODO: replace absolute paths in the eclipse project configurations
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp>
#include "FileSource.h"
#include "Evaluator.h"
#include "Recogniser.h"

namespace po = boost::program_options;
using namespace slsr;

int main(int argc, char* argv[]) {
	boost::timer::auto_cpu_timer measureProgram(
			"Whole progrem: %t sec CPU, %w sec real");
	//Handle command line arguments
	std::string path;
	std::string annotationFile;
	po::options_description parameterDescription(
			"Allowed options\nOnly one file or directory path can be set.");
	parameterDescription.add_options()("help", "Show help.")("f",
			po::value < std::string > (&path), "file")("d",
			po::value < std::string > (&path), "directory")("a",
			po::value < std::string > (&annotationFile),
			"annotation file for performance statistics")("force",
			"force re-process")("debug", "enable debug images")("separate",
			"separate result images");
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
	bool isReprocess = parametersMap.count("force") > 0;
	bool isDirectory = (parametersMap.count("d") == 1);
	bool isSeparateResults = parametersMap.count("separate") > 0;
	bool isDebug = parametersMap.count("debug") > 0;
	std::shared_ptr < Evaluator > evaluator;
	if (parametersMap.count("a") == 1) {
		std::cout << "Reading annotation file..." << std::endl;
		evaluator.reset(new Evaluator(annotationFile));
	}
	FileSource fileSource;
	fileSource.setSourcePath(path, isDirectory);

	Recogniser recogniser;
	recogniser.setLoadResultIfAvailable(!isReprocess);
	recogniser.setEnableDebug(isDebug);
	recogniser.setSeparateResults(isSeparateResults);
	recogniser.start(fileSource, evaluator);

	return 0;
}
