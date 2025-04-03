#include <vector>
using std::vector;

#include "../inc/Eigen/Core"
using Eigen::Vector3d;
using Eigen::Vector3i;

#ifndef BOUNDINGBOX_H  // Verifica que el identificador no haya sido definido antes
#define BOUNDINGBOX_H  // Define el identificador para evitar redefiniciones

class Particle;

class BoundingBox {

private:

    Vector3d minPoint;
    Vector3d maxPoint;
    Vector3d size;

    //vector de particulas del subdominio
    vector<vector<Particle*>> subdomains;

    /* data */
public:

    BoundingBox();
    BoundingBox(Vector3d minPoint, Vector3d maxPoint, unsigned subdomainsNumber);
    ~BoundingBox();

	void setupSubdomains();

    float getWidth(){ return this->size.x(); };

    int getSubdomainsNumber(){ return this->subdomains.size(); };
    int getSubdomainSize(int i){ return this->subdomains[i].size(); };
    //void clearSubdomian(int i){ this->subdomains[i] = new vector<Particle*>();};
    void setSubdomainParticle(int pos, Particle* particle){ this->subdomains[pos].push_back(particle); }
};

#endif