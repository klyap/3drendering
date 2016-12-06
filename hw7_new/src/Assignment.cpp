#include "Assignment.hpp"

#include "model.hpp"
#include "UI.hpp"
#include "Scene.hpp"
#include "PNGMaker.hpp"

#define XRES 250
#define YRES 250

using namespace std;

namespace Assignment {
    /* Assignment Part A, Part 1 */
    static const float k_min_io_test_x = -7.0;
    static const float k_max_io_test_x = 7.0;
    static const float k_min_io_test_y = -7.0;
    static const float k_max_io_test_y = 7.0;
    static const float k_min_io_test_z = -7.0;
    static const float k_max_io_test_z = 7.0;
    static const int k_io_test_x_count = 15;
    static const int k_io_test_y_count = 15;
    static const int k_io_test_z_count = 15;

    // BEGIN -----------------------------------------------
    Matrix4f makeRotationQuat(Vector4f unit_q){
      // Convert to unit quarternion
      cout << "ini rotation quarternion: " << unit_q << endl;
      unit_q = unit_q.normalized();
      cout << "rotation normalized: " << unit_q << endl;
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

      Matrix4f r(4,4);
      r << a0, a1, a2, 0,
           b0, b1, b2, 0,
           c0, c1, c2, 0,
           0, 0, 0, 1;

      return r;
    }

    Matrix4f makeTransform(Transformation t) {
      TransformationType type = t.type;
      Matrix4f transform(4,4);

      if (type == TRANS){
        cout << "Transform stack: trans " << type << endl;
        transform << 1, 0, 0, -t.trans[0],
                 0, 1, 0, -t.trans[1],
                 0, 0, 1, -t.trans[2],
                 0, 0, 0, 1;
      }
      else if (type == SCALE){
        cout << "Transform scale: scale " << type << endl;
        transform <<
                 1/t.trans[0], 0, 0, 0,
                 0, 1/t.trans[1], 0, 0,
                 0, 0, 1/t.trans[2], 0,
                 0, 0, 0, 1;

      } else if (type == ROTATE){
        // Given coords in quaternion
        // Convert to normal axes and invert
        cout << "Transform scale: rotate " << type << endl;
        //makeRotateMat(float *matrix, float x, float y, float z, float angle);
        transform = makeRotationQuat(t.trans);
      }

      cout << transform << endl;
      return transform;
    }

    float sq_io(float x, float y, float z, float e, float n){
      float test = pow(pow(x * x, 1/e) + pow(y * y, 1/e), e/n) + pow(z*z, 1/n) - 1;
      // cout << "sq_io: " << test << endl;
      // if (test < 0){
      //   // inside object
      //   return -1;
      // } else if (test > 0){
      //   //outside object
      //   return 1;
      // }
      // //on surface

      //return 0;

      return test;
    }

    //The normal at any point xyz is simply the gradient of the sq_io function
    Vector3f grad_sq_io(float x, float y, float z, float e, float n){
      float dx = 2 * x * pow(x * x, 1/e - 1) * pow(pow(x * x, 1/e) + pow(y * y, 1/e), e/n-1);
      float dy = 2 * y * pow(y * y, 1/e - 1) * pow(pow(x * x, 1/e) + pow(y * y, 1/e), e/n-1);
      float dz = (2 * z * pow(z * z, 1/n - 1))/ n;

      return Vector3f(dx, dy, dz);
    }

//----------------------------------------------------------------------------

