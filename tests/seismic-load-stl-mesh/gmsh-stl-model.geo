//+
Point(1) = {0, 0, 10, 1.0};
//+
Point(2) = {5, 0, 5, 1.0};
//+
Point(3) = {15, 0, 5, 1.0};
//+
Point(4) = {20, 0, 0, 1.0};
//+
Line(1) = {1, 2};
//+
Line(2) = {2, 3};
//+
Line(3) = {3, 4};
//+
Extrude {0,10, 0} {
  Curve{1}; Curve{2}; Curve{3}; 
}
