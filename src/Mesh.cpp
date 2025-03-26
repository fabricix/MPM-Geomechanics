// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include <vector>
using std::vector;

#include <unordered_map>
using std::unordered_map;

#include <cmath>
using std::floor;

#include "Mesh/Mesh.h"
#include "Mesh/Node.h"
#include "Mesh/NodeMixture.h"
#include "Warning.h"
#include "Particle/Particle.h"

Mesh::Mesh() {

	nGhosts=2;
	nCells.setZero();
    cellDim.setZero();
    minLimit.setZero();
    maxLimit.setZero();
}

Mesh::~Mesh() {
    
}

//
// set methods
//

void Mesh::setCellDimension(double dx, double dy, double dz) {

    if (dx<=0.0 || dy<=0.0 || dz<=0.0)
    {
        Warning::printMessage("Cell dimension must be greater that 0");
        throw (0);
    }

    cellDim=Vector3d(dx,dy,dz);    
}

void Mesh::setCellDimension(const Vector3d& cdim) {

    setCellDimension(cdim.x(), cdim.y(), cdim.z());
}

void Mesh::setNumCells(int nx, int ny, int nz) {

    if (nx<=0 || ny<=0 || nz<=0) {
        
        Warning::printMessage("Number of cells must be greater that 0");
        throw (0);
    }

    nCells=Vector3i(nx,ny,nz);
}

void Mesh::setNumCells(const Vector3i& n) {

    setNumCells(n.x(), n.y(), n.z());
}

void Mesh::setNumGhosts(int ng) {

    if (ng<0){

        Warning::printMessage("Number of Ghosts must be positive");
        throw (0);
    }

    nGhosts=ng;
}

//
// get methods
//

Vector3i Mesh::getTotalCells() const { 
   
    return Vector3i(nCells.x()+nGhosts*2, nCells.y()+nGhosts*2, nCells.z()+nGhosts*2);
}

Vector3d Mesh::getGridCoordinates(const Vector3d& position) const {

    // grid coordinates
    const double i = (position.x()-minLimit.x())/cellDim.x()+nGhosts;
    const double j = (position.y()-minLimit.y())/cellDim.y()+nGhosts;
    const double k = (position.z()-minLimit.z())/cellDim.z()+nGhosts;

    // return vector of coordinates
    return Vector3d(i,j,k);
}

Vector3i Mesh::getParentNodeCoordinates(const Vector3d& position) const {

    // grid coordinates
    const Vector3d gridCoords = getGridCoordinates(position);
    
    // return the floor of the grid coordinates
    return Vector3i(int(floor(gridCoords.x())),int(floor(gridCoords.y())),int(floor(gridCoords.z())));
}

int Mesh::getCellIdbyPosition(const Vector3d& position) const {

    // parent node coordinate
    const Vector3i gridParentNodeCoords = getParentNodeCoordinates(position);
    const int i = gridParentNodeCoords.x();
    const int j = gridParentNodeCoords.y();
    const int k = gridParentNodeCoords.z();

    // cell id
    return ((j*nRows.x()+i)+(nRows.x()*nRows.y()*k));
}

int Mesh::getParentCellIdConstribution(const Vector3d& position) const {

    // parent node coordinates
	const Vector3i gridParentNodeCoords = getParentNodeCoordinates(position);
    const int pi=gridParentNodeCoords.x();
    const int pj=gridParentNodeCoords.y();
    const int pk=gridParentNodeCoords.z();

    // grid coordinates
    const Vector3d gridCoords = getGridCoordinates(position);
    const double gi=gridCoords.x();
    const double gj=gridCoords.y();
    const double gk=gridCoords.z();

    // relative distance to parent node
    const double relDx = gi-double(pi);
    const double relDy = gj-double(pj);
    const double relDz = gk-double(pk);

    // calculate the correct the parent node
    const int i = relDx<0.5 ? pi-1 : pi;
    const int j = relDy<0.5 ? pj-1 : pj;
    const int k = relDz<0.5 ? pk-1 : pk;

    // return the parent node of 27 node contribution
    return ((j*nRows.x()+i)+(nRows.x()*nRows.y()*k));
}

vector<int> Mesh::getNodesInCell(const Vector3d& position) const {
    
    const int cellId = getCellIdbyPosition(position);
    
    const int idNode1 = cellId;
    const int idNode2 = idNode1 + 1;
    const int idNode3 = idNode1 + nRows.x();
    const int idNode4 = idNode3 + 1;
    const int idNode5 = idNode1 + (nRows.x()*nRows.y());
    const int idNode6 = idNode2 + (nRows.x()*nRows.y());
    const int idNode7 = idNode3 + (nRows.x()*nRows.y());
    const int idNode8 = idNode4 + (nRows.x()*nRows.y());

    vector<int> v {idNode1,idNode2,idNode3,idNode4,idNode5,idNode6,idNode7,idNode8};
    return v;
}

