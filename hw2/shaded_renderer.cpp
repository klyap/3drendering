// Refactored version for cleaner parsing

#include "shaded_renderer.h"

void debug_base_objs(map<string, Obj> original){
  typedef map<string, Obj >::iterator it_type1;
  for(it_type1 it = original.begin(); it != original.end(); it++) {
    cout << "Object name: " << it->first << endl;
    cout << "Number of instances: " << it->second.v.size() << endl;
  }
}

void debug_transformed(Obj obj, map<string, Obj> original, map<string, vector<MatrixXd>> transformed){
  typedef map<string, vector<MatrixXd> >::iterator it_type;
  for(it_type it = transformed.begin(); it != transformed.end(); it++){
    cout << it->first << endl;
    vector<MatrixXd> matrices = it->second;
    for (int i = 0; i < matrices.size(); i++){
      // For each object instance
      Obj obj = original[it->first];
      cout << "Obj: " << obj.filename << " " << i << endl;
      for (int vertex = 0; vertex < obj.v.size(); vertex++){
        Vertex this_vertex = obj.v[vertex];
        MatrixXd v(4,1); 
        cout << "Transform: " << endl << matrices[i] << endl;
        v << this_vertex.x, this_vertex.y, this_vertex.z, 1;
        cout << "Transformed v: " << endl << matrices[i] * v << endl;
      }
    }
  }
}

void debug_camera(Camera cam){

  cout << "Camera position: ";
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
  cout << endl;

}
void debug_color(Color c){
  cout << "COLOR: " << 
  c.r << " " << c.g << " " << c.b << endl;
}

void debug_lights(vector<Light> lights){
  cout << "LIGHT: " << endl;
  for (int i = 0; i < lights.size(); i++){
    Light l = lights[i];
    cout << "   xyz: " << l.x << l.y << l.z << " ";
    cout << "   rgb: " << l.rgb.r << l.rgb.g << l.rgb.b << endl;
    cout << "   attenuation: " << l.attenuation << endl;
  }
}

void debug_material(Material m){
  cout << "MATERIAL:" << endl;
  cout << "ambient: ";
  debug_color(m.ambient); // ambient material reflectance
  cout << "diffuse: ";
  debug_color(m.diffuse); // diffuse material reflectance
  cout << "specular: ";
  debug_color(m.specular); // specular material reflectance
  cout << "shininess: ";
  cout << m.shininess << endl; // Phong exponent for material shininess
}

void debug_obj(Obj obj){
  cout << "OBJECT: " << obj.filename << endl;
  
  cout << " v: " << endl;
  for (int i = 0; i < obj.v.size(); i++){
    cout << "  " << obj.v[i].x << " " << obj.v[i].y << " " << obj.v[i].z << endl;
  }
  
  cout << " f: " << endl;
  for (int i = 0; i < obj.f.size(); i++){
    cout << "  " << obj.f[i].x << " " << obj.f[i].y << " " << obj.f[i].z << endl;
  }
  cout << " vn: " << endl;
  for (int i = 0; i < obj.vn.size(); i++){
    cout << "  " << obj.vn[i].x << " " << obj.vn[i].y << " " << obj.vn[i].z << endl;
  }
  cout << " fn: " << endl;
  for (int i = 0; i < obj.fn.size(); i++){
    cout << "  " << obj.fn[i].x << " " << obj.fn[i].y << " " << obj.fn[i].z << endl;
  }
  cout << " ";
  debug_material(obj.material);
}

