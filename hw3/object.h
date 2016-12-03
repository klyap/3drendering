#ifndef OBJECT_H
#define OBJECT_H

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

typedef struct geoTransform {
  string type; // "t" | "r" | "s"

  // Has size 4 for rotate and 3 for the rest
  // params[3] = angle param for rotate
  vector<float> params;
} GeoTransform;


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
extern string directory;
extern int xres; // x res
extern int yres; // y res
// TEMP
extern string textfile;
/*clock_t begin, end; // for time debug
float time_spent;*/

// Holds object data for each .obj file
extern map <string, Obj> original;
// Holds transformed shape data of each object instance
//extern map <string, vector<GeoTransform> > transformed;
// Holds material properties for shading of each object instance
//map <string, vector<Material> > materials;

// Holds all object instances
extern vector<Obj> instances;
// Holds list of lights
extern vector<Light> lights;
// Holds camera transform params
extern Camera cam;

// Implementation
extern string light_text;
extern string originals_text;
extern string instance_text;

// For Arc-Ball
// Initialize to identity
extern Vector4d last_rotation;
extern Vector4d current_rotation;
extern int px_start, py_start, px_current, py_current;

// For mouse
extern int mouse_x, mouse_y;
extern float mouse_scale_x, mouse_scale_y;

extern const float step_size;
extern const float x_view_step, y_view_step;
extern float x_view_angle, y_view_angle;

extern bool is_pressed;
extern bool wireframe_mode;

// END GLOBAL


// Functions
void debug_base_objs(map<string, Obj> original);

void debug_vertex(Vertex v);

void debug_camera(Camera cam);

void debug_color(Color c);

void debug_lights(vector<Light> lights);

void debug_transform(GeoTransform t);

void debug_material(Material m);

void debug_obj(Obj obj);

void create_lights(string light_text);

void create_camera(string cam_text);

void parse_points(Obj &obj, char * filename);

void parse_transform(istringstream &infile, vector<GeoTransform> &all);

void create_originals(string originals_text);

void create_instances(string instance_text);

void parse(string file);

#endif