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

    bool IOTest(
        Renderable *ren,
        vector<Transformation> &transformation_stack,
        float x,
        float y,
        float z)
    {
        if (ren->getType() == PRM) {
            Primitive *prm = dynamic_cast<Primitive*>(ren);

            /* TODO
             *
             *     Apply inverse of the transformations passed in through
             * transformation_stack to (x, y, z) and perform the IO test
             * oulined in the lecture notes. If the point is inside the
             * primitive, return true. Otherwise return false
             **/

             for (auto &t : transformation_stack){

             }

            return true; // <-- replace with your code ***

        } else if (ren->getType() == OBJ) {
            Object *obj = dynamic_cast<Object*>(ren);
            const vector<Transformation>& overall_trans =
                obj->getOverallTransformation();
            for (int i = overall_trans.size() - 1; i >= 0; i--) {
                transformation_stack.push_back(overall_trans.at(i));
            }

            bool IO_result = false;
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

        return true;
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

    Ray findIntersection(const Ray &camera_ray) {
        /* TODO */

        while (){
          
        }

        /* END TODO */

        Ray intersection_ray;
        intersection_ray.origin_x = 0.0;
        intersection_ray.origin_y = 1.0;
        intersection_ray.origin_z = 0.0;
        intersection_ray.direction_x = 1.0;
        intersection_ray.direction_y = 0.0;
        intersection_ray.direction_z = 0.0;

        return intersection_ray;
    }

    void drawIntersectTest(Camera *camera) {
        Ray camera_ray;
        camera_ray.origin_x = 0.0;      // TODO: replace 0.0 with correct value
        camera_ray.origin_y = 0.0;      // TODO: replace 0.0 with correct value
        camera_ray.origin_z = 0.0;      // TODO: replace 0.0 with correct value
        camera_ray.direction_x = 0.0;   // TODO: replace 0.0 with correct value
        camera_ray.direction_y = 0.0;   // TODO: replace 0.0 with correct value
        camera_ray.direction_z = 0.0;   // TODO: replace 0.0 with correct value

        Ray intersection_ray = findIntersection(camera_ray);

        const float IO_test_color[3] = {0.0, 0.0, 0.5};
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
