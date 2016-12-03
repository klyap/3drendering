#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <map>
#include <math.h>
#include <cassert>
#include <float.h>

#include "../../Eigen/Dense"
#include <GL/glew.h>
#include <GL/glut.h>

#define _USE_MATH_DEFINES

using namespace Eigen;
using namespace std;


typedef struct geoTransform {
  string type; // "t" | "r" | "s"

  // Has size 4 for rotate and 3 for the rest
  // params[3] = angle param for rotate
  vector<float> params;
} GeoTransform;

struct Camera {
  vector<int> position;
  vector<int> orientation;
  map<string, float> perspective;
};

/* Global variables :> */
extern string file;
extern int total_frames;
extern Camera cam; // Holds hard coded camera transform params
extern string instance_text;
// Stores keyframe number and the corresponding transform
extern map<int, vector<GeoTransform>> keyframes;
extern vector<int> keyframe_indices;
extern int total_keyframes;
extern int p1, p2, p3, p4; // pi-1, pi, pi+1, pi+2

/* Functions */
void debug_transform(GeoTransform t);

void debug_camera(Camera cam);

void create_camera();

void parse_transform(istringstream &infile, vector<GeoTransform> &all);

void create_instances(); // uses instance_text

void parse(string file);

#endif
