/* This is a code snippet for drawing the "I-bar". The 'quadratic' object can be made
 * into a global variable in your program if you want. Line 13, where 'quadratic' gets
 * initialized should be done in your 'init' function or somewhere close to the start
 * of your program.
 *
 * The I-bar is an object that Prof. Al Barr once used in one of his papers to demonstrate
 * an interpolation technique. You might call it "Al Barr's I-Bar" ;)
 */
#include "object.h"

/* Needed to draw the cylinders using glu */
GLUquadricObj *quadratic;
int current_frame = 0;
int p1 = total_keyframes - 1;
int p2 = 0;
int p3 = 1;
int p4 = 2;

Vector3d i_translation;
Vector3d i_scale;
Vector4d i_rotation;

void init()
{
    /* Hard coded constants */
    float near = 1.0;
    float far = 60.0;
    float left = 1.0;
    float right = 1.0;
    float bottom = -1.0;
    float top = 1.0;

    /* Arbitrary light */
    const float light_color[3] = {1, 1, 1};
    const float light_position[3] = {0, 0, -2};

    /* Arbitrary material properties for our pendulum */
    const float ambient_reflect[3] = {0.3, 0.2, 0.4};
    const float diffuse_reflect[3] = {0.7, 0.2, 0.8};
    const float specular_reflect[3] = {1, 1, 1};
    const float shininess = 0.1;


    // ... other code
    glEnableClientState(GL_VERTEX_ARRAY);

    // Similar to how we have vertex arrays and normal arrays,
    // we also have color arrays for setting the colors of each
    // vertex
    glEnableClientState(GL_COLOR_ARRAY);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Trying to fix weird faces
    // glShadeModel(GL_SMOOTH);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    // Set lighting
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_color);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    //Set material properties of our I bar
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_reflect);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_reflect);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_reflect);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    quadratic = gluNewQuadric();
    // ... other code
    create_camera();
    cout << "about to glFrustum" << endl;
    //glFrustum(left, right, bottom, top, near, far);
    glFrustum(cam.perspective["left"], cam.perspective["right"],
              cam.perspective["bottom"], cam.perspective["top"],
              cam.perspective["near"], cam.perspective["far"]);

    glMatrixMode(GL_MODELVIEW);

    cout << "about to parse" << endl;
    parse(file);

}

void drawIBar()
{
    /* Parameters for drawing the cylinders */
    float cyRad = 0.2, cyHeight = 1.0;
    // const float cyRad = 2;
    // const float cyHeight = 10.0;
    int quadStacks = 4, quadSlices = 4;

    glPushMatrix();
    glColor3f(0, 0, 1);
    glTranslatef(0, cyHeight, 0);
    glRotatef(90, 1, 0, 0);
    gluCylinder(quadratic, cyRad, cyRad, 2.0 * cyHeight, quadSlices, quadStacks);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0, 1, 1);
    glTranslatef(0, cyHeight, 0);
    glRotatef(90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1, 0, 1);
    glTranslatef(0, cyHeight, 0);
    glRotatef(-90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1, 1, 0);
    glTranslatef(0, -cyHeight, 0);
    glRotatef(-90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0, 1, 0);
    glTranslatef(0, -cyHeight, 0);
    glRotatef(90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();
}


