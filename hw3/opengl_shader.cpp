#include "opengl_shader.h"


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
  for (int i = 0; i < obj.v.size(); i++){
    debug_vertex(obj.v[i]);
  }
  cout << " f: " << endl;
  for (int i = 0; i < obj.f.size(); i++){
    cout << "  " << obj.f[i].x << " " << obj.f[i].y << " " << obj.f[i].z << endl;
  }
  cout << " normal buffer: " << obj.vn.size() << endl;
  for (int i = 0; i < obj.vn.size(); i++){
    debug_vertex(obj.v[i]);
  }
  cout << " fn: " << endl;
  for (int i = 0; i < obj.fn.size(); i++){
    cout << "  " << obj.fn[i].x << " " << obj.fn[i].y << " " << obj.fn[i].z << endl;
  }
  cout << " ";
  debug_material(obj.material);
}



///////////////////////////////////////////////////////////////////////////////////////////////////

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




/* 'init_lights' function:
 * 
 * OpenGL has 8 built-in lights in all, each one with its own unique, integer
 * ID value. 
 * The first light's ID value is stored in 'GL_LIGHT0'. The second light's ID
 * value is stored in 'GL_LIGHT1'.
 */
void init_lights()
{
    /* This tells it to automatically apply the
     * Phong reflection model or lighting model to 
     * every pixel it will render.
     */
    glEnable(GL_LIGHTING);
    
    int num_lights = lights.size();
    
    for(int i = 0; i < num_lights; ++i)
    {
        /* In this loop, we are going to associate each of our point lights
         * with one of OpenGL's built-in lights. The simplest way to do this
         * is to just let our first point light correspond to 'GL_LIGHT0', our
         * second point light correspond to 'GL_LIGHT1', and so on.
         */
        int light_id = GL_LIGHT0 + i;
        
        glEnable(light_id);
        
        /* The following lines of code use 'glLightfv' to set the color of
         * the light. The parameters for 'glLightfv' are:
         *
         * - enum light_ID: an integer between 'GL_LIGHT0' and 'GL_LIGHT7'
         * - enum property: this varies depending on what you are setting
         * - float* values: a set of values to set for the specified property
         *                  e.g. an array of RGB values for the light's color
         */

       

        float * color = new float[2];
        
        color[0] = (float)lights[i].rgb.r;
        cout << color[0] << endl;
        color[1] = (float)lights[i].rgb.g;
        cout << color[1] << endl;
        color[2] = (float)lights[i].rgb.b;
        cout << color[2] << endl;
        glLightfv(light_id, GL_AMBIENT, color);
        cout << light_id << endl;
        glLightfv(light_id, GL_DIFFUSE, color);
        glLightfv(light_id, GL_SPECULAR, color);
        
        /* The following line of code sets the attenuation k constant of the
         * light.
         */
        glLightf(light_id, GL_QUADRATIC_ATTENUATION, (float)lights[i].attenuation);
    }
}

/* 'set_lights' function:
 *
 * While the 'init_lights' function enables and sets the colors of the lights,
 * the 'set_lights' function is supposed to position the lights.
 */
void set_lights()
{
    int num_lights = lights.size();
    
    for(int i = 0; i < num_lights; ++i)
    {
        int light_id = GL_LIGHT0 + i;

        float * pos_f = new float[2];
        pos_f[0] = lights[i].x;
        pos_f[1] = lights[i].y;
        pos_f[2] = lights[i].z;

        glLightfv(light_id, GL_POSITION, pos_f);
    }
}

