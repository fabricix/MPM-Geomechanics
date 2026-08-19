
// Define the mesh size
mesh_size = 1;

// Define the points
Point(1) = {0, 0, 2, mesh_size};
Point(2) = {32, 0, 2, mesh_size};
//+
Line(1) = {1, 2};
//+
Extrude {0, 14, 0} {
  Curve{1}; 
}
