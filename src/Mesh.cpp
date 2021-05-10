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
#include "Particle.h"

Mesh::Mesh() {

	nGhosts=2;
	nCells.setZero();
    cellDim.setZero();
    minLimit.setZero();
    maxLimit.setZero();
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

    cellDim=Vector3d(dx,dy,dz);
    
    }

void Mesh::setNumCells(int nx, int ny, int nz) {

    if (nx<=0 || ny<=0 || nz<=0)
    {
        Warning::printMessage("Number of cells must be greater that 0");
    }

    nCells=Vector3i(nx,ny,nz);
}

void Mesh::setNumGhosts(int ng) {

    if (ng<0)
    {
        Warning::printMessage("Number of Ghosts must be positive");
    }

    nGhosts=ng;
}

void Mesh::setLimits(double minX, double minY, double minZ, double maxX, double maxY, double maxZ) {

    minLimit=Vector3d(minX,minY,minZ);
    maxLimit=Vector3d(maxX,maxY,maxZ);
}

//
// get methods
//

Vector3d Mesh::getCellDimension() {

    return cellDim;
}

Vector3i Mesh::getNumCells() {

    return nCells;
}

Vector3i Mesh::getTotalCells() { 
   
    return Vector3i(nCells.x()+nGhosts*2, nCells.y()+nGhosts*2, nCells.z()+nGhosts*2);
}

vector<Node>* Mesh::getNodes() {

    return &gridNodes;
}

int Mesh::getNumNodes() {

    return gridNodes.size();
}

int Mesh::getNumGhosts() {

    return nGhosts;
}

Vector3d Mesh::getGridCoordinates(Vector3d position) {

    // grid coordinates
    double i = (position.x()-minLimit.x())/cellDim.x()+nGhosts;
    double j = (position.y()-minLimit.y())/cellDim.y()+nGhosts;
    double k = (position.z()-minLimit.z())/cellDim.z()+nGhosts;

    // return vector of coordinates
    return Vector3d(i,j,k);
}

Vector3i Mesh::getParentNodeCoordinates(Vector3d position) {

    // grid coordinates
    Vector3d gridCoords = getGridCoordinates(position);
    
    // return the floor of the grid coordinates
    return Vector3i(int(floor(gridCoords.x())),int(floor(gridCoords.y())),int(floor(gridCoords.z())));
}

int Mesh::getCellIdbyPosition(Vector3d position)
{
    // parent node coordinate
    Vector3i gridParentNodeCoords = getParentNodeCoordinates(position);
    int i = gridParentNodeCoords.x();
    int j = gridParentNodeCoords.y();
    int k = gridParentNodeCoords.z();

    // cell id
    return ((j*nRows.x()+i)+(nRows.x()*nRows.y()*k));
}

int Mesh::getParentCellIdConstribution(Vector3d position)
{
    // parent node coordinates
    Vector3i gridParentNodeCoords = getParentNodeCoordinates(position);
    int pi=gridParentNodeCoords.x();
    int pj=gridParentNodeCoords.y();
    int pk=gridParentNodeCoords.z();

    // grid coordinates
    Vector3d gridCoords = getGridCoordinates(position);
    double gi=gridCoords.x();
    double gj=gridCoords.y();
    double gk=gridCoords.z();

    // relative distance to parent node
    double relDx = gi-double(pi);
    double relDy = gj-double(pj);
    double relDz = gk-double(pk);

    // calculate the correct the parent node
    int i = relDx<0.5 ? pi-1 : pi;
    int j = relDy<0.5 ? pj-1 : pj;
    int k = relDz<0.5 ? pk-1 : pk;

    // return the parent node of 27 node contribution
    return ((j*nRows.x()+i)+(nRows.x()*nRows.y()*k));
}

Vector3d Mesh::getMinLimits() {

	return minLimit;
}

Vector3d Mesh::getMaxLimits() {

	return maxLimit;
}