// int i is index of the object in "instances" that we are
// drawing right now
void apply_geometric_transforms(int i){
  int num_geo_transform = instances[i].geo_transform.size();
  for (int j = num_geo_transform; j >= 0; j--){
    if (instances[i].geo_transform[j].type == "t"){
      glTranslatef(instances[i].geo_transform[j].params[0],
                   instances[i].geo_transform[j].params[1],
                   instances[i].geo_transform[j].params[2]);
    } else if (instances[i].geo_transform[j].type == "r"){
      glRotatef(instances[i].geo_transform[j].params[3],
               instances[i].geo_transform[j].params[0],
               instances[i].geo_transform[j].params[1],
               instances[i].geo_transform[j].params[2]);
    } else if (instances[i].geo_transform[j].type == "s"){
      glScalef(instances[i].geo_transform[j].params[0],
               instances[i].geo_transform[j].params[1],
               instances[i].geo_transform[j].params[2]);
    }
  }
}

/* 'draw_objects' function:
 *
 * This function has OpenGL render our objects to the display screen. It
 */
void draw_objects()
{
    int num_objects = instances.size();
    
    for(int i = 0; i < num_objects; ++i)
    {   
        glPushMatrix();
        /* The following brace is not necessary, but it keeps things organized.
         */
        {
            apply_geometric_transforms(i);
            
            /* The 'glMaterialfv' and 'glMaterialf' functions tell OpenGL
             * the material properties of the surface we want to render.
             * The parameters for 'glMaterialfv' are (in the following order):
             *
             * - enum face: Options are 'GL_FRONT' for front-face rendering,
             *              'GL_BACK' for back-face rendering, and
             *              'GL_FRONT_AND_BACK' for rendering both sides.
             * - enum property: this varies on what you are setting up
             *                  e.g. 'GL_AMBIENT' for ambient reflectance
             * - float* values: a set of values for the specified property
             *                  e.g. an array of RGB values for the reflectance
             *
             * The 'glMaterialf' function is the same, except the third
             * parameter is only a single float value instead of an array of
             * values. 'glMaterialf' is used to set the shininess property.
             */

             float * ambient = new float[3];
             ambient[0] = instances[i].material.ambient.r;
             ambient[1] = instances[i].material.ambient.g;
             ambient[2] = instances[i].material.ambient.b;

             float * diffuse = new float[3];
             diffuse[0] = instances[i].material.diffuse.r;
             diffuse[1] = instances[i].material.diffuse.g;
             diffuse[2] = instances[i].material.diffuse.b;

             float * specular = new float[3];
             specular[0] = instances[i].material.specular.r;
             specular[1] = instances[i].material.specular.g;
             specular[2] = instances[i].material.specular.b;

            glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
            glMaterialf(GL_FRONT, GL_SHININESS, instances[i].material.shininess);
            
            /* The next few lines of code are how we tell OpenGL to render
             * geometry for us. First, let us look at the 'glVertexPointer'
             * function.
             * 
             * 'glVertexPointer' tells OpenGL the specifications for our
             * "vertex array". As a recap of the comments from the 'Object'
             * struct, the "vertex array" stores all the faces of the surface
             * we want to render. The faces are stored in the array as
             * consecutive points. For instance, if our surface were a cube,
             * then our "vertex array" could be the following:
             *
             * [face1vertex1, face1vertex2, face1vertex3, face1vertex4,
             *  face2vertex1, face2vertex2, face2vertex3, face2vertex4,
             *  face3vertex1, face3vertex2, face3vertex3, face3vertex4,
             *  face4vertex1, face4vertex2, face4vertex3, face4vertex4,
             *  face5vertex1, face5vertex2, face5vertex3, face5vertex4,
             *  face6vertex1, face6vertex2, face6vertex3, face6vertex4]
             * 
             * Obviously to us, some of the vertices in the array are repeats.
             * However, the repeats cannot be avoided since OpenGL requires
             * this explicit specification of the faces.
             *
             * The parameters to the 'glVertexPointer' function are as
             * follows:
             *
             * - int num_points_per_face: this is the parameter that tells
             *                            OpenGL where the breaks between
             *                            faces are in the vertex array.
             *                            Below, we set this parameter to 3,
             *                            which tells OpenGL to treat every
             *                            set of 3 consecutive vertices in
             *                            the vertex array as 1 face. So
             *                            here, our vertex array is an array
             *                            of triangle faces.
             *                            If we were using the example vertex
             *                            array above, we would have set this
             *                            parameter to 4 instead of 3.
             * - enum type_of_coordinates: this parameter tells OpenGL whether
             *                             our vertex coordinates are ints,
             *                             floats, floats, etc. In our case,
             *                             we are using floats, hence 'GL_FLOAT'.
             * - sizei stride: this parameter specifies the number of bytes
             *                 between consecutive vertices in the array.
             *                 Most often, you will set this parameter to 0
             *                 (i.e. no offset between consecutive vertices).
             * - void* pointer_to_array: this parameter is the pointer to
             *                           our vertex array.
             */
            glVertexPointer(3, GL_FLOAT, 0, &instances[i].v[0]);
            /* The "normal array" is the equivalent array for normals.
             * Each normal in the normal array corresponds to the vertex
             * of the same index in the vertex array.
             *
             * The 'glNormalPointer' function has the following parameters:
             *
             * - enum type_of_normals: e.g. int, float, float, etc
             * - sizei stride: same as the stride parameter in 'glVertexPointer'
             * - void* pointer_to_array: the pointer to the normal array
             */
            glNormalPointer(GL_FLOAT, 0, &instances[i].vn[0]);
            
            int buffer_size = instances[i].v.size();
            
            if(!wireframe_mode)
                /* Finally, we tell OpenGL to render everything with the
                 * 'glDrawArrays' function. The parameters are:
                 * 
                 * - enum mode: in our case, we want to render triangles,
                 *              so we specify 'GL_TRIANGLES'. If we wanted
                 *              to render squares, then we would use
                 *              'GL_QUADS' (for quadrilaterals).
                 * - int start_index: the index of the first vertex
                 *                    we want to render in our array
                 * - int num_vertices: number of vertices to render
                 *
                 * As OpenGL renders all the faces, it automatically takes
                 * into account all the specifications we have given it to
                 * do all the lighting calculations for us. It also applies
                 * the Modelview and Projection matrix transformations to
                 * the vertices and converts everything to screen coordinates
                 * using our Viewport specification. Everything is rendered
                 * onto the off-screen buffer.
                 */
                glDrawArrays(GL_TRIANGLES, 0, buffer_size);
            else
                /* If we are in "wireframe mode" (see the 'key_pressed'
                 * function for more information), then we want to render
                 * lines instead of triangle surfaces. To render lines,
                 * we use the 'GL_LINE_LOOP' enum for the mode parameter.
                 * However, we need to draw each face frame one at a time
                 * to render the wireframe correctly. We can do so with a
                 * for loop:
                 */
                for(int j = 0; j < buffer_size; j += 3)
                    glDrawArrays(GL_LINE_LOOP, j, 3);
        }
        /* As discussed before, we use 'glPopMatrix' to get back the
         * version of the Modelview Matrix that we had before we specified
         * the object transformations above. We then move on in our loop
         * to the next object we want to render.
         */
        glPopMatrix();
    }
    
    
    /* The following code segment uses OpenGL's built-in sphere rendering
     * function to render the blue-ground that you are walking on when
     * you run the program. The blue-ground is just the surface of a big
     * sphere of radius 100.
     */
    /*glPushMatrix();
    {
        glTranslatef(0, -103, 0);
        glutSolidSphere(100, 100, 100);
    }
    glPopMatrix();*/
}

