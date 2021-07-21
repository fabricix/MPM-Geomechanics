/*
 * BodyPolygon.h
 *
 *  Created on: 16 de jul de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef BODYPOLYGON_H_
#define BODYPOLYGON_H_

#include <Body/Body.h>

/// \class BodyPolygon
/// \brief Represents a body in 3D created the extrusion of a polygon in 2D

/// ## Introduction
///
/// This class allows to create a 3D body in the space using a 2D polygon extrusion.
/// The 3D created body is fill up with particles according to a discretization parameter.
/// 
/// The polygon is a group of points in the space where only two components are considered during the
/// extrusion. The extrusion is performed according to an direction and a displacement in that direction.
/// The direction of the extrusion is represented by the `extrudeDirection` attribute.
/// The displacement of the extrusion is represented by the `extrudeDisplacement` attribute.
///
/// ## Extrusion direction
///
/// The extrude direction is the axis through with the polygon will be extruded.
///
///```
///
/// Extrusion in x   Extrusion in y    Extrusion in z 
///
///  z               x                 y
///  ^               ^                 ^
///  |               |                 |
///  o---> y         o---> z           o---> x
///
/// o : is the axis pointing out of the plane
///
///```
/// ## Extrusion displacement
///
/// Is the total displacement of the polygon thought the extrusion direction.
///

class BodyPolygon: public Body {

public:

    /// \brief Default constructor
    ///
	BodyPolygon();

    /// \brief Default constructor
    ///
	virtual ~BodyPolygon();

    /// \brief Configure the geometrical points of the polygon
    /// \param[in] polygon_points Geometrical definition of the polygon
    inline void setPoints(vector<Vector3d> polygon_points) { this->polygonPoints=polygon_points; }

    /// \brief Configure the material id
    /// \param[in] material_id Material identification
    inline virtual void setMaterialId(int material_id) { this->materialId=material_id; }
    
    /// \brief Return the material identification
    /// \return Material identification
    inline virtual int getMaterialId() const { return this->materialId; }

    /// \brief Create a body in space using a polygon in 2D
    /// \param[in] &mesh Mesh reference
    /// \param[in] *material Material pointer 
    virtual void create(Mesh& mesh, Material* material);

    /// \brief Configures the extrude direction
    /// \param[in] extrude_direction Extrude direction can be "y"
    inline void setExtrudeDirection(string extrude_direction) { this->extrudeDirection=extrude_direction; }

    /// \brief Configures the extrude displacement
    /// \param[in] extrude_displacement Extrude displacement
    inline void setExtrudeDisplacement(double extrude_displacement) { this->extrudeDisplacement=extrude_displacement; }

    /// \brief Configures the discretization parameter
    /// \param[in] distretization_length Distretization length
    inline void setDiscretizationLength(double distretization_length) { this->distretizationLength=distretization_length; }

private:

    int materialId; //!< material identification

    vector<Vector3d> polygonPoints; //!< geometrical definition of the polygon

    string extrudeDirection; //!< extrude direction

    double extrudeDisplacement; //!< extrude displacement

    double distretizationLength; //!< length of the local mesh used to create the body
};

#endif /* BODYPOLYGON_H_ */
