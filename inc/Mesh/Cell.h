
#ifndef CELL_H_
#define CELL_H_

#include <vector>

#include <Eigen/Eigenvalues>
using namespace Eigen;

#include "Node.h"

class Cell {

    public:
        std::vector<Node*> nodes;
        double volume;
    
        Cell() : volume(0.0) {}
    
        void computeVolume() {

            if (nodes.size() != 8) {
                volume = 0.0;
                return;
            }
        
            // base vectors of the parallelepiped 
            Vector3d v1 = nodes[1]->getCoordinates() - nodes[0]->getCoordinates(); // X direction
            Vector3d v2 = nodes[2]->getCoordinates() - nodes[0]->getCoordinates(); // Y direction
            Vector3d v3 = nodes[4]->getCoordinates() - nodes[0]->getCoordinates(); // Z direction
        
            // cross product of two base vectors
            volume = std::abs(v1.cross(v2).dot(v3));
    }
};

#endif /* CELL_H_ */