/* 'display' function:
 * 
 * You will see down below in the 'main' function that whenever we create a
 * window in OpenGL, we have to specify a function for OpenGL to call whenever
 * it wants to render anything. We typically name this function 'display' or
 * 'render'.
 *
 * The 'display' function is supposed to handle all the processing of points
 * in world and camera space.
 */
void display(void)
{
    // Reset color buffer = setting to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Reset OpenGL's Modelview Matrix
    glLoadIdentity();
    
    // Set camera rotations set by mouse
    /* - float angle: rotation angle in DEGREES
     * - float x: x-component of rotation axis
     * - float y: y-component of rotation axis
     * - float z: z-component of rotation axis
     * See the 'mouse_moved' function for more details for 
     * y_view_angle and x_view_angle
     */
    // glRotatef(y_view_angle, 1, 0, 0);
    // glRotatef(x_view_angle, 0, 1, 0);
    
    /* 
     * Specify the inverse rotation of the camera by its
     * orientation angle about its orientation axis:
     */
    glRotatef(-cam.orientation[3], // cam orientation angle
              cam.orientation[0], cam.orientation[1], cam.orientation[2]);
    /* We then specify the inverse translation of the camera by its position using
     * the 'glTranslatef' function, which takes the following parameters in the
     * following order:
     *
     * - float x: x-component of translation vector
     * - float y: x-component of translation vector
     * - float z: x-component of translation vector
     */
    glTranslatef(-cam.position[0], -cam.position[1], -cam.position[2]);
    /* ^ And that should be it for the camera transformations.
     */
    
    set_lights();

    // TODO
    draw_objects();

    glutSwapBuffers();
}

