#include "object.h"

// GLOBAL VARIABLES
string directory = "data/";
int xres = 500; // x res
int yres = 500; // y res
// For Set 5
//static Mesh_Data *mesh;
// static vector<HEV*> *hevs = new vector<HEV*>;
// static vector<HEF*> *hefs = new vector<HEF*>;

// TEMP
string textfile = "data/scene_bunny.txt";


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

// Implementation
string light_text;
string originals_text;
string instance_text;

// For Arc-Ball
// Initialize to identity
Vector4d last_rotation(1, 0, 0, 0);
Vector4d current_rotation(1, 0, 0, 0);
int px_start, py_start, px_current, py_current;

/* The following are parameters for creating an interactive first-person camera
 * view of the scene. The variables will make more sense when explained in
 * context, so you should just look at the 'mousePressed', 'mouseMoved', and
 * 'keyPressed' functions for the details.
 */

int mouse_x, mouse_y;
float mouse_scale_x, mouse_scale_y;

const float step_size = 0.2;
const float x_view_step = 90.0, y_view_step = 90.0;
float x_view_angle = 0, y_view_angle = 0;

bool is_pressed = false;
bool wireframe_mode = false;

// For smoothing
// vector<HEV*> *hevs;
// vector<HEF*> *hefs;


///////////////////////
// DEBUG
///////////////////////

void debug_base_objs(map<string, Obj> original){
  typedef map<string, Obj >::iterator it_type1;
  for(it_type1 it = original.begin(); it != original.end(); it++) {
    cout << "Object name: " << it->first << endl;
    cout << "Vertex buffer: " << it->second.v.size() << endl;
  }
}