    bool IOTest(
        Renderable *ren,
        vector<Transformation> &transformation_stack,
        float x,
        float y,
        float z)
    {
        bool IO_result = false;
        if (ren->getType() == PRM) {
            Primitive *prm = dynamic_cast<Primitive*>(ren);

            /* TODO
             *
             *     Apply inverse of the transformations passed in through
             * transformation_stack to (x, y, z) and perform the IO test
             * oulined in the lecture notes. If the point is inside the
             * primitive, return true. Otherwise return false
             **/

             Vector4f coords(x, y, z, 1);
             Vector3f ini_scale_coords = prm->getCoeff();

             Transformation ini_scale =
                Transformation(SCALE, ini_scale_coords[0], ini_scale_coords[1],ini_scale_coords[2],1.0);
             Matrix4f transform = makeTransform(ini_scale);
             coords = coords.transpose() * transform;
             cout << "coords after init scale: " << coords << endl;

             for (int i = 0; i < transformation_stack.size(); i++){
               Matrix4f transform = makeTransform(transformation_stack.at(i));
               coords = coords.transpose() * transform;
             }

             // Do in/out test
             float check_inside = sq_io(coords[0], coords[1], coords[2],
               prm->getExp0(), prm->getExp1());
             cout << "sq_io returned: " << check_inside << endl;
             if (check_inside < 0){
               cout << "check inside is -" << endl;
               return true;
             }
            return false;

        } else if (ren->getType() == OBJ) {
            Object *obj = dynamic_cast<Object*>(ren);
            const vector<Transformation>& overall_trans =
                obj->getOverallTransformation();
            for (int i = overall_trans.size() - 1; i >= 0; i--) {
                transformation_stack.push_back(overall_trans.at(i));
            }

            // Iterating over tree
            //bool IO_result = false;
            for (auto& child_it : obj->getChildren()) {
                const vector<Transformation>& child_trans =
                    child_it.second.transformations;
                for (int i = child_trans.size() - 1; i >= 0; i--) {
                    transformation_stack.push_back(child_trans.at(i));
                }
                IO_result |= IOTest(
                    Renderable::get(child_it.second.name),
                    transformation_stack,
                    x, y, z);
                transformation_stack.erase(
                    transformation_stack.end() - child_trans.size(),
                    transformation_stack.end());
            }

            transformation_stack.erase(
                transformation_stack.end() - overall_trans.size(),
                transformation_stack.end());
        } else {
            fprintf(stderr, "Renderer::draw ERROR invalid RenderableType %d\n",
                ren->getType());
            exit(1);
        }

        return IO_result;
    }


    void drawIOTest() {
        const Line* cur_state = CommandLine::getState();
        Renderable* current_selected_ren = NULL;

        if (cur_state) {
            current_selected_ren = Renderable::get(cur_state->tokens[1]);
        }

        if (current_selected_ren == NULL) {
            return;
        }

        const float IO_test_color[3] = {0.5, 0.0, 0.0};
        glMaterialfv(GL_FRONT, GL_AMBIENT, IO_test_color);
        for (int x = 0; x < k_io_test_x_count; x++) {
            for (int y = 0; y < k_io_test_y_count; y++) {
                for (int z = 0; z < k_io_test_z_count; z++) {
                    float test_x = k_min_io_test_x
                        + x * (k_max_io_test_x - k_min_io_test_x)
                            / (float) k_io_test_x_count;
                    float test_y = k_min_io_test_y
                        + y * (k_max_io_test_y - k_min_io_test_y)
                            / (float) k_io_test_y_count;
                    float test_z = k_min_io_test_z
                        + z * (k_max_io_test_z - k_min_io_test_z)
                            / (float) k_io_test_z_count;

                    vector<Transformation> transformation_stack;
                    if (IOTest(
                            current_selected_ren,
                            transformation_stack,
                            test_x,
                            test_y,
                            test_z))
                    {
                        glPushMatrix();
                        glTranslatef(test_x, test_y, test_z);
                        glutWireSphere(0.05, 4, 4);
                        glPopMatrix();
                        cout << "IOTEST is true" << endl;
                    }
                }
            }
        }
    }

    /* Assignment Part A, Part 2 */
    struct Ray {
        float origin_x;
        float origin_y;
        float origin_z;

        float direction_x;
        float direction_y;
        float direction_z;

        Vector3f getLocation(float t) {
            Vector3f loc;
            loc << origin_x + t * direction_x,
                origin_y + t * direction_y,
                origin_z + t * direction_z;
            return loc;
        }
    };



    float getTplus(float a, float b, float c) {
      return 2.0 * c / (-b - sqrt(b*b - 4*a*c));
    }

    float getTminus(float a, float b, float c) {
      return (-b - sqrt(b*b - 4.0*a*c)) / (2.0 * a);
    }

    Vector3f getRay(float t, Vector3f av, Vector3f bv){
      cout << "Got ray: " << av * t + bv << endl;
      return av * t + bv;
    }