void create_lights(string light_text){
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
}

// Parses .obj file
void parse_points(Obj &obj, char * filename){
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

  // Construct vertex buffer
  vector<Vertex> vertex_buffer;
  for (auto &f_i : faces){
    vertex_buffer.push_back(vertices[f_i.x]);
    vertex_buffer.push_back(vertices[f_i.y]);
    vertex_buffer.push_back(vertices[f_i.z]);
  }

  // Construct normal buffer
  vector<Vertex> normal_buffer;
  for (auto &fn_i : normals){
    normal_buffer.push_back(surfacenormals[fn_i.x]);
    normal_buffer.push_back(surfacenormals[fn_i.y]);
    normal_buffer.push_back(surfacenormals[fn_i.z]);
  }

  obj.v = vertex_buffer;
  obj.vn = normal_buffer;
  obj.f = faces;
  obj.fn = normals;

}  

void parse_transform(vector<GeoTransform> &all, char * transform_text){
  // Read in transform data from file
  istringstream infile(transform_text);
  char type;
  float x, y, z, a;
  
  while (infile >> type)
  {    
    if (type == 't' || 's'){
      // TRANSLATE MATRIX
      infile >> x >> y >> z;

      GeoTransform t;
      t.type = type;
      vector<float> params {x, y, z};
      t.params = params;
      
      all.push_back(t);
    } else if (type == 'r'){
      // ROTATE MATRIX
      infile >> x >> y >> z >> a;

      GeoTransform t;
      t.type = type;
      vector<float> params {x, y, z, a};
      t.params = params;
      all.push_back(t);
    }
  }
}