Obj parse_obj(Obj &obj, char * filename){
  // Set up structs
  Vertex v; // Placeholder vertex for 1 index

  std::vector<Vertex> vertices;
  vertices.push_back(v); // For 1 index

  std::vector<Vertex> surfacenormals;
  surfacenormals.push_back(v); // For 1 index

  std::vector<Face> faces;

  std::vector<Face> normals;

  //Obj obj;
  obj.filename = filename;

  // READ IN FILE
  ifstream infile(directory + (string)filename);
  string type;
  double x, y, z;
  while (infile >> type)
  {
    
    if (type == "v"){
      infile >> x >> y >> z;

      Vertex v;
      v.x = x;
      v.y = y;
      v.z = z;
      vertices.push_back(v);
    
    } else if (type == "f"){
      string v1;
      string v2;
      string v3;

      infile >> v1 >> v2 >> v3;
      std::string delimiter = "//";
      string x = v1.substr(0, v1.find(delimiter));
      string y = v2.substr(0, v2.find(delimiter));
      string z = v3.substr(0, v3.find(delimiter));

      string xn = v1.substr(v1.find(delimiter)+2, v1.size());
      string yn = v2.substr(v2.find(delimiter)+2, v2.size());
      string zn = v3.substr(v3.find(delimiter)+2, v3.size());
      
      Face f;
      f.x = atoi(x.c_str());
      f.y = atoi(y.c_str());
      f.z = atoi(z.c_str());
      faces.push_back(f);

      Face fn;
      fn.x = atoi(xn.c_str());
      fn.y = atoi(yn.c_str());
      fn.z = atoi(zn.c_str());
      normals.push_back(fn);

    } else if (type == "vn"){
      infile >> x >> y >> z;

      Vertex v;
      v.x = x;
      v.y = y;
      v.z = z;
      surfacenormals.push_back(v);
    }
  }

  obj.v = vertices;
  obj.f = faces;
  obj.vn = surfacenormals;
  obj.fn = normals;

  return obj;
}

MatrixXd make_translate_matrix(
  double x, double y, double z){

  MatrixXd m(4,4); // m is constructed as a 4x4 matrix  
      m << 1, 0, 0, x, // row1  
           0, 1, 0, y, // row2  
           0, 0, 1, z, // row3  
           0, 0, 0, 1; // row4
  return m;
}

MatrixXd make_rotation_matrix(
    double x, double y, double z, double a){

  MatrixXd axis(3,1);
  axis << x, y, z;
  axis.normalize();

  x = axis(0,0);
  y = axis(1,0);
  z = axis(2,0);

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
 double a3 = (r + l) / (r - l);
 double b2 = (2 * n) / (t - b);
 double b3 = (t + b) / (t - b);
 double c3 = -1 * (f + n) / (f-n);
 double c4 = -2 * (f * n) / (f-n);
 
 MatrixXd m(4,4); // m is constructed as a 4x4 matrix  
  m << a1, 0, a3, 0, // row1  
       0, b2, b3, 0, // row2  
       0, 0, c3, c4, // row3  
       0, 0, -1, 0;  // row4

  return m;
}

GeoTransform parse_transform(char * filename){
  // READ IN FILE
  istringstream infile(filename); // Assume filename = string of filedata
  char type;
  double x, y, z, a;
  
  GeoTransform all_transforms;
  vector<MatrixXd> translate;
  vector<MatrixXd> rotate;
  vector<MatrixXd> scale;
  vector<MatrixXd> all;
  vector<MatrixXd> SR;

  while (infile >> type)
  {    
    if (type == 't'){
      // TRANSLATE MATRIX
      infile >> x >> y >> z;

      MatrixXd m = make_translate_matrix(x, y, z);
      translate.push_back(m);
      all.push_back(m);
    } else if (type == 'r'){
      // ROTATE MATRIX
      infile >> x >> y >> z >> a;

      MatrixXd m = make_rotation_matrix(x, y, z, a);
      rotate.push_back(m);
      all.push_back(m);
      SR.push_back(m);
    } else if (type == 's'){
      // SCALE MATRIX
      infile >> x >> y >> z;
      MatrixXd m(4,4); // m is constructed as a 4x4 matrix  
      m << x, 0, 0, 0, // row1  
           0, y, 0, 0, // row2  
           0, 0, z, 0, // row3  
           0, 0, 0, 1; // row4

      scale.push_back(m);
      all.push_back(m);
      SR.push_back(m);
    }
  }

  // COMBINE MATRICES
  MatrixXd m = all.back();
  all.pop_back();

  while (all.size() > 0){
    m *= all.back();
    all.pop_back();
  }

  // COMBINE MATRICES
  MatrixXd m2 = SR.back();
  SR.pop_back();

  while (SR.size() > 0){
    m2 *= SR.back();
    SR.pop_back();
  }

  all_transforms.t = translate;
  all_transforms.r = rotate;
  all_transforms.s = scale;
  all_transforms.all = m;
  all_transforms.SR = m2;
  return all_transforms;
}

