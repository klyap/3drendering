#include "object.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/* The following are parameters for creating an interactive first-person camera
 * view of the scene. The variables will make more sense when explained in
 * context, so you should just look at the 'mousePressed', 'mouseMoved', and
 * 'keyPressed' functions for the details.
 */

/*int mouse_x, mouse_y;
float mouse_scale_x, mouse_scale_y;

const float step_size = 0.2;
const float x_view_step = 90.0, y_view_step = 90.0;
float x_view_angle = 0, y_view_angle = 0;

bool is_pressed = false;
bool wireframe_mode = false;*/


/* 'deg2rad' function:
 * 
 * Converts given angle in degrees to radians.
 */
float deg2rad(float angle)
{
    return angle * M_PI / 180.0;
}

/* 'deg2rad' function:
 * 
 * Converts given angle in degrees to radians.
 */
float rad2deg(float rad)
{
    return rad * 180.0 /  M_PI;
}


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

    debug_lights(lights);
    
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

        float * color = new float[3];
        
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
        GLuint light_id = GL_LIGHT0 + i;

        float pos_f[4];
        pos_f[0] = lights[i].x;
        pos_f[1] = lights[i].y;
        pos_f[2] = lights[i].z;
        pos_f[3] = 1;
        cout << "SET LIGHTS: ";
        cout << pos_f[0] << " " << pos_f[1] << " " << pos_f[2] << endl;
        glLightfv(light_id, GL_POSITION, pos_f);
    }
}

