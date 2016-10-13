#include "shaded_renderer.h"

Obj parse_obj(char * filename){
  
  // Set up structs
  std::vector<Vertex> vertices;
  Vertex v;
  vertices.push_back(v); // For 1 index

  std::vector<Face> faces;
  Obj obj;
  obj.filename = filename;

  // READ IN FILE
  ifstream infile(directory + (string)filename);
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
  /*cout << "\nObject create! " << obj.filename << ":" << endl;
  for (int i = 1; i < obj.v.size(); i++){
    // Print out all vertices
    // 1 indexed so start loop at 1, where 0th element is NULL
    cout << "v is " << obj.v[i].x << " " << obj.v[i].y << " " << obj.v[i].z << endl;
  }
  for (int i = 0; i < obj.f.size(); i++){
    // Print out all faces
    cout << "f is " << obj.f[i].x << " " << obj.f[i].y << " " << obj.f[i].z << endl;
  }*/
  // END PRINT
  return obj;

}

MatrixXd make_translate_matrix(
  double x, double y, double z){

  MatrixXd m(4,4); // m is constructed as a 4x4 matrix  
      m << 1, 0, 0, x, // row1  
           0, 1, 0, y, // row2  
           0, 0, 1, z, // row3  
           0, 0, 0, 1; // row4
  // DEBUG
  /*cout << "Translation: \n";
  cout << "xyz: " << x << y << z << endl;
  cout << m << endl;*/
  // DEBUG
  return m;
}

MatrixXd make_rotation_matrix(
    double x, double y, double z, double a){

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

  // DEBUG
  /*cout << "Rotation: \n";
  cout << "xyza: " << x << y << z << a << endl;
  cout << m << endl;*/
  // DEBUG
  return m;
}