////////////////////
// SHADING
////////////////////

// Transform surface normals
void normal_transform(Obj &obj){

  MatrixXd SR = obj.geo_transform.SR;
  //cout << "SR: \n" << SR << endl;
  MatrixXd SR_inv = SR.inverse();
  MatrixXd M = SR_inv.transpose();
  //cout << "M: \n" << M << endl;
  
  // Transform every normal in this obj in place
  for (int i = 1; i < obj.vn.size(); i++){
    //Vector4d cur_vn(obj.vn[i].x, obj.vn[i].y, obj.vn[i].z, 1);
    MatrixXd cur_vn(4,1);
    cur_vn << obj.vn[i].x, obj.vn[i].y, obj.vn[i].z, 1;
    //cout << "cur_vn: \n" << cur_vn << endl;
    MatrixXd t = M * cur_vn;
    //cout << "transformed vn: \n" << transformed << endl;
    Vector3d transformed(t(0,0), t(1,0), t(2,0));
    transformed.normalize();
    //cout << "transformed vn normalize: \n" << transformed << endl;

    obj.vn[i].x = transformed(0,0);
    obj.vn[i].y = transformed(1,0);
    obj.vn[i].z = transformed(2,0);
  }
} 


// Converts a vertex from world to Cartesian NDC
MatrixXd world_to_NDC(const MatrixXd &t, const MatrixXd &inv_cam_transform, 
                    //MatrixXd cam_pos, 
                    //MatrixXd cam_orientation,
                    const MatrixXd &cam_proj){
  //cout << "world to NDC:" << inv_cam_transform << endl;
  MatrixXd C_vertex = (inv_cam_transform * t);
  MatrixXd homo_NDC = cam_proj * C_vertex;
  
  MatrixXd cart_NDC = -1/C_vertex(2,0) * homo_NDC;
  
  return cart_NDC;
}


// Converts NDC to screen coords
void int_NDC_to_screen(double a, double b, int &x, int &y){
  x = (a + 1)/2.0 * (x_dim);
  y = y_dim - ((b + 1)/2.0 * (y_dim));
}


// n = normal vector of x, y, z
// e = camera position
Vector3d lighting_model(const Vertex &Pv, const Vertex &nv, const Material &material,
                    const vector<Light> &lights, const MatrixXd &em){
  // Empty vector
  Vector3d empty(0,0,0);
  Vector3d ones(1,1,1);

  // Convert to vector
  Vector3d P(Pv.x, Pv.y, Pv.z); // point position
  Vector3d n(nv.x, nv.y, nv.z); // surface normal
  Vector3d e(em(0,0), em(1,0), em(2,0)); // camera pos

  Vector3d cd(material.diffuse.r, material.diffuse.g, material.diffuse.b);
  Vector3d ca(material.ambient.r, material.ambient.g, material.ambient.b);
  Vector3d cs(material.specular.r, material.specular.g, material.specular.b);
  double p = material.shininess;

  Vector3d diffuse_sum(0,0,0);
  Vector3d specular_sum(0,0,0);

  Vector3d e_direction = (e - P).normalized();

  Vector3d lp(0,0,0); // light position
  Vector3d lc(0,0,0); // light color

  Vector3d l_direction, l_diffuse, l_specular;

  for (auto &l : lights){
    lp << l.x, l.y, l.z;
    lc << l.rgb.r, l.rgb.g, l.rgb.b;

    // Include attenuation
    // Distance between light and point
    double d = (lp - P).norm();
    lc = lc * (1.0 / (1 + l.attenuation * d * d));
    
    // Find diffuse
    Vector3d l_direction = (lp - P).normalized();
    l_diffuse = lc * max(0.0, n.dot(l_direction));
    diffuse_sum += l_diffuse;
    
    // Find specular
    l_specular = lc * pow(max(0.0, 
      n.dot((e_direction + l_direction).normalized())), p);
    specular_sum += l_specular;
  }

  //component-wise min function and component-wise product
  Vector3d c = ones.cwiseMin(ca 
                + diffuse_sum.cwiseProduct(cd)
                + specular_sum.cwiseProduct(cs));
  return c;
}

