#ifndef ASSIGNMENT_HPP
#define ASSIGNMENT_HPP

#include <vector>
#include "model.hpp"
//class Camera;
struct Camera;
class Scene;

namespace Assignment {
    Matrix4f makeTransform(Transformation t);
    int sq_io(float x, float y, float z, float e, float n);
    void drawIOTest();
    void drawIntersectTest(Camera *camera);
    void raytrace(Camera camera, Scene scene);
};

#endif
