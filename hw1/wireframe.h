//#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <map>
#include <math.h>
#include <cassert>

#include "../Eigen/Dense"

using namespace Eigen;
using namespace std;

string directory = "data/";
int x_dim = 100;
int y_dim = 100;


typedef struct vertex {
  double x;
  double y;
  double z;
} Vertex;

typedef struct face {
  int x;
  int y;
  int z;
} Face;

typedef struct obj {
  char * filename;
  std::vector<Vertex> v;
  std::vector <Face> f;
} Obj;

typedef struct camera {
  vector<int> position;
  vector<int> orientation;
  map<string, double> perspective;
} Camera;