void create_originals(string originals_text){
    // Assume format: "objectname objectfile.obj"
    string objectname;
    char objectfile[500];

    istringstream line_stream(originals_text);
    while (line_stream >> objectname >> objectfile){
        Obj obj;
        parse_points(obj, objectfile);
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
    
  char transform_data[1000];
  Material material;
  string objectname;
  istringstream line_stream(instance_text);
  
  Obj obj;
  vector<GeoTransform> transform_list;
  string cur_line;
  string param;
  while (line_stream >> param){
    if (objectname.size() == 0){
      // Starting new object transform
      // Copy out base object from "original"
      objectname = param;
      obj = original[objectname];

    } else {
      
      if (param.size() == 1){
        // Param is a transform (s, r, or t)
        // Collect transform data
        strcat(transform_data, cur_line.c_str());
      } else if (param == "//") { // Got to empty line
      
        // Parse out series of geometric transforms
        parse_transform(transform_list, transform_data);
        // Store transform list
        obj.geo_transform = transform_list;

        // Store material data
        obj.material = material;

        // Prep for next object
        //   Not resetting material 
        //     (assuming that all params will be 
        //     replaced by new data)
        objectname = "";
        strcpy(transform_data, "");
      } else {
        //cout << "Param = " << param << endl;
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
      } // end of param checking
    } // end of case if line = param
  }
  // Execute transform of last thing
  parse_transform(transform_list, transform_data);
  // Store transform matrix of last thing
  obj.geo_transform = transform_list;
  // Store material data of last thing
  obj.material = material;

  // Store completed instance object
  instances.push_back(obj);

  /*cout << "Final instance: -----" << endl;
  debug_obj(obj);*/
  
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
  //debug_camera(cam);

  //////////////////////////////
  // Generate lights
  ///////////////////////////////
  string light_text;
  while (getline(infile, line) && !line.empty()){
    light_text += line + " ";
  }
  create_lights(light_text);
  //debug_lights(lights);

  //////////////////////////////
  // Generate "originals" map associating name with object template
  ///////////////////////////////
  getline(infile, line);
  assert(line == "objects:");
  string originals_text;
  while (getline(infile, line) && !line.empty()){
    // Assume format: "objectname objectfile.obj"
    originals_text += line + " ";
  }
  create_originals(originals_text);

  ///////////////////////////////
  // Generate map associating object with list of transforms
  ///////////////////////////////
  
  string instance_text;
  while (getline(infile, line)){
    if (line.size() == 0){
        instance_text += " \\ ";
    } else {
        instance_text += line + " ";
    }
  }
  create_instances(instance_text);
  /*for (auto &i : instances){
    cout << "Instance-----" << endl;
    debug_obj(i);
  }*/

}




/* 'init' function:
 * 
 * The most important task of the 'init' function is to set OpenGL to the
 * states that we want it to be in.
 */
void init(string textfile)
{
    /* The following line of code tells OpenGL to use "smooth shading" (aka
     * Gouraud shading) when rendering.
     */
    glShadeModel(GL_SMOOTH);
    
    /* Use "culling" when rendering. 
     * Specify the particular "culling" technique
     * we want it to use is backface culling.
     */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    /* The following line tells OpenGL to use depth buffering when rendering.
     */
    glEnable(GL_DEPTH_TEST);
    
     /* The following line tells OpenGL to automatically normalize our normal
     * vectors before it passes them into the normal arrays discussed below.
     * This is required for correct lighting, but it also slows down our
     * program. An alternative to this is to manually scale the normal vectors.
     */
    glEnable(GL_NORMALIZE);
    
    /* The following two lines tell OpenGL to enable its "vertex array" and
     * "normal array" functionality. More details on these arrays are given
     * in the comments on the 'Object' struct and the 'draw_objects' and
     * 'create_objects' functions.
     */
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    
    /* The next 4 lines work with OpenGL's two main matrices: the "Projection
     * Matrix" and the "Modelview Matrix". Only one of these two main matrices
     * can be modified at any given time. We specify the main matrix that we
     * want to modify with the 'glMatrixMode' function.
     *
     * The Projection Matrix is the matrix that OpenGL applies to points in
     * camera space. For our purposes, we want the Projection Matrix to be
     * the perspective projection matrix, since we want to convert points into
     * NDC after they are in camera space.
     *
     * The line of code below:
     */
    glMatrixMode(GL_PROJECTION);
    /* ^tells OpenGL that we are going to modify the Projection Matrix. From
     * this point on, any matrix comamnds we give OpenGL will affect the
     * Projection Matrix. For instance, the line of code below:
     */
    glLoadIdentity();
    /* ^tells OpenGL to set the current main matrix (which is the Projection
     * Matrix right now) to the identity matrix. Then, the next line of code:
     */

    parse(textfile);
    //cout << "Parsed objects!" << endl;
    glFrustum(cam.perspective["left"], cam.perspective["right"],
              cam.perspective["bottom"], cam.perspective["top"],
              cam.perspective["near"], cam.perspective["far"]);

    
    /* The following line of code tells OpenGL that we are going to modify the
     * Modelview Matrix. From this point on, any matrix commands we give OpenGL
     * will affect the Modelview Matrix.
     *
     * We generally modify the Modelview Matrix in the 'display' function.
     */
    glMatrixMode(GL_MODELVIEW);
    //cout << "glMatrixMode(GL_MODELVIEW)" << endl;
    /* The next two lines call our 2 helper functions that create some Point
     * Light and Object structs for us to render.
     *
     * The reason we have these procedures as separate functions is to make
     * the code more organized.
     */
    //create_objects();
    //create_lights();
    
    init_lights();
}


void reshape(int width, int height)
{
    /* The following two lines of code prevent the width and height of the
     * window from ever becoming 0 to prevent divide by 0 errors later.
     * Typically, we let 1x1 square pixel be the smallest size for the window.
     */
    height = (height == 0) ? 1 : height;
    width = (width == 0) ? 1 : width;
    
    /* The 'glViewport' function tells OpenGL to determine how to convert from
     * NDC to screen coordinates given the dimensions of the window. The
     * parameters for 'glViewport' are (in the following order):
     *
     * - int x: x-coordinate of the lower-left corner of the window in pixels
     * - int y: y-coordinate of the lower-left corner of the window in pixels
     * - int width: width of the window
     * - int height: height of the window
     *
     * We typically just let the lower-left corner be (0,0).
     *
     * After 'glViewport' is called, OpenGL will automatically know how to
     * convert all our points from NDC to screen coordinates when it tries
     * to render them.
     */
    glViewport(0, 0, width, height);
    
    /* The following two lines are specific to updating our mouse interface
     * parameters. Details will be given in the 'mouse_moved' function.
     */

    mouse_scale_x = (float) (cam.perspective["right"] - cam.perspective["left"])
                   / (float) width;
    mouse_scale_y = (float) (cam.perspective["top"] - cam.perspective["bottom"])
                   / (float) height;
    
    /* The following line tells OpenGL that our program window needs to
     * be re-displayed, meaning everything that was being displayed on
     * the window before it got resized needs to be re-rendered.
     */
    glutPostRedisplay();
}

/* 'mouse_pressed' function:
 * 
 * This function is meant to respond to mouse clicks and releases. The
 * parameters are:
 * 
 * - int button: the button on the mouse that got clicked or released,
 *               represented by an enum
 * - int state: either 'GLUT_DOWN' or 'GLUT_UP' for specifying whether the
 *              button was pressed down or released up respectively
 * - int x: the x screen coordinate of where the mouse was clicked or released
 * - int y: the y screen coordinate of where the mouse was clicked or released
 *
 * The function doesn't really do too much besides set some variables that
 * we need for the 'mouse_moved' function.
 */
void mouse_pressed(int button, int state, int x, int y)
{
    /* If the left-mouse button was clicked down, then...
     */
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        /* Store the mouse position in our global variables.
         */
        mouse_x = x;
        mouse_y = y;
        
        /* Since the mouse is being pressed down, we set our 'is_pressed"
         * boolean indicator to true.
         */
        is_pressed = true;
    }
    /* If the left-mouse button was released up, then...
     */
    else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        /* Mouse is no longer being pressed, so set our indicator to false.
         */
        is_pressed = false;
    }
}

