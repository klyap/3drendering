#ifndef ASSIGNMENT_HPP
#define ASSIGNMENT_HPP

#include <vector>
#include "model.hpp"
//class Camera;
struct Camera;
class Scene;

namespace Assignment {
    Matrix4f makeInvTransform(Transformation t);
    Matrix4f makeTransform(Transformation t);
    Matrix4f makeRotation(Vector4f unit_q);
    float sq_io(float x, float y, float z, float e, float n);
    float getTminus(float a, float b, float c);
    float getTplus(float a, float b, float c);
    Vector3f grad_sq_io(float x, float y, float z, float e, float n);
    Vector3f getRay(float t, Vector3f av, Vector3f bv);
    pair <Vector3f, Vector3f> iter_newton(float t, Vector3f av, Vector3f bv, float e, float n);
    pair<Vector3f, Vector3f> newton(Vector3f av, Vector3f bv, float e, float n);

    void recurse_findIntersection(float &t, Vector3f &ray,
      Vector3f av, Vector3f bv, Renderable *ren,
      vector<Transformation> &transformation_stack);

    void drawIOTest();
    void drawIntersectTest(Camera *camera);
    void raytrace(Camera camera, Scene scene);
};

#endif
