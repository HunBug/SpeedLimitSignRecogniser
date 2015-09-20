//============================================================================
// Name        : SpeedLimitSignRecogniser.cpp
// Author      : Ákos Strack
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
using namespace std;

//TODO: replace absolute paths in the eclipse project configurations

int main(int argc, char* argv[]) {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	 cv::Mat im;
	 im = cv::imread("c:\\Alom\\cica.png");
	 cout<<"im height:" << im.size().height <<" width: " << im.size().width << std::endl;
	return 0;
}