MatrixXd make_projection_matrix(map<string, double> perspective){
 double n = perspective["near"];
 double f = perspective["far"];
 double r = perspective["right"];
 double l = perspective["left"];
 double t = perspective["top"];
 double b = perspective["bottom"];

 double a1 = (2 * n) / (r - l);
 //cout << a1 << endl;
 double a3 = (r + l) / (r - l);
 //cout << a3 << endl;
 double b2 = (2 * n) / (t - b);
 //cout << b2 << endl;
 double b3 = (t + b) / (t - b);
 //cout << b3 << endl;
 double c3 = -1 * (f + n) / (f-n);
 //cout << c3 << endl;
 double c4 = -2 * (f * n) / (f-n);
 //cout << c4 << endl;
 
 //cout << (double) a1  << endl;
 //<<  a3 << b2 << b3 << c3 << c4 << endl;
 MatrixXd m(4,4); // m is constructed as a 4x4 matrix  
  m << a1, 0, a3, 0, // row1  
       0, b2, b3, 0, // row2  
       0, 0, c3, c4, // row3  
       0, 0, -1, 0;  // row4

  return m;
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

      /*MatrixXd m(4,4); // m is constructed as a 4x4 matrix  
      m << 1, 0, 0, x, // row1  
           0, 1, 0, y, // row2  
           0, 0, 1, z, // row3  
           0, 0, 0, 1; // row4*/

      MatrixXd m = make_translate_matrix(x, y, z);

      all_transforms.push_back(m);

    } else if (type == 'r'){
      // ROTATE MATRIX
      infile >> x >> y >> z >> a;

      /*double a1 = x * x + (1 - x * x) * cos(a);
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
           0, 0, 0, 1; // row4*/

      MatrixXd m = make_rotation_matrix(x, y, z, a);

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

void swap(int &a, int &b) {
  int tmp = a;
  a = b;
  b = tmp;
}

void rasterize(
  int * grid, int x_dim, int y_dim, Vertex v1, Vertex v2){
  // Assume range of frustrum = [-1, 1] st v1 lies in this range
  // and range of resolution = [0, dim]

  int x1 = (v1.x + 1)/2.0 * (x_dim);
  int y1 = (v1.y + 1)/2.0 * (y_dim);
  int x2 = (v2.x + 1)/2.0 * (x_dim);
  int y2 = (v2.y + 1)/2.0 * (y_dim);
  
  double dx = x2 - x1;
  double dy = y2 - y1;
  double m;

  int color = 999;

  if (dx == 0){
    // If m = infinity, bucket it with Octant 2/7
    m = dy / 0.000000001;
  } else {
    m = dy/dx;
  }

  double e = 0;
  

  if (m >= 0 && m <= 1){

    // Octant 1: Positive slope + horizonal
    color = 1;
    if (x1 >= x2 && y1 >= y2){
      swap(x1, x2);
      swap(y1, y2);
      color = 2;
    }

    /*cout << "v1 = " << x1 << "," << y1<< endl;
    cout << "v2 = " << x2 << "," << y2<< endl;*/

    // dy = abs(dy);
    // dx = abs(dx);
    dy = y2 - y1;
    dx = x2 - x1;
    int y = y1;
    
    for (int x = x1; x < x2; x++){
      if (y_dim - int(y) >= 0 
          && int(x) >= 0 
          && (y_dim - int(y)) < y_dim 
          && int(x) < x_dim){

        grid[(y_dim - int(y)) * x_dim + int(x)] = color;

        if (2 * (e + dy) < dx){
          e = e + dy;
        } else {
          e = e + dy - dx;
          y = y + 1;
        }
      }
    }
  } else if (m >= 1){
    // Octant 2 / 6
  
    // Flip along y = x so Oct 2 is flipped to Oct 1
    // (and Oct 6 to Oct 5)
    swap(x1, y1);
    swap(x2, y2);

    color = 3;
    // Swap vertices if it's in Oct 5
    // so the line now lies in Oct 1
    if (x1 >= x2 && y1 >= y2){
      swap(x1, x2);
      swap(y1, y2);
      color = 4;
    }

    dy = y2 - y1;
    dx = x2 - x1;

    int y = y1;
    
    for (int x = x1; x < x2; x++){

      if (y_dim - int(x) >= 0 
          && int(y) >= 0 
          && y_dim - int(x) < y_dim 
          && int(y) < x_dim){
        // When chosen pixel at (x,y),
        // color in pixel (y,x) to flip it back
        // to the original octant
        grid[(y_dim - int(x)) * x_dim + int(y)] = color;

        if (2 * (e + dy) < dx){
          e = e + dy;
        } else {
          e = e + dy - dx;
          y = y + 1;
        }
      }
    }
  } else if (m < 0 && m >= -1){
    // Octant 4/8

    // Flip along x = 0 from Oct 4 to 1 
    // (or Oct 8 to 5)
    swap(x1, x2);

    color = 5;
    // If this is now in Octant 5, swap vertices
    // so it lies in Octant 1
    if (x1 <= x2 && y1 >= y2){
      swap(x1, x2);
      swap(y1, y2);
      color = 6;
    }

    dy = y2 - y1;
    dx = x2 - x1;
    int y = y1;
    
    for (int x = x1; x < x2; x++){
      // Since slope is drawn downwards,
      // shift the colored pixel up by the vertical distance
      // that the slope traveled (ie dy)
      if (int(y_dim - y - dy) >= 0 
          && int(x) >= 0 
          && int(y_dim - y - dy) < y_dim 
          && int(x) < x_dim){

        grid[int(y_dim - y - dy) * x_dim + int(x)] = color;

        if (2 * (e + dy) < dx){
          e = e + dy;
        } else {
          e = e + dy - dx;
          // Decrement slope instead of incrementing
          y = y - 1;
        }
      }
      
    }
  }else if (m < -1){
    // Octant 3 / 7

    // Flip along x = 0 from Oct 3 to 2
    // (or Oct 7 to 6)
    swap(x1, x2);
    // Flip along y = x from Oct 2 to 1
    // (or Oct 6 to 5)
    swap(x1, y1);
    swap(x2, y2);

    color = 7;

    // If this lies in Octant 5, swap vertices
    // so they both are in Octant 1
    if (x1 >= x2 && y1 >= y2){
      swap(x1, x2);
      swap(y1, y2);
      color = 8;
    }

    dy = y2 - y1;
    dx = x2 - x1;

    int y = y1;
    
    for (int x = x1; x < x2; x++){

      if (int(y_dim - x) >= 0 
          && int(y + dy) >= 0 
          && int(y_dim - x) < y_dim 
          && int(y + dy) < x_dim){

        // Shift upwards to accomodate for
        // slope going down
        // Then swap x and y to flip it back
        // so that its slope goes in the right direction
        grid[int(y_dim - x) * x_dim + int(y + dy)] = color;

        if (2 * (e + dy) < dx){
          e = e + dy;
        } else {
          e = e + dy - dx;
          // Decrement slope
          y = y - 1;
        }
      }
    }
  }
  
  // DEBUG : Mark the vertices only
  //grid[int(y1) * x_dim + int(x1)] = 1;
  //cout << "rasterize: marked v1" << endl;
  //grid[int(y2) * x_dim + int(x2)] = 1;
  //cout << "rasterize: marked v2" << endl;
  // DEBUG

}

void draw_lines(int * grid, int x_dim, int y_dim, Obj o){

  vector<Vertex> vertices = o.v;
  vector<Face> faces = o.f;
  for (int f = 0; f < faces.size(); f++){
    Vertex v1 = vertices[faces[f].x];
    Vertex v2 = vertices[faces[f].y];
    Vertex v3 = vertices[faces[f].z];

    rasterize(grid, x_dim, y_dim, v1, v2);
    rasterize(grid, x_dim, y_dim, v1, v3);
    rasterize(grid, x_dim, y_dim, v2, v3);
  }

  // DEBUG
  /*
  for (int x = 0; x < x_dim * y_dim; x++){
    cout << grid[x] << " ";
  }*/
  // DEBUG
}

void to_ppm(int * grid){
  cout << "P3\n";
  cout << x_dim << " " << y_dim << "\n";
  cout << "255\n";
  for (int x = 0; x < x_dim * y_dim; x++){
    if (grid[x] == 0){
      // Black
     cout << "0 0 0" << endl;
    } else if (grid[x] == 1) {
      // White
      cout << "225 225 225" << endl;
    } else if (grid[x] == 2) {
      cout << "0 225 225" << endl;
    } else if (grid[x] == 3) {
      cout << "225 0 225" << endl;
    } else if (grid[x] == 4) {
      cout << "225 225 0" << endl;
    }else if (grid[x] == 5) {
      cout << "0 0 225" << endl;
    }else if (grid[x] == 6) {
      cout << "225 0 0" << endl;
    }else if (grid[x] == 7) {
      cout << "0 225 0" << endl;
    }else if (grid[x] == 8) {
      cout << "111 111 0" << endl;
    } else {
      cout << "0 111 111" << endl;
    }
  }
}


int main (int argc, char* argv[])
{ 

  // Holds object data for each .obj file
  map <string, Obj> original;
  // Holds transformed data of each object instance
  map <string, vector<MatrixXd> > transformed;
  // Holds camera transform params
  Camera cam;
  // Grid marking whether a pixel is marked or not
  int * grid = new int [x_dim * y_dim]();

  string line;
  ifstream infile(argv[1]);

  //////////////////////////////
  // Absorb camera info
  // Assume format is:
  //
  // camera:  
  //  position 0 0 5  
  //  orientation 0 1 0 0  
  //  near 1  
  //  far 10  
  //  left -1  
  //  right 1  
  //  top 1  
  //  bottom -1  
  ///////////////////////////////
  getline(infile, line);
  assert(line == "camera:");
  while (getline(infile, line) && !line.empty()){
    istringstream line_stream(line);
    string property;
    int x, y, z, a; // a = angle
    double p; // perspective param
    line_stream >> property;
    if (property == "position"){
      line_stream >> x >> y >> z;
      vector<int> new_pos {x, y, z};
      cam.position = new_pos;
    } else if (property == "orientation"){
      line_stream >> x >> y >> z >> a;
      vector<int> new_orientation {x, y, z, a};
      cam.orientation = new_orientation;
    } else {
      line_stream >> p;
      cam.perspective[property] = p;
    }

  }

  // DEBUG
  /*cout << "Camera position: ";
  for (int i = 0; i < 3; i++){
    cout << cam.position[i] << " ";
  }
  cout << endl;

  cout << "Camera orientation: ";
  for (int i = 0; i < 4; i++){
    cout << cam.orientation[i] << " ";
  }
  cout << endl;

  cout << "Camera perspective: " << endl;
  cout << cam.perspective["near"] << " ";
  cout << cam.perspective["far"] << " ";
  cout << cam.perspective["left"] << " ";
  cout << cam.perspective["right"] << " ";
  cout << cam.perspective["top"] << " ";
  cout << cam.perspective["bottom"] << " ";
  cout << endl;*/
  // DEBUG


  //////////////////////////////
  // Generate world coordinates to camera coordinates
  ///////////////////////////////
  
  // Camera position = translation
  MatrixXd cam_pos = make_translate_matrix(
      cam.position[0],cam.position[1],cam.position[2]);
  // Camera orientation = rotation
  MatrixXd cam_orientation = make_rotation_matrix(
      cam.orientation[0],cam.orientation[1],cam.orientation[2],cam.orientation[3]);
  // Perspective projection
  MatrixXd cam_proj = make_projection_matrix(cam.perspective);

  // DEBUG
  /*cout << "Camera matrices:\n" 
  << cam_pos << "\n" << cam_orientation << "\n" << cam_proj << endl;
  */
  // DEBUG

  

  //////////////////////////////
  // Generate map associating name with object
  ///////////////////////////////
  getline(infile, line);
  assert(line == "objects:");
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
  /*typedef map<string, Obj >::iterator it_type1;
  for(it_type1 it = original.begin(); it != original.end(); it++) {
    cout << "Object name: " << it->first << endl;
    cout << "Object data: " << it->second.v.size() << endl;
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

    if (objectname.size() == 0){
      // Starting new object transform
      objectname = line;
    } else if (!line.empty()){
      // Collect transform data
      strcat(transform_data, line.c_str());
    } else {
      // Got to empty line
      // Execute transform
      MatrixXd m = parse_transform(transform_data);
      // Store transform matrix
      if (transformed.count(objectname)){
        transformed[objectname].push_back(m);
      } else {
        vector<MatrixXd> tmp;
        tmp.push_back(m);
        transformed[objectname] = tmp;
      }

      objectname = "";
      strcpy(transform_data, "");
    }

  }

  // Execute transform of last thing
  MatrixXd m = parse_transform(transform_data);

  // Store transform matrix
  if (transformed.count(objectname)){
    transformed[objectname].push_back(m);
  } else {
    vector<MatrixXd> tmp;
    tmp.push_back(m);
    transformed[objectname] = tmp;
  }


  // DEBUG
  /*typedef map<char *, vector<MatrixXd> >::iterator it_type;
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
  // Iterate over geometric transforms 
  // Apply transforms to instances
  // Print out transformed instances
  ///////////////////////////////
  
  typedef map<string, vector<MatrixXd> >::iterator it_type;
  for(it_type it = transformed.begin(); it != transformed.end(); it++) {
    // For each instance
    string name = it->first;
    vector<MatrixXd> matrices = it->second;
    Obj obj = original[name];

    for (int i = 0; i < matrices.size(); i++){
      // For each of the instance's transforms
      
      // DEBUG
      /*cout << name << "_" << i << endl;
      typedef map<char *, Obj >::iterator it_type1;
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

      // Transform all of instance's vertices
      for (int vertex = 1; vertex < obj.v.size(); vertex++){
        Vertex this_vertex = obj.v[vertex];
        MatrixXd v(4,1); 
        v << this_vertex.x, this_vertex.y, this_vertex.z, 1;
        
        // Vertex after geometric transform
        MatrixXd t = matrices[i] * v;

        // Vertex converted to homogenous NDC
        MatrixXd cam_transform = cam_pos * cam_orientation;
        MatrixXd inv_cam_transform = cam_transform.inverse();

        MatrixXd C_vertex = (inv_cam_transform * t);
        MatrixXd homo_NDC = cam_proj * C_vertex;
        
        // DEBUG
        /*
        cout << "homogenous NDC: " << endl;
        cout << homo_NDC.row(0) << " " << homo_NDC.row(1) << " " << homo_NDC.row(2) << endl;
        */
        // DEBUG

        // Vertex converted to cartesian NDC
        // by dividing by -z_c (z coord of vertex)
        double neg_z_c = -1/C_vertex(2,0);
        MatrixXd cart_NDC = neg_z_c * homo_NDC;
        /*
        cout << "Cartesian NDC = " << neg_z_c << endl;
        cout << cart_NDC.row(0) << " " << cart_NDC.row(1) << " " << cart_NDC.row(2) << endl;
        */
        Vertex v1;
        v1.x = cart_NDC(0,0);
        v1.y = cart_NDC(1,0);
        v1.z = cart_NDC(2,0);
        obj.v[vertex] = v1; // Store transformed vertex
        
        // DEBUG : DRAW THIS VERTEX ONLY
        //rasterize(grid, x_dim, y_dim, v1, v1);
      }
    }

    // CONNECT VERTICES
    // For this object, iterate through all faces
    // and rasterize each face's outline
    draw_lines(grid, x_dim, y_dim, obj);
  } 

  // DEBUG
  /*for (int x = 0; x < x_dim * y_dim; x++){
    cout << grid[x] << " ";
  }*/
  // DEBUG

  to_ppm(grid);

  delete[] grid;
  return 0;
}