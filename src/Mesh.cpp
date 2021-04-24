/*
 * Mesh.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <vector>
using std::vector;

#include <cmath>
using std::floor;

#include "Mesh.h"
#include "Warning.h"

Mesh::Mesh() {

	setNumCells(0, 0, 0);
	setNumGhosts(0);
    setCellDimension(0.0, 0.0, 0.0);
    setLimits(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}

Mesh::~Mesh() {
    // TODO Auto-generated destructor stub
}

//
// set methods
//

void Mesh::setCellDimension(double dx, double dy, double dz) {

    if (dx<=0.0 || dy<=0.0 || dz<=0.0)
    {
        Warning::printMessage("Cell dimension must be greater that 0");
    }

    cellDimX=dx;
    cellDimY=dy;
    cellDimZ=dz;
}

void Mesh::setNumCells(int nx, int ny, int nz) {

    if (nx<=0 || ny<=0 || nz<=0)
    {
        Warning::printMessage("Number of cells must be greater that 0");
    }

    nCellX=nx;
    nCellY=ny;
    nCellZ=nz;
}

void Mesh::setNumGhosts(int ng) {

    if (ng<0)
    {
        Warning::printMessage("Number of Ghosts must be positive");
    }

    nGhosts=ng;
}

void Mesh::setLimits(double minx, double miny, double minz, double maxx, double maxy, double maxz) {

    minLimitX=minx;
    minLimitY=miny;
    minLimitZ=minz;
    maxLimitX=maxx;
    maxLimitY=maxy;
    maxLimitZ=maxz;
}

//
// get methods
//


vector<double> Mesh::getCellDimension() {

    vector<double> v {cellDimX, cellDimY, cellDimZ};
    return v;
}

vector<int> Mesh::getNumCells() {

    vector<int> v {nCellX, nCellY, nCellZ};
    return v;
}

vector<int> Mesh::getTotalCells() { 
   
    vector<int> v {nCellX+nGhosts*2, nCellY+nGhosts*2, nCellZ+nGhosts*2};
    return v;
}

const vector<Node> & Mesh::getNodes() {

    return gridNodes;
}

int Mesh::getNumNodes() {

    return gridNodes.size();
}

int Mesh::getNumGhosts() {

    return nGhosts;
}

vector<double> Mesh::getGridCoordinates(Vector3d position) {

    // grid coordinates
    double i = (position(0)-minLimitX)/cellDimX+nGhosts;
    double j = (position(1)-minLimitY)/cellDimY+nGhosts;
    double k = (position(2)-minLimitZ)/cellDimZ+nGhosts;

    // return vector of coordinates
    vector<double> v{i,j,k};
    return v;
}

vector<int> Mesh::getParentNodeCoordinates(Vector3d position) {

    // grid coordinates
    vector<double> gridCoords = getGridCoordinates(position);
    
    // return the floor of the grid coordinates
    vector<int> v{int(floor(gridCoords.at(0))),int(floor(gridCoords.at(1))), int(floor(gridCoords.at(2)))};
    return v;
}

int Mesh::getCellIdbyPosition(Vector3d position)
{
    // parent node coordinate
    vector<int> gridParentNodeCoords = getParentNodeCoordinates(position);
    int i = gridParentNodeCoords.at(0);
    int j = gridParentNodeCoords.at(1);
    int k = gridParentNodeCoords.at(2);

    // cell id
    return ((j*nRowsX+i)+(nRowsX*nRowsY*k));
}

int Mesh::getParentCellIdConstribution(Vector3d position)
{
    // parent node coordinates
    vector<int> gridParentNodeCoords = getParentNodeCoordinates(position);
    int pi=gridParentNodeCoords.at(0);
    int pj=gridParentNodeCoords.at(1);
    int pk=gridParentNodeCoords.at(2);

    // grid coordinates
    vector<double> gridCoords = getGridCoordinates(position);
    double gi=gridCoords.at(0);
    double gj=gridCoords.at(1);
    double gk=gridCoords.at(2);

    // relative distance to parent node
    double relDx = gi-double(pi);
    double relDy = gj-double(pj);
    double relDz = gk-double(pk);

    // calculate the correct the parent node
    int i = relDx<0.5 ? pi-1 : pi;
    int j = relDy<0.5 ? pj-1 : pj;
    int k = relDz<0.5 ? pk-1 : pk;

    // return the parent node of 27 node contribution
    return ((j*nRowsX+i)+(nRowsX*nRowsY*k));
}

vector<double> Mesh::getMinLimits() {

	vector<double>v{minLimitX, minLimitY, minLimitZ};
	return v;
}

vector<double> Mesh::getMaxLimits() {

	vector<double>v{maxLimitX, maxLimitY, maxLimitZ};
	return v;
}

vector<int> Mesh::getNodesInCell(Vector3d position)
{
    int cellId = getCellIdbyPosition(position);
    
    int idNode1 = cellId;
    int idNode2 = idNode1+1;
    int idNode3 = idNode1+nRowsX;
    int idNode4 = idNode3+1;
    int idNode5 = idNode1 + (nRowsX*nRowsY);
    int idNode6 = idNode2 + (nRowsX*nRowsY);
    int idNode7 = idNode3 + (nRowsX*nRowsY);
    int idNode8 = idNode4 + (nRowsX*nRowsY);

    vector<int> v {idNode1,idNode2,idNode3,idNode4,idNode5,idNode6,idNode7,idNode8};
    return v;
}

vector<int> Mesh::getContributionNodes(Vector3d position) {

    int cellId = getParentCellIdConstribution(position);
    
    int nXYGridNodes = nRowsX*nRowsY;
    int I = nRowsX;

    int zPlane = nXYGridNodes*0;
    int idNode1 = cellId       + zPlane;
    int idNode2 = cellId+1     + zPlane;
    int idNode3 = cellId+2     + zPlane;
    int idNode4 = cellId+I     + zPlane;
    int idNode5 = cellId+I+1   + zPlane;
    int idNode6 = cellId+I+2   + zPlane;
    int idNode7 = cellId+I+I   + zPlane;
    int idNode8 = cellId+I+I+1 + zPlane;
    int idNode9 = cellId+I+I+2 + zPlane;

    zPlane = nXYGridNodes*1;
    int idNode10 = cellId       + zPlane;
    int idNode11 = cellId+1     + zPlane;
    int idNode12 = cellId+2     + zPlane;
    int idNode13 = cellId+I     + zPlane;
    int idNode14 = cellId+I+1   + zPlane;
    int idNode15 = cellId+I+2   + zPlane;
    int idNode16 = cellId+I+I   + zPlane;
    int idNode17 = cellId+I+I+1 + zPlane;
    int idNode18 = cellId+I+I+2 + zPlane;

    zPlane = nXYGridNodes*2;
    int idNode19 = cellId       + zPlane;
    int idNode20 = cellId+1     + zPlane;
    int idNode21 = cellId+2     + zPlane;
    int idNode22 = cellId+I     + zPlane;
    int idNode23 = cellId+I+1   + zPlane;
    int idNode24 = cellId+I+2   + zPlane;
    int idNode25 = cellId+I+I   + zPlane;
    int idNode26 = cellId+I+I+1 + zPlane;
    int idNode27 = cellId+I+I+2 + zPlane;
    
    
    vector<int> v { idNode1,  idNode2,  idNode3,
                    idNode4,  idNode5,  idNode6,
                    idNode7,  idNode8,  idNode9,
                    idNode10, idNode11, idNode12,
                    idNode13, idNode14, idNode15,
                    idNode16, idNode17, idNode18,
                    idNode19, idNode20, idNode21,
                    idNode22, idNode23, idNode24,
                    idNode25, idNode26, idNode27};
    return v;
}

//
// private methods
//


//
// public methods
//

void Mesh::createGrid(void) {

    // set the rows in each direction
    nRowsX = nCellX+2*nGhosts+1;
    nRowsY = nCellY+2*nGhosts+1;
    nRowsZ = nCellZ+2*nGhosts+1;

    // resize the node vector
    gridNodes.resize(nRowsX*nRowsY*nRowsZ);

    // initialize nodes
    for (size_t i=0;i<gridNodes.size();i++){
    	gridNodes.at(i).setCoordinates(Vector3d(0.0,0.0,0.0));
    }

    // create a grid
    for (int k=0; k<nRowsZ; k++)
    {
        for(int j=0; j<nRowsY; j++)
        {
            for(int i=0; i<nRowsX; i++)
            {
                // grid node coordinates
                double x = (i-nGhosts)*cellDimX+minLimitX;
                double y = (j-nGhosts)*cellDimY+minLimitY;
                double z = (k-nGhosts)*cellDimZ+minLimitZ;

                // grid node id
                int nodeId=(j*nRowsX+i)+(nRowsY*nRowsX*k);

                // set node id
                gridNodes[nodeId].setId(nodeId);

                // grid node position
                gridNodes[nodeId].setCoordinates(Vector3d(x,y,z));
            }
        }
    }
}

void Mesh::activateNodes(const vector<int>& nodesId,bool activeValue) {

    for (size_t i = 0; i < nodesId.size(); ++i)
    {
        gridNodes.at(nodesId.at(i)).setActive(activeValue);
    }
}

bool updateNodesContribution(const vector<Particle>&)
{
    return true;
    // TODO
}