/* 'mouse_moved' function:
 *
 * This function is meant to respond to when the mouse is being moved. There
 * are just two parameters to this function:
 * 
 * - int x: the x screen coordinate of where the mouse was clicked or released
 * - int y: the y screen coordinate of where the mouse was clicked or released
 *
 * We compute our camera rotation angles based on the mouse movement in this
 * function.
 */
void mouse_moved(int x, int y)
{
    /* If the left-mouse button is being clicked down...
     */
    if(is_pressed)
    {
        /* You see in the 'mouse_pressed' function that when the left-mouse button
         * is first clicked down, we store the screen coordinates of where the
         * mouse was pressed down in 'mouse_x' and 'mouse_y'. When we move the
         * mouse, its screen coordinates change and are captured by the 'x' and
         * 'y' parameters to the 'mouse_moved' function. We want to compute a change
         * in our camera angle based on the distance that the mouse traveled.
         *
         * We have two distances traveled: a dx equal to 'x' - 'mouse_x' and a
         * dy equal to 'y' - 'mouse_y'. We need to compute the desired changes in
         * the horizontal (x) angle of the camera and the vertical (y) angle of
         * the camera.
         * 
         * Let's start with the horizontal angle change. We first need to convert
         * the dx traveled in screen coordinates to a dx traveled in camera space.
         * The conversion is done using our 'mouse_scale_x' variable, which we
         * set in our 'reshape' function. We then multiply by our 'x_view_step'
         * variable, which is an arbitrary value that determines how "fast" we
         * want the camera angle to change. Higher values for 'x_view_step' cause
         * the camera to move more when we drag the mouse. We had set 'x_view_step'
         * to 90 at the top of this file (where we declared all our variables).
         * 
         * We then add the horizontal change in camera angle to our 'x_view_angle'
         * variable, which keeps track of the cumulative horizontal change in our
         * camera angle. 'x_view_angle' is used in the camera rotations specified
         * in the 'display' function.
         */
        x_view_angle += ((float) x - (float) mouse_x) * mouse_scale_x * x_view_step;
        
        /* We do basically the same process as above to compute the vertical change
         * in camera angle. The only real difference is that we want to keep the
         * camera angle changes realistic, and it is unrealistic for someone in
         * real life to be able to change their vertical "camera angle" more than
         * ~90 degrees (they would have to detach their head and spin it vertically
         * or something...). So we decide to restrict the cumulative vertical angle
         * change between -90 and 90 degrees.
         */
        float temp_y_view_angle = y_view_angle +
                                  ((float) y - (float) mouse_y) * mouse_scale_y * y_view_step;
        y_view_angle = (temp_y_view_angle > 90 || temp_y_view_angle < -90) ?
                       y_view_angle : temp_y_view_angle;
        
        /* We update our 'mouse_x' and 'mouse_y' variables so that if the user moves
         * the mouse again without releasing it, then the distance we compute on the
         * next call to the 'mouse_moved' function will be from this current mouse
         * position.
         */
        mouse_x = x;
        mouse_y = y;
        
        /* Tell OpenGL that it needs to re-render our scene with the new camera
         * angles.
         */
        glutPostRedisplay();
    }
}

