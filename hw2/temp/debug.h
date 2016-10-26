
#ifndef DEBUG_H
#define DEBUG_H

#include "shaded_renderer.h"

void debug_base_objs(map<string, Obj> original);

void debug_transformed(Obj obj, map<string, Obj> original, map<string, 
  vector<MatrixXd>> transformed);

void debug_camera(Camera cam);
void debug_color(Color c);

void debug_lights(vector<Light> lights);

void debug_material(Material m);

void debug_obj(Obj obj);

Obj parse_obj(Obj &obj, char * filename);

#endif