    // Finds final t based on tplus and tminus conditions
    pair<float, Vector3f> newton(Vector3f av, Vector3f bv, float e, float n){
      cout << "== newton == "<< endl;
      float a = av.dot(av);
      float b = 2.0 * av.dot(bv);
      float c = bv.dot(bv) - 3.0;

      cout << "== abc: " << a << " " << b << " " << c << endl;

      float tminus = getTminus(a, b, c);
      float tplus = getTplus(a, b, c);
      float d =  b*b - 4.0*a*c; // discriminant
      float t_chosen = 0.0;
      Vector3f ray = Vector3f(0,0,0);
      cout << "tminus = " << tminus << " tplus = " << tplus << endl;

      if (d < 0){
        cout << "== d < 0 ==" << d << endl;
        return make_pair(t_chosen, Vector3f(0,0,0));
      } else if (tminus == tplus){
        cout << "== equal ==" << endl;
        t_chosen = tminus;
        // return make_pair(t_chosen,
        //   getRay(iter_newton(tminus, av, bv, e, n), av, bv));
      } else if (tminus > 0 && tplus > 0){ // Start of 2 solution cases
        cout << "== both + ==" << endl;
        t_chosen = tminus;
        // return make_pair(t_chosen,
        //   getRay(iter_newton(tminus, av, bv, e, n), av, bv));
        return iter_newton(tminus, av, bv, e, n);
      } else if (tminus * tplus < 0) { // they're of opposite signs
          cout << "== opposite signs ==" << endl;
          // float actual_tminus = iter_newton(tminus, av, bv, e, n);
          // float actual_tplus = iter_newton(tplus, av, bv, e, n);
          //
          // int test_minus = sign(actual_tminus);
          // int test_plus = sign(actual_tplus);
          //
          // if (test_minus + test_plus == 0){
          //   // Different signs
          //   t_chosen = actual_tminus;
          //   return make_pair(t_chosen, getRay(actual_tminus, av, bv));
          // } else if (test_minus + test_plus > 0){
          //   // They're both positive
          //   t_chosen = min(actual_tminus, actual_tplus);
          //   return make_pair(t_chosen, getRay(t_chosen, av, bv));
          // } else {
          //   // They're both negative
          //   return make_pair(t_chosen, ray);
          // }
      }
      // else {
      //   // t+ and t- are both negative
      //   return Vector3f(0,0,0);
      // }
      cout << "== newton didn't go into any tests ==" << endl;
      return make_pair(t_chosen, ray); // should default to Vector3f(0,0,0)
    }

    // Gets final t
    pair<float, Vector3f> iter_newton(float t, Vector3f av, Vector3f bv, float e, float n){
      cout << "== Iter newton ==" << endl;
      Vector3f rayt = getRay(t, av, bv);
      float gprime = av.dot(grad_sq_io(rayt[0], rayt[1], rayt[2], e, n));
      float g = sq_io(rayt[0], rayt[1], rayt[2], e, n);
      cout << "==INIT g prime: " << gprime << " g: " << g << endl;
      cout << "==INIT t: " << t << endl;
      while ((-1.0/20 >= g || g >= 1.0/20) && gprime <= 0.0){ // stopping condition
        rayt = getRay(t, av, bv);
        gprime = av.dot(grad_sq_io(rayt[0], rayt[1], rayt[2], e, n));
        g = sq_io(rayt[0], rayt[1], rayt[2], e, n);
        t = t - g/gprime;

        cout << "==NEW g prime: " << gprime << " g: " << g << endl;
        cout << "==NEW t: " << t << endl;
      }
      cout << "==FINAL g prime: " << gprime << " g: " << g << endl;
      cout << "==FINAL t: " << t << endl;

      return make_pair(t, rayt);
    }