double f_ij(double x, double y, double xi, double yi, 
            double xj, double yj){
  return (yi - yj) * x + 
         (xj - xi) * y + 
         xi * yj - xj * yi;
}


void compute_barycentric(
  double &alpha, double &beta, double &gamma,
  double x_a, double y_a, double x_b, double y_b,
  double x_c, double y_c, int x_p, int y_p){

  alpha = f_ij(x_p, y_p, x_b, y_b, x_c, y_c) / 
          f_ij(x_a, y_a, x_b, y_b, x_c, y_c);
  beta = f_ij(x_p, y_p, x_a, y_a, x_c, y_c) /
         f_ij(x_b, y_b, x_a, y_a, x_c, y_c);
  gamma = f_ij(x_p, y_p, x_a, y_a, x_b, y_b) /
         f_ij(x_c, y_c, x_a, y_a, x_b, y_b);
}


// Filling in grid for shader
// with RGB packaged as an int
void fill(int x, int y, Color * grid, Color &color){
  grid[y * x_dim + x] = color;
}

void color_to_ppm(Color * grid){
  cout << "P3\n";
  cout << x_dim << " " << y_dim << "\n";
  cout << "255\n";
  for (int x = 0; x < x_dim * y_dim; x++){
    int red = grid[x].r;
    int green = grid[x].g;
    int blue = grid[x].b;
    cout << red << " " << green << " " << blue << endl;
  }
}

// NDC is a vector of x, y, z 
bool in_NDC_cube (const Vector3d &NDC){
  if (NDC(0,0) >= -1 && NDC(0,0) <= 1 &&
      NDC(1,0) >= -1 && NDC(1,0) <= 1 &&
      NDC(2,0) >= -1 && NDC(2,0) <= 1)
    return true;
  return false;
}

