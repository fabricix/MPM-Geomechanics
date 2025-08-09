#include "Parallelization.h"
#include <iostream>
#include "Particle/Particle.h"
#include "Mesh/Node.h"
#include "Update.h"

#include "omp.h"

#include <unordered_map>
#include <iostream>
using namespace std;


// Copy of the particles per thread
vector<vector<Particle*>> particlesPerThreadAux;

// Numbers of threads
int NumberOfThreads = 0;

int particlesProccesed;

void Parallelization::calculateParticlesPerThread(Mesh* mesh, vector<vector<Particle*>*>& particlesPerThread, vector<Particle*>& particles, int factor)
{
  // numbers of thread is 2^factor
  NumberOfThreads = 1 << factor;

  particlesPerThreadAux.clear();

  // Initialize the particles per thread vector
  particlesPerThreadAux.resize(NumberOfThreads);

  // the real particles per thread vector is initialized. One more is added to store the interface particles
  particlesPerThread.resize(NumberOfThreads + 1);

  // Initialize the particles per thread vector
  for (int i = 0; i < NumberOfThreads; i++)
  {
    particlesPerThreadAux.at(i) = vector<Particle*>();
    particlesPerThread.at(i) = new vector<Particle*>();
  }

  // the last vector is used to store the interface particles
  particlesPerThread.at(NumberOfThreads) = new vector<Particle*>();

  // minX represents the minimum x position of the particles
  float minX = std::numeric_limits<float>::max();;

  // maxX represents the maximum x position of the particles
  float maxX = std::numeric_limits<float>::lowest();;

  // obtain the minimum and maximum x position of the particles
  for (size_t i = 0; i < particles.size(); i++) {
    if (particles.at(i)->getPosition().x() < minX) {
      minX = particles.at(i)->getPosition().x();
    }
    if (particles.at(i)->getPosition().x() > maxX) {
      maxX = particles.at(i)->getPosition().x();
    }
  }

  // the fontier is the distance between the minimum and maximum x position of the particles divided by the number of threads
  double fontier = (maxX - minX) / (NumberOfThreads);

  // the thread id
  int threadId = 0;

  for (size_t i = 0; i < particles.size(); i++)
  {
    // the thread id is determined by the position of the particle in the x axis 
    threadId = std::min((int)((particles.at(i)->getPosition().x() - minX) / fontier), NumberOfThreads - 1);

    // set the thread id of the particle
    particles.at(i)->threadId = threadId;

    // set the original thread id of the particle
    particles.at(i)->originalThreadId = threadId;

    // add the particles to the particles per thread vector
    particlesPerThreadAux.at(threadId).push_back(particles.at(i));
  }

  // show the number of particles per thread
  for (size_t i = 0; i < particlesPerThreadAux.size(); i++)
  {
    std::cout << "Thread " << i << " size: " << particlesPerThreadAux.at(i).size() << std::endl;
  }
}

void Parallelization::calculateInterfaceParticles(Mesh* mesh, vector<vector<Particle*>*>& particlesPerThread) //vector<vector<Particle*>*>& interfaceParticlesPerThread
{

  // Clear the particles per thread vector
  for (size_t i = 0; i < particlesPerThread.size(); i++)
  {
    particlesPerThread.at(i)->clear();
  }

  // Initialization of necessary variables
  vector<Contribution>* contribution;
  vector<Node*>* nodes = mesh->getNodes();
  Node* nodeI;
  bool isInterface = false;

  // Iterate through the set of particle sets (vectors of particles per thread)
  for (int t = 0; t < particlesPerThreadAux.size(); t++)
  {

    // Iterate through the particles of a thread
    for (int p = 0; p < particlesPerThreadAux.at(t).size(); p++)
    {

      // Contribution nodes of the particle
      contribution = particlesPerThreadAux.at(t).at(p)->getContributionNodes();

      // Iterate through the contribution nodes of the particle
      for (size_t j = 0; j < contribution->size(); ++j) {

        // Node corresponding to the particle
        nodeI = nodes->at(contribution->at(j).getNodeId());

        // Determine if the node's thread corresponds to the same thread as the particle. If it is equal to -1, then it is a node without a thread.
        if (nodeI->getThreadId() == -1 || nodeI->getThreadId() == particlesPerThreadAux.at(t).at(p)->originalThreadId)
        {
          // Assign the particle's thread to the node
          nodeI->setThreadId(t);
        }
        else
        {
          // The node has a thread different from the particle's thread, so it is considered an interface
          nodeI->setThreadId(-2);

          // The particle is an interface particle
          isInterface = true;
        }
      }
      if (isInterface)
      {
        // Interface particles are stored in thread -2
        particlesPerThreadAux.at(t).at(p)->threadId = -2;

        // Add to the list of **original** interface particles
        particlesPerThread.at(NumberOfThreads)->push_back(particlesPerThreadAux.at(t).at(p));
      }
      else {
        // Identify the thread of the particle
        particlesPerThreadAux.at(t).at(p)->threadId = t;

        // Add to the list of **original** particles corresponding to the thread
        particlesPerThread.at(t)->push_back(particlesPerThreadAux.at(t).at(p));

      }
      isInterface = false;
    }
  }
}

