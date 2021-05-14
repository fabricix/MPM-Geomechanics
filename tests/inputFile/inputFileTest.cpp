//============================================================================
// Name        : json.cpp
// Author      : Fabricio Fernandez <fabricio.hmf@gmail.com>
// Version     :
// Copyright   : 
// Description :
//============================================================================

#include <iostream>
using namespace std;
using std::cout;

#include <fstream>
using std::ifstream;

#include<string>
using std::string;

#include<vector>
using std::vector;

#include "Json/json.hpp"
using json = nlohmann::json;

#include "Input.h"

int main() {

	// test using the json structure
#if 0
	// read a JSON file
	std::ifstream i("inputFile.json");
	json j;
	i >> j;

	vector<string> keys;
	keys.push_back("stressSchemeUpdate");
	keys.push_back("dtFraction");
	keys.push_back("nThreads");
	keys.push_back("damping");
	keys.push_back("results");
	keys.push_back("mesh");
	keys.push_back("materials");
	keys.push_back("body");
	keys.push_back("particles");

	for (size_t i=0; i<keys.size();i++)
	{
		cout<<keys.at(i)<<" = "<<j[keys.at(i)]<<"\n";
	}

	// verify numbers
	if(!j["nThreads"].is_number_unsigned())
	{
		cout<<"The number of threads must be greater than 0\n";
	}

	// verify list size
	if(j["particles"]["id"].size()!=j["particles"]["position"].size())
	{
		cout<<"The size of the id and the position must be equal\n";
	}
#else
	// test using the class input

	Input input;
	input.readInputFile("inputFile.json");

	// print all values
	cout<<input.getJson()<<"\n";

	// print
	cout<<"print Stress Scheme Update:\n";
	cout<<input.getJson()[input.getKeyWords()[Input::stressSchemeUpdate]]<<'\n';

#endif
	return 0;
}
