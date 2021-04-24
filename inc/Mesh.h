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
    void activateNodes(const vector<int>&,bool); //!< active nodes by its id
    bool updateNodesContribution(const vector<Particle>&); //!< update the nodes contributing at each particle

    // set methods
    void setCellDimension(double,double,double); //!< set cells dimension in each direction
    void setNumCells(int,int,int); //!< set number of cell in each direction
    void setNumGhosts(int); //!< set number of ghosts around the domain
    void setLimits(double,double,double,double,double,double); //!< set domain limits

    // get methods
    int getNumNodes(); //!< returns total nodes in mesh
    const vector<Node> & getNodes(); //!< returns nodes in mesh
    vector<double> getCellDimension(); //!< returns the cells dimension in each direction
    vector<int> getNumCells(); //!< returns total cells in the mesh without ghosts
    vector<int> getTotalCells(); //!< returns total cells including ghosts
    int getNumGhosts(); //!< get number of ghosts
    vector<double> getMinLimits(); //!< returns lower mesh coordinates
    vector<double> getMaxLimits(); //!< returns higher mesh coordinates
    vector<int> getNodesInCell(Vector3d); //!< returns the nodes of the cell containing a point
    vector<int> getContributionNodes(Vector3d); //!< returns the nodes contributing at point

private:
       
    int nCellX; //!< number of cells in x without ghost
    int nCellY; //!< number of cells in y without ghost
    int nCellZ; //!< number of cells in z without ghost

    int nGhosts; //!< number of ghost cells

    double cellDimX; //!< cell dimension in x
    double cellDimY; //!< cell dimension in y
    double cellDimZ; //!< cell dimension in z

    int nRowsX; //!< number of rows in x
    int nRowsY; //!< number of rows in y
    int nRowsZ; //!< number of rows in z

    double minLimitX; //!< lower x coordinate domain without ghosts
    double minLimitY; //!< lower y coordinate domain without ghosts
    double minLimitZ; //!< lower z coordinate domain without ghosts

    double maxLimitX; //!< high x coordinate of domain without ghosts
    double maxLimitY; //!< high y coordinate of domain without ghosts
    double maxLimitZ; //!< high z coordinate of domain without ghosts

    vector<Node> gridNodes; //!< all nodes in mesh
   
    // private methods
    int getCellIdbyPosition(Vector3d); //!< returns the cell id in a position coordinates
    vector<double> getGridCoordinates(Vector3d); //!< returns the grid coordinates of a position
    vector<int> getParentNodeCoordinates(Vector3d); //!< return the grid parent node coordinate of a position
    int getParentCellIdConstribution(Vector3d); //!< return the id of the parent node contributing at the point
};

#endif /* MESH_H_ */
