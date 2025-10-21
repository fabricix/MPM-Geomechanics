// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#pragma once

#include <Body/Body.h>
#include <Mesh/Mesh.h>
#include <map>

/// \class BodyGmsh
/// \brief Is a type of Body representing a body defined by a Gmsh mesh file
/// each physical group in the Gmsh mesh file is a different body with its own material

class BodyGmsh : public Body 
{
    public:
        /// \brief Default constructor
        BodyGmsh(
                const std::string& mesh_file = "",
                std::map<std::string,int> physical_to_material = {},
                const std::string& particle_distribution = "barycentric");

        /// \brief Default destructor
        ///
        virtual ~BodyGmsh();

        /// \brief Seeds particles in the body defined by a Gmsh mesh file
        /// \param[in] mesh Computational Mesh reference
        /// \param[in] material Material
        virtual void create(Mesh& mesh, Material* material) override;

        /// \brief Configure the material id
        /// \param[in] material_id Material identification
        inline virtual void setMaterialId(int material_id) { this->materialId=material_id; }
        
        /// \brief Return the material identification
        /// \return Material identification
        inline virtual int getMaterialId() const { return this->materialId; }

        /// \brief Reset the shared Gmsh mesh to force reloading from file
        static void resetSharedMesh();

    private:
        string meshFile; //!< Gmsh mesh file name
        std::map<std::string,int> physicalToMaterial; //!< map from physical group name to material id
        string particleDistribution; //!< particle distribution in mesh elements
        int materialId; //!< material identification
};