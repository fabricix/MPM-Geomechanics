#include "Parallelization.h"
#include <iostream>
#include "Particle.h"
#include "Node.h"
#include "Update.h"

// Copy of the particles per thread
vector<vector<Particle>>* particlesPerThreadAux;

// Numbers of threads
int NumberOfThreads = 0;

void Parallelization::calculateParticlesPerThread(Mesh* mesh, vector<vector<Particle*>*>& particlesPerThread, vector<Particle*>& particles, int factor)
{
  // numbers of thread is 2^factor
  NumberOfThreads = 1 << factor;

  // Initialize the particles per thread vector
  particlesPerThreadAux = new vector<vector<Particle>>(NumberOfThreads);

  // the real particles per thread vector is initialized. One more is added to store the interface particles
  particlesPerThread.resize(NumberOfThreads + 1);

  // Initialize the particles per thread vector
  for (int i = 0; i < NumberOfThreads; i++)
  {
    particlesPerThreadAux->at(i) = *(new vector<Particle>());
    particlesPerThread.at(i) = new vector<Particle*>();
  }

  // the last vector is used to store the interface particles
  particlesPerThread.at(NumberOfThreads) = new vector<Particle*>();

  // minX represents the minimum x position of the particles
  float minX = -1.0;

  // maxX represents the maximum x position of the particles
  float maxX = 0.0;

  // obtain the minimum and maximum x position of the particles
  for (size_t i = 0; i < particles.size(); i++) {
    if (particles.at(i)->getPosition().x() < minX || minX == -1) {
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
    threadId = (int)particles.at(i)->getPosition().x() / fontier;

    // set the thread id of the particle
    particles.at(i)->threadId = threadId;

    // set the original thread id of the particle
    particles.at(i)->originalThreadId = threadId;

    // add the particles to the particles per thread vector
    particlesPerThreadAux->at(threadId).push_back(*particles.at(i));

    // the particles per thread aux vector is a copy, then the real particle is added to the particles per thread vector
    particlesPerThreadAux->at(threadId).at(particlesPerThreadAux->at(threadId).size() - 1).real = particles.at(i);
  }

  // show the number of particles per thread
  for (size_t i = 0; i < particlesPerThreadAux->size(); i++)
  {
    std::cout << "Thread " << i << " size: " << particlesPerThreadAux->at(i).size() << std::endl;
  }
}

void Parallelization::calculateInterfaceParticles(Mesh* mesh, vector<vector<Particle*>*>& particlesPerThread)
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
  for (int t = 0; t < particlesPerThreadAux->size(); t++)
  {

    // Iterate through the particles of a thread
    for (int p = 0; p < particlesPerThreadAux->at(t).size(); p++)
    {

      // Contribution nodes of the particle
      contribution = particlesPerThreadAux->at(t).at(p).real->getContributionNodes();

      // Iterate through the contribution nodes of the particle
      for (size_t j = 0; j < contribution->size(); ++j) {

        // Node corresponding to the particle
        nodeI = nodes->at(contribution->at(j).getNodeId());

        // Determine if the node's thread corresponds to the same thread as the particle. If it is equal to -1, then it is a node without a thread.
        if (nodeI->threadId == -1 || nodeI->threadId == particlesPerThreadAux->at(t).at(p).real->originalThreadId)
        {
          // Assign the particle's thread to the node
          nodeI->threadId = t;
        }
        else
        {
          // The node has a thread different from the particle's thread, so it is considered an interface
          nodeI->threadId = -2;

          // The particle is an interface particle
          isInterface = true;
        }
      }
      if (isInterface)
      {
        // Interface particles are stored in thread -2
        particlesPerThreadAux->at(t).at(p).real->threadId = -2;

        // Add to the list of **original** interface particles
        particlesPerThread.at(NumberOfThreads)->push_back(particlesPerThreadAux->at(t).at(p).real);
      }
      else {
        // Identify the thread of the particle
        particlesPerThreadAux->at(t).at(p).real->threadId = t;

        // Add to the list of **original** particles corresponding to the thread
        particlesPerThread.at(t)->push_back(particlesPerThreadAux->at(t).at(p).real);
      }
      isInterface = false;
    }
  }
}