/* 'deg2rad' function:
 * 
 * Converts given angle in degrees to radians.
 */
float deg2rad(float angle)
{
    return angle * M_PI / 180.0;
}

/* 'key_pressed' function:
 * 
 * This function is meant to respond to key pressed on the keyboard. The
 * parameters are:
 *
 * - unsigned char key: the character of the key itself or the ASCII value of
 *                      of the key
 * - int x: the x screen coordinate of where the mouse was when the key was pressed
 * - int y: the y screen coordinate of where the mouse was when the key was pressed
 *
 * Our function is pretty straightforward as you can see below. We also do not make
 * use of the 'x' and 'y' parameters.
 */
void key_pressed(unsigned char key, int x, int y)
{
    /* If 'q' is pressed, quit the program.
     */
    if(key == 'q')
    {
        exit(0);
    }
    /* If 't' is pressed, toggle our 'wireframe_mode' boolean to make OpenGL
     * render our cubes as surfaces of wireframes.
     */
    else if(key == 't')
    {
        wireframe_mode = !wireframe_mode;
        /* Tell OpenGL that it needs to re-render our scene with the cubes
         * now as wireframes (or surfaces if they were wireframes before).
         */
        glutPostRedisplay();
    }
    else
    {
        /* These might look a bit complicated, but all we are really doing is
         * using our current change in the horizontal camera angle (ie. the
         * value of 'x_view_angle') to compute the correct changes in our x and
         * z coordinates in camera space as we move forward, backward, to the left,
         * or to the right.
         *
         * 'step_size' is an arbitrary value to determine how "big" our steps
         * are.
         *
         * We make the x and z coordinate changes to the camera position, since
         * moving forward, backward, etc is basically just shifting our view
         * of the scene.
         */
        
        float x_view_rad = deg2rad(x_view_angle);
        
        /* 'w' for step forward
         */
        if(key == 'w')
        {
            cam.position[0] += step_size * sin(x_view_rad);
            cam.position[2] -= step_size * cos(x_view_rad);
            glutPostRedisplay();
        }
        /* 'a' for step left
         */
        else if(key == 'a')
        {
            cam.position[0] -= step_size * cos(x_view_rad);
            cam.position[2] -= step_size * sin(x_view_rad);
            glutPostRedisplay();
        }
        /* 's' for step backward
         */
        else if(key == 's')
        {
            cam.position[0] -= step_size * sin(x_view_rad);
            cam.position[2] += step_size * cos(x_view_rad);
            glutPostRedisplay();
        }
        /* 'd' for step right
         */
        else if(key == 'd')
        {
            cam.position[0] += step_size * cos(x_view_rad);
            cam.position[2] += step_size * sin(x_view_rad);
            glutPostRedisplay();
        }
    }
}
/* The 'main' function:
 *
 * This function is short, but is basically where everything comes together.
 */
