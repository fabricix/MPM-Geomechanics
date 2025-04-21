#include "BoundingBox.h"

BoundingBox::BoundingBox(){}

BoundingBox::BoundingBox(Vector3d minPoint, Vector3d maxPoint, unsigned subdomainsNumber){

    this->maxPoint = maxPoint;
    this->minPoint = minPoint;

    this->size = maxPoint - minPoint;

    //this->subdomains.resize(subdomainsNumber);
    this->subdomains.resize(2);

}

// Definición del destructor (si es necesario)
BoundingBox::~BoundingBox() {
    // Si necesitas liberar recursos o hacer algo antes de que el objeto sea destruido, hazlo aquí.
}
