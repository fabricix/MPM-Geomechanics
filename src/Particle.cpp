/*
 * Particle.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include<string>
using std::string;
using std::to_string;

#include "Particle.h"
#include "Mesh.h"

int Particle::totalParticles=0;

Particle::Particle() {

	initializeValues();

	totalParticles++;

	setId(totalParticles);
}

Particle::Particle(Vector3d position, Material* material, Vector3d size){

	initializeValues();

	setPosition(position);
	
	setMaterial(material);

	setDensity(material!=0?material->getDensity():0.0);

	setSize(size);

	setMass(size.x()*size.y()*size.z()*getDensity());

	setId(totalParticles);

	totalParticles++;
}

Particle::Particle(Vector3d position, Material* material) {

	initializeValues();

	setPosition(position);
	
	setMaterial(material);

	setDensity(material!=0?material->getDensity():0.0);

	setId(totalParticles);

	totalParticles++;
}

Particle::Particle(Vector3d position) {

	initializeValues();

	setPosition(position);

	setId(totalParticles);

	totalParticles++;
}

Particle::~Particle() {
	
	totalParticles--;
}

//
// get methods
//

int Particle::getId() const {

	return id;
}

double Particle::getMass() const{

	return mass;
}

Vector3d Particle::getPosition() const{

	return position;
}

vector<Contribution>* Particle::getContributionNodes(){

	return &contributionNodes;
}

int Particle::getTotalParticles() {

	return totalParticles;
}

Matrix3d Particle::getStress() const {

	return stress;
}

double Particle::getDensity() const {

	return density;
}

int Particle::getMaterialId() const {

	return material!=0?material->getId():-1;
}

Vector3d Particle::getExternalForce() const {

	return externalForce;
}

Vector3d Particle::getVelocity() const {

	return velocity;
}

Matrix3d Particle::getStrainIncrement() const {

	return strainIncrement;
}

Matrix3d Particle::getVorticityIncrement() const {

	return vorticityIncrement;
}

//
// set methods
//

void Particle::setSize(Vector3d pSize){

	size=pSize;
}

void Particle::setMass(double m)
{
	mass=m;
}

void Particle::setPosition(Vector3d x)
{
	position=x;
}

void Particle::setId(int i)
{
	id=i;
}

void Particle::setMaterial(Material* mat)
{
	material=mat;
}

void Particle::setShape(Shape*sh)
{
	shape=sh;
}

void Particle::setStrainIncrement(Matrix3d ds){

	strainIncrement=ds;
}

void Particle::setVorticityIncrement(Matrix3d dvor){

	vorticityIncrement=dvor;
}

void Particle::setDensity(double den){

	density=den;
}

void Particle::setStress(Matrix3d s){

	stress=s;
}

void Particle::setVelocity(Vector3d v){

	velocity = v;
}

void Particle::setExternalForce(Vector3d f)
{
	externalForce=f;
}

void Particle::addExternalForce(Vector3d df)
{
	externalForce+=df;
}

//
// public methods
//

void Particle::updateStress(){

	material->updateStress(this);
}

void Particle::updateContributionNodes(Mesh& mesh)
{
	// nodes that the particle contributed
	const vector<int> nodesId = mesh.getContributionNodes(position);
	
	// reference to grid nodes
	vector<Node> * gNodes = mesh.getNodes();

	// update the shape function for all nodes that this particle contributes
	for (size_t i = 0; i < nodesId.size(); ++i)
	{	
		// set the node id
		contributionNodes.at(i).setNodeId(nodesId.at(i));
		
		// active the node
		gNodes->at(nodesId.at(i)).setActive(true);
	
		// update the shape functions and gradients
		shape->update(position,gNodes->at(nodesId.at(i)).getCoordinates(),mesh.getCellDimension(),size);

		// get shape function and its derivates
		Vector3d shapeFn = shape->getShape();
		Vector3d derivateFn = shape->getDerivate();

		// update weight
		contributionNodes.at(i).setWeight(shapeFn.x()*shapeFn.y()*shapeFn.z());

		// update gradients
		Vector3d gradient;
		gradient.x() = derivateFn.x()*shapeFn.y()*shapeFn.z();
		gradient.y() = shapeFn.x()*derivateFn.y()*shapeFn.z();
		gradient.z() = shapeFn.x()*shapeFn.y()*derivateFn.z();

		contributionNodes.at(i).setGradients(gradient);
	}
}

//
// private methods
// 

void Particle::initializeValues(){

	id=0;
	bodyId=0;

	mass=0.0;
	density=0.0;
	volume=0.0;
	initialVolume=0.0;
	plasticStrain=0.0;
	porosity=0.0;

	position.setZero();
	velocity.setZero();
	externalForce.setZero();
	size.setZero();

	stress.setZero();
	strain.setZero();
	strainIncrement.setZero();
	vorticityIncrement.setZero();
	deformationGradientIncrement.setZero();
	velocityGradient.setZero();

	contributionNodes.clear();
	contributionNodes.resize(ModelSetup::getContributionNodesNum());
	
	shape=0;
	material=0;
}
