//#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>

using namespace std;

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

void parse_obj(char * filename){
  
  // Set up structs
  std::vector<Vertex> vertices;
  std::vector<Face> faces;
  Obj obj;
  obj.filename = filename;

  // READ IN FILE
  
  ifstream infile(filename);
  char type;
  double x, y, z;
  while (infile >> type >> x >> y >> z)
  {
    //cout << type << " " << x << " " << y << 
    if (type == 'v'){
      Vertex v;
      v.x = x;
      v.y = y;
      v.z = z;
      vertices.push_back(v);

    } else if (type == 'f'){
      Face f;
      f.x = (int) x;
      f.y = (int) y;
      f.z = (int) z;
      faces.push_back(f);
    }

    obj.v = vertices;
    obj.f = faces;
  }


  // PRINT RESULTS
  cout << "\n" << obj.filename << ":" << endl;
  for (int i = 1; i < obj.v.size(); i++){
    // Print out all vertices
    // 1 indexed so start loop at 1, where 0th element is NULL
    cout << "v " << obj.v[i].x << " " << obj.v[i].y << " " << obj.v[i].z << endl;
  }
  for (int i = 0; i < obj.f.size(); i++){
    // Print out all faces
    cout << "f " << obj.f[i].x << " " << obj.f[i].y << " " << obj.f[i].z << endl;
  }

}


int main (int argc, char* argv[])
{
  char name[500];
  for (int i = 1; i < argc; i++){
    // Each arg is a .obj file
    parse_obj(argv[i]);
  }
  
  return 0;
}