void raster_colored_triangle(const Obj &obj, const Face &f, const Face &fn,
    const MatrixXd &NDC_a, const MatrixXd &NDC_b, const MatrixXd &NDC_c,
    const Vector3d &color_a, const Vector3d &color_b, const Vector3d &color_c,
    Color * grid, double * buffer){

  Vector3d ndc_a(NDC_a(0,0), NDC_a(1,0), NDC_a(2,0));
  Vector3d ndc_b(NDC_b(0,0), NDC_b(1,0), NDC_b(2,0));
  Vector3d ndc_c(NDC_c(0,0), NDC_c(1,0), NDC_c(2,0));
  Vector3d cross = (ndc_c - ndc_b).cross(ndc_a - ndc_b);
  
  if (cross[2] < 0){ 
    return;
  }

  double x_a_double = NDC_a(0,0);
  double y_a_double = NDC_a(1,0);
  int x_a, y_a;
  int_NDC_to_screen(x_a_double, y_a_double, x_a, y_a);
  
  double x_b_double = NDC_b(0,0);
  double y_b_double = NDC_b(1,0);
  int x_b, y_b;
  int_NDC_to_screen(x_b_double, y_b_double, x_b, y_b);
  
  double x_c_double = NDC_c(0,0);
  double y_c_double = NDC_c(1,0);
  int x_c, y_c;
  int_NDC_to_screen(x_c_double, y_c_double, x_c, y_c);
  
  int x_min = min(x_a, min(x_b, x_c));
  int x_max = max(x_a, max(x_b, x_c));
  int y_min = min(y_a, min(y_b, y_c));
  int y_max = max(y_a, max(y_b, y_c));

  for (int x = x_min; x < x_max; x++){
    for (int y = y_min; y < y_max; y++){
      double alpha, beta, gamma;
      compute_barycentric(alpha, beta, gamma, 
        x_a, y_a, x_b, y_b, x_c, y_c, x, y);

      if (0 <= alpha && alpha <= 1 &&
          0 <= beta && beta <= 1 &&
          0 <= gamma && gamma <= 1 ){
       
        Vector3d NDC = alpha * ndc_a + beta * ndc_b + gamma * ndc_c;
        
        if (in_NDC_cube(NDC) && NDC[2] <= buffer[y * x_dim + x]){
          buffer[y * x_dim + x] = NDC(2,0); // Store NDC.z

          Color color;
          color.r = (alpha * color_a[0] + beta * color_b[0] + gamma * color_c[0]) * 255;
          color.g = (alpha * color_a[1] + beta * color_b[1] + gamma * color_c[1]) * 255;
          color.b = (alpha * color_a[2] + beta * color_b[2] + gamma * color_c[2]) * 255;
          
          fill(x, y, grid, color);
        }
      }
    }
  }
 }


void gouraud_shading(const Obj &obj, const Face &f, const Face &fn,
    const Material &material, const vector<Light> &lights,
    Color * grid, double * buffer, const MatrixXd &e,
    const MatrixXd &inv_cam_transform,
    const MatrixXd &cam_proj){

  //cout << "gouraud_shading: " << inv_cam_transform << endl;

  Vector3d color_a = lighting_model(
    obj.v[f.x], obj.vn[fn.x], material, lights, e);
  Vector3d color_b = lighting_model(
    obj.v[f.y], obj.vn[fn.y], material, lights, e);
  Vector3d color_c = lighting_model(
    obj.v[f.z], obj.vn[fn.z], material, lights, e);

  MatrixXd a(4,1); // Vertex A's coords
  MatrixXd b(4,1); // Vertex B's coords
  MatrixXd c(4,1); // Vertex C's coords
  a << obj.v[f.x].x, obj.v[f.x].y, obj.v[f.x].z, 1;
  b << obj.v[f.y].x, obj.v[f.y].y, obj.v[f.y].z, 1;
  c << obj.v[f.z].x, obj.v[f.z].y, obj.v[f.z].z, 1;
  
  // Vertex converted to homogenous NDC
  MatrixXd ndc_a = world_to_NDC(a, inv_cam_transform, cam_proj);
  MatrixXd ndc_b = world_to_NDC(b, inv_cam_transform, cam_proj);
  MatrixXd ndc_c = world_to_NDC(c, inv_cam_transform, cam_proj);

  raster_colored_triangle(
              obj, f, fn,
              ndc_a, ndc_b, ndc_c,
              color_a, color_b, color_c,
              grid, buffer);
}

