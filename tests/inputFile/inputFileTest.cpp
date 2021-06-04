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

int main(int argc, char **argv) {

	string fileName;

	if(argc<2){
		cout<< "insert the name of the file as program argument " << "\n";
		return 0;
	}
	else{
		fileName=string(argv[1]);
	}

	// test using the json structure
#if 0
	// read a JSON file
	std::ifstream i(fileName);
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

	Input::readInputFile(fileName);

	json j = Input::getJson();

	// print
//	cout<<"print Stress Scheme Update:\n";
//	if(input.getJson().contains(input.getKeyWords()[Input::stressSchemeUpdate]))
//	{
//		cout<<input.getJson()[input.getKeyWords()[Input::stressSchemeUpdate]]<<'\n';
//	}
//	else
//	{
//		cout << "Please verify the input file name and keywords\n";
//	}
//
//	if(input.getJson().contains(input.getKeyWords()[Input::alpha])){
//
//		cout<< "testing containing alpha -> OK\n";
//	}
//	else
//	{
//		cout<< "testing containing alpha -> FAIL\n";
//	}
//
	//cout<< "couaints body = ";
	//cout<<j["body"].contains("cuboid")<<"\n";

	//cout<< "cuboids size = ";
	//cout<<j["body"]["cuboid"]["id"].size()<<"\n";

	cout<<"body size "<<j["body"].size()<<"\n";
	cout<<"material size "<<j["material"].size()<<"\n";
	cout<<"material elastic id size "<<j["material"]["elastic"]["id"].size()<<"\n";

	// iteration over the materials

	json::iterator it;

	for( it = j["material"].begin(); it!=j["material"].end();it++){
		cout<<*it<<"\n";
	}

	// delete materials
	j.erase("material");

	cout<<"print the materials again \n";

	for( it = j["material"].begin(); it!=j["material"].end();it++){
			cout<<*it<<"\n";
	}

	// verify data
	cout<< "mesh was found: "<<Input::verifyData(j,Input::KeyWords::mesh)<<"\n";
	cout<< "nCels was found: "<<Input::verifyData(j["mesh"],Input::KeyWords::nCells)<<"\n";

#endif
	return 0;
}
