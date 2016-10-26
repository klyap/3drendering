#ifndef SHADED_H
#define SHADED_H

#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <map>
#include <math.h>
#include <cassert>
#include <float.h>

#include "../Eigen/Dense"
#include <GL/glew.h>
#include <GL/glut.h>


using namespace Eigen;
using namespace std;

struct Vertex {
  float x;
  float y;
  float z;
};

struct Face {
  int x;
  int y;
  int z;
};

struct Color {
  float r;
  float g;
  float b;
};

struct Light {
  float x;
  float y;
  float z;
  Color rgb;
  float attenuation;
};

struct Material {
  Color ambient; // ambient material reflectance
  Color diffuse; // diffuse material reflectance
  Color specular; // specular material reflectance
  float shininess; // Phong exponent for material shininess
};

struct GeoTransform {
  string type; // "t" | "r" | "s"

  // Has size 4 for rotate and 3 for the rest
  // params[3] = angle param for rotate
  vector<float> params;
};

/*struct GeoTransform {
  vector<MatrixXd> t; // Translate
  vector<MatrixXd> r; // Rotate
  vector<MatrixXd> s; // Scale
  vector<Transform> all; // Transform objs
  MatrixXd SR;
  MatrixXd all;
};*/

struct Obj {
  char * filename;
  std::vector<Vertex> v;
  std::vector <Face> f;
  std::vector<Vertex> vn;
  std::vector <Face> fn;
  Material material;
  vector<GeoTransform> geo_transform;
};


struct Camera {
  vector<int> position;
  vector<int> orientation;
  map<string, float> perspective;
};


// GLOBAL VARIABLES
string directory = "data/";
int xres = 500; // x res
int yres = 500; // y res
// TEMP
string textfile = "data/scene_cube1.txt";
/*clock_t begin, end; // for time debug
float time_spent;*/

// Holds object data for each .obj file
map <string, Obj> original;
// Holds transformed shape data of each object instance
map <string, vector<GeoTransform> > transformed;
// Holds material properties for shading of each object instance
map <string, vector<Material> > materials;
// Holds all object instances
vector<Obj> instances;
// Holds list of lights
vector<Light> lights;
// Holds camera transform params
Camera cam;


#endif