int main(int argc, char* argv[])
{
    if (argc < 4){
        cout << "Usage: ./shaded_renderer [scene_description_file.txt] [xres] [yres]"
          << endl;
        return 0;
    }
    //cout << "Starting main" << endl;
    // Use input arguments
    xres = atoi(argv[2]);
    yres = atoi(argv[3]);
    textfile = argv[1];
    
    /* 'glutInit' intializes the GLUT (Graphics Library Utility Toolkit) library.
     * This is necessary, since a lot of the functions we used above and below
     * are from the GLUT library.
     *
     * 'glutInit' takes the 'main' function arguments as parameters. This is not
     * too important for us, but it is possible to give command line specifications
     * to 'glutInit' by putting them with the 'main' function arguments.
     */
    glutInit(&argc, argv);
    //cout << "glutInit" << endl;
    /* The following line of code tells OpenGL that we need a float buffer,
     * a RGB pixel buffer, and a depth buffer.
     */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    //cout << "glutInitDisplayMode" << endl;
    /* The following line tells OpenGL to create a program window of size
     * 'xres' by 'yres'.
     */
    glutInitWindowSize(xres, yres);
    //cout << "glutInitWindowSize" << endl;
    /* The following line tells OpenGL to set the program window in the top-left
     * corner of the computer screen (0, 0).
     */
    glutInitWindowPosition(0, 0);
    //cout << "glutInitWindowPosition" << endl;
    /* The following line tells OpenGL to name the program window "Test".
     */
    glutCreateWindow("Test");
    //cout << "glutCreateWindow" << endl;
    /* Call our 'init' function...
     */
    init(textfile);
    //cout << "init" << endl;
    /* Specify to OpenGL our display function.
     */
    glutDisplayFunc(display);
    // /* Specify to OpenGL our reshape function.
    //  */
    glutReshapeFunc(reshape);
    // cout << "glutReshapeFunc" << endl;
    // /* Specify to OpenGL our function for handling mouse presses.
    //  */
    glutMouseFunc(mouse_pressed);
    // cout << "glutMouseFunc" << endl;
    // /* Specify to OpenGL our function for handling mouse movement.
    //  */
    glutMotionFunc(mouse_moved);
    // cout << "glutMotionFunc" << endl;
    //  Specify to OpenGL our function for handling key presses.
     
    glutKeyboardFunc(key_pressed);
    // cout << "glutKeyboardFunc" << endl;
    // /* The following line tells OpenGL to start the "event processing loop". This
    //  * is an infinite loop where OpenGL will continuously use our display, reshape,
    //  * mouse, and keyboard functions to essentially run our program.
    //  */
    glutMainLoop();
    cout << "glutMainLoop" << endl;
}