void phong_shading(const Obj &obj, const Face &f, const Face &fn,
  const vector<Light> &lights, 
  const MatrixXd &e,
  const MatrixXd &cam_pos, const MatrixXd &cam_orientation, const MatrixXd &cam_proj,
  Color * grid, double * buffer){

  MatrixXd a(4,1); // Vertex A's coords
  MatrixXd b(4,1); // Vertex B's coords
  MatrixXd c(4,1); // Vertex C's coords
  a << obj.v[f.x].x, obj.v[f.x].y, obj.v[f.x].z, 1;
  b << obj.v[f.y].x, obj.v[f.y].y, obj.v[f.y].z, 1;
  c << obj.v[f.z].x, obj.v[f.z].y, obj.v[f.z].z, 1;
  
  // Vertex converted to homogenous NDC
  MatrixXd cam_transform = cam_orientation * cam_pos;
  MatrixXd inv_cam_transform = cam_transform.inverse();

  MatrixXd NDC_a = world_to_NDC(a, inv_cam_transform, cam_proj);
  MatrixXd NDC_b = world_to_NDC(b, inv_cam_transform, cam_proj);
  MatrixXd NDC_c = world_to_NDC(c, inv_cam_transform, cam_proj);

  // Convert to vector just for cross product
  Vector3d ndc_a(NDC_a(0,0), NDC_a(1,0), NDC_a(2,0));
  Vector3d ndc_b(NDC_b(0,0), NDC_b(1,0), NDC_b(2,0));
  Vector3d ndc_c(NDC_c(0,0), NDC_c(1,0), NDC_c(2,0));
  Vector3d cross = (ndc_c - ndc_b).cross(ndc_a - ndc_b);
  
  if (cross[2] < 0){ // If cross.z < 0
    return;
  }

  double x_a_double = NDC_a(0,0); // ERROR: x's are negative ?!
  double y_a_double = NDC_a(1,0);
  int x_a, y_a;
  int_NDC_to_screen(x_a_double, y_a_double, x_a, y_a);

  double x_b_double = NDC_b(0,0);
  double y_b_double = NDC_b(1,0);
  int x_b, y_b;
  int_NDC_to_screen(x_b_double, y_b_double, x_b, y_b);
  
  double x_c_double = NDC_c(0,0);
  double y_c_double = NDC_c(1,0);
  int x_c, y_c;
  int_NDC_to_screen(x_c_double, y_c_double, x_c, y_c);
  
  int x_min = min(x_a, min(x_b, x_c));
  int x_max = max(x_a, max(x_b, x_c));
  int y_min = min(y_a, min(y_b, y_c));
  int y_max = max(y_a, max(y_b, y_c));

  // Vectors a, b, c
  Vector3d v_a;
  v_a << obj.v[f.x].x, obj.v[f.x].y, obj.v[f.x].z;
  Vector3d v_b;
  v_b << obj.v[f.y].x, obj.v[f.y].y, obj.v[f.y].z;
  Vector3d v_c;
  v_c << obj.v[f.z].x, obj.v[f.z].y, obj.v[f.z].z;

  // Vector normals for vertices a, b, c
  Vector3d vn_a;
  vn_a << obj.vn[fn.x].x, obj.vn[fn.x].y, obj.vn[fn.x].z;
  Vector3d vn_b;
  vn_b << obj.vn[fn.y].x, obj.vn[fn.y].y, obj.vn[fn.y].z;
  Vector3d vn_c;
  vn_c << obj.vn[fn.z].x, obj.vn[fn.z].y, obj.vn[fn.z].z;

  for (int x = x_min; x < x_max; x++){
    for (int y = y_min; y < y_max; y++){
      double alpha, beta, gamma;
      compute_barycentric(alpha, beta, gamma, 
        x_a, y_a, x_b, y_b, x_c, y_c, x, y);

      if (0 <= alpha && alpha <= 1 &&
          0 <= beta && beta <= 1 &&
          0 <= gamma && gamma <= 1){
        Vector3d NDC = alpha * ndc_a + beta * ndc_b + gamma * ndc_c;
        
        // If it's in cube and not a backface, draw it
        if (in_NDC_cube(NDC) && NDC[2] <= buffer[y * x_dim + x]){
          
          buffer[y * x_dim + x] = NDC(2,0); // Store NDC.z

          Vector3d vn_vec = alpha * vn_a + beta * vn_b + gamma * vn_c;
          // Convert to vertex
          Vertex vn;
          vn.x = vn_vec[0];
          vn.y = vn_vec[1];
          vn.z = vn_vec[2];

          Vector3d v_vec = alpha * v_a + beta * v_b + gamma * v_c;
          // Convert to vertex
          Vertex v;
          v.x = v_vec[0];
          v.y = v_vec[1];
          v.z = v_vec[2];
          
          Vector3d c = lighting_model(v, vn, obj.material, lights, e);
          Color color;
          color.r = c[0] * 255;
          color.g = c[1] * 255;
          color.b = c[2] * 255;

          fill(x, y, grid, color);
        }
      }
    }
  }
}

