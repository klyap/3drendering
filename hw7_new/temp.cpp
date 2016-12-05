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
         int check_inside = sq_io(coords[0], coords[1], coords[2],
           prm->getExp0(), prm->getExp1());
         cout << "sq_io returned: " << check_inside << endl;
         if (check_inside == -1){
           cout << "check inside == -1" << endl;
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
