//============================================================================
// Name        : SpeedLimitSignRecogniser.cpp
// Author      : Ákos Strack
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
using namespace std;

//TODO: replace absolute paths in the eclipse project configurations

int main(int argc, char* argv[]) {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	std::string s;
	 int a[10];
	 for(int i=0;i<10;i++)
	 {
	 a[i]=i+10;
	 }
	cin >> s;
	 BOOST_FOREACH(int k, a)
	 {
		 cout<<"\t"<<k;
	 }
	 std::cout << argv[0] << " " << boost::filesystem::file_size(argv[0]) << '\n';
	return 0;
}
