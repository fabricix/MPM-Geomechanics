/*
 * Loads.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef LOADS_H_
#define LOADS_H_

#include "Body/Body.h"

#include <vector>
using std::vector;

#include "Eigen/Core"
using Eigen::Vector3d;

/// \namespace Loads
/// \brief Operations to manage loads in the MPM model.
namespace Loads {

    /// \struct LoadDistributedBox
    /// \brief Structure to define a distributed load in 
    /// particles inside the cuboid defined by point p1 and p2
    struct LoadDistributedBox
    {
        Vector3d pointP1; //!< left lower point of the box
        Vector3d pointP2; //!< right upper point of the box
        Vector3d load; //!< load to be distributed in particles inside the box
    };

    /// \struct NodalPointLoad
    /// \brief Define a structure to store point loads
    struct NodalPointLoadData
    {
        std::vector<Vector3d> points; //!< Coordinates [px, py, pz]
        std::vector <Vector3d> loads; //!< Load [lx, ly, lz]
        std::vector<int> nodal_ids; //!< nodal id
    };

    /// \struct PressureBox
    /// \brief Structure to represent a box with a pressure.
    /// Pressure in applied to particles inside the cuboid defined by point p1 and p2.
    struct PressureBox
    {
        Vector3d pointP1; //!< left lower point of the box
        Vector3d pointP2; //!< right upper point of the box
        double pressure; //!< pressure in particles inside the box
    };

    /// \struct PressureBoundaryForceBox
    /// \brief Structure to represent a boundary force in fluid due the action of the pressure
    struct PressureBoundaryForceBox
    {
        Vector3d pointP1; //!< left lower point of the box
        Vector3d pointP2; //!< right upper point of the box
        Vector3d pressureForce; //!< force due the pressure applied at the boundary
    };

    /// \struct PressureMaterial
    /// \brief Structure to represent a material with a pressure.
    /// Pressure is applied to particles with the specified material id.
    struct PressureMaterial
    {
        int materialId; //!< material id
        double pressure; //!< pressure in particles inside the box
    };

    /// \struct PrescribedPorePressure
    /// \brief Structure to represent a particle with a pressure.
    /// Pressure is applied to particles with the specified index.
    struct PrescribedPorePressure{

        int bodyIndex; //!< body index in body vector
        int particleIndex; //!< particle index in particle vector
        double pressure; //!< pressure to be applied to the particle

        /// \fn PrescribedPorePressure
        /// \brief Structure constructor
        /// \param[in] body_index Body index
        /// \param[in] particle_index Particle index
        /// \param[in] pressure_value Pressure value
        PrescribedPorePressure(int body_index, int particle_index, double pressure_value) {
            
            bodyIndex=body_index;
            particleIndex=particle_index;
            pressure=pressure_value;
        }
    };

    /**
     * @brief Seismic data structure to manage acceleration and velocity of an earthquake record.
     * 
     */
    struct SeismicData
    {
        std::vector<double> time;
        std::vector<Eigen::Vector3d> acceleration;
        std::vector<Eigen::Vector3d> velocity;
    };

    /**
     * @brief Set the Seismic Data object
     * 
     */
    void setSeismicData();

    /**
     * @brief Get the Seismic Data object
     * 
     * @return SeismicData& 
     */
    SeismicData& getSeismicData();

    /// \brief Configures the gravity load in particles
	/// \param[in] bodies A list containing Body pointers 
	void setGravity(vector<Body*>& bodies);
    
    /// \brief Configure distributed load in particles inside of a box
    /// \param[in] bodies A list containing Body pointers 
    /// \param[in] loads A list containing loads to be applied
    void setLoadDistributedBox(vector<Body*>& bodies, vector<Loads::LoadDistributedBox> loads);

    /// \brief Configure prescribed pore pressure in particles inside of a box
    /// \param[in] bodies A list containing Body pointers 
    /// \param[in] pressures A list containing pressures to be applied
    void setPrescribedPorePressureBox(vector<Body*>& bodies, vector<Loads::PressureBox> pressures);

    /// \brief Configure initial pore pressure in particles inside of a box
    /// \param[in] bodies A list containing Body pointers 
    /// \param[in] pressures A list containing pressures to be applied
    void setInitialPorePressureBox(vector<Body*>& bodies, vector<Loads::PressureBox> pressures);

    /// \brief Configure initial pore pressure in particles by material id
    /// \param[in] bodies A list containing Body pointers 
    /// \param[in] pressures A list containing material id to be applied the pressure
    void setInitialPorePressureMaterial(vector<Body*>& bodies, vector<Loads::PressureMaterial> pressures);

    /// \brief Set prescribed pore pressure in particles
    /// \param[in] bodies A list containing Body pointers 
    void updatePrescribedPorePressure(vector<Body*>* bodies);
    
    /// \brief Set external boundary force in fluid phase due the prescribed pore pressure in particles
    ///
    /// \f$ f_{i}^{w,ext,\Gamma} = \int t(x_i)^\Gamma \Delta A \f$
    /// \param[in] bodies A list containing Body pointers
    /// \param[in] loads A list of pressure force boxes
    void setPrescribedPorePressureBoundaryForceBox(vector<Body*>& bodies, vector<Loads::PressureBoundaryForceBox> loads);

    /// \brief Set initial velocity in bodies
    /// \param[in] bodies A list containing Body pointers
    void setInitialVelocity(vector<Body*>& bodies);

    /// \brief configure nodeid-point-load data for nodal force application
    /// \param [in] mesh mesh pointer
    void configureNodalPointLoads(Mesh* mesh);

    /// \brief Get (node-id load) vector for set nodal point load. 
    NodalPointLoadData& getNodalPointList();
};

#endif /* LOADS_H_ */