    void recurse_findIntersection(
      float &t, Vector3f &ray, Vector3f &normal,
      Vector3f av, Vector3f bv,
      Renderable *ren,
      vector<Transformation> &transformation_stack){

      if (ren->getType() == PRM) {
        // Got to "leaf" of recursion; do stuff here
        cout << "== In PRM == " << endl;
        Primitive *prm = dynamic_cast<Primitive*>(ren);

        // Apply all transforms to av
        // Apply only scales and rotates to bv
        Vector3f ini_scale_coords = prm->getCoeff();

        Transformation ini_scale =
           Transformation(SCALE, ini_scale_coords[0], ini_scale_coords[1],ini_scale_coords[2],1.0);
        Matrix4f transform = makeTransform(ini_scale);
        Vector4f av4(av[0],av[1],av[2],1);
        av4 = av4.transpose() * transform;
        cout << "av4 origin after init scale: " << av4 << endl;

        for (int i = 0; i < transformation_stack.size(); i++){
          Matrix4f transform = makeTransform(transformation_stack.at(i));
          av4 = av4.transpose() * transform;
        }

        cout << "ray4 origin after all transforms: " << av4 << endl;
        av[0] = av4[0];
        av[1] = av4[1];
        av[2] = av4[2];
        cout << "av origin after all transforms: " << av << endl;

        // Do computations in superquadratic space
        pair<float, Vector3f> new_ray_t =
          newton(av, bv, prm->getExp0(), prm->getExp1());
        float new_t = new_ray_t.first;
        Vector3f new_ray = new_ray_t.second;
        cout << "==Computed t: " << new_t << endl;
        cout << "==Computed ray: " << new_ray << endl;

        // "Return": Set t and ray
        // Choose smallest positive t
        new_t = fmax(0.0, new_t);
        t = min(t, new_t);

        if (t == new_t){
          ray = new_ray;
        }

        // Do computations in superquadratic space
        normal = grad_sq_io(ray[0], ray[1], ray[2],
          prm->getExp0(), prm->getExp1());




        cout << "==Global t: " << t << endl;
        cout << "==Global ray: " << ray << endl;
        cout << "==Global normal: " << normal << endl;

      } else if (ren->getType() == OBJ) {
        cout << "== In obj. About to recurse == " << endl;
        // Iterate over children until you get to a primitive
         Object *obj = dynamic_cast<Object*>(ren);
         const vector<Transformation>& overall_trans =
             obj->getOverallTransformation();
         for (int i = overall_trans.size() - 1; i >= 0; i--) {
             transformation_stack.push_back(overall_trans.at(i));
         }

         // Iterating over tree
         for (auto& child_it : obj->getChildren()) {
             const vector<Transformation>& child_trans =
                 child_it.second.transformations;
             for (int i = child_trans.size() - 1; i >= 0; i--) {
                 transformation_stack.push_back(child_trans.at(i));
             }
             // Updates referenced t and ray with min + t
             // and the associated Vec3f
              recurse_findIntersection(t, ray, normal,
               av, bv,
               Renderable::get(child_it.second.name),
               transformation_stack);
             transformation_stack.erase(
                 transformation_stack.end() - child_trans.size(),
                 transformation_stack.end());
         }

         transformation_stack.erase(
             transformation_stack.end() - overall_trans.size(),
             transformation_stack.end());
      } else {
         fprintf(stderr, "Renderer::draw ERROR invalid RenderableType %d\n",
             ren->getType());
         exit(1);
      }
    }

    Ray findIntersection(const Ray &camera_ray) {
      /* TODO
      *
      **/
      Ray intersection_ray;
      intersection_ray.origin_x = 0.0;
      intersection_ray.origin_y = 1.0;
      intersection_ray.origin_z = 0.0;
      intersection_ray.direction_x = 1.0;
      intersection_ray.direction_y = 0.0;
      intersection_ray.direction_z = 0.0;

      // Direction camera is pointing
      Vector3f av = Vector3f(
          camera_ray.direction_x, camera_ray.direction_y, camera_ray.direction_z);;
      // Position of camera
      Vector3f bv = Vector3f(
        camera_ray.origin_x, camera_ray.origin_y, camera_ray.origin_z);
      cout << "av= " << av << endl;
      cout << "bv= " << bv << endl;

      // Prep for recursing over tree
      const Line* cur_state = CommandLine::getState();
      Renderable* ren = NULL; // current selected ren

      if (cur_state) {
         ren = Renderable::get(cur_state->tokens[1]);
      }

       if (ren == NULL) {
           return intersection_ray;
       }

      // the "return" values
      vector<Transformation> transformation_stack;
      float t = 1000000000000;
      Vector3f ray;
      Vector3f normal;
      recurse_findIntersection(t, ray, normal,
        av, bv, ren, transformation_stack);

      cout << "==Returned ray origin: " << ray << endl;
      cout << "==Returned ray dir/normal: " << ray << endl;


      // Package into Ray obj
      intersection_ray.origin_x = ray[0];
      intersection_ray.origin_y = ray[1];
      intersection_ray.origin_z = ray[2];
      intersection_ray.direction_x = normal[0];
      intersection_ray.direction_y = normal[1];
      intersection_ray.direction_z = normal[2];

      return intersection_ray;
    }

