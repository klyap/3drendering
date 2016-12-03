#include "object.h"

/* Global variables */
int current_frame = 0;
string directory = "keyframes/";
int total_vertices;
vector<int> keyframe_indices;
map<int, VertexList> keyframes;

int p1 = 0;
int p2 = 0;
int p3 = 1;
int p4 = 2;

/* Updates p's and gets u value */
float get_u(int current_frame){
  float u;
  int n = keyframe_indices.size();

  // Hardcoding in number of intermediate frames per interval
  float interval = 5.0;

  if (current_frame < keyframe_indices[1]){
    p1 = 0;
    p2 = 0;
    p3 = 1;
    p4 = 2;
  } else if (current_frame > keyframe_indices[n-2]){
    // In last leg
    p1 = n-3;
    p2 = n-2;
    p3 = n-1;
    p4 = n-1;
  } else if (current_frame < keyframe_indices[2]){
    p1 = 0;
    p2 = 1;
    p3 = 2;
    p4 = 3;
  } else if (current_frame > keyframe_indices[p3]){
    p1++;
    p2++;
    p3++;
    p4++;
  }

  // Get u
  u = (current_frame - keyframe_indices[p2]) * 1.0 / interval;
  assert(u >= 0.0 && u <= 1.0);

  return u;
}

// Generates a single element (component)
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


void update(string filename, int current_frame)
{
    std::ofstream out(filename);
    float u = get_u(current_frame);

    int frame1 = keyframe_indices[p1];
    int frame2 = keyframe_indices[p2];
    int frame3 = keyframe_indices[p3];
    int frame4 = keyframe_indices[p4];

    vector<float> xlist1 = keyframes[frame1].xlist;
    vector<float> xlist2 = keyframes[frame2].xlist;
    vector<float> xlist3 = keyframes[frame3].xlist;
    vector<float> xlist4 = keyframes[frame4].xlist;

    vector<float> ylist1 = keyframes[frame1].ylist;
    vector<float> ylist2 = keyframes[frame2].ylist;
    vector<float> ylist3 = keyframes[frame3].ylist;
    vector<float> ylist4 = keyframes[frame4].ylist;

    vector<float> zlist1 = keyframes[frame1].zlist;
    vector<float> zlist2 = keyframes[frame2].zlist;
    vector<float> zlist3 = keyframes[frame3].zlist;
    vector<float> zlist4 = keyframes[frame4].zlist;

    for (int i = 0; i < total_vertices; i++){
      // grab its x component from each of the 4 frames
      Vector4d px(xlist1[i], xlist2[i], xlist3[i], xlist4[i]);
      float i_x = catmull_rom(u, px);
      // grab its y component from each of the 4 frames
      Vector4d py(ylist1[i], ylist2[i], ylist3[i], ylist4[i]);
      float i_y = catmull_rom(u, py);
      // grab its z component from each of the 4 frames
      Vector4d pz(zlist1[i], zlist2[i], zlist3[i], zlist4[i]);
      float i_z = catmull_rom(u, pz);

      // write the interpolated x,y,z to file
      out << "v " << i_x << " " << i_y << " " << i_z << endl;
    }

    out.close();
}

string make_filename(int current_frame){
  string filename;
  if (current_frame < 10){
    filename = "bunny0" + to_string(current_frame) + ".obj";
  } else {
    filename = "bunny" + to_string(current_frame) + ".obj";
  }
  return filename;
}

// Read in given keyframes
void parse_keyframes(){
  keyframe_indices.push_back(0);
  keyframe_indices.push_back(5);
  keyframe_indices.push_back(10);
  keyframe_indices.push_back(15);
  keyframe_indices.push_back(20);
  for (int i = 0; i < 5; i++){
    VertexList v;

    int frame_num = keyframe_indices[i];
    string filename = make_filename(keyframe_indices[i]);
    ifstream infile(directory + filename);

    string type;
    float x, y, z;
    while (infile >> type)
    {
      if (type == "v"){
        infile >> x >> y >> z;

        v.xlist.push_back(x);
        v.ylist.push_back(y);
        v.zlist.push_back(z);
      }
    }

    // Done storing all vertices in this keyframe
    keyframes[frame_num] = v;
  }

  total_vertices = keyframes[keyframe_indices[0]].xlist.size();

}

int main(){
  // Read in data for keyframe values
  parse_keyframes();

  // Write an .obj file for each interpolated frame
  for (int current_frame = 0; current_frame <
    keyframe_indices[keyframe_indices.size()-1];
    current_frame++){
    if (current_frame % 5){
      // If it's a multiple of 5, we already have it
      string filename = make_filename(current_frame);
      // make a new obj file
      update(filename, current_frame);
    }
  }
}
