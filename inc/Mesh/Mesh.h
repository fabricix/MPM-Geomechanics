// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef MESH_H_
#define MESH_H_

#include <vector>
using std::vector;

#include "../inc/Eigen/Core"
using Eigen::Vector3d;
using Eigen::Vector3i;

#include <unordered_map>
using std::unordered_map;

#include "Node.h"
#include "Boundary.h"
#include "Cell.h"

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

    struct ContactNodeData 
    {
        int nodeId = -1;
        int bodyMasterId = -1;
        int bodySlaveId = -1;
        double mu = 0.0;
        bool hasContact = true;

        double massMaster = 0.0;
        double massSlave = 0.0;
        double closestParticleDistanceMaster = 999.0;
        double closestParticleDistanceSlave = 999.0;
        Vector3d momentumMaster = Vector3d::Zero();
        Vector3d momentumSlave = Vector3d::Zero();
        Vector3d velocityMaster = Vector3d::Zero();
        Vector3d velocitySlave = Vector3d::Zero();
        Vector3d internalForceMaster = Vector3d::Zero();
        Vector3d internalForceSlave = Vector3d::Zero();
        Vector3d externalForceMaster = Vector3d::Zero();
        Vector3d externalForceSlave = Vector3d::Zero();
        Vector3d dampingForceMaster = Vector3d::Zero();
        Vector3d dampingForceSlave = Vector3d::Zero();
        Vector3d totalForceMaster = Vector3d::Zero();
        Vector3d totalForceSlave = Vector3d::Zero();
        Vector3d contactForce = Vector3d::Zero();
        Vector3d normalContactForce = Vector3d::Zero();
        Vector3d tangentialContactForce = Vector3d::Zero();
        Vector3d normalMaster = Vector3d::Zero();
        Vector3d normalSlave = Vector3d::Zero();
        Vector3d normal = Vector3d::Zero();
    };

    /// \brief Create a structured mesh grid
    /// \param[in] is_two_phase_simulation True if two phase simulation is required
    void createGrid( bool is_two_phase_simulation );

    /// \brief Set cells dimension in each direction
    /// \param[in] cell_dimension_x Cell dimension in x
    /// \param[in] cell_dimension_y Cell dimension in y
    /// \param[in] cell_dimension_z Cell dimension in z
    void setCellDimension(double cell_dimension_x, double cell_dimension_y, double cell_dimension_z);
    
    /// \brief Set cells dimension in each direction
    /// \param[in] cell_dimension A vector containing the Cell dimension in each direction
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

    void setBoundaryRestrictionsSeismic();

    /// \brief Activate node by its id
    /// \param[in] id The id of the node to modify its active attribute
    /// \param[in] active_value Active value can be true for activate
    /// the node or false for inactivate the node
    void activateNode(int id, bool active_value=true);

    /// \brief Return total nodes in mesh
    /// \return Total nodes in the mesh
    inline unsigned int getNumNodes() const { return (unsigned int) this->gridNodes.size(); }
    
    /// \brief Return nodes in mesh
    /// \return A pointer to the mesh nodes
    inline vector<Node*>* getNodes() { return &(this->gridNodes); }

    /// \brief Return cells in mesh
    /// \return A pointer to the mesh cells
    inline vector<Cell*>* getCells() { return &(this->gridCells); }

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
    /// \param[in] contributionIds A vector that will contain the indexes
    /// of the nodes that the a particle placed in the coordinate point
    /// contributes
    void getContributionNodes(const Vector3d& point, vector<int>& contributionIds) const;

    /// \brief return mesh boundaries
    /// \return boundary_pointer A pointer to the Boundary structure
    inline Boundary* getBoundary() { return &(this->boundary); }

    /// \brief return nodes where there is contact between two bodies
    /// \return an unordered_map of the nodes where there is contact between two bodies
    unordered_map<int, ContactNodeData>& getContactNodes() { return contactNodes; }

    /// \brief Configures the restriction of the boundary nodes
    /// \param[in] restrictions Vector containing the restriction to the planes
    /// X0, Y0, Z0, Xn, Yn and Zn
    void setBoundaryRestrictions(vector<Boundary::BoundaryType> restrictions);

    /// \brief Configures the restriction of the boundary nodes
    /// \param[in] plane The plane to set the restriction
    /// \param[in] restriction The type of restriction to be applied
    void setRestriction(Boundary::BoundaryPlane plane, Boundary::BoundaryType restriction);

    /// \brief Configures the restriction of fluid phase at the boundary nodes
    /// \param[in] restrictions Vector containing the restriction to the planes
    /// X0, Y0, Z0, Xn, Yn and Zn
    void setBoundaryRestrictionsFluid(vector<Boundary::BoundaryType> restrictions);

    /// \brief Configures the contact nodes
    /// \param[in] unordered_map containing the contact nodes
    void setContactNodes(unordered_map<int, ContactNodeData> _contactNodes) { this->contactNodes = _contactNodes; }

    /// \brief Verify if the position is inside the limits
    /// \param[in] point A vector containing the
    /// coordinates of a point
    /// \return True if the point is inside the mesh
    bool getIsInsideMesh(const Vector3d& point) const;

    /// \brief Return compute the nodal volumes
    void computeNodeVolumes();

    /// \Clear Contact Nodes
    void clearContactNodes() { contactNodes.clear(); };


private:
    
    int nGhosts; //!< number of ghost cells
    
    Vector3i nCells; //!< number of cells in each direction without ghost

    Vector3i nRows; //!< number of rows in each direction
    
    Vector3d cellDim; //!< cell dimension in each direction

    Vector3d minLimit; //!< lower coordinates domain without ghosts
    
    Vector3d maxLimit; //!< high coordinates of domain without ghosts
    
    std::vector<Node*> gridNodes; //!< all nodes in mesh
    
    std::vector<Cell*> gridCells; //!< all cells in mesh

    Boundary boundary; //!< mesh boundary

    unordered_map<int, ContactNodeData> contactNodes; //!< nodes in contact
    
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

    gridNodes.at(nodeId)->setActive(activeValue);
}

#endif /* MESH_H_ */
