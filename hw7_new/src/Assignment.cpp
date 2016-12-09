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
        transform <<
                 1, 0, 0, t.trans[0],
                 0, 1, 0, t.trans[1],
                 0, 0, 1, t.trans[2],
                 0, 0, 0, 1;
      }
      else if (type == SCALE){
        cout << "Transform scale: scale " << type << endl;
        transform <<
                 t.trans[0], 0, 0, 0,
                 0, t.trans[1], 0, 0,
                 0, 0, t.trans[2], 0,
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

    Matrix4f makeInvTransform(Transformation t) {
      TransformationType type = t.type;
      Matrix4f transform(4,4);

      if (type == TRANS){
        cout << "Transform stack: trans " << type << endl;
        transform << 1, 0, 0, -1.0 * t.trans[0],
                 0, 1, 0, -1.0 * t.trans[1],
                 0, 0, 1, -1.0 * t.trans[2],
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
        transform = makeRotationQuat(t.trans).transpose();
      }

      cout << transform << endl;
      return transform;
    }

    float sq_io(float x, float y, float z, float e, float n){
      float test = pow(pow(x * x, 1/e) + pow(y * y, 1/e), e/n) + pow(z*z, 1/n) - 1;

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

             Vector4f coords(x, y, z, 1);
             Vector3f ini_scale_coords = prm->getCoeff();

             Transformation ini_scale =
                Transformation(SCALE, ini_scale_coords[0], ini_scale_coords[1],ini_scale_coords[2],1.0);
             Matrix4f transform = makeInvTransform(ini_scale);
             coords = coords.transpose() * transform;
             cout << "coords after init scale: " << coords << endl;

             for (int i = 0; i < transformation_stack.size(); i++){
               Matrix4f transform = makeInvTransform(transformation_stack.at(i));
               coords = transform * coords;
             }

             // Do in/out test
             float check_inside = sq_io(coords[0], coords[1], coords[2],
               prm->getExp0(), prm->getExp1());
             cout << "sq_io returned: " << check_inside << endl;
             if (check_inside < 0){
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
      return av * t + bv;
    }

    // Finds final t based on tplus and tminus conditions
    // Returns the normal and position (aka new av and bv)
    pair<Vector3f, Vector3f> newton(Vector3f av, Vector3f bv, float e, float n){
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
      Vector3f normal = Vector3f(0,0,0);;

      cout << "tminus = " << tminus << " tplus = " << tplus << endl;

      if (d < 0){
        cout << "== d < 0 ==" << d << endl;
        return make_pair(Vector3f(0,0,0), Vector3f(0,0,0));
      } else if (tminus == tplus){
        cout << "== equal ==" << endl;
        t_chosen = tminus;
        return iter_newton(t_chosen, av, bv, e, n);
        // return make_pair(t_chosen,
        //   getRay(iter_newton(tminus, av, bv, e, n), av, bv));
      } else if (tminus > 0 && tplus > 0){ // Start of 2 solution cases
        cout << "== both + ==" << endl;
        t_chosen = tminus;

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
          //   //return make_pair(t_chosen, getRay(actual_tminus, av, bv));
          //   return iter_newton(t_chosen, av, bv, e, n);
          // } else if (test_minus + test_plus > 0){
          //   // They're both positive
          //   t_chosen = min(actual_tminus, actual_tplus);
          //   //return make_pair(t_chosen, getRay(t_chosen, av, bv));
          //   return iter_newton(t_chosen, av, bv, e, n);
          // } else {
          //   // They're both negative
          //   //return make_pair(t_chosen, ray);
          //   return make_pair(Vector3f(0,0,0), Vector3f(0,0,0));
          // }
      }
      // else {
      //   // t+ and t- are both negative
      //   return Vector3f(0,0,0);
      // }
      cout << "== newton didn't go into any tests ==" << endl;
      return make_pair(normal, ray); // should default to Vector3f(0,0,0)
    }

    // Gets final t; returns normal and position
    pair<Vector3f, Vector3f> iter_newton(float t, Vector3f av, Vector3f bv, float e, float n){
      cout << "== Iter newton ==" << endl;
      Vector3f rayt = getRay(t, av, bv);
      Vector3f normal = grad_sq_io(rayt[0], rayt[1], rayt[2], e, n);
      float gprime = av.dot(normal);
      float g = sq_io(rayt[0], rayt[1], rayt[2], e, n);
      cout << "==INIT g prime: " << gprime << " g: " << g << endl;
      cout << "==INIT t: " << t << endl;
      while ((-1.0/20 >= g || g >= 1.0/20) && gprime <= 0.0){ // stopping condition
        rayt = getRay(t, av, bv);
        gprime = av.dot(grad_sq_io(rayt[0], rayt[1], rayt[2], e, n));
        g = sq_io(rayt[0], rayt[1], rayt[2], e, n);
        t = t - g/gprime;

        // cout << "==NEW g prime: " << gprime << " g: " << g << endl;
        // cout << "==NEW t: " << t << endl;
      }
      if (gprime < 0.0 && (-1.0/20 < g && g < 1.0/20)){
        return make_pair(normal, rayt);
      }
      cout << "==FINAL g prime: " << gprime << " g: " << g << endl;
      cout << "==FINAL t: " << t << endl;

      Vector3f zeroes(0, 0, 0);
      return make_pair(zeroes , zeroes);
    }

    void recurse_findIntersection(
      float &d, Vector3f &ray, Vector3f &normal, Primitive** p,
      Vector3f av, Vector3f bv,
      Renderable *ren,
      vector<Transformation> &transformation_stack){

      Vector3f cam_pos = bv;

      if (ren->getType() == PRM) {
        // Got to "leaf" of recursion; do stuff here
        cout << "== In PRM == " << endl;
        Primitive *prm = dynamic_cast<Primitive*>(ren);

        // Apply all inverse transforms to av
        // Apply only inverse scales and rotates to bv
        Vector4f av4(av[0],av[1],av[2],1);
        //av4 = av4.transpose() * transform;
        // cout << "av4 origin after init scale: " << av4 << endl;
        Vector4f bv4(bv[0],bv[1],bv[2],1);
        // bv4 = bv4.transpose() * transform;
        // cout << "bv4 origin after init scale: " << bv4 << endl;

        Matrix4f forward;
        forward <<
          1, 0, 0, 0,
          0, 1, 0, 0,
          0, 0, 1, 0,
          0, 0, 0, 1;
        Matrix4f forward_SR;
        forward_SR <<
          1, 0, 0, 0,
          0, 1, 0, 0,
          0, 0, 1, 0,
          0, 0, 0, 1;

        // Initial coeff scaling
        Vector3f ini_scale_coords = prm->getCoeff();
        Transformation ini_scale =
           Transformation(SCALE, ini_scale_coords[0], ini_scale_coords[1], ini_scale_coords[2],1.0);
        Matrix4f transform = makeTransform(ini_scale);
        forward *= transform;
        forward_SR *= transform;

        //for (int i = transformation_stack.size(); i > 0; i--){
        for (int i = 0; i < transformation_stack.size(); i++){
          Matrix4f transform = makeTransform(transformation_stack.at(i));
          cout << "Transform stack = " << transform << endl;
          forward *= transform;
          if (transformation_stack.at(i).type == SCALE ||
              transformation_stack.at(i).type == ROTATE){
            // Add to SR
            forward_SR *= transform;
          }
        }

        cout << "forward" << forward << endl;
        cout << "forward_SR" << forward_SR << endl;

        // Direction/normal (SR only)
        Matrix4f forward_SR_inv = forward_SR.inverse();
        av4 = forward_SR_inv * av4;
        cout << "direction after all transforms: " << av4 << endl;
        av[0] = av4[0];
        av[1] = av4[1];
        av[2] = av4[2];
        //cout << "av origin after all transforms: " << av << endl;


        // Position (all transforms)
        Matrix4f forward_inv = forward.inverse();
        bv4 = forward_inv * bv4;
        cout << "position after all transforms: " << bv4 << endl;
        bv[0] = bv4[0];
        bv[1] = bv4[1];
        bv[2] = bv4[2];
        //cout << "bv after all transforms: " << bv << endl;

        // Do newton computations in superquadric space (or whatever it's called)
        pair<Vector3f, Vector3f> new_ray_t =
          newton(av, bv, prm->getExp0(), prm->getExp1());

        Vector3f new_normal = new_ray_t.first;
        Vector3f new_position = new_ray_t.second;
        cout << "==Output position: " << new_position << endl; // New position
        cout << "==Output normal: " << new_normal << endl; // New normal

        // Transform position and normals back out of superquadric space
        // Pull into Vec4 for 4x4 rotation matrices
        Vector4f ray4(new_position[0], new_position[1], new_position[2], 1);
        Vector4f normal4(new_normal[0], new_normal[1], new_normal[2], 1);
        cout << "== Before Position transformed back out: " << ray4 << endl;
        cout << "== Before normal4 transformed back out: " << normal4 << endl;

        // Transform position and normal back into normal coords
        //Matrix4f forward_inv = forward.inverse();
        //Matrix4f forward_inv_t = forward_inv.transpose();
        ray4 = forward * ray4;

        //Matrix4f forward_SR_inv = forward_SR.inverse();
        //Matrix4f forward_SR_inv_t = forward_SR_inv.transpose();
        normal4 = forward_inv.transpose() * normal4;

        // Don't need this since it's inside the transform matrices
        // already
        // Transformation last_scale =
        //    Transformation(SCALE, ini_scale_coords[0], ini_scale_coords[1], ini_scale_coords[2],1.0);
        // Matrix4f transform = makeTransform(last_scale);
        // backward *= transform;
        // backward_SR *= transform;

        cout << "== Position transformed back out: " << ray4 << endl;
        cout << "== normal4 transformed back out: " << normal4 << endl;

        // Return: pick minimum distance from camera to new intersection position
        float d_new = (ray - cam_pos).norm();
        cout << "d_new: " << d_new << endl;
        cout << "d old: " << d << endl;
        if (d_new < d){
          // Update new minimum a and b
          d = d_new;

          // Put back into vec3
          ray[0] = ray4[0]; ray[1] = ray4[1]; ray[2] = ray4[2];
          normal[0] = normal4[0]; normal[1] = normal4[1]; normal[2] = normal4[2];
          *p = prm;

          cout << "found new min d: " << d << endl;
          cout << "pos: " << ray << endl;
          cout << "normal: " << normal << endl;
        }

        cout << "==Global ray: " << ray << endl;
        cout << "==Global normal: " << normal << endl;

      } else if (ren->getType() == OBJ) {
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
             // and the associated Vector3f
              recurse_findIntersection(d, ray, normal, p,
               av, cam_pos,
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

    pair<Ray, Primitive*> findIntersection(const Ray &camera_ray) {
      cout << "===============findIntersection=============" << endl;
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
          Primitive* temp_p;
          return make_pair(intersection_ray, temp_p);
       }

      // the "return" values
      vector<Transformation> transformation_stack;
      float d = 1000000000000; // Min distance from camera
      Vector3f ray;
      Vector3f normal;
      Primitive* prm;
      recurse_findIntersection(d, ray, normal, &prm,
        av, bv, ren, transformation_stack);

      cout << "==Returned ray origin: " << ray << endl;
      cout << "==Returned ray dir/normal: " << normal << endl;
      cout << "==Returned primitive: " << prm->getSpecular() << endl;

      // Package into Ray obj
      intersection_ray.origin_x = ray[0];
      intersection_ray.origin_y = ray[1];
      intersection_ray.origin_z = ray[2];
      intersection_ray.direction_x = normal[0];
      intersection_ray.direction_y = normal[1];
      intersection_ray.direction_z = normal[2];

      return make_pair(intersection_ray, prm);
    }

    void drawIntersectTest(Camera *camera) {

        // float matrix[16];
        // makeRotateMat(matrix, camera->getAxis()[0],
        //   camera->getAxis()[1], camera->getAxis()[2],
        //   degToRad(camera->getAngle()));
        //
        // Matrix4f cam_rotation_matrix;
        // cam_rotation_matrix <<
        //   matrix[0], matrix[1], matrix[2], matrix[3],
        //   matrix[4], matrix[5], matrix[6], matrix[7],
        //   matrix[8], matrix[9], matrix[10], matrix[11],
        //   matrix[12], matrix[13], matrix[14], matrix[15];

        Ray camera_ray;
        camera_ray.origin_x = 0;
        camera_ray.origin_y = 0;
        camera_ray.origin_z = 10;
        camera_ray.direction_x = 0.0;
        camera_ray.direction_y = 0.0;
        camera_ray.direction_z = -1.0;
        // camera_ray.direction_x = cam_dir[0];
        // camera_ray.direction_y = cam_dir[1];
        // camera_ray.direction_z = cam_dir[2];

        pair<Ray, Primitive*> ray_prm = findIntersection(camera_ray);
        Ray intersection_ray = ray_prm.first;

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
    // n = normal vector of x, y, z
    // e = camera position
    // Vector3d lighting_model(const Vertex &Pv, const Vertex &nv, const Material &material,
    //                     const vector<Light> &lights, const MatrixXd &em){
    //   // Empty vector
    //   Vector3d empty(0,0,0);
    //   Vector3d ones(1,1,1);
    //
    //   // Convert to vector
    //   Vector3d P(Pv.x, Pv.y, Pv.z); // point position
    //   Vector3d n(nv.x, nv.y, nv.z); // surface normal
    //   Vector3d e(em(0,0), em(1,0), em(2,0)); // camera pos
    //
    //   Vector3d cd(material.diffuse.r, material.diffuse.g, material.diffuse.b);
    //   Vector3d ca(material.ambient.r, material.ambient.g, material.ambient.b);
    //   Vector3d cs(material.specular.r, material.specular.g, material.specular.b);
    //   double p = material.shininess;
    //
    //   Vector3d diffuse_sum(0,0,0);
    //   Vector3d specular_sum(0,0,0);
    //
    //   Vector3d e_direction = (e - P).normalized();
    //
    //   Vector3d lp(0,0,0); // light position
    //   Vector3d lc(0,0,0); // light color
    //
    //   Vector3d l_direction, l_diffuse, l_specular;
    //
    //   for (auto &l : lights){
    //     lp << l.x, l.y, l.z;
    //     lc << l.rgb.r, l.rgb.g, l.rgb.b;
    //
    //     // Include attenuation
    //     // Distance between light and point
    //     double d = (lp - P).norm();
    //     lc = lc * (1.0 / (1 + l.attenuation * d * d));
    //
    //     // Find diffuse
    //     Vector3d l_direction = (lp - P).normalized();
    //     l_diffuse = lc * max(0.0, n.dot(l_direction));
    //     diffuse_sum += l_diffuse;
    //
    //     // Find specular
    //     l_specular = lc * pow(max(0.0,
    //       n.dot((e_direction + l_direction).normalized())), p);
    //     specular_sum += l_specular;
    //   }
    //
    //   //component-wise min function and component-wise product
    //   Vector3d c = ones.cwiseMin(ca
    //                 + diffuse_sum.cwiseProduct(cd)
    //                 + specular_sum.cwiseProduct(cs));
    //   return c;
    // }

    // n = normal vector of x, y, z
    // e = camera position
    Vector3f dummylighting(const Vector3f &Pv, const Vector3f &nv,
      const Primitive* prm, const vector<PointLight> &lights, const Vector3f &em){

      //printInfo(const Renderable* ren, int indent)
      printInfo(prm, 2);

      // // Empty vector
      // Vector3d empty(0,0,0);
      // Vector3d ones(1,1,1);
      //
      // // Convert to vector
      // Vector3d P(Pv.x, Pv.y, Pv.z); // point position
      // Vector3d n(nv.x, nv.y, nv.z); // surface normal
      // Vector3d e(em(0,0), em(1,0), em(2,0)); // camera pos
      Vector3f temp;
      return temp;
    }


    /* Ray traces the scene. */
    void raytrace(Camera camera, Scene scene) {
        // LEAVE THIS UNLESS YOU WANT TO WRITE YOUR OWN OUTPUT FUNCTION
        PNGMaker png = PNGMaker(XRES, YRES);

        // Assume that camera never moves
        Vector3f e1(0, 0, -1); // Camera direction
        Vector3f e2(1, 0, 0); // Comes out right of camera
        Vector3f e3(0, 1, 0); // Comes out top of camera

        // REPLACE THIS WITH YOUR CODE
        for (int i = 0; i < XRES; i++) {
            for (int j = 0; j < YRES; j++) {
              float h = 2.0 * camera.near * tan(camera.fov / 2);
              float w = camera.aspect * h;

              float xi = (i - XRES/2.0) * w/XRES;
              float yj = (j - YRES/2.0) * h/YRES;

              // av = direction, bv = position
              Vector3f av = camera.near * e1 + xi * e2 + yj * e3;

              // a = intersection point - light position
              // light position = scene->lights[0].position (know there's only 1 light)

              // Find intersection point p
              Ray camera_ray;
              camera_ray.origin_x = camera.getPosition()[0];
              camera_ray.origin_y = camera.getPosition()[1];
              camera_ray.origin_z = camera.getPosition()[2];
              camera_ray.direction_x = av[0];
              camera_ray.direction_y = av[1];
              camera_ray.direction_z = av[2];

              pair<Ray, Primitive*> ray_prm = findIntersection(camera_ray);
              Ray intersection_ray = ray_prm.first;

              av[0] = intersection_ray.direction_x; // Normal (ie vn)
              av[1] = intersection_ray.direction_y;
              av[2] = intersection_ray.direction_z;
              Vector3f bv(intersection_ray.origin_x, // Point (ie v)
                  intersection_ray.origin_y,
                  intersection_ray.origin_z);
              Primitive * prm = ray_prm.second;

              if (prm == NULL){
                // Camera doesn't intersect object so no shine or shadow
                return;
              }

              Vector3f c =
                //lighting_model(b, av, prm, lights, e); // Do intersect with lights
                dummylighting(bv, av, prm, scene.lights, e1);

              png.setPixel(i, j, c[0] * 255, c[0] * 255, c[0] * 255);
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
