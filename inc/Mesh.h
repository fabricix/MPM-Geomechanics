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
#include "Boundary.h"

class Particle;

/// \class Mesh
/// \brief Class representing a rectangular grid mesh.
/// 
/// A rectangular structured grid mesh is used to integrate
/// the equation of motion. 
///
class Mesh {

public:

    /// \brief default constructor
    ///
    Mesh();
    
    /// \brief Default destructor
    ///
    virtual ~Mesh();

    /// \brief Create a structured mesh grid
    ///
    void createGrid();

    /// \brief Set cells dimension in each direction
    /// \param[in] cell_dimension_x Cell dimesion in x
    /// \param[in] cell_dimension_y Cell dimesion in y
    /// \param[in] cell_dimension_z Cell dimesion in z
    void setCellDimension(double cell_dimension_x, double cell_dimension_y, double cell_dimension_z);
    
    /// \brief Set cells dimension in each direction
    /// \param[in] cell_dimension A vector containing the Cell dimesion in each direction
    void setCellDimension(const Vector3d& cell_dimension);
    
    /// \brief Set number of cell in each direction
    /// \param[in] number_cells_x Number of cells in x
    /// \param[in] number_cells_y Number of cells in y
    /// \param[in] number_cells_z Number of cells in z
    void setNumCells(int number_cells_x, int number_cells_y, int number_cells_z);
    
    /// \brief Set number of cell in each direction
    /// \param[in] number_cells Vector containing the number of cells in each direction
    void setNumCells(const Vector3i& number_cells);
    
    /// \brief Set number of ghosts around the domain
    /// \param[in] ghosts Number of ghosts around the domain
    void setNumGhosts(int ghosts); 
    
    /// \brief Set domain limits
    /// \param[in] x_min Minimum X coordinate
    /// \param[in] y_min Minimum Y coordinate
    /// \param[in] z_min Minimum Z coordinate
    /// \param[in] x_max Maximum X coordinate
    /// \param[in] y_max Maximum Y coordinate
    /// \param[in] z_max Maximum Z coordinate
    void setLimits(double x_min, double y_min, double z_min, double x_max, double y_max, double z_max); 
    
    /// \brief Set origin of coordinates
    /// \param[in] x X Coordinate
    /// \param[in] y Y Coordinate
    /// \param[in] z Z Coordinate
    inline void setOrigin(double x, double y, double z) { this->minLimit=Vector3d(x,y,z); }
    
    /// \brief Set origin of coordinates
    /// \param[in] origin_coordinate A vector containing the x,y and z 
    /// coordinates of the origin
    inline void setOrigin(const Vector3d& origin_coordinate) { this->minLimit=origin_coordinate; }
    
    /// \brief Activate nodes by its id
    /// \param[in] id_list A list containing the identification of the nodes
    /// to be activated
    /// \param[in] active_value Active value can be true for activate
    /// the node or false for inactivate the node
    void activateNodes(const vector<int>& id_list, bool active_value=true);
    
    /// \brief Activate node by its id
    /// \param[in] id The id of the node to modify its active attribute
    /// \param[in] active_value Active value can be true for activate
    /// the node or false for inactivate the node
    void activateNode(int id, bool active_value=true);

    /// \brief Return total nodes in mesh
    /// \return Total nodes in the mesh
    inline int getNumNodes() const { return this->gridNodes.size(); }
    
    /// \brief Return nodes in mesh
    /// \return A pointer to the mesh nodes
    inline vector<Node>* getNodes() { return &(this->gridNodes); }

    /// \brief Return the cells dimension in each direction
    /// \return A vector containing the cell dimension
    /// in each direction
    inline const Vector3d& getCellDimension() const { return this->cellDim; }
    
    /// \brief Return total cells in the mesh without ghosts
    /// \return A vector containing the number of cells
    /// in each direction
    inline const Vector3i& getNumCells() const { return this->nCells; }
    
