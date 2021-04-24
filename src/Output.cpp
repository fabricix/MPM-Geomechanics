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
#endif

#include "Output.h"

#include<string>
using std::string;

#include <fstream>
using std::ofstream;

#include <iomanip>
using std::fixed;
using std::scientific;

#include <vector>
using std::vector;

namespace OutputTolerance
{
    double deltaValue=1.0e-38;
}

namespace Folders
{
    bool gridFolderExist=false;
    string edian="";
    string gridFolderName="grid";
    string gridFileName="eulerianGrid.vtu";
}

Output::Output() {
    // TODO Auto-generated
}

Output::~Output() {
    // TODO Auto-generated destructor stub
}

void Output::defineEdian()
{
    int16_t i = 1;
    int8_t *p = (int8_t*) &i;
    Folders::edian=(p[0]==1)?"LittleEndian":"BigEndian";
}

double Output::checkTolerance(double val)
{
    if (val==0.0) return val;
    if (val<0.0) return val>(-OutputTolerance::deltaValue)?(-OutputTolerance::deltaValue):val;
    return val<OutputTolerance::deltaValue?OutputTolerance::deltaValue:val;
}

void Output::createGridFolder()
{
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

void Output::writeGrid(Mesh& mesh)
{
    // define edian
    if(Folders::edian==""){
        defineEdian();
    }

    // create grid folder
    if(!Folders::gridFolderExist){
        createGridFolder();
    }
    
    // open grid file
    ofstream gridfile;
    gridfile.open((Folders::gridFolderName+"/"+Folders::gridFileName).c_str());
    gridfile.precision(4);

    // mesh data
    int nPoints=mesh.getNumNodes();
    vector<int> nCellsVec = mesh.getTotalCells();
    int nCells=nCellsVec.at(0)*nCellsVec.at(1)*nCellsVec.at(2);

    // write results
    gridfile<<"<?xml version=\"1.0\"?>\n";
    gridfile<<"<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\""<<Folders::edian.c_str()<<"\">\n";
    gridfile<<"<UnstructuredGrid>\n";
    
    // piece
    gridfile<<fixed<<"<Piece NumberOfPoints=\""<<nPoints<<"\" NumberOfCells=\""<<nCells<<"\">\n";
    
    // points
    gridfile<<"<Points>\n";
    
    // node position
    gridfile<<"<DataArray type=\"Float32\" NumberOfComponents=\"3\" Format=\"ascii\">\n";
    vector<Node>inodes = mesh.getNodes();
    for (int i = 0; i < nPoints; ++i) {
        Vector3d pos=inodes.at(i).getCoordinates();
        gridfile<<scientific<<pos(0)<<" "<<pos(1)<<" "<<pos(2)<<"\n";
    }
    gridfile<<"</DataArray>\n";

    // end points
    gridfile<<"</Points>\n";
    
    // point data
    gridfile<<"<PointData Scalars=\"scalars\">\n";
    
    // local ID of nodes
    gridfile<<"<DataArray type=\"Float32\" Name=\"Id-MPM\" Format=\"ascii\">\n";
    for (int i = 0; i < nPoints; ++i) {
        gridfile<<scientific<<inodes.at(i).getId()<<"\n";
    }
    gridfile<<"</DataArray>\n";

    // active nodes
    gridfile<<"<DataArray type=\"Float32\" Name=\"Active\" Format=\"ascii\">\n";
    for (int i = 0; i < nPoints; ++i) {
        gridfile<<scientific<<(inodes.at(i).getActive())<<"\n";
    }
    gridfile<<"</DataArray>\n";

    // end point data
    gridfile<<"</PointData>\n";
    
    // cells
    gridfile<<"<Cells>\n";

    // connectivity
    gridfile<<"<DataArray type=\"Int32\" Name=\"connectivity\" Format=\"ascii\">\n";
    
    int nCellsX = nCellsVec.at(0);
    int nCellsY = nCellsVec.at(1);
    int nCellsZ = nCellsVec.at(2);

    for (int k = 0; k < nCellsZ; ++k){
        for (int j = 0; j < nCellsY; ++j) {
            for (int i = 0; i < nCellsX; ++i){
                gridfile<<scientific
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
    gridfile<<"</DataArray>\n";
    
    // offsets
    gridfile<<"<DataArray type=\"Int32\" Name=\"offsets\" Format=\"ascii\">\n";
    for (int i = 0; i < nCells; ++i) {
        gridfile<<""<<8*(i+1)<< "\n";
    }
    gridfile<<"</DataArray>\n";
    
    // types
    gridfile<<"<DataArray type=\"UInt8\" Name=\"types\" Format=\"ascii\">\n";
    for (int i = 0; i < nPoints; ++i) {
        //gridfile<<""<<12<<"\n"; // cells
        gridfile<<""<<1<<"\n";    // points
    }
    gridfile<<"</DataArray>\n";
    
    // end cells
    gridfile<<"</Cells>\n";
    
    // end piece
    gridfile<<"</Piece>\n";
    
    // end file
    gridfile<<"</UnstructuredGrid>\n";
    gridfile<<"</VTKFile>\n";

    // close file
    gridfile.close();
}
