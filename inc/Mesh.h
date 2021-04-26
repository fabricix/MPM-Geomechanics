/*
 * Mesh.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef MESH_H_
#define MESH_H_

#include <vector>
using std::vector;

#include "../inc/Eigen/Core"
using Eigen::Vector3d;
using Eigen::Vector3i;

#include "Node.h"
#include "Particle.h"

class Particle;

/// \class Mesh
/// \brief Class representing a rectangular grid mesh.
/// 
/// A rectangular structured grid mesh is used to integrate
/// the equation of motion. 
///
class Mesh {

public:

    Mesh(); //!< default constructor
    virtual ~Mesh(); //!< default destructor

    void createGrid(); //!< create a structured mesh grid
    bool updateNodesContribution(const vector<Particle>&); //!< update the nodes contributing at each particle

    // set methods
    void setCellDimension(double,double,double); //!< set cells dimension in each direction
    void setNumCells(int,int,int); //!< set number of cell in each direction
    void setNumGhosts(int); //!< set number of ghosts around the domain
    void setLimits(double,double,double,double,double,double); //!< set domain limits
    void activateNodes(const vector<int>&,bool=true); //!< active nodes by its id
    void activateNode(int,bool=true); //!< active node by its id

    // get methods
    int getNumNodes(); //!< returns total nodes in mesh
    const vector<Node> & getNodes(); //!< returns nodes in mesh
    Vector3d getCellDimension(); //!< returns the cells dimension in each direction
    Vector3i getNumCells(); //!< returns total cells in the mesh without ghosts
    Vector3i getTotalCells(); //!< returns total cells including ghosts
    int getNumGhosts(); //!< get number of ghosts
    Vector3d getMinLimits(); //!< returns lower mesh coordinates
    Vector3d getMaxLimits(); //!< returns higher mesh coordinates
    vector<int> getNodesInCell(Vector3d); //!< returns the nodes of the cell containing a point
    vector<int> getContributionNodes(Vector3d); //!< returns the nodes contributing at point

private:
       
    int nGhosts; //!< number of ghost cells

    Vector3i nCell; //!< number of cells in each direction without ghost
    Vector3i nRows; //!< number of rows in each direction
    
    Vector3d cellDim; //!< cell dimension in each direction
    Vector3d minLimit; //!< lower coordinates domain without ghosts
    Vector3d maxLimit; //!< high coordinates of domain without ghosts
    
    vector<Node> gridNodes; //!< all nodes in mesh
   
    int getCellIdbyPosition(Vector3d); //!< returns the cell id in a position coordinates
    Vector3d getGridCoordinates(Vector3d); //!< returns the grid coordinates of a position
    Vector3i getParentNodeCoordinates(Vector3d); //!< return the grid parent node coordinate of a position
    int getParentCellIdConstribution(Vector3d); //!< return the id of the parent node contributing at the point
};

#endif /* MESH_H_ */