    /// \brief Return total cells including ghosts
    /// \return A vector containing the number of cells
    /// in each direction
    Vector3i getTotalCells() const;
    
    /// \brief Get number of ghosts
    /// \return Number of additional cells around the mesh
    inline int getNumGhosts() const { return this->nGhosts; }

    /// \brief Return lower mesh coordinates
    /// \return A vector containing the lower
    /// coordinates of the mesh
    inline const Vector3d& getMinLimits() const { return this->minLimit;}

    /// \brief Return higher mesh coordinates
    /// \return A vector containing the higher
    /// coordinates of the mesh
    inline const Vector3d& getMaxLimits() const { return this->maxLimit; }

    /// \brief Return the nodes of the cell containing a point
    /// \param[in] point A vector containing the 
    /// coordinates of a point
    /// \return A list containing the indexes
    /// of the nodes of the cell containing the point
    vector<int> getNodesInCell(const Vector3d& point) const;

    /// \brief Return the nodes contributing at point
    /// \param[in] point A vector containing the 
    /// coordinates of a point
    /// \return Contributed nodes. A list containing the indexes
    /// of the nodes that the a particle placed in the coordinate point
    /// contributes
    vector<int> getContributionNodes(const Vector3d& point) const;

    /// \brief return mesh boundaries
    /// \param[out] boundary_pointer A pointer to the Boundary structure
    inline Boundary* getBoundary() { return &(this->boundary); }

    /// \brief Configures the restriction of the boundary nodes
    /// \param[in] restrictions Vector containing the restriction to the planes
    /// X0, Y0, Z0, Xn, Yn and Zn.
    void setBoundaryRestrictions(vector<Boundary::BoundaryType> restrictions);

private:
    
    int nGhosts; //!< number of ghost cells
    
    Vector3i nCells; //!< number of cells in each direction without ghost

    Vector3i nRows; //!< number of rows in each direction
    
    Vector3d cellDim; //!< cell dimension in each direction

    Vector3d minLimit; //!< lower coordinates domain without ghosts
    
    Vector3d maxLimit; //!< high coordinates of domain without ghosts
    
    vector<Node> gridNodes; //!< all nodes in mesh
    
    Boundary boundary; //!< mesh boundary
    
    /// \brief Return the cell id in a position coordinates
    /// \param[in] point A vector containing the 
    /// coordinates of a point
    /// \return The identification of the cell containing
    /// the point
    int getCellIdbyPosition(const Vector3d& point) const;
    
    /// \brief Return the grid coordinates of a position
    /// \param[in] point A vector containing the 
    /// coordinates of a point
    /// \return The cell coordinates in
    /// grid coordinates: \f$i\f$, \f$j\f$ and \f$k\f$
    Vector3d getGridCoordinates(const Vector3d& point) const;
    
    /// \brief Return the grid parent node coordinate of a position
    /// \param[in] point A vector containing the 
    /// coordinates of a point
    /// \return Parent node grid coordinate. The floor (lower-left)
    /// grid coordinate of the cell containing the point
    Vector3i getParentNodeCoordinates(const Vector3d& point) const;
    
    /// \brief Return the id of the parent node contributing at the point
    /// \param[in] point A vector containing the 
    /// coordinates of a point
    /// \return Parent node grid id. The identification of the cell
    /// containing the point
    int getParentCellIdConstribution(const Vector3d& point) const;
    
    /// \brief Updates the boundary nodes index
    ///
    void configureBoundaries();
};

inline void Mesh::activateNode(int nodeId, const bool activeValue) {

    gridNodes.at(nodeId).setActive(activeValue);
}

inline void Mesh::setLimits(double minX, double minY, double minZ, double maxX, double maxY, double maxZ) {

    minLimit=Vector3d(minX,minY,minZ);
    maxLimit=Vector3d(maxX,maxY,maxZ);
}

#endif /* MESH_H_ */