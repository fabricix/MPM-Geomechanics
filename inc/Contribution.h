/*
 * Contribution.h
 *
 *  Created on: 22 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef CONTRIBUTION_H_
#define CONTRIBUTION_H_

#include "Eigen/Core"
using Eigen::Vector3d;


#include <unordered_map>
using namespace std;

class Particle;

/// \class Contribution
/// \brief Represents the node that the particle contributes
///
/// During the interpolation process values stored in to the particles are calculated in mesh nodes using interpolation functions \f$ N_I(x_p) \f$ and its gradients \f$ N_{I,j}(x_p) \f$. 
/// Each of these functions are defined at the node \f$ I \f$ and evaluated at particle position \f$ x_p \f$. 
///
/// This class store the nodal id \f$ I \f$ and the values of \f$ N_I(x_p) \f$ and \f$ N_{I,j}(x_p) \f$.
///
class Contribution {

public:

	/// \brief Default constructor
	///
	Contribution();
	
	/// \brief Default destructor
	///
	virtual ~Contribution();

	/// \brief Return the node identification
	/// \return Node identification
	inline int getNodeId() const { return this->nodeId; }

	/// \brief Return the node identification
	/// \return Node identification
	inline void setDomainId(unsigned domainId, Particle* particle) { this->domains[domainId].push_back(particle); }

	/// \brief Return the node identification
	/// \return Node identification
	inline void checkInterface() { this->is_interface = this->domains.size() > 1; }

	/// \brief Return the node identification
	/// \return Node identification
	inline void clearSubdomain(int index) { this->domains.clear(); this->is_interface = false;}

	/// \brief Return the node identification
	/// \return Node identification
	inline bool getInterface() { return this->is_interface; }

	/// \brief Return the weight value
	/// \return Nodal weight. Value of the nodal weight evaluated at the particle position
	inline double getWeight() const { return this->weight; }

	/// \brief Return the gradients values
	/// \return Nodal gradients. Nodal gradients in each direction evaluated at the particle position
	inline const Vector3d& getGradients() const { return this->gradient; }

	/// \brief Configure node identification
	/// \param[in] nodal_id Node identification
	inline void setNodeId(int nodal_id) { this->nodeId=nodal_id; }
	
	/// \brief Configure weight
	/// \param[in] nodal_weight Value of the nodal weight evaluated at the particle position
	inline void setWeight(double nodal_weight) { this->weight=nodal_weight; }
	
	/// \brief Configure gradients
	/// \param[in] nodal_gradients Nodal gradients in each direction evaluated at the particle position
	inline void setGradients(const Vector3d& nodal_gradients) { this->gradient=nodal_gradients; };

private:
	
	int nodeId; //!< identification of node \f$ I \f$

    unordered_map<unsigned, vector<Particle*>> domains;
	bool is_interface;

	double weight; //!< weight value of a node at a particle position \f$ N_I(x_p) \f$

	Vector3d gradient; //!< gradient value of a node at a particle position \f$ N_{I,j}(x_p)  = dN/dx_{I}(x_p), dN/dy_{I}(x_p), dN/dz_{I}(x_p) \f$.
};

inline Contribution::Contribution()
:nodeId(0),weight(0.0),gradient(0.0,0.0,0.0) 
{ }

inline Contribution::~Contribution() 
{ }

#endif /* CONTRIBUTION_H_ */