vector<int> Mesh::getNodesInCell(Vector3d position)
{
    int cellId = getCellIdbyPosition(position);
    
    int idNode1 = cellId;
    int idNode2 = idNode1+1;
    int idNode3 = idNode1+nRows.x();
    int idNode4 = idNode3+1;
    int idNode5 = idNode1 + (nRows.x()*nRows.y());
    int idNode6 = idNode2 + (nRows.x()*nRows.y());
    int idNode7 = idNode3 + (nRows.x()*nRows.y());
    int idNode8 = idNode4 + (nRows.x()*nRows.y());

    vector<int> v {idNode1,idNode2,idNode3,idNode4,idNode5,idNode6,idNode7,idNode8};
    return v;
}

vector<int> Mesh::getContributionNodes(Vector3d position) {

    int cellId = getParentCellIdConstribution(position);
    
    int nXYGridNodes = nRows.x()*nRows.y();
    int I = nRows.x();

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

const Boundary& Mesh::getBoundary(){

    return boundary;
}

//
// public methods
//

void Mesh::createGrid(void) {

    // set the rows in each direction
    nRows.x() = nCells.x()+2*nGhosts+1;
    nRows.y() = nCells.y()+2*nGhosts+1;
    nRows.z() = nCells.z()+2*nGhosts+1;

    // resize the node vector
    gridNodes.resize(nRows.x()*nRows.y()*nRows.z());

    // initialize nodes
    for (size_t i=0;i<gridNodes.size();i++){
    	gridNodes.at(i).setCoordinates(Vector3d(0.0,0.0,0.0));
    }

    // create a grid
    for (int k=0; k<nRows.z(); k++)
    {
        for(int j=0; j<nRows.y(); j++)
        {
            for(int i=0; i<nRows.x(); i++)
            {
                // grid node coordinates
                double x = (i-nGhosts)*cellDim.x()+minLimit.x();
                double y = (j-nGhosts)*cellDim.y()+minLimit.y();
                double z = (k-nGhosts)*cellDim.z()+minLimit.z();

                // grid node id
                int nodeId=(j*nRows.x()+i)+(nRows.y()*nRows.x()*k);

                // set node id
                gridNodes[nodeId].setId(nodeId);

                // grid node position
                gridNodes[nodeId].setCoordinates(Vector3d(x,y,z));
            }
        }
    }

    // update boundaries
    updateBoundaries();
}

void Mesh::activateNode(int nodeId,bool activeValue) {

    gridNodes.at(nodeId).setActive(activeValue);
}

void Mesh::activateNodes(const vector<int>& nodesId,bool activeValue) {

    for (size_t i = 0; i < nodesId.size(); ++i)
    {
        activateNode(nodesId.at(i),activeValue);
    }
}

//
// private methods
//

void Mesh::updateBoundaries()
{
    // current limits
    Vector3d minLimitsGhosts = minLimit-nGhosts*cellDim;
    Vector3d maxLimitsGhosts = Vector3d(nCells.x()*cellDim.x(),nCells.y()*cellDim.y(),nCells.z()*cellDim.z())+nGhosts*cellDim;

    for (size_t i = 0; i < gridNodes.size(); ++i)
    {
        // node coordinates
        Vector3d nodeCoordinates=gridNodes.at(i).getCoordinates();
        
        // node id
        int nodeId = gridNodes.at(i).getId();

        // plane X0
        if (nodeCoordinates.x()<=minLimitsGhosts.x())
        {
            boundary.planeX0.push_back(nodeId);
        }

        // plane Y0
        if (nodeCoordinates.y()<=minLimitsGhosts.y())
        {
            boundary.planeY0.push_back(nodeId);
        }
        
        // plane Z0
        if (nodeCoordinates.z()<=minLimitsGhosts.z())
        {
            boundary.planeZ0.push_back(nodeId);
        }

        // plane Xn
        if (nodeCoordinates.x()>=maxLimitsGhosts.x())
        {
            boundary.planeXn.push_back(nodeId);
        }

        // plane Yn
        if (nodeCoordinates.y()>=maxLimitsGhosts.y())
        {
            boundary.planeYn.push_back(nodeId);
        }        

        // plane Zn
        if (nodeCoordinates.z()>=maxLimitsGhosts.z())
        {
            boundary.planeZn.push_back(nodeId);
        }
    }
}
