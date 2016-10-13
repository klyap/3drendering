//#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <map>
#include <math.h>

#include "../Eigen/Dense"

using namespace Eigen;
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

Obj parse_obj(char * filename){
  
  // Set up structs
  std::vector<Vertex> vertices;
  Vertex v;
  vertices.push_back(v); // For 1 index

  std::vector<Face> faces;
  Obj obj;
  obj.filename = filename;

  // READ IN FILE
  ifstream infile(filename);
  char type;
  double x, y, z;
  while (infile >> type >> x >> y >> z)
  {
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
 /* cout << "\n Object create! " << obj.filename << ":" << endl;
  for (int i = 1; i < obj.v.size(); i++){
    // Print out all vertices
    // 1 indexed so start loop at 1, where 0th element is NULL
    cout << "v is " << obj.v[i].x << " " << obj.v[i].y << " " << obj.v[i].z << endl;
  }
  for (int i = 0; i < obj.f.size(); i++){
    // Print out all faces
    cout << "f is " << obj.f[i].x << " " << obj.f[i].y << " " << obj.f[i].z << endl;
  }*/

  return obj;

}

MatrixXd parse_transform(char * filename){

  // READ IN FILE
  istringstream infile(filename); // Assume filename = string of filedata
  char type;
  double x, y, z, a;
  vector<MatrixXd> all_transforms;
  while (infile >> type)
  {    
    if (type == 't'){
      // TRANSLATE MATRIX
      infile >> x >> y >> z;

      MatrixXd m(4,4); // m is constructed as a 4x4 matrix  
      m << 1, 0, 0, x, // row1  
           0, 1, 0, y, // row2  
           0, 0, 1, z, // row3  
           0, 0, 0, 1; // row4

      all_transforms.push_back(m);

    } else if (type == 'r'){
      // ROTATE MATRIX
      infile >> x >> y >> z >> a;

      double a1 = x * x + (1 - x * x) * cos(a);
      double a2 = x * y * (1 - cos(a)) - z * sin(a);
      double a3 = x * z * (1 - cos(a)) + y * sin(a);
      double b1 = y * x * (1 - cos(a)) + z * sin(a);
      double b2 = y * y + (1 - y * y) * cos(a);
      double b3 = y * z * (1 - cos(a)) - x * sin(a);
      double c1 = z * x * (1 - cos(a)) - y * sin(a);
      double c2 = z * y * (1 - cos(a)) + x * sin(a);
      double c3 = z * z + (1 - z * z) * cos(a);

      MatrixXd m(4,4); // m is constructed as a 4x4 matrix  
      m << a1, a2, a3, 0, // row1  
           b1, b2, b3, 0, // row2  
           c1, c2, c3, 0, // row3  
           0, 0, 0, 1; // row4

      all_transforms.push_back(m);

    } else if (type == 's'){
      // SCALE MATRIX
      infile >> x >> y >> z;
      MatrixXd m(4,4); // m is constructed as a 4x4 matrix  
      m << x, 0, 0, 0, // row1  
           0, y, 0, 0, // row2  
           0, 0, z, 0, // row3  
           0, 0, 0, 1; // row4

      all_transforms.push_back(m);
    }
  }

  // COMBINE MATRICES
  MatrixXd m = all_transforms.back();
  all_transforms.pop_back();

  while (all_transforms.size() > 0){
    m *= all_transforms.back();
    all_transforms.pop_back();
  }

  /*MatrixXd m_inv = m.inverse();
  return m_inv;*/
  return m;
}


int main (int argc, char* argv[])
{ 

  // Holds object data for each .obj file
  map <string, Obj> original;
  // Holds transformed data of each object instance
  map <string, vector<MatrixXd> > transformed;

  string line;
  ifstream infile(argv[1]);

  //////////////////////////////
  // Generate map associating name with object
  ///////////////////////////////
  while (getline(infile, line) && !line.empty()){
    // Assume format: "objectname objectfile.obj"
    istringstream line_stream(line);
    string objectname;
    char objectfile[500];
    line_stream >> objectname >> objectfile;
    
    Obj obj = parse_obj(objectfile);
    original[objectname] = obj;
    
  }

  // DEBUG
  /*typedef map<char *, Obj >::iterator it_type1;
  for(it_type1 it = original.begin(); it != original.end(); it++) {
    cout << "1 original key: " << it->first << endl;
    cout << "1 original obj " << it->second.v.size() << endl;
  }*/
  // DEBUG

  ///////////////////////////////
  // Generate map associating object with list of transforms
  ///////////////////////////////
  string objectname;
  char transform_data[1000];
  while (getline(infile, line)){
    //cout << "LINE 2: " << line << endl;

    // Assume format:
    // objectname
    // t x y z
    //
    // objectname
    // t x y z

    //if (strlen(objectname) == 0){
    if (objectname.size() == 0){
      // Starting new object transform
      ///strcpy(objectname,line.c_str());
      objectname = line;
    } else if (!line.empty()){
      // Collect transform data
      strcat(transform_data, line.c_str());
    } else {
      // Got to empty line
      // Execute transform
      MatrixXd m = parse_transform(transform_data);
      //cout << "Transform: " << m << endl;
      // Store transform matrix
      if (transformed.count(objectname)){
        transformed[objectname].push_back(m);
      } else {
        vector<MatrixXd> tmp;
        tmp.push_back(m);
        transformed[objectname] = tmp;
      }

      //strcpy(objectname, "");
      objectname = "";
      strcpy(transform_data, "");
    }

  }

  // Execute transform of last thing
  MatrixXd m = parse_transform(transform_data);

  // Store transform matrix
  if (transformed.count(objectname)){
    transformed[objectname].push_back(m);
    //cout << "FINAL TRANSFORM COUNT: " << transformed[objectname].size() << endl;
  } else {
    vector<MatrixXd> tmp;
    tmp.push_back(m);
    transformed[objectname] = tmp;
  }


  // DEBUG
/*  typedef map<char *, vector<MatrixXd> >::iterator it_type;
  for(it_type it = transformed.begin(); it != transformed.end(); it++){
    cout << it->first << endl;
    vector<MatrixXd> matrices = it->second;
    for (int i = 0; i < matrices.size(); i++){
      // For each transform instance
      Obj obj = original[it->first];
      cout << "obj: " << obj.filename << endl;
      for (int vertex = 0; vertex < obj.v.size(); vertex++){
        Vertex this_vertex = obj.v[vertex];
        MatrixXd v(4,1); 
        cout << "Transform: " << matrices[i] << endl;
        v << this_vertex.x, this_vertex.y, this_vertex.z, 1;
        cout << matrices[i] * v << endl;
      }
    }
  }*/

  

  //////////////////////////////
  // Iterate over transforms and print out transformed instances
  ///////////////////////////////
  

  typedef map<string, vector<MatrixXd> >::iterator it_type;
  for(it_type it = transformed.begin(); it != transformed.end(); it++) {
    string name = it->first;
    vector<MatrixXd> matrices = it->second;

    for (int i = 0; i < matrices.size(); i++){
      // For each transform instance
      cout << name << "_" << i << endl;
      Obj obj = original[name];
      
      // DEBUG
      /*typedef map<char *, Obj >::iterator it_type1;
      for(it_type1 it2 = original.begin(); it2 != original.end(); it2++) {
        cout << "original key>>" << it2->first << "<<" << endl;
        cout << "original obj " << it2->second.v.size() << endl;
        string name1 = it2->first;
        //if (strcmp(it2->first, name)){
        if (name1 == name){
          cout << "SAME: " << it2->first << " = " << name << endl;
        }
      }*/
      // DEBUG

      for (int vertex = 1; vertex < obj.v.size(); vertex++){
        Vertex this_vertex = obj.v[vertex];
        MatrixXd v(4,1); 
        v << this_vertex.x, this_vertex.y, this_vertex.z, 1;
        MatrixXd t = (matrices[i] * v);
        cout << t.row(0) << " " << t.row(1) << " " << t.row(2) << endl;
      }
    }
    
  } 
  return 0;
}