void Parallelization::interpolateMass(Mesh* mesh, vector<vector<Particle*>*>& particlesPerThread, int factor)
{

  // get nodes
  vector<Node*>* nodes = mesh->getNodes();

  unordered_map<int, double> totalSums;
#pragma omp parallel num_threads(1 << factor)
  {
    int thread = omp_get_thread_num();
    // std::cout << "omp_get_thread_num():  " << thread << std::endl;

    for (int i = 0; i < particlesPerThread[thread]->size(); ++i) {
      Particle* particle = particlesPerThread[thread]->at(i);

      // only active particle can contribute
      if (!particle->getActive()) { continue; }

      // get nodes and weights that the particle contributes
      const vector<Contribution>* contribution = particle->getContributionNodes();

      // get the particle mass
      const double pMass = particle->getMass();

      // for each node in the contribution list 
      for (size_t j = 0; j < contribution->size(); ++j) {

        // get the contributing node
        Node* nodeI = nodes->at(contribution->at(j).getNodeId());

        // compute the weighted nodal mass
        const double nodalMass = pMass * contribution->at(j).getWeight();

        // // check any mass in node
        if (nodalMass <= 0.0) { continue; }

        // the node is inactivate if he doesn't have mass
        nodeI->setActive(true);
        nodeI->addMass(nodalMass);
      }
    }
  }



  //interpolate interface particles values
  for (size_t i = 0; i < particlesPerThread[particlesPerThread.size() - 1]->size(); i++)
  {

    bool isInterface = false;

    Particle* particle = particlesPerThread[particlesPerThread.size() - 1]->at(i);

    // only active particle can contribute
    if (!particle->getActive()) { continue; }

    // get nodes and weights that the particle contributes
    const vector<Contribution>* contribution = particle->getContributionNodes();

    // get the particle mass
    const double pMass = particle->getMass();

    // for each node in the contribution list 
    for (size_t j = 0; j < contribution->size(); ++j) {

      // get the contributing node
      Node* nodeI = nodes->at(contribution->at(j).getNodeId());
      if (nodeI->getThreadId() == -2) isInterface = true;

      // compute the weighted nodal mass
      const double nodalMass = pMass * contribution->at(j).getWeight();

      if (nodalMass <= 0.0) { continue; }

      nodeI->setActive(true);
      nodeI->addMass(nodalMass);
    }
  }
}

void Parallelization::nodalMomentum(Mesh* mesh, vector<vector<Particle*>*>& particlesPerThread, int factor)
{

  // get nodes
  vector<Node*>* nodes = mesh->getNodes();

  double totalSumsX[nodes->size()] = { 0.0 }; //all the elements are zero
  double totalSumsY[nodes->size()] = { 0.0 }; //all the elements are zero
  double totalSumsZ[nodes->size()] = { 0.0 }; //all the elements are zero

#pragma omp parallel for num_threads(1 << factor) reduction(+:totalSumsX, totalSumsY, totalSumsZ)
  for (int i = 0; i < particlesPerThread[omp_get_thread_num()]->size(); ++i) {
    Particle* particle = particlesPerThread[omp_get_thread_num()]->at(i);

    // only active particle can contribute
    if (!particle->getActive()) { continue; }

    // get nodes and weights that the particle contributes
    const vector<Contribution>* contribution = particle->getContributionNodes();

    // get the particle mass
    const double pMass = particle->getMass();

    // get particle velocity
    const Vector3d pVelocity = particle->getVelocity();

    // for each node in the contribution list 
    for (size_t j = 0; j < contribution->size(); ++j) {

      // get the contributing node
      Node* nodeI = nodes->at(contribution->at(j).getNodeId());

      if (nodeI->getThreadId() != -2) {
        nodeI->addMomentum(pMass * pVelocity * contribution->at(j).getWeight());
        continue;
      }

      //an interface node go to totalSum array
      totalSumsX[nodeI->getId()] += pMass * pVelocity.x() * contribution->at(j).getWeight();
      totalSumsY[nodeI->getId()] += pMass * pVelocity.y() * contribution->at(j).getWeight();
      totalSumsZ[nodeI->getId()] += pMass * pVelocity.z() * contribution->at(j).getWeight();

    }
  }

  //unification of sums
  for (int i = 0; i < sizeof(totalSumsX); i++)
  {
    Vector3d result = Vector3d(totalSumsX[i], totalSumsY[i], totalSumsZ[i]);
    nodes->at(i)->addMomentum(result);
  }
}