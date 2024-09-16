/*
 * Output.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#if defined (linux) || defined(__linux__)
#include <sys/stat.h>
#endif

#if defined (_WIN64) || defined(_WIN32)
#include <direct.h>
#include <windows.h>
#endif

#include "Output.h"
#include <DynamicRelaxation.h>

#include <iostream>
using std::cout;
using std::left;
using std::right;

#include<string>
using std::string;
using std::to_string;

#include <fstream>
using std::ofstream;

#include <iomanip>
using std::fixed;
using std::scientific;
using std::setw;

#include <vector>
using std::vector;

namespace Output{

	vector<string> printFields;

	void configureResultFiels(vector<string> fields)
	{
		printFields=fields;
	}

	bool isFieldRequired(string ifield) {

		for (size_t i = 0; i < printFields.size(); ++i) {

			if (printFields.at(i)==ifield || printFields.at(i)=="all" ) {

				return true;
			}
		}
		return false;
	}

	namespace OutputTolerance
	{
		double deltaValue=1.0e-38;
	}

	namespace Folders
	{
		string edian="";

		// grid
		bool gridFolderExist=false;
		string gridFolderName="grid";
		string gridFileName="eulerianGrid.vtu";
		
		// particles
		bool particleFolderExist=false;
		string particleFolderName="particles";
		string particleFileName="particles";
		string particleFileTimeSerie="particleTimeSerie";
		vector<double> particleFilesTime;
	}

	void defineEdian(){

		int16_t i = 1;
		int8_t *p = (int8_t*) &i;
		Folders::edian=(p[0]==1)?"LittleEndian":"BigEndian";
	}

	void createGridFolder(){

		if (Folders::gridFolderExist)
			return;
		
		int status=0;

		#if defined (linux) || defined(__linux__)
		status=mkdir(Folders::gridFolderName.c_str(),0777);
		#endif

		#if defined (_WIN64) || defined(_WIN32)
		status=_mkdir(Folders::gridFolderName.c_str());
		#endif

		if (status==-1)
			Folders::gridFolderExist=true;
	}

	void createParticleFolder(){

		if (Folders::particleFolderExist)
			return;
		
		int status=0;

		#if defined (linux) || defined(__linux__)
		status=mkdir(Folders::particleFolderName.c_str(),0777);
		#endif

		#if defined (_WIN64) || defined(_WIN32)
		status=_mkdir(Folders::particleFolderName.c_str());
		#endif

		if (status==-1)
			Folders::particleFolderExist=true;
	}
	
	void writeParticles(vector<Particle*>* particles, double time){

		// define edian
		if(Folders::edian==""){
			defineEdian();
		}

		// create particle folder
		if(!Folders::particleFolderExist){
			createParticleFolder();
		}
		
		// add time in loop time vector
		Folders::particleFilesTime.push_back(time);

		// open particle file
		ofstream partFile;
		partFile.open(Folders::particleFolderName+"/"+Folders::particleFileName+"_"+to_string(Folders::particleFilesTime.size())+".vtu");
		partFile.precision(4);

		// particle data
		int nPoints= static_cast<unsigned int>(particles->size());

		// write results
		partFile<<"<?xml version=\"1.0\"?>\n";
		partFile<<"<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\""<<Folders::edian.c_str()<<"\">\n";
		partFile<<"<UnstructuredGrid>\n";
		
		// piece
		partFile<<fixed<<"<Piece NumberOfPoints=\""<<nPoints<<"\" NumberOfCells=\""<<nPoints<<"\">\n";
		
		// points
		partFile<<"<Points>\n";
		
		// particle position
		partFile<<"<DataArray type=\"Float64\" NumberOfComponents=\"3\" Format=\"ascii\">\n";
		for (int i = 0; i < nPoints; ++i) {
			Vector3d pos=particles->at(i)->getPosition();
			partFile<<scientific<<pos(0)<<" "<<pos(1)<<" "<<pos(2)<<"\n";
		}
		partFile<<"</DataArray>\n";

		// end points
		partFile<<"</Points>\n";
		
		// point data
		partFile<<"<PointData>\n";
		
		if (isFieldRequired("id")) {

			// particle Id
			partFile<<"<DataArray type=\"UInt64\" Name=\"Particle Id\" Format=\"ascii\">\n";
			for (int i = 0; i < nPoints; ++i) {
				partFile<<scientific<<particles->at(i)->getId()<<"\n";
			}
			partFile<<"</DataArray>\n";
		}

		if (isFieldRequired("material")){
			
			// particle material Id
			partFile<<"<DataArray type=\"UInt32\" Name=\"Material Id\" Format=\"ascii\">\n";
			for (int i = 0; i < nPoints; ++i) {
				partFile<<scientific<<particles->at(i)->getMaterialId()<<"\n";
			}
			partFile<<"</DataArray>\n";
		}

		if (isFieldRequired("active")){
			
			// particle active status
			partFile<<"<DataArray type=\"UInt8\" Name=\"Particle Active\" Format=\"ascii\">\n";
			for (int i = 0; i < nPoints; ++i) {
				partFile<<scientific<<particles->at(i)->getActive()<<"\n";
			}
			partFile<<"</DataArray>\n";
		}

		if (isFieldRequired("pressure")){
			
			// fluid pressure
			partFile<<"<DataArray type=\"Float64\" Name=\"Pressure\" Format=\"ascii\">\n";
			for (int i = 0; i < nPoints; ++i) {
				partFile<<scientific<<particles->at(i)->getPressureFluid()<<"\n";
			}
			partFile<<"</DataArray>\n";
		}
		
		if (isFieldRequired("plastic_strain")){
			
			// effective plastic strain
			partFile<<"<DataArray type=\"Float64\" Name=\"Effective Plastic Strain\" Format=\"ascii\">\n";
			for (int i = 0; i < nPoints; ++i) {
				partFile<<scientific<<particles->at(i)->getPlasticStrain()<<"\n";
			}
			partFile<<"</DataArray>\n";
		}

		if (isFieldRequired("external_force")){

			// external force in particle
			partFile<<"<DataArray type=\"Float64\" Name=\"External Force\" NumberOfComponents=\"3\" Format=\"ascii\">\n";
			for (int i = 0; i < nPoints; ++i) {
				partFile<<scientific<<particles->at(i)->getExternalForce().transpose()<<"\n";
			}
			partFile<<"</DataArray>\n";
		}

		if (isFieldRequired("displacement")){

			// particle displacement
			partFile<<"<DataArray type=\"Float64\" Name=\"Displacement\" NumberOfComponents=\"3\" Format=\"ascii\">\n";
			for (int i = 0; i < nPoints; ++i) {
				partFile<<scientific<<(particles->at(i)->getPosition()-particles->at(i)->getInitialPosition()).transpose()<<"\n";
			}
			partFile<<"</DataArray>\n";
		}

		if (isFieldRequired("stress")){

			// particle stress xx
			partFile<<"<DataArray type=\"Float64\" Name=\"Stress_XX\" NumberOfComponents=\"1\" Format=\"ascii\">\n";
			for (int i = 0; i < nPoints; ++i) {
				partFile<<scientific<<particles->at(i)->getStress()(0,0)<<"\n";
			}
			partFile<<"</DataArray>\n";

			// particle stress yy
			partFile<<"<DataArray type=\"Float64\" Name=\"Stress_YY\" NumberOfComponents=\"1\" Format=\"ascii\">\n";
			for (int i = 0; i < nPoints; ++i) {
				partFile<<scientific<<particles->at(i)->getStress()(1,1)<<"\n";
			}
			partFile<<"</DataArray>\n";

			// particle stress zz
			partFile<<"<DataArray type=\"Float64\" Name=\"Stress_ZZ\" NumberOfComponents=\"1\" Format=\"ascii\">\n";
			for (int i = 0; i < nPoints; ++i) {
				partFile<<scientific<<particles->at(i)->getStress()(2,2)<<"\n";
			}
			partFile<<"</DataArray>\n";

			// particle stress xy
			partFile<<"<DataArray type=\"Float64\" Name=\"Stress_XY\" NumberOfComponents=\"1\" Format=\"ascii\">\n";
			for (int i = 0; i < nPoints; ++i) {
				partFile<<scientific<<particles->at(i)->getStress()(0,1)<<"\n";
			}
			partFile<<"</DataArray>\n";

			// particle stress xz
			partFile<<"<DataArray type=\"Float64\" Name=\"Stress_XZ\" NumberOfComponents=\"1\" Format=\"ascii\">\n";
			for (int i = 0; i < nPoints; ++i) {
				partFile<<scientific<<particles->at(i)->getStress()(0,2)<<"\n";
			}
			partFile<<"</DataArray>\n";

			// particle stress yz
			partFile<<"<DataArray type=\"Float64\" Name=\"Stress_YZ\" NumberOfComponents=\"1\" Format=\"ascii\">\n";
			for (int i = 0; i < nPoints; ++i) {
				partFile<<scientific<<particles->at(i)->getStress()(1,2)<<"\n";
			}
			partFile<<"</DataArray>\n";

		}

		if (isFieldRequired("body")){
			
			// particle body Id
			partFile<<"<DataArray type=\"UInt32\" Name=\"Body Id\" Format=\"ascii\">\n";
			for (int i = 0; i < nPoints; ++i) {
				partFile<<scientific<<particles->at(i)->getBodyId()<<"\n";
			}
			partFile<<"</DataArray>\n";
		}

		// end point data
		partFile<<"</PointData>\n";
		
		// cells
		partFile<<"<Cells>\n";

		// connectivity
		partFile<<"<DataArray type=\"UInt64\" Name=\"connectivity\" Format=\"ascii\">\n";
		for (int i = 0; i < int(particles->size()); ++i)
		{
			partFile<<i<<"\n";
		}
		partFile<<"</DataArray>\n";
		
		// offsets
		partFile<<"<DataArray type=\"UInt64\" Name=\"offsets\" Format=\"ascii\">\n";
		for (int i = 0; i < int(particles->size()); ++i)
		{
			partFile<<i+1<<"\n";
		}
		partFile<<"</DataArray>\n";
		
		// types
		partFile<<"<DataArray type=\"UInt8\" Name=\"types\" Format=\"ascii\">\n";
		for (int i = 0; i < nPoints; ++i) {
			partFile<<1<<"\n";
		}
		partFile<<"</DataArray>\n";
		
		// end cells
		partFile<<"</Cells>\n";
		
		// end piece
		partFile<<"</Piece>\n";
		
		// end file
		partFile<<"</UnstructuredGrid>\n";
		partFile<<"</VTKFile>\n";

		// close file
		partFile.close();
	}

	void writeGrid(Mesh* mesh, CellType gridType){

		// define edian
		if(Folders::edian==""){
			defineEdian();
		}

		// create grid folder
		if(!Folders::gridFolderExist){
			createGridFolder();
		}
		
		// open grid file
		ofstream gridFile;
		gridFile.open(Folders::gridFolderName+"/"+Folders::gridFileName);
		gridFile.precision(4);

		// mesh data
		int nPoints=mesh->getNumNodes();
		Vector3i nCellsVec = mesh->getTotalCells();
		int nCells=nCellsVec(0)*nCellsVec(1)*nCellsVec(2);

		// write results
		gridFile<<"<?xml version=\"1.0\"?>\n";
		gridFile<<"<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\""<<Folders::edian.c_str()<<"\">\n";
		gridFile<<"<UnstructuredGrid>\n";
		
		// piece
		gridFile<<fixed<<"<Piece NumberOfPoints=\""<<nPoints<<"\" NumberOfCells=\""<<nCells<<"\">\n";
		
		// points
		gridFile<<"<Points>\n";
		
		// node position
		gridFile<<"<DataArray type=\"Float64\" NumberOfComponents=\"3\" Format=\"ascii\">\n";
		vector<Node*>* inodes = mesh->getNodes();
		for (int i = 0; i < nPoints; ++i) {
			Vector3d pos=inodes->at(i)->getCoordinates();
			gridFile<<scientific<<pos(0)<<" "<<pos(1)<<" "<<pos(2)<<"\n";
		}
		gridFile<<"</DataArray>\n";

		// end points
		gridFile<<"</Points>\n";
		
		// point data
		gridFile<<"<PointData>\n";
		
		// local ID of nodes
		gridFile<<"<DataArray type=\"UInt64\" Name=\"Id\" Format=\"ascii\">\n";
		for (int i = 0; i < nPoints; ++i) {
			gridFile<<scientific<<inodes->at(i)->getId()<<"\n";
		}
		gridFile<<"</DataArray>\n";

		// active nodes
		gridFile<<"<DataArray type=\"UInt8\" Name=\"Active\" Format=\"ascii\">\n";
		for (int i = 0; i < nPoints; ++i) {
			gridFile<<scientific<<(inodes->at(i)->getActive())<<"\n";
		}
		gridFile<<"</DataArray>\n";

		// nodal mass
		gridFile<<"<DataArray type=\"Float64\" Name=\"Mass\" Format=\"ascii\">\n";
		for (int i = 0; i < nPoints; ++i) {
			gridFile<<scientific<<(inodes->at(i)->getMass())<<"\n";
		}
		gridFile<<"</DataArray>\n";

		// nodal velocity
		gridFile << "<DataArray type=\"Float64\" NumberOfComponents=\"3\" Name=\"Velocity\" Format=\"ascii\">\n";
		for (int i = 0; i < nPoints; ++i) {
			gridFile << scientific << (inodes->at(i)->getVelocity()) << "\n";
		}
		gridFile << "</DataArray>\n";

		// end point data
		gridFile<<"</PointData>\n";
		
		// cells
		gridFile<<"<Cells>\n";

		// connectivity
		gridFile<<"<DataArray type=\"UInt64\" Name=\"connectivity\" Format=\"ascii\">\n";
		
		const int nCellsX = nCellsVec(0);
		const int nCellsY = nCellsVec(1);
		const int nCellsZ = nCellsVec(2);

		for (int k = 0; k < nCellsZ; ++k){
			for (int j = 0; j < nCellsY; ++j) {
				for (int i = 0; i < nCellsX; ++i){
					gridFile<<scientific
					<<k*(nCellsY+1)*(nCellsX+1)+j    *(nCellsX+1)+i                              <<" "
					<<k*(nCellsY+1)*(nCellsX+1)+j    *(nCellsX+1)+i+1                            <<" "
					<<k*(nCellsX+1)*(nCellsY+1)+(j+1)*(nCellsX)  +i+2+j                          <<" "
					<<k*(nCellsX+1)*(nCellsY+1)+(j+1)*(nCellsX)  +i+1+j                          <<" "
					<<k*(nCellsY+1)*(nCellsX+1)+j    *(nCellsX+1)+i    +(nCellsX+1)*(nCellsY+1)  <<" "
					<<k*(nCellsY+1)*(nCellsX+1)+j    *(nCellsX+1)+i    +(nCellsX+1)*(nCellsY+1)+1<<" "
					<<k*(nCellsX+1)*(nCellsY+1)+(j+1)*(nCellsX)  +i+2+j+(nCellsX+1)*(nCellsY+1)  <<" "
					<<k*(nCellsX+1)*(nCellsY+1)+(j+1)*(nCellsX)  +i+1+j+(nCellsX+1)*(nCellsY+1)  <<"\n";
				}
			}
		}
		gridFile<<"</DataArray>\n";
		
		// offsets
		gridFile<<"<DataArray type=\"UInt64\" Name=\"offsets\" Format=\"ascii\">\n";
		for (int i = 0; i < nCells; ++i) {
			gridFile<<""<<8*(i+1)<< "\n";
		}
		gridFile<<"</DataArray>\n";
		
		// types
		int cellsType=gridType==CellType::POINTS?1:(gridType==CellType::CELLS?12:1);

		gridFile<<"<DataArray type=\"UInt8\" Name=\"types\" Format=\"ascii\">\n";
		for (int i = 0; i < nPoints; ++i) {
			gridFile<<cellsType<<"\n";
		}
		gridFile<<"</DataArray>\n";
		
		// end cells
		gridFile<<"</Cells>\n";
		
		// end piece
		gridFile<<"</Piece>\n";
		
		// end file
		gridFile<<"</UnstructuredGrid>\n";
		gridFile<<"</VTKFile>\n";

		// close file
		gridFile.close();
	}

	void writeBody(Body* body, double time){

		writeParticles(body->getParticles(),time);
	}

	void writeBodies(vector<Body*>* bodies, double time){

		vector<Particle*> particles;
		for (size_t i = 0; i < bodies->size(); ++i)
		{
			particles.insert(particles.end(),bodies->at(i)->getParticles()->begin(),bodies->at(i)->getParticles()->end());
		}

		writeParticles(&particles,time);
	}

	void writeResultsSeries() {

		// define edian
		if(Folders::edian=="") {
			defineEdian();
		}

		// create particle folder
		if(!Folders::particleFolderExist) {
			createParticleFolder();
		}

		// open particle serie file
		ofstream serieFile;
		serieFile.open(Folders::particleFolderName+"/"+Folders::particleFileTimeSerie+".pvd");

		// write the file
		serieFile <<"<?xml version=\"1.0\"?>\n";
		serieFile <<"<VTKFile type=\"Collection\" version=\"0.1\" byte_order=\"LittleEndian\" compressor=\"vtkZLibDataCompressor\">\n";
		serieFile <<"\t<Collection>\n";
		for (size_t i = 0; i < Folders::particleFilesTime.size(); ++i)
		{
			serieFile <<"\t\t<DataSet timestep=\""<<Folders::particleFilesTime.at(i)<<"\" group=\"\" part=\"0\" file=\""<<Folders::particleFileName<<"_"<<i+1<<".vtu\"/>\n";
		}
		serieFile <<"\t</Collection>\n";
		serieFile <<"</VTKFile>\n";
	}

	void clearScreen() {
	
		#ifdef _WIN32
    		system("cls");
		#else
    		std::cout << "\033[2J\033[1;1H" << std::flush; // Clear screen for Unix systems
		#endif
	}

	void showProgressBar(double progress, int width = 47) {
		std::cout << "[";
		int pos = static_cast<int>(width * progress);
		for (int i = 0; i < width; ++i) {
			if (i < pos) std::cout << "=";
			else if (i == pos) std::cout << ">";
			else std::cout << " ";
		}
		std::cout << "] " << int(progress * 100.0) << " %\r";
		std::cout.flush();
	}

	void welcomeScreen() {

		// common format
		int width = 55;

		// information
		string programName="MPM-Geomechanics";
		string programDescription="A material point method program for geomechanics";
		string programAuthor="";
		string hLines(width-2,'-');
		string hSpaces(width-2,' ');
		string hSpacesArterisc(width-6,' ');
		
		// set screen
		cout<<"\n"<<left<<" "<<setw(width)<<hLines<<" \n";
		cout<<"| *"+hSpacesArterisc+"* |"<<"\n";
		cout<<"|"<<right<<setw(34)<<programName<<setw(21)<<right<<"|\n";
		cout<<"|"+hSpaces+"|"<<"\n";
		cout<<"|"<<right<<setw(50)<<programDescription<<setw(5)<<right<<"|\n";
		cout<<"|"+hSpaces+"|"<<"\n";
		cout<<left<<" "<<setw(width)<<hLines<<"\n";
	}

	void farewellScreen() {
		 int width = 55;
		 string hLines(width,'-');
		 cout<<"\n"<<left<<setw(width)<<hLines<<"\n";
	}

	void updateTerminal(vector<Body*>* bodies, double itime)
	{
		std::cout <<"Time: "<< std::setw(8) << std::scientific << std::setprecision(4) << itime << " s, ";
		std::cout <<"Energy: "<< std::setw(8) << std::scientific << std::setprecision(4) << DynamicRelaxation::computeKineticEnergy(bodies) << " J, ";
		std::cout << std::setw(1) << std::fixed << std::setprecision(0) <<"(" << int(100 * itime / ModelSetup::getTime()) << "%) \n";
	} 

	void printModelInfo(vector<Body*>* bodies, double itime)
	{
		std::cout << "     Time : " << std::setw(8) << std::scientific << std::setprecision(4) << ModelSetup::getTime() << "s" << std::endl;
		std::cout << "Time step : " << std::setw(8) << std::scientific << std::setprecision(4) << ModelSetup::getTimeStep() << "s" << std::endl;
		std::cout << "Particles : " << Particle::getTotalParticles() << std::endl;
		std::cout << "  Threads : " << ModelSetup::getThreads() << std::endl;
		std::cout << "  Results : " << ModelSetup::getResultNum() << std::endl;
	}

	void initializeCSVFile(const std::string& filename) {
		// Check if the file exists
		if (std::ifstream(filename)) {
			// If it exists, delete the file
			std::remove(filename.c_str());
		}
	}

	void writeCSVEnergyFile(std::vector<Body*>* bodies, double iTime) {
    // Get total kinetic energy
    double ienergy = DynamicRelaxation::computeKineticEnergy(bodies);
    
    // Open the simulation CSV file in append mode
    std::ofstream csv_file("simulation_data.csv", std::ios::app);
    if (!csv_file.is_open()) {
        std::cerr << "Error opening the CSV file" << std::endl;
    }
    
    // Write time and energy to the file
    csv_file << iTime << "," << ienergy << "\n";
    
    // Close the file
    csv_file.close();
}

	void writeResultInStep(int loopCounter, int resultSteps,vector<Body*>* bodies, double iTime)
	{
		if (iTime == 0) { printModelInfo(bodies, iTime); initializeCSVFile("simulation_data.csv");}

		if (loopCounter%resultSteps==0)
		{
			// write model results
			writeBodies(bodies,iTime);

			// update terminal
			updateTerminal(bodies,iTime);

			writeCSVEnergyFile(bodies,iTime);
		}
	}

	void printElapsedTime() {
		// hours to minutes and seconds
		std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - ModelSetup::getInitialSimulationTime();
		auto hours = std::chrono::duration_cast<std::chrono::hours>(elapsed_seconds);
		auto minutes = std::chrono::duration_cast<std::chrono::minutes>(elapsed_seconds) - hours;
		double seconds = elapsed_seconds.count() - (hours.count() * 3600 + minutes.count() * 60);
		std::cout << "\nElapsed time: " << hours.count() << " h, " << minutes.count() << " m, " << std::fixed << std::setprecision(2) << seconds << " s" << std::endl;
		farewellScreen();
	}
}