// int i is index of the object in "instances" that we are
// drawing right now
void apply_geometric_transforms(int i){
  int num_geo_transform = instances[i].geo_transform.size();

  cout << "apply_geometric_transforms: " << i << endl;

  for (int j = num_geo_transform-1; j >= 0; j--){
    debug_transform(instances[i].geo_transform[j]);

    if (instances[i].geo_transform[j].type == "t"){
      glTranslatef(instances[i].geo_transform[j].params[0],
                   instances[i].geo_transform[j].params[1],
                   instances[i].geo_transform[j].params[2]);
    } else if (instances[i].geo_transform[j].type == "r"){
      glRotatef(rad2deg(instances[i].geo_transform[j].params[3]),
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
            
          
            glVertexPointer(3, GL_FLOAT, 0, &instances[i].v[0]);
            
            glNormalPointer(GL_FLOAT, 0, &instances[i].vn[0]);
            
            int buffer_size = instances[i].v.size();
            
            if(!wireframe_mode)
                
                glDrawArrays(GL_TRIANGLES, 0, buffer_size);
            else
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
    
}

MatrixXd Get_Current_Rotation(){
  // Convert to unit quarternion
  /*float norm = sqrt(current_rotation[0] * current_rotation[0] +
               current_rotation[1] * current_rotation[1] +
               current_rotation[2] * current_rotation[2] +
               current_rotation[3] * current_rotation[3]);*/
  //Vector4d unit_q = current_rotation * (1.0/norm);
  
  Vector4d unit_q = current_rotation.normalized();

  float qs = unit_q[0];
  float qx = unit_q[1];
  float qy = unit_q[2];
  float qz = unit_q[3];

  float a0 = 1 - 2 * qy * qy - 2 * qz * qz;
  float a1 = 2 * (qx * qy - qz * qs);
  float a2 = 2 * (qx * qz + qy * qs);

  float b0 = 2 * (qx * qy + qz * qs);
  float b1 = 1 - 2 * qx * qx - 2 * qz * qz;
  float b2 = 2 * (qy * qz - qx * qs);

  float c0 = 2 * (qx * qz - qy * qs);
  float c1 = 2 * (qy * qz + qx * qs);
  float c2 = 1 - 2 * qx * qx - 2 * qy * qy;

  MatrixXd r(4,4);
  r << a0, a1, a2, 0,
       b0, b1, b2, 0,
       c0, c1, c2, 0,
       0, 0, 0, 1;

  return r;
}


/* 'display' function:
 * 
 * You will see down below in the 'main' function that whenever we create a
 * window in OpenGL, we have to specify a function for OpenGL to call whenever
 * it wants to render anything.
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
    glRotatef(y_view_angle, 1, 0, 0);
    glRotatef(x_view_angle, 0, 1, 0);
    
    /* 
     * Specify the inverse rotation of the camera by its
     * orientation angle about its orientation axis:
     */
    glRotatef(-rad2deg(cam.orientation[3]), // cam orientation angle
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

    // Arcball TODO
    glMultMatrixd(Get_Current_Rotation().data());
    
    //set_lights();

    draw_objects();

    glutSwapBuffers();
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
     * Matrix right now) to the identity matrix.
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
    create_originals(originals_text);
    create_instances(instance_text);
    create_lights(light_text);

    init_lights();
    set_lights(); // test
}

// Converts screen coords to NDC coords
void screen_to_NDC(int a, int b, float &x, float &y){
  x = (a + 1)/2.0 * (xres);
  y = yres - ((b + 1)/2.0 * (yres));
  /*x = (a / xres) * 2.0 - 1.0;
  y = 1.0 - ((b + 1) / yres) * 2.0;*/

}

double get_z(int x, int y){
    float xy_squared = x * x + y * y;
    if (xy_squared <= 1){
        return fabs(sqrt(1 - xy_squared));
    }

    return 0;
}

void Compute_Rotation_Quarternion(
    Vector4d &q,
    int px_current, int py_current,
    int px_start, int py_start){

    //p' = px_current, py_current
    //p = px_start, py_start
    
    // Construct p and p'
    // x and y are already in NDC
    float x1, y1, x2, y2;
    x1 = px_start;
    y1 = py_start;
    x2 = px_current;
    y2 = py_current;

    // Get z component
    float z1 = get_z(x1, y1);
    float z2 = get_z(x2, y2);

    // Construct p/p1 (current point) and p'/p2 (new point)
    Vector3d p1(x1, y1, z1);
    Vector3d p2(x2, y2, z2);

    // Find u and theta
    float theta = rad2deg(acos(min(1.0, p1.dot(p2) / (p1.norm() * p2.norm()))));
    Vector3d u = p1.cross(p2).normalized();
    
    // Compute quarternion and assign
    q[0] = cos(theta/2.0);        // qs
    q[1] = u[0] * sin(theta/2.0); // qx
    q[2] = u[1] * sin(theta/2.0); // qy
    q[3] = u[2] * sin(theta/2.0); // qz

}

void reshape(int width, int height)
{
    /* Let 1x1 square pixel be the smallest size for the window. */
    height = (height == 0) ? 1 : height;
    width = (width == 0) ? 1 : width;
    
    /* 
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
    
    /* Re-render */
    glutPostRedisplay();
}

Vector4d q_multiply(Vector4d a,Vector4d b){
  
  Vector3d va(a[1], a[2], a[3]);
  Vector3d vb(b[1], b[2], b[3]);
  Vector3d v;
  Vector4d product;

  float s = a[0] * b[0] - va.dot(vb);
  Vector3d cross = va.cross(vb);
  v = a[0] * vb + b[0] * va + cross;

  product << s, v[0], v[1], v[2];

  return product;
}

/*
 * The function doesn't really do too much besides set some variables that
 * we need for the 'mouse_moved' function.
 */
void mouse_pressed(int button, int state, int x, int y)
{
    // If the left-mouse button was clicked down, then...
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // Store the mouse position in our global variables.
        mouse_x = x;
        mouse_y = y;

        // For ArcBall
        px_start = x;
        py_start = y;

        is_pressed = true;
    }
    // If the left-mouse button was released up, then...
     
    else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        // Mouse is no longer being pressed, so set our indicator to false.
        is_pressed = false;

        // Arcball
        last_rotation = q_multiply(current_rotation,last_rotation);
        current_rotation << 1, 0, 0, 0; // Set to identity
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
    //If the left-mouse button is being clicked down...
     
    if(is_pressed)
    {
      
      px_current = x;
      py_current = y;

      // Update current_rotation with new quarternion
      Compute_Rotation_Quarternion(
        current_rotation,
        px_current, py_current, px_start, py_start);
      
      mouse_x = x;
      mouse_y = y;
      
      // Re-render our scene with the new camera angles
      glutPostRedisplay();
    }
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
        cout << "Usage: ./opengl_shader [scene_description_file.txt] [xres] [yres]"
          << endl;
        return 0;
    }
    //cout << "Starting main" << endl;
    // Use input arguments
    xres = atoi(argv[2]);
    yres = atoi(argv[3]);
    textfile = argv[1];
    
    /* 'glutInit' intializes the GLUT (Graphics Library Utility Toolkit) library.
     */
    glutInit(&argc, argv);
    //cout << "glutInit" << endl;
    
    /* Tell OpenGL that we need a float buffer,
     * a RGB pixel buffer, and a depth buffer.
     */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    //cout << "glutInitDisplayMode" << endl;
    
    /* Create a program window of size
     * 'xres' by 'yres'.
     */
    glutInitWindowSize(xres, yres);
    //cout << "glutInitWindowSize" << endl;
    
    /* Set the program window in the top-left
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
    
}

