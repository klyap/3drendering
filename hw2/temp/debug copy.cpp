#include "shaded_renderer.h"

void debug_base_objs(map<string, Obj> original){
  typedef map<string, Obj >::iterator it_type1;
  for(it_type1 it = original.begin(); it != original.end(); it++) {
    cout << "Object name: " << it->first << endl;
    cout << "Number of instances: " << it->second.v.size() << endl;
  }
}

void debug_transformed(Obj obj, map<string, Obj> original, map<string, vector<MatrixXd>> transformed){
  typedef map<string, vector<MatrixXd> >::iterator it_type;
  for(it_type it = transformed.begin(); it != transformed.end(); it++){
    cout << it->first << endl;
    vector<MatrixXd> matrices = it->second;
    for (int i = 0; i < matrices.size(); i++){
      // For each object instance
      Obj obj = original[it->first];
      cout << "Obj: " << obj.filename << " " << i << endl;
      for (int vertex = 0; vertex < obj.v.size(); vertex++){
        Vertex this_vertex = obj.v[vertex];
        MatrixXd v(4,1); 
        cout << "Transform: " << endl << matrices[i] << endl;
        v << this_vertex.x, this_vertex.y, this_vertex.z, 1;
        cout << "Transformed v: " << endl << matrices[i] * v << endl;
      }
    }
  }
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
  cout << "OBJECT: " << obj.filename << endl;
  
  cout << " v: " << endl;
  for (int i = 0; i < obj.v.size(); i++){
    cout << "  " << obj.v[i].x << " " << obj.v[i].y << " " << obj.v[i].z << endl;
  }
  
  cout << " f: " << endl;
  for (int i = 0; i < obj.f.size(); i++){
    cout << "  " << obj.f[i].x << " " << obj.f[i].y << " " << obj.f[i].z << endl;
  }
  cout << " vn: " << endl;
  for (int i = 0; i < obj.vn.size(); i++){
    cout << "  " << obj.vn[i].x << " " << obj.vn[i].y << " " << obj.vn[i].z << endl;
  }
  cout << " fn: " << endl;
  for (int i = 0; i < obj.fn.size(); i++){
    cout << "  " << obj.fn[i].x << " " << obj.fn[i].y << " " << obj.fn[i].z << endl;
  }
  cout << " ";
  debug_material(obj.material);
}