void create_lights(string light_text, vector<Light> &lights){
  istringstream line_stream(light_text);
  string property;
  string comma; // Placeholder for delimiting comma
  while (line_stream >> property){
    //line_stream >> property;
    //cout << "property: " << property << endl;
    if (property == "light"){
      Light l;
      line_stream >> l.x >> l.y >> l.z >> comma;
      line_stream >> l.rgb.r >> l.rgb.g >> l.rgb.b >> comma;
      line_stream >> l.attenuation;
      lights.push_back(l);
    }
  }
}

void create_camera(string cam_text, Camera &cam){
  istringstream line_stream(cam_text);
  string property;
  int x, y, z, a; // a = angle
  double p; // perspective param
  while(line_stream >> property){
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
}



int main (int argc, char* argv[])
{ 
  if (argc < 5){
    cout << "Usage: ./shaded_renderer [scene_description_file.txt] [xres] [yres] [mode]"
      << endl;
    return 0;
  }

  // Grid marking whether a pixel is marked or not
  x_dim = atoi(argv[2]);
  y_dim = atoi(argv[3]);
  Color * grid = new Color [x_dim * y_dim];
  
  // Buffer grid for backface culling in shading
  double * buffer = new double [x_dim * y_dim]();
  for (int i = 0; i < x_dim * y_dim; i++){
    buffer[i] = DBL_MAX;
  }

  // Shading mode; 0 = Gourand shading, 1 = Phong shading
  int mode = atoi(argv[4]);
  // Holds object data for each .obj file
  map <string, Obj> original;
  // Holds transformed shape data of each object instance
  map <string, vector<GeoTransform> > transformed;
  // Holds material properties for shading of each object instance
  map <string, vector<Material> > materials;
  // Holds list of lights
  vector<Light> lights;
  // Holds camera transform params
  Camera cam;
  

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
  string cam_text;
  while (getline(infile, line) && !line.empty()){
    cam_text += line + " ";
  }
  create_camera(cam_text, cam);

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
  
  //////////////////////////////
  // Generate light
  ///////////////////////////////
  string light_text;
  while (getline(infile, line) && !line.empty()){
    light_text += line;
  }
  create_lights(light_text, lights);


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

    Obj obj;
    parse_obj(obj, objectfile);
    original[objectname] = obj;
  }

  ///////////////////////////////
  // Generate map associating object with list of transforms
  ///////////////////////////////
  string objectname;
  char transform_data[1000];
  Material material;
  while (getline(infile, line)){

    // Assume format:
    // objectname
    // ambient 0.2 0 0  
    // diffuse 0.8 0 0  
    // specular 0 0 0  
    // shininess 0.2  
    // s 1 1 1  
    // r 0 1 0 0.6  
    // t 0 0 0  

    if (objectname.size() == 0){
      // Starting new object transform
      objectname = line;
    } else if (!line.empty()){
      string param;
      istringstream line_stream(line);

      line_stream >> param;
      if (param.size() == 1){
        // Param is a transform (s, r, or t)
        // Collect transform data
        strcat(transform_data, line.c_str());
      } else {
        // Param is for shading
        if (param == "shininess"){
          line_stream >> material.shininess;
        } else if (param == "ambient") {
          line_stream >> material.ambient.r 
            >> material.ambient.g >> material.ambient.b;
        } else if (param == "specular") {
          line_stream >> material.specular.r 
            >> material.specular.g >> material.specular.b;
        } else if (param == "diffuse") {
          line_stream >> material.diffuse.r 
            >> material.diffuse.g >> material.diffuse.b;
        }
      }
    } else {
      // Got to empty line
      
      // Execute transform
      GeoTransform m = parse_transform(transform_data);
      // Store transform matrix
      if (transformed.count(objectname)){
        transformed[objectname].push_back(m);
      } else {
        vector<GeoTransform> tmp;
        tmp.push_back(m);
        transformed[objectname] = tmp;
      }

      // Store material data
      if (materials.count(objectname)){
        materials[objectname].push_back(material);
      } else {
        vector<Material> tmp;
        tmp.push_back(material);
        materials[objectname] = tmp;
      }

      // Prep for next object
      //   Not resetting material 
      //     (assuming that all params will be 
      //     replaced by new data)
      objectname = "";
      strcpy(transform_data, "");
    }

  }

  // Execute transform of last thing
  GeoTransform m = parse_transform(transform_data);

  // Store transform matrix
  if (transformed.count(objectname)){
    transformed[objectname].push_back(m);
  } else {
    vector<GeoTransform> tmp;
    tmp.push_back(m);
    transformed[objectname] = tmp;
  }
  // Store material data
  if (materials.count(objectname)){
    materials[objectname].push_back(material);
  } else {
    vector<Material> tmp;
    tmp.push_back(material);
    materials[objectname] = tmp;
  }

  //////////////////////////////
  // Iterate over geometric transforms 
  // Apply transforms to instances
  ///////////////////////////////
  
  // transformed = name, list of geometric transforms
  //    There is one transform matrix for each instance
  typedef map<string, vector<GeoTransform> >::iterator it_type;
  for(it_type it = transformed.begin(); it != transformed.end(); it++) {
    // For each obj
    string name = it->first; 
    vector<GeoTransform> matrices = it->second;

    for (int i = 0; i < matrices.size(); i++){
      // For each transform, copy out a new base obj
      Obj obj = original[name];
      
      // STORE MATERIAL DATA IN THIS OBJ
      obj.material = materials[name][i];
      // STORE GEOMETRIC TRANSFORM DATA IN THIS OBJ
      obj.geo_transform = matrices[i];

      // DO NORMAL TRANSFORM ON VN
      normal_transform(obj);
      
      // Geometric transform all of instance's vertices
      for (int vertex = 1; vertex < obj.v.size(); vertex++){
        Vertex this_vertex = obj.v[vertex];
        MatrixXd v(4,1); 
        v << this_vertex.x, this_vertex.y, this_vertex.z, 1;
        
        // Vertex after geometric transform
        MatrixXd t = matrices[i].all * v;
        
        Vertex v1;
        v1.x = t(0,0);
        v1.y = t(1,0);
        v1.z = t(2,0);
        obj.v[vertex] = v1; // Store transformed vertex*/
      }
      
      // SHADE FACES
      MatrixXd e(3,1);
      e << cam.position[0], cam.position[1], cam.position[2];
      MatrixXd cam_transform = cam_orientation * cam_pos;
      MatrixXd inv_cam_transform = cam_transform.inverse();
      
      if (mode == 0){
        for (int i = 0; i < obj.f.size(); i++){
          gouraud_shading(obj, obj.f[i], obj.fn[i],
          obj.material, lights,
          grid, buffer, e,
          inv_cam_transform,
          cam_proj);
        }
      } else if (mode == 1){
        // Phong
        for (int i = 0; i < obj.f.size(); i++){
          phong_shading (obj, obj.f[i], obj.fn[i], lights,
          e, cam_pos, cam_orientation, cam_proj,
          grid, buffer);
        }
      }
    }
  }
  
  color_to_ppm(grid);

  delete[] grid;
  delete[] buffer;
  return 0;
}