void debug_vertex(Vertex v){
  cout << "Vertex: " << v.x << " " << v.y << " " << v.z << endl;
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

void debug_transform(GeoTransform t){
  cout << "---------" << endl;
  cout << "GeoTransform: " << endl;

  cout << "type = " << t.type << endl;
  if (t.type == "r"){
    cout << "angle = " << t.params[3] << endl;
  }
  cout << "xyz = " <<
    t.params[0] << " " << t.params[1] << " " << t.params[2] << endl;
  cout << "--------" << endl;
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
  cout << "=== OBJECT: " << obj.filename << endl;

  cout << " vertex buffer: " << obj.v.size() << endl;
  // for (int i = 0; i < obj.v.size(); i++){
  //   debug_vertex(obj.v[i]);
  // }
  cout << " f: " << endl;
  // for (int i = 0; i < obj.f.size(); i++){
  //   cout << "  " << obj.f[i].x << " " << obj.f[i].y << " " << obj.f[i].z << endl;
  // }
  cout << " normal buffer: " << obj.vn.size() << endl;
  // for (int i = 0; i < obj.vn.size(); i++){
  //   debug_vertex(obj.v[i]);
  // }
  cout << " fn: " << endl;
  // for (int i = 0; i < obj.fn.size(); i++){
  //   cout << "  " << obj.fn[i].x << " " << obj.fn[i].y << " " << obj.fn[i].z << endl;
  // }
  // cout << " ";
  debug_material(obj.material);
}

///////////////////////
// PARSE TEXT AND OBJ FILES
///////////////////////

void create_lights(string light_text){
  istringstream line_stream(light_text);
  string property;
  string comma; // Placeholder for delimiting comma
  while (line_stream >> property){
    //line_stream >> property;
    cout << "property: " << property << endl;
    if (property == "light"){
      Light l;
      line_stream >> l.x >> l.y >> l.z >> comma;
      line_stream >> l.rgb.r >> l.rgb.g >> l.rgb.b >> comma;
      line_stream >> l.attenuation;
      lights.push_back(l);
    }
  }
}

void create_camera(string cam_text){
  istringstream line_stream(cam_text);
  string property;
  int x, y, z, a; // a = angle
  float p; // perspective param
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
  debug_camera(cam);
}

///////////////////////
// PARSE
///////////////////////

// Calculating normal
// from three vertices associated with face
void calculate_normal(vector<Vertex> &vn_list,
  std::vector<HEV*> *hevs,
  std::vector<HEF*> *hefs){

    Vertex placeholder;
    placeholder.x = 0;
    placeholder.y = 0;
    placeholder.z = 0;
    vn_list.push_back(placeholder);

  for (auto &v : *hevs){
    Vector3d n(0.0, 0.0, 0.0);
    HE *he;
    if (v && v->out && v->out->face){
      he = v->out;
      do{
        HEF *f = he->face;
        HEV *v1_v = f->edge->vertex;
        HEV *v2_v = f->edge->next->vertex;
        HEV *v3_v = f->edge->next->next->vertex;

        // Convert HEV's to Eigen
        Vector3d v1(v1_v->x, v1_v->y, v1_v->z);
        Vector3d v2(v2_v->x, v2_v->y, v2_v->z);
        Vector3d v3(v3_v->x, v3_v->y, v3_v->z);

        // cross product of (v2 - v1) x (v3 - v1)
        Vector3d face_normal = (v2 - v1).cross(v3 - v1);
        // face_area = 1/2 | face_normal |
        float face_area = 0.5 * face_normal.norm();
        // n += face_normal ⋆ face_area
        n += face_normal * face_area;

        // gives us the halfedge to the next adjacent vertex
        he = he->flip->next;

      } while (he != v->out);

      // Normalize n and store it in hevs
      n.normalize();
      Vec3f norm_n;
      norm_n.x = n[0];
      norm_n.y = n[1];
      norm_n.z = n[2];
      v->normal = norm_n;

      // Store n in the vn list for obj
      Vertex vn;
      vn.x = n[0];
      vn.y = n[1];
      vn.z = n[2];
      vn_list.push_back(vn);
    }
  }
}

// Parses .obj file
void parse_points(Obj &obj, char * filename,
  vector<HEV*> *hevs, vector<HEF*> *hefs, Mesh_Data * mesh){

  // Set up structs
  Vertex v; // Placeholder vertex for 1 index

  std::vector<Vertex> vertices;
  vertices.push_back(v); // For 1 index

  std::vector<Face> faces;
  std::vector<Vertex> vn; // vertex normals

  obj.filename = filename;

  // READ IN FILE
  ifstream infile(directory + (string)filename);
  string type;
  float x, y, z;

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
      int v1, v2, v3;

      infile >> v1 >> v2 >> v3;

      Face f;
      f.idx1 = v1;
      f.idx2 = v2;
      f.idx3 = v3;
      faces.push_back(f);
    }
  }

  // For Set 5
  // Generate vertices_pointers and faces_pointers
  // using data from regular vertices and faces
  vector<Face*> *faces_pointers = new vector<Face*>;
  vector<Vertex*> *vertices_pointers = new vector<Vertex*>;
  for (auto &v : vertices){
    Vertex * pointer = new Vertex(v);
    vertices_pointers->emplace_back(pointer);
  }

  for (auto &f : faces){
    Face * pointer = new Face(f);
    faces_pointers->emplace_back(pointer);
  }

  // Create mesh object using regular
  // vertices and faces
  mesh = new Mesh_Data;
  mesh->vertices = vertices_pointers;
  mesh->faces = faces_pointers;

  // Using the mesh obj we just created,
  // create the vectors hevs and hefs
  build_HE(mesh, hevs, hefs);

  // Go back into hevs and hefs and:
  // 1. fill in normal calculations in hevs & hefs
  // 2. populate vn
  calculate_normal(vn, hevs, hefs);

  // Construct normal buffer (aka vn)
  vector<Vertex> normal_buffer;
  for (auto &fn_i : faces){
    normal_buffer.push_back(vn[fn_i.idx1]);
    normal_buffer.push_back(vn[fn_i.idx2]);
    normal_buffer.push_back(vn[fn_i.idx3]);
  }

  // Construct vertex buffer
  vector<Vertex> vertex_buffer;
  for (auto &f_i : faces){
    vertex_buffer.push_back(vertices[f_i.idx1]);
    vertex_buffer.push_back(vertices[f_i.idx2]);
    vertex_buffer.push_back(vertices[f_i.idx3]);
  }

  obj.v = vertex_buffer;
  obj.vn = normal_buffer;
  obj.f = faces;
  obj.plain_v = vertices;

}


