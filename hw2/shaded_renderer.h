#ifndef SHADED_H
#define SHADED_H

//#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <map>
#include <math.h>
#include <cassert>
#include <float.h>

#include "../Eigen/Dense"

using namespace Eigen;
using namespace std;

string directory = "data/";
int x_dim; // x res
int y_dim; // y res

clock_t begin, end; // for time debug
double time_spent;

struct Vertex {
  double x;
  double y;
  double z;
};

struct Face {
  int x;
  int y;
  int z;
};

struct Color {
  double r;
  double g;
  double b;
};

struct Light {
  double x;
  double y;
  double z;
  Color rgb;
  double attenuation;
};

struct Material {
  Color ambient; // ambient material reflectance
  Color diffuse; // diffuse material reflectance
  Color specular; // specular material reflectance
  double shininess; // Phong exponent for material shininess
};

struct GeoTransform {
  vector<MatrixXd> t; // Translate
  vector<MatrixXd> r; // Rotate
  vector<MatrixXd> s; // Scale
  MatrixXd SR;
  MatrixXd all;
};

struct Obj {
  char * filename;
  std::vector<Vertex> v;
  std::vector <Face> f;
  std::vector<Vertex> vn;
  std::vector <Face> fn;
  Material material;
  GeoTransform geo_transform;
};

struct Camera {
  vector<int> position;
  vector<int> orientation;
  map<string, double> perspective;
};

#endif