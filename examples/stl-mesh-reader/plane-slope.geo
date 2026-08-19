
// Define the mesh size
mesh_size = 1.0;

// Define the geometrical parameters
h=10;
b=5;
l=10;

// Define the points
Point(1) = {0, 0, h, mesh_size};
Point(2) = {l, 0, 0, mesh_size};
//+
Line(1) = {1, 2};
//+
Extrude {0, b, 0} {
  Curve{1}; 
}
