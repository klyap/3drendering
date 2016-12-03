#include "object.h"

string file;
int total_frames;
Camera cam; // Holds hard coded camera transform params
string instance_text;
map<int, vector<GeoTransform>> keyframes;
vector<int> keyframe_indices;
int total_keyframes;

void debug_transform(GeoTransform t){
  cout << "---------" << endl;
  cout << "GeoTransform: " << endl;

  cout << "type = " << t.type << endl;
  if (t.type == "rotation"){
    cout << "angle = " << t.params[3] << endl;
  }
  cout << "xyz = " <<
    t.params[0] << " " << t.params[1] << " " << t.params[2] << endl;
  cout << "--------" << endl;
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

// Using hardcoded parameters
void create_camera(){

  vector<int> new_pos {0, 0, 40};
  cam.position = new_pos;

  // x, y, z, a
  vector<int> new_orientation {0, 0, 40, 0};
  cam.orientation = new_orientation;

  cam.perspective["near"] = 1.0;
  cam.perspective["far"] = 60.0;
  cam.perspective["left"] = -1.0;
  cam.perspective["right"] = 1.0;
  cam.perspective["bottom"] = -1.0;
  cam.perspective["top"] = 1.0;

  debug_camera(cam);
}



void parse(string file){
  string line;
  ifstream infile(file);

  //////////////////////////////
  // Store total number of frames to generate
  ///////////////////////////////
  getline(infile, line);
  total_frames = stoi(line);
  cout << "Total frames = " << total_frames << endl;

  //////////////////////////////
  // Store transformation for each keyframe
  ///////////////////////////////
  while (getline(infile, line)){
      // Start new object
      vector<GeoTransform> transform_list;

      // line = "Frame X"
      istringstream frame_title(line);
      string frame;
      int frame_number;
      frame_title >> frame;
      assert(frame == "Frame");
      frame_title >> frame_number;
      std::cout << "got frame #" << frame_number << std::endl;
      keyframe_indices.push_back(frame_number);

      // Get 3 transforms
      for (int i = 0; i < 3; i++){
        getline(infile, line);
        istringstream transform_line(line);
        string type;
        float x, y, z, a;
        transform_line >> type;
        if (type == "translation" || type == "scale"){
          // TRANSLATE or SCALE MATRIX
          transform_line >> x >> y >> z;

          GeoTransform t;
          t.type = type;
          vector<float> params {x, y, z};
          t.params = params;

          //debug_transform(t);
          transform_list.push_back(t);

        } else if (type == "rotation"){
          // ROTATE MATRIX
          transform_line >> x >> y >> z >> a;

          GeoTransform t;
          t.type = type;
          vector<float> params {x, y, z, a};
          t.params = params;

          //debug_transform(t);
          transform_list.push_back(t);
        }
        cout << "DONE TRANSFORMS: " << transform_list.size() << endl;

         // Store transform list
         keyframes[frame_number] = transform_list;

         for(auto &t : keyframes[frame_number]){
           debug_transform(t);
         }
         cout << "===DONE OBJECT===" << endl;
         cout << "=================" << endl;
      } // end for loop
    }

    total_keyframes = keyframe_indices.size();
}