    void drawIntersectTest(Camera *camera) {

        // Vector4f cam_rotation = Vector4f(camera->getAxis()[0],
        //   camera->getAxis()[1], camera->getAxis()[2],
        //   camera->getAngle());

        float matrix[16];
        makeRotateMat(matrix, camera->getAxis()[0],
          camera->getAxis()[1], camera->getAxis()[2],
          degToRad(camera->getAngle()));


        // cout << "-------------------------" << endl;
        // cout << camera->getAxis()[0] << "  " <<
        //   camera->getAxis()[1] << "  " << camera->getAxis()[2] << "  " <<
        //   camera->getAngle() << endl;
        Matrix4f cam_rotation_matrix;
        cam_rotation_matrix <<
          matrix[0], matrix[1], matrix[2], matrix[3],
          matrix[4], matrix[5], matrix[6], matrix[7],
          matrix[8], matrix[9], matrix[10], matrix[11],
          matrix[12], matrix[13], matrix[14], matrix[15];

        // Vector4f cam_ori = Vector4f(0, 0, -1, 1);
        // Vector4f cam_dir = cam_ori.transpose() * cam_rotation_matrix;

        Ray camera_ray;
        // camera_ray.origin_x = camera->getPosition()[0];
        // camera_ray.origin_y = camera->getPosition()[1];
        // camera_ray.origin_z = camera->getPosition()[2];
        camera_ray.origin_x = 0;
        camera_ray.origin_y = 0.3;
        camera_ray.origin_z = 10;
        camera_ray.direction_x = 0.0;
        camera_ray.direction_y = 0.0;
        camera_ray.direction_z = -1.0;
        // camera_ray.direction_x = cam_dir[0];
        // camera_ray.direction_y = cam_dir[1];
        // camera_ray.direction_z = cam_dir[2];

        Ray intersection_ray = findIntersection(camera_ray);



        const float IO_test_color[3] = {0.0, 0.5, 0.5};
        glMaterialfv(GL_FRONT, GL_AMBIENT, IO_test_color);
        glLineWidth(3.0);
        glBegin(GL_LINES);
        glVertex3f(
            intersection_ray.origin_x,
            intersection_ray.origin_y,
            intersection_ray.origin_z);
        Vector3f endpoint = intersection_ray.getLocation(1.0);
        glVertex3f(endpoint[0], endpoint[1], endpoint[2]);
        glEnd();

        cout << "drew camera_ray origin = " << camera_ray.origin_x << " " <<
          camera_ray.origin_y << " " << camera_ray.origin_z << endl;
        cout << "drew intersection_ray origin = " << intersection_ray.origin_x << " " <<
          intersection_ray.origin_y << " " << intersection_ray.origin_z << endl;
        cout << "drew intersection_ray end = " << endpoint << endl;
    }

    /* Assignment Part B */

    /* Ray traces the scene. */
    void raytrace(Camera camera, Scene scene) {
        // LEAVE THIS UNLESS YOU WANT TO WRITE YOUR OWN OUTPUT FUNCTION
        PNGMaker png = PNGMaker(XRES, YRES);

        // REPLACE THIS WITH YOUR CODE
        for (int i = 0; i < XRES; i++) {
            for (int j = 0; j < YRES; j++) {
                png.setPixel(i, j, 1.0, 1.0, 1.0);
            }
        }

        // LEAVE THIS UNLESS YOU WANT TO WRITE YOUR OWN OUTPUT FUNCTION
        if (png.saveImage()) {
            fprintf(stderr, "Error: couldn't save PNG image\n");
        } else {
            printf("DONE!\n");
        }
    }
};