/* Updates p's and gets u value */
float get_u(){
  float u;
  int n = keyframe_indices.size();

  // When reached end of keyframes and need to loop back to frame 0
  // Parker says I can partition this transition into whatever number of
  // frames I want, so I'm choosing "last_leg" number of frames.
  float last_leg = 10.0;

  if (current_frame == keyframe_indices[n-1] ||
      current_frame > keyframe_indices[n-1] + last_leg ||
      (current_frame < keyframe_indices[n-1] &&
        current_frame > keyframe_indices[p3])){
    // Trigger a rotation of M_M_PIvot points
    p1++;
    p2++;
    p3++;
    p4++;
  }

  p1 = p1 % n;
  p2 = p2 % n;
  p3 = p3 % n;
  p4 = p4 % n;

  cout << "P= " << p1 << " " << p2 << " " << p3 << " " << p4 << endl;

  // Get u
  cout << "Current frame = " << current_frame << endl;
  if (current_frame > keyframe_indices[n-1]){
    // In last leg
    u = (current_frame - keyframe_indices[n-1])  * 1.0 / last_leg;
    cout << "U = " << current_frame - keyframe_indices[n-1]
         << "/" << last_leg
         << endl;
  } else {
    u = fabs(keyframe_indices[p2] - current_frame) * 1.0 /
        fabs(keyframe_indices[p3] - keyframe_indices[p2]);
    cout << "U = " << keyframe_indices[p2] - current_frame
         << "/" << keyframe_indices[p3] - keyframe_indices[p2]
         << endl;
  }
  assert(u >= 0.0 && u <= 1.0);
  cout << "--------" << endl;
  cout << "u = " << u << endl;
  cout << "--------" << endl;

  // Reset current_frame if it is finished with one loop
  if (current_frame >= keyframe_indices[n-1]+last_leg){
    current_frame = -1;
  }

  return u;
}


// Generates a single element (component) in either a
// translation/rotation/scaling vector
// Assume that we have stored the frames
float catmull_rom(float u, Vector4d p){
  MatrixXd u_vec(1, 4);
  u_vec << 1, u, u*u, u*u*u;
  MatrixXd B(4,4);
  B << 0, 1, 0, 0,
      -0.5, 0, 0.5, 0,
      1, -2.5, 2, -0.5,
      -0.5, 1.5, -1.5, 0.5;
  return (u_vec * B * p)[0];

}

Vector4d vec_to_quarternion(vector<float> u){
  // u[0], u[1], u[2], u[3] = x,y,z,theta
  Vector4d q;
  // Compute quarternion and assign
  q[0] = cos(u[3]/2.0 * M_PI / 180.0);        // qs
  q[1] = u[0] * sin(u[3]/2.0 * M_PI / 180.0); // qx
  q[2] = u[1] * sin(u[3]/2.0 * M_PI / 180.0); // qy
  q[3] = u[2] * sin(u[3]/2.0 * M_PI / 180.0); // qz

  cout << "q = " << q << endl;
  return q;
}

Vector4d quarternion_to_vec(Vector4d q){
  // q[1], q[2], q[3] = x,y,z
  // q[0] = theta
  q.normalize();

  Vector4d u;

  if (q[0] * q[0] >= 1){
    // Catch singularity
    // And set u to arbitrary axis
    u[3] = 2.0 * acos(q[0]) * (M_PI / 180.0);
    u[0] = q[1]; // x
    u[1] = q[2]; // y
    u[2] = q[3]; // z
  } else {
    // Compute quarternion and assign
    u[3] = 2.0 * acos(q[0]) * (M_PI / 180.0) ;  // theta
    u[0] = q[1] / sqrt(1 - q[0] * q[0]); // x
    u[1] = q[2] / sqrt(1 - q[0] * q[0]); // y
    u[2] = q[3] / sqrt(1 - q[0] * q[0]); // z
  }


  cout << "u = " << u << endl;
  return u;
}

