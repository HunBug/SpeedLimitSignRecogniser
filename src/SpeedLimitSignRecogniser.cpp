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

namespace po = boost::program_options;

int main(int argc, char* argv[]) {

	std::string path;
	bool isDirectory;

	po::options_description parameterDescription("Allowed options\nOnly one file or directory path can be set.");
	parameterDescription.add_options()("help", "Show help.")("f", po::value<std::string>(&path),
			"file")("d", po::value<std::string>(&path), "file");

	po::variables_map parametersMap;
	po::store(po::parse_command_line(argc, argv, parameterDescription), parametersMap);
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

	return 0;
}
