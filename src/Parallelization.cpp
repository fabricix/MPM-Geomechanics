#include "Parallelization.h"
#include <iostream>
#include "Particle.h"
#include "Node.h"
#include "Update.h"

vector<Particle*> particlesPerThreadAux;

void Parallelization::calculateParticlesPerThread(vector<vector<Particle*>*>& particlesPerThread, vector<Particle*>& particles, int factor, Mesh* mesh)
{
  vector<Node*>* nodes = mesh->getNodes();
  Update::contributionNodesWithParticles(mesh, &particles);

  float minX = -1.0;
  float maxX = 0.0;
  for (size_t i = 0; i < particles.size(); i++) {
    if (particles.at(i)->getPosition().x() < minX || minX == -1) {
      minX = particles.at(i)->getPosition().x();
    }
    if (particles.at(i)->getPosition().x() > maxX) {
      maxX = particles.at(i)->getPosition().x();
    }
  }
  double fontier = (maxX - minX) / factor;
  std::cout << fontier << std::endl;

  vector<Contribution>* contribution;
  Node* nodeI;

  int count = 0;
  int count2 = 0;

  for (size_t i = 0; i < particles.size(); i++)
  {
    contribution = particles.at(i)->getContributionNodes();
    if (particles.at(i)->getPosition().x() > fontier) {
      count++;
      particlesPerThread.at(0)->push_back(particles.at(i));
      particles.at(i)->threadId = 0;
      for (size_t j = 0; j < contribution->size(); ++j) {
        nodeI = nodes->at(contribution->at(j).getNodeId());
        nodeI->threadId = 0;
      }
    }
    else {
      count2++;
      particlesPerThreadAux.push_back(particles.at(i));
      for (size_t j = 0; j < contribution->size(); ++j) {
        nodeI = nodes->at(contribution->at(j).getNodeId());
        nodeI->threadId = 1;
      }
    }
    particles.at(i)->isInterface = false;
  }

  std::cout << count << std::endl;
  std::cout << count2 << std::endl;

  for (size_t i = 0; i < particlesPerThreadAux.size(); i++)
  {
    bool isInterface = false;
    contribution = particlesPerThreadAux.at(i)->getContributionNodes();
    for (size_t j = 0; j < contribution->size(); ++j) {

      nodeI = nodes->at(contribution->at(j).getNodeId());
      if (nodeI->threadId == 0 || nodeI->threadId == 2)
      {
        isInterface = true;
        nodeI->threadId = 2;
        break;
      }
      else
      {
        nodeI->threadId = 1;
      }
    }
    if (isInterface)
    {
      particlesPerThread.at(2)->push_back(particlesPerThreadAux.at(i));
      particlesPerThread.at(2)->at(particlesPerThread.at(2)->size() - 1)->isInterface = true;
      particlesPerThread.at(2)->at(particlesPerThread.at(2)->size() - 1)->threadId = 2;
    }
    else {
      particlesPerThread.at(1)->push_back(particlesPerThreadAux.at(i));
      particlesPerThread.at(1)->at(particlesPerThread.at(1)->size() - 1)->isInterface = false;
      particlesPerThread.at(1)->at(particlesPerThread.at(1)->size() - 1)->threadId = 1;
    }
  }

  if (particlesPerThread.size() > 1) {
    size_t sizeThread1 = particlesPerThread.at(0)->size();
    size_t sizeThread2 = particlesPerThread.at(1)->size();
    size_t sizeThread3 = particlesPerThread.at(2)->size();

    std::cout << "CPU 1 size: " << sizeThread1 << std::endl;
    std::cout << "CPU 2 size: " << sizeThread2 << std::endl;
    std::cout << "Interface size: " << sizeThread3 << std::endl;
  }
  else {
    std::cerr << "Error: particlesPerThread no tiene suficientes sub-vectores." << std::endl;
  }
}

void Parallelization::calculateInterfaceParticles(vector<vector<Particle*>*>& particlesPerThread, vector<Particle*>& particles, Mesh* mesh)
{
  vector<Node*>* nodes = mesh->getNodes();
  vector<Contribution>* contribution;
  Node* nodeI;

  particlesPerThread.at(1)->clear();
  particlesPerThread.at(2)->clear();

  for (size_t i = 0; i < particlesPerThreadAux.size(); i++)
  {
    bool isInterface = false;
    contribution = particlesPerThreadAux.at(i)->getContributionNodes();
    for (size_t j = 0; j < contribution->size(); ++j) {

      nodeI = nodes->at(contribution->at(j).getNodeId());
      if (nodeI->threadId == 0 || nodeI->threadId == 2)
      {
        isInterface = true;
        nodeI->threadId = 2;
        break;
      }
      else
      {
        nodeI->threadId = 1;
      }
    }
    if (isInterface)
    {
      particlesPerThread.at(2)->push_back(particlesPerThreadAux.at(i));
      particlesPerThread.at(2)->at(particlesPerThread.at(2)->size() - 1)->isInterface = true;
      particlesPerThread.at(2)->at(particlesPerThread.at(2)->size() - 1)->threadId = 2;
    }
    else {
      particlesPerThread.at(1)->push_back(particlesPerThreadAux.at(i));
      particlesPerThread.at(1)->at(particlesPerThread.at(1)->size() - 1)->isInterface = false;
      particlesPerThread.at(1)->at(particlesPerThread.at(1)->size() - 1)->threadId = 1;
    }
  }
}
