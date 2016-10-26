// Original camera movement code before ArcBall

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

        // For ArcBall
        px_start = x;
        py_start = y;
        
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




// ArcBall

// Converts NDC to screen coords
void int_NDC_to_screen(double a, double b, int &x, int &y){
  x = (a + 1)/2.0 * (x_dim);
  y = y_dim - ((b + 1)/2.0 * (y_dim));
}

// Converts screen coords to NDC coords
void screen_to_NDC(double a, double b, int &x, int &y){
  x = (a + 1)/2.0 * (x_dim);
  y = y_dim - ((b + 1)/2.0 * (y_dim));
}

int get_z(int &z, int x, int y){
    float xy_squared = x * x + y * y;
    if (xy_squared <= 1){
        return sqrt(1 - xy_squared);
    }

    return 0;
}
void Compute_Rotation_Quarternion(
    int px_current, int py_current,
    int px_start, int py_start){

    //p' = px_current, py_current
    //p = px_start, py_start
    
    // Construct p and p'
    // Convert xy coords to NDC
    int x1, y1, x2, y2;
    screen_to_NDC(px_current, py_current);
    screen_to_NDC(px_start, py_start);

    // Get z component
    int z1 = get_z(x1, y1);
    int z2 = get_z(x2, y2);

    // Construct p/p1 and p'/p2
    Vector3d p1(x1, y1, z1);
    Vector3d p2(x2, y2, z2);

    // Find u and theta
    float theta = p1.dot(p2) / abs(p1).cross(abs(p2));

    

}






















