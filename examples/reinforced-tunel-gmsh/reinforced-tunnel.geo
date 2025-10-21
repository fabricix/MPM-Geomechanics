// reinforced-tunnel.geo
// Gmsh geometry file for a reinforced tunnel in soil
SetFactory("OpenCASCADE");

Mesh.CharacteristicLengthMin = 0.1;
Mesh.CharacteristicLengthMax = 0.15;

// parameters
D = 1;
lc = 0.1;
angle = 1.0*Pi;
xc=0;
yc=0;
zc=1.5*D;
dxc=0.0;
dyc=6.0*D-3.5*D;
dzc=0.0;
rc = D/2.0;

// soil
Box(1) = {0, 0, 0, 2.5*D, 6.0*D, 3.5*D};
Cylinder(2) = {0, 0, 1.5*D, 0, dyc, 0, D/2.0, Pi};
BooleanDifference{ Volume{1}; Delete; }{ Volume{2}; Delete; }
Physical Volume("Soil") = {1};

// reinforced
Cylinder(3) = {xc, yc, zc, dxc, dyc, dzc, rc, angle};
Cylinder(4) = {xc, yc, zc, dxc, dyc, dzc, rc-lc, angle};
BooleanDifference{ Volume{3}; Delete; }{ Volume{4}; Delete; }
Physical Volume("Reinforce") = {3};