void parse_transform(istringstream &infile, vector<GeoTransform> &all){
  // Read in transform data from file
  //istringstream infile(transform_text);
  string type;
  float x, y, z, a;

  // While we haven't reached the end
  // and the input is a valid transform
  while (infile >> type && (type == "t" || type == "s" || type == "r")){
    if (type == "t" || type == "s"){
      // TRANSLATE or SCALE MATRIX
      infile >> x >> y >> z;

      GeoTransform t;
      t.type = type;
      vector<float> params {x, y, z};
      t.params = params;

      debug_transform(t);

      all.push_back(t);
    } else if (type == "r"){
      // ROTATE MATRIX
      infile >> x >> y >> z >> a;

      GeoTransform t;
      t.type = type;
      vector<float> params {x, y, z, a};
      t.params = params;

      debug_transform(t);

      all.push_back(t);
    }
  }
}


void create_originals(string originals_text,
  vector<HEV*> *hevs, vector<HEF*> *hefs, Mesh_Data * mesh){
    // Assume format: "objectname objectfile.obj"
    string objectname;
    char objectfile[500];

    istringstream line_stream(originals_text);
    while (line_stream >> objectname >> objectfile){
        Obj obj;
        parse_points(obj, objectfile, hevs, hefs, mesh);
        original[objectname] = obj;
        /*cout << "Original-----" << endl;
        debug_obj(obj);*/
    }
}

void create_instances(string instance_text){
    // Assume instance_text =
    // objectname
    // ambient 0.2 0 0
    // diffuse 0.8 0 0
    // specular 0 0 0
    // shininess 0.2
    // s 1 1 1
    // r 0 1 0 0.6
    // t 0 0 0
    //
    // objectname
    // ambient 0.2 0 0
    // diffuse 0.8 0 0
    // specular 0 0 0
    // shininess 0.2
    // s 1 1 1
    // r 0 1 0 0.6
    // t 0 0 0

  istringstream line_stream(instance_text);
  cout << "Instance text = " << instance_text << endl;

  //char transform_data[1000];

  string param;
  while (line_stream >> param){
    cout << "while loop: " << param << endl;
    // Start new object
    Obj obj;
    string objectname;
    Material material;
    vector<GeoTransform> transform_list;

    // Copy out base object from "original"
    objectname = param;
    obj = original[objectname];
    cout << "Working on object>" << objectname << "<" << endl;

    // Parse material
    // Assuming that all 4 material properties are
    // always there
    for (int i = 0; i < 4; i++){
      line_stream >> param;
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

    debug_material(material);
    obj.material = material; // Store material data

    // Parse transforms, if any
    parse_transform(line_stream, transform_list);
    // Store transform list
    obj.geo_transform = transform_list;
    // Store instance
    instances.push_back(obj);
    cout << "===DONE OBJECT===" << endl;
    cout << "=================" << endl;
  }
}

void parse(string file){
  string line;
  ifstream infile(file);

  //////////////////////////////
  // Generate camera
  ///////////////////////////////
  getline(infile, line);
  assert(line == "camera:");
  string cam_text;
  while (getline(infile, line) && !line.empty()){
    cam_text += line + " ";
  }
  create_camera(cam_text);

  //////////////////////////////
  // Generate lights
  ///////////////////////////////
  //string light_text;
  while (getline(infile, line) && !line.empty()){
    light_text += line + " ";
  }

  //////////////////////////////
  // Generate "originals" map associating name with object template
  ///////////////////////////////
  getline(infile, line);
  assert(line == "objects:");
  //string originals_text;
  while (getline(infile, line) && !line.empty()){
    // Assume format: "objectname objectfile.obj"
    originals_text += line + " ";
  }

  ///////////////////////////////
  // Generate map associating object with list of transforms
  ///////////////////////////////

  //string instance_text;
  // Use artifical end delimited to separate instance specs
  while (getline(infile, line)){

    if (line.size() <= 1){
      instance_text += " /end/ ";
    } else {
      instance_text += line + " ";
    }
  }
  instance_text += " /end/";

  cout << "done parse" << endl;
}
