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

	nGhosts=0;
	nCell.setZero();
    cellDim.setZero();
    minLimit.setZero();
    maxLimit.setZero();

    BoundaryX.resize(2);
    BoundaryY.resize(2);
    BoundaryZ.resize(2);
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

    nCell=Vector3i(nx,ny,nz);
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

    return nCell;
}

Vector3i Mesh::getTotalCells() { 
   
    return Vector3i(nCell(0)+nGhosts*2, nCell(1)+nGhosts*2, nCell(2)+nGhosts*2);
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

Vector3d Mesh::getGridCoordinates(Vector3d position) {

    // grid coordinates
    double i = (position(0)-minLimit(0))/cellDim(0)+nGhosts;
    double j = (position(1)-minLimit(1))/cellDim(1)+nGhosts;
    double k = (position(2)-minLimit(2))/cellDim(2)+nGhosts;

    // return vector of coordinates
    return Vector3d(i,j,k);
}

Vector3i Mesh::getParentNodeCoordinates(Vector3d position) {

    // grid coordinates
    Vector3d gridCoords = getGridCoordinates(position);
    
    // return the floor of the grid coordinates
    return Vector3i(int(floor(gridCoords(0))),int(floor(gridCoords(1))),int(floor(gridCoords(2))));
}

int Mesh::getCellIdbyPosition(Vector3d position)
{
    // parent node coordinate
    Vector3i gridParentNodeCoords = getParentNodeCoordinates(position);
    int i = gridParentNodeCoords(0);
    int j = gridParentNodeCoords(1);
    int k = gridParentNodeCoords(2);

    // cell id
    return ((j*nRows(0)+i)+(nRows(0)*nRows(1)*k));
}

int Mesh::getParentCellIdConstribution(Vector3d position)
{
    // parent node coordinates
    Vector3i gridParentNodeCoords = getParentNodeCoordinates(position);
    int pi=gridParentNodeCoords(0);
    int pj=gridParentNodeCoords(1);
    int pk=gridParentNodeCoords(2);

    // grid coordinates
    Vector3d gridCoords = getGridCoordinates(position);
    double gi=gridCoords(0);
    double gj=gridCoords(1);
    double gk=gridCoords(2);

    // relative distance to parent node
    double relDx = gi-double(pi);
    double relDy = gj-double(pj);
    double relDz = gk-double(pk);

    // calculate the correct the parent node
    int i = relDx<0.5 ? pi-1 : pi;
    int j = relDy<0.5 ? pj-1 : pj;
    int k = relDz<0.5 ? pk-1 : pk;

    // return the parent node of 27 node contribution
    return ((j*nRows(0)+i)+(nRows(0)*nRows(1)*k));
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
    int idNode3 = idNode1+nRows(0);
    int idNode4 = idNode3+1;
    int idNode5 = idNode1 + (nRows(0)*nRows(1));
    int idNode6 = idNode2 + (nRows(0)*nRows(1));
    int idNode7 = idNode3 + (nRows(0)*nRows(1));
    int idNode8 = idNode4 + (nRows(0)*nRows(1));

    vector<int> v {idNode1,idNode2,idNode3,idNode4,idNode5,idNode6,idNode7,idNode8};
    return v;
}

vector<int> Mesh::getContributionNodes(Vector3d position) {

    int cellId = getParentCellIdConstribution(position);
    
    int nXYGridNodes = nRows(0)*nRows(1);
    int I = nRows(0);

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
    nRows(0) = nCell(0)+2*nGhosts+1;
    nRows(1) = nCell(1)+2*nGhosts+1;
    nRows(2) = nCell(2)+2*nGhosts+1;

    // resize the node vector
    gridNodes.resize(nRows(0)*nRows(1)*nRows(2));

    // initialize nodes
    for (size_t i=0;i<gridNodes.size();i++){
    	gridNodes.at(i).setCoordinates(Vector3d(0.0,0.0,0.0));
    }

    // create a grid
    for (int k=0; k<nRows(2); k++)
    {
        for(int j=0; j<nRows(1); j++)
        {
            for(int i=0; i<nRows(0); i++)
            {
                // grid node coordinates
                double x = (i-nGhosts)*cellDim(0)+minLimit(0);
                double y = (j-nGhosts)*cellDim(1)+minLimit(1);
                double z = (k-nGhosts)*cellDim(2)+minLimit(2);

                // grid node id
                int nodeId=(j*nRows(0)+i)+(nRows(1)*nRows(0)*k);

                // set node id
                gridNodes[nodeId].setId(nodeId);

                // grid node position
                gridNodes[nodeId].setCoordinates(Vector3d(x,y,z));
            }
        }
    }
}

void Mesh::activateNode(int nodesId,bool activeValue) {

    if (nodesId<int(gridNodes.size()))
    {
        gridNodes.at(nodesId).setActive(activeValue);
    }    
}

void Mesh::activateNodes(const vector<int>& nodesId,bool activeValue) {

    for (size_t i = 0; i < nodesId.size(); ++i)
    {
        activateNode(nodesId.at(i),activeValue);
    }
}

void Mesh::updateBoundaries()
{
    for (size_t i = 0; i < gridNodes.size(); ++i)
    {
        Vector3d nodeCoordinates=gridNodes.at(i).getCoordinates();
        int nodeId = gridNodes.at(i).getId();

        // plane X0
        if (nodeCoordinates(0)<=minLimit(0))
        {
            BoundaryX.at(0).nodesIndex.push_back(nodeId);
        }

        // plane Y0
        else if (nodeCoordinates(1)<=minLimit(1))
        {
            BoundaryY.at(0).nodesIndex.push_back(nodeId);
        }
        
        // plane Z0
        else if (nodeCoordinates(2)<=minLimit(2))
        {
            BoundaryZ.at(0).nodesIndex.push_back(nodeId);
        }

        // plane Xn
        else if (nodeCoordinates(0)>=maxLimit(0))
        {
            BoundaryX.at(1).nodesIndex.push_back(nodeId);
        }

        // plane Yn
        else if (nodeCoordinates(1)>=maxLimit(1))
        {
            BoundaryY.at(1).nodesIndex.push_back(nodeId);
        }        

        // plane Zn
        else if (nodeCoordinates(2)>=maxLimit(2))
        {
            BoundaryZ.at(1).nodesIndex.push_back(nodeId);
        }
    }
}
