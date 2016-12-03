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

#include "../../../Eigen/Dense"

using namespace Eigen;
using namespace std;

typedef struct vertexlist {
  vector<float> xlist;
  vector<float> ylist;
  vector<float> zlist;
} VertexList;

/* Global vars */
extern int total_keyframes;
extern int p1, p2, p3, p4; // pi-1, pi, pi+1, pi+2

/* Functions */
float get_u(int current_frame);

float catmull_rom(float u, Vector4d p);

void update(string filename, int current_frame);

string make_filename(int current_frame);

void parse_keyframes();

#endif