void Mesh::getContributionNodes(const Vector3d& position, vector<int>& contributionIds) const {

    const int cellId = getParentCellIdConstribution(position);
    const int nXY = nRows.x()*nRows.y();
    const int nX = nRows.x();

    contributionIds.resize(27,0);

    contributionIds.at(0) = cellId+0;
    contributionIds.at(1) = cellId+1;
    contributionIds.at(2) = cellId+2;
    contributionIds.at(3) = cellId+nX+0;
    contributionIds.at(4) = cellId+nX+1;
    contributionIds.at(5) = cellId+nX+2;
    contributionIds.at(6) = cellId+nX+nX+0;
    contributionIds.at(7) = cellId+nX+nX+1;
    contributionIds.at(8) = cellId+nX+nX+2;

    contributionIds.at(9)  = cellId+0+nXY*1;
    contributionIds.at(10) = cellId+1+nXY*1;
    contributionIds.at(11) = cellId+2+nXY*1;
    contributionIds.at(12) = cellId+nX+0+nXY*1;
    contributionIds.at(13) = cellId+nX+1+nXY*1;
    contributionIds.at(14) = cellId+nX+2+nXY*1;
    contributionIds.at(15) = cellId+nX+nX+0+nXY*1;
    contributionIds.at(16) = cellId+nX+nX+1+nXY*1;
    contributionIds.at(17) = cellId+nX+nX+2+nXY*1;

    contributionIds.at(18) = cellId+0+nXY*2;
    contributionIds.at(19) = cellId+1+nXY*2;
    contributionIds.at(20) = cellId+2+nXY*2;
    contributionIds.at(21) = cellId+nX+0+nXY*2;
    contributionIds.at(22) = cellId+nX+1+nXY*2;
    contributionIds.at(23) = cellId+nX+2+nXY*2;
    contributionIds.at(24) = cellId+nX+nX+0+nXY*2;
    contributionIds.at(25) = cellId+nX+nX+1+nXY*2;
    contributionIds.at(26) = cellId+nX+nX+2+nXY*2;
}

//
// public methods
//

void Mesh::createGrid(bool is_two_phase_simulation) {

    // set the rows in each direction
    nRows.x() = nCells.x()+2*nGhosts+1;
    nRows.y() = nCells.y()+2*nGhosts+1;
    nRows.z() = nCells.z()+2*nGhosts+1;

    // create the node vector
    gridNodes.clear();
    
    for (int i = 0; i < nRows.x()*nRows.y()*nRows.z() ; ++i) {

        gridNodes.push_back(is_two_phase_simulation ? new NodeMixture() : new Node());
    }

    // initialize nodes
    for (size_t i=0;i<gridNodes.size();i++){

    	gridNodes.at(i)->setCoordinates(Vector3d(0.0,0.0,0.0));
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
                gridNodes[nodeId]->setId(nodeId);

                // grid node position
                gridNodes[nodeId]->setCoordinates(Vector3d(x,y,z));
            }
        }
    }

    gridCells.clear();

    for (int k = 0; k < nCells.z()+2*nGhosts; k++) {
        for (int j = 0; j < nCells.y()+2*nGhosts; j++) {
            for (int i = 0; i < nCells.x()+2*nGhosts; i++) {
                
                // central position of the cell
                Vector3d cellPosition = Vector3d(
                    (i - nGhosts + 0.5) * cellDim.x() + minLimit.x(),
                    (j - nGhosts + 0.5) * cellDim.y() + minLimit.y(),
                    (k - nGhosts + 0.5) * cellDim.z() + minLimit.z()
                );

                // get nodes in cell
                vector<int> nodeIds = getNodesInCell(cellPosition);

                // create cell
                Cell* cell = new Cell();

                // set nodes in cell
                std::vector<Node*> nodes;
                for (int id : nodeIds) {
                    nodes.push_back(gridNodes[id]);
                }
				cell->setNodes(nodes);

                // compute cell volume
                cell->computeVolume();

                // store the cell
                gridCells.push_back(cell);
            }
        }
    }

    // update boundaries
    configureBoundaries();

    // set max limits of the mesh
    maxLimit.x()=nCells.x()*cellDim.x()+minLimit.x();
    maxLimit.y()=nCells.y()*cellDim.y()+minLimit.y();
    maxLimit.z()=nCells.z()*cellDim.z()+minLimit.z();
}

void Mesh::computeNodeVolumes() {

    // map of node volumes
    std::unordered_map<Node*, double> nodeVolumes;

    // initialize node volumes
    for (auto& node : gridNodes) {
        nodeVolumes[node] = 0.0;
    }

    // distribute the volume of each cell
    for (auto& cell : gridCells) {
        double sharedVolume = cell->getVolume() / 8.0;
        for (auto& node : cell->getNodes()) {
            nodeVolumes[node] += sharedVolume;
        }
    }

    // assign the volume to each node
    for (auto& node : gridNodes) {
        node->setVolume(nodeVolumes[node]);
    }
}