void update()
{
    // Increment p's if current frame > p_(i+1)
    // keyframe_indices = list of key frame numbers
    //    keyframe_indices[p] = keyframe number for key in "keyframes"
    float u = get_u();

    int frame1 = keyframe_indices[p1];
    int frame2 = keyframe_indices[p2];
    int frame3 = keyframe_indices[p3];
    int frame4 = keyframe_indices[p4];
    cout << "Frames: " << frame1 << " " << frame2 <<
            " " << frame3 << " " << frame4 << endl;

    /* Assume 0 = translate, 1 = scale, 2 = rotate */
    // Translation transform vectors
    GeoTransform t1 = keyframes[frame1][0];
    GeoTransform t2 = keyframes[frame2][0];
    GeoTransform t3 = keyframes[frame3][0];
    GeoTransform t4 = keyframes[frame4][0];
    // Scaling transform vectors
    GeoTransform s1 = keyframes[frame1][1];
    GeoTransform s2 = keyframes[frame2][1];
    GeoTransform s3 = keyframes[frame3][1];
    GeoTransform s4 = keyframes[frame4][1];

    // Translation: Get the x/y/z components and use this as p
    Vector4d t_x(t1.params[0], t2.params[0], t3.params[0], t4.params[0]);
    Vector4d t_y(t1.params[1], t2.params[1], t3.params[1], t4.params[1]);
    Vector4d t_z(t1.params[2], t2.params[2], t3.params[2], t4.params[2]);
    // Scale: Get the x/y/z components and use this as p
    Vector4d s_x(s1.params[0], s2.params[0], s3.params[0], s4.params[0]);
    Vector4d s_y(s1.params[1], s2.params[1], s3.params[1], s4.params[1]);
    Vector4d s_z(s1.params[2], s2.params[2], s3.params[2], s4.params[2]);

    // Update interpolated translation
    i_translation <<
      catmull_rom(u, t_x),
      catmull_rom(u, t_y),
      catmull_rom(u, t_z);

    // Update interpolated scale
    i_scale <<
      catmull_rom(u, s_x),
      catmull_rom(u, s_y),
      catmull_rom(u, s_z);

    // Rotation transform vectors
    GeoTransform r1 = keyframes[frame1][2];
    GeoTransform r2 = keyframes[frame2][2];
    GeoTransform r3 = keyframes[frame3][2];
    GeoTransform r4 = keyframes[frame4][2];
    // Update interpolated rotation
    Vector4d r1_quar = vec_to_quarternion(r1.params);
    Vector4d r2_quar = vec_to_quarternion(r2.params);
    Vector4d r3_quar = vec_to_quarternion(r3.params);
    Vector4d r4_quar = vec_to_quarternion(r4.params);
    // Rotate: Get the x/y/z components and use this as p
    Vector4d r_t(r1_quar[0], r2_quar[0], r3_quar[0], r4_quar[0]);
    Vector4d r_x(r1_quar[1], r2_quar[1], r3_quar[1], r4_quar[1]);
    Vector4d r_y(r1_quar[2], r2_quar[2], r3_quar[2], r4_quar[2]);
    Vector4d r_z(r1_quar[3], r2_quar[3], r3_quar[3], r4_quar[3]);

    Vector4d i_rotation_quar(
      catmull_rom(u, r_t),
      catmull_rom(u, r_x),
      catmull_rom(u, r_y),
      catmull_rom(u, r_z));

    i_rotation = quarternion_to_vec(i_rotation_quar);

    // cout << "i_rotation: ";
    // cout << i_rotation << endl;

    glutPostRedisplay();
}


void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    cout << -cam.position[0] << " " << -cam.position[1] << " " << -cam.position[2] << endl;
    glTranslatef(-cam.position[0], -cam.position[1], -cam.position[2]);

    // Apply transforms on I bar
    if (i_translation.size() > 0){
      glTranslatef(i_translation[0], i_translation[1], i_translation[2]);
      glScalef(i_scale[0], i_scale[1], i_scale[2]);
      glRotatef(i_rotation[3], i_rotation[0], i_rotation[1], i_rotation[2]);
    }

    //glEnable(GL_LIGHTING);
    drawIBar();
    //glDisable(GL_LIGHTING);

    glutSwapBuffers();
}

void reshape(int width, int height)
{
    height = (height == 0) ? 1 : height;
    width = (width == 0) ? 1 : width;

    glViewport(0, 0, width, height);

    glutPostRedisplay();
}

void key_pressed(unsigned char key, int x, int y)
{
    if(key == 'q')
    {
        exit(0);
    }

    if(key == 's')
    {
        update();
        current_frame++;
    }
}


int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        cerr << "\nUsage: ./keyframe [xres] [yres] [script file]" << endl;
        exit(1);
    }

    // xres and yres are given command line arguments
    int xres = atoi(argv[1]);
    int yres = atoi(argv[2]);
    file = argv[3];

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(xres, yres);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Spring Pendulum Demo");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key_pressed);
    glutMainLoop();
}
