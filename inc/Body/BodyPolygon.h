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
/// \brief Is a type of Body created by the extrusion of a 2D polygon.
///
/// ## Introduction
///
/// This class allows to create a 3D body in the space by extrusion of a 2D polygon.
/// 
/// The extrusion is performed according to a direction and to a displacement.
/// The direction of the extrusion is represented by the `extrudeDirection` attribute.
/// The displacement of the extrusion is represented by the `extrudeDisplacement` attribute.
/// This body is fill up with particles according to a discretization parameter `distretizationLength`.
///
/// ## Extrusion direction
///
/// The extrude direction is the axis by with the polygon will be extruded.
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
/// Is the total displacement of the polygon through the extrusion direction.
///
/// ## The Discretization Length
///
/// When the particles are put in the body an auxiliary grid is used.
/// The `discretizationLength` attribute is the dimension of a grid cell.
/// 
/// ## How the material points are placed in the body
///
/// Once the body was created by extrusion, the material points need to be
/// distributed inside the body. For do that the create method performs this operations:
///
/// 1. calculates the bounding box of the body;
/// 2. defines the material point size by the discretization parameter and by assuming 2 
/// material points in each direction inside a cell;
/// 3. puts the material points beginning by the left lower point of the bounding box, 
/// verifying if the material point is inside the polygon.
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