void Mesh::activateNodes(const vector<int>& nodesId,const bool activeValue) {

    for (size_t i = 0; i < nodesId.size(); ++i)
    {
        activateNode(nodesId.at(i),activeValue);
    }
}

void Mesh::setBoundaryRestrictions(vector<Boundary::BoundaryType> restrictions) {

    // verify if there are restrictions to the six planes
    if (restrictions.size()==6)
    {
    	this->boundary.setRestrictions(Boundary::BoundaryPlane::X0, restrictions.at(0));
    	this->boundary.setRestrictions(Boundary::BoundaryPlane::Y0, restrictions.at(1));
    	this->boundary.setRestrictions(Boundary::BoundaryPlane::Z0, restrictions.at(2));
    	this->boundary.setRestrictions(Boundary::BoundaryPlane::Xn, restrictions.at(3));
    	this->boundary.setRestrictions(Boundary::BoundaryPlane::Yn, restrictions.at(4));
    	this->boundary.setRestrictions(Boundary::BoundaryPlane::Zn, restrictions.at(5));
    }
    else{
        Warning::printMessage("Error setting the boundary conditions");
    }
}

void Mesh::setBoundaryRestrictionsFluid(vector<Boundary::BoundaryType> restrictions) {

    // verify if there are restrictions to the six planes
    if (restrictions.size()==6)
    {
        this->boundary.setRestrictionsFluid(Boundary::BoundaryPlane::X0, restrictions.at(0));
        this->boundary.setRestrictionsFluid(Boundary::BoundaryPlane::Y0, restrictions.at(1));
        this->boundary.setRestrictionsFluid(Boundary::BoundaryPlane::Z0, restrictions.at(2));
        this->boundary.setRestrictionsFluid(Boundary::BoundaryPlane::Xn, restrictions.at(3));
        this->boundary.setRestrictionsFluid(Boundary::BoundaryPlane::Yn, restrictions.at(4));
        this->boundary.setRestrictionsFluid(Boundary::BoundaryPlane::Zn, restrictions.at(5));
    }
    else{
        Warning::printMessage("Error setting the boundary conditions in fluid");
    }
}

//
// private methods
//

void Mesh::configureBoundaries(){
    
    // model's limits
    Vector3d minLimitsGhosts = minLimit;
    Vector3d maxLimitsGhosts = Vector3d(nCells.x()*cellDim.x(),nCells.y()*cellDim.y(),nCells.z()*cellDim.z());

    // node vectors
    vector<int> nodesPlaneX0, nodesPlaneY0, nodesPlaneZ0;
    vector<int> nodesPlaneXn, nodesPlaneYn, nodesPlaneZn;

    // get boundary nodes
    for (size_t i = 0; i < gridNodes.size(); ++i)
    {
        // node coordinates
        Vector3d nodeCoordinates=gridNodes.at(i)->getCoordinates();
        
        // node id
        int nodeId = gridNodes.at(i)->getId();

        // plane X0
        if (nodeCoordinates.x()<=minLimitsGhosts.x())
        {
            nodesPlaneX0.push_back(nodeId);
        }
        // plane Y0
        if (nodeCoordinates.y()<=minLimitsGhosts.y())
        {
            nodesPlaneY0.push_back(nodeId);
        }
        
        // plane Z0
        if (nodeCoordinates.z()<=minLimitsGhosts.z())
        {
            nodesPlaneZ0.push_back(nodeId);
        }

        // plane Xn
        if (nodeCoordinates.x()>=maxLimitsGhosts.x())
        {
            nodesPlaneXn.push_back(nodeId);
        }

        // plane Yn
        if (nodeCoordinates.y()>=maxLimitsGhosts.y())
        {
            nodesPlaneYn.push_back(nodeId);
        }        

        // plane Zn
        if (nodeCoordinates.z()>=maxLimitsGhosts.z())
        {
            nodesPlaneZn.push_back(nodeId);
        }
    }

    // set vectors in structure
    boundary.setNodesPlaneX0(nodesPlaneX0);
    boundary.setNodesPlaneY0(nodesPlaneY0);
    boundary.setNodesPlaneZ0(nodesPlaneZ0);

    boundary.setNodesPlaneXn(nodesPlaneXn);
    boundary.setNodesPlaneYn(nodesPlaneYn);
    boundary.setNodesPlaneZn(nodesPlaneZn);
}

bool Mesh::getIsInsideMesh(const Vector3d& point) const {

    if (point.x()>maxLimit.x()||point.y()>maxLimit.y()||point.z()>maxLimit.z()) return false;
    if (point.x()<minLimit.x()||point.y()<minLimit.y()||point.z()<minLimit.z()) return false;
    return true;
}
