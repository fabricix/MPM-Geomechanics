// ----- params -----
SetFactory("OpenCASCADE");

// block
lc = 1.0; // global mesh size
Lx = 2; Ly = 2; Lz = 4; // block dimension

// cylinder geometry
xc = Lx/2; yc = Ly/2; zc = 0; // center
rz = 0.25; // radii
hz = Lz; //height

// create volumes
Box(1) = {0, 0, 0, Lx, Ly, Lz};      
Cylinder(2) = {xc, yc, zc, 0, 0, hz, rz};

// fragment the volume. two volumes with common interface.
frag() = BooleanFragments{ Volume{1}; Delete; }{ Volume{2}; Delete; };

// adding physical tags
Physical Volume("Soil")    = {frag(1)};
Physical Volume("Geotextil") = {frag(0)};
Physical Surface("Force", 28) = {12, 8};

// meshing (control global length)
Mesh.CharacteristicLengthMax = lc;//+

//+
Show "*";
