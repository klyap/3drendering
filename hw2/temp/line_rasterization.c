////////////////////
// RASTERIZATION
////////////////////
// Converts NDC to screen coords
void NDC_to_screen(double &x, double &y){
  x = (x + 1)/2.0 * (x_dim);
  y = (y + 1)/2.0 * (y_dim);
}


// Converts a vertex from world to Cartesian NDC
/*MatrixXd vector_world_to_NDC(Vector4d t, MatrixXd cam_pos, 
                    MatrixXd cam_orientation,
                    MatrixXd cam_proj){

  // Vertex converted to homogenous NDC
  MatrixXd cam_transform = cam_pos * cam_orientation;
  MatrixXd inv_cam_transform = cam_transform.inverse();

  MatrixXd C_vertex = (inv_cam_transform * t);
  MatrixXd homo_NDC = cam_proj * C_vertex;
  
  // DEBUG
  //cout << "homogenous NDC: " << endl;
  //cout << homo_NDC.row(0) << " " << homo_NDC.row(1) << " " << homo_NDC.row(2) << endl;
  // DEBUG

  // Vertex converted to cartesian NDC
  // by dividing by -z_c (z coord of vertex)
  double neg_z_c = -1/C_vertex(2,0);
  MatrixXd cart_NDC = neg_z_c * homo_NDC;
  
  //cout << "Cartesian NDC = " << neg_z_c << endl;
  //cout << cart_NDC.row(0) << " " << cart_NDC.row(1) << " " << cart_NDC.row(2) << endl;
  return cart_NDC;
}*/


void to_ppm(int * grid){
  cout << "P3\n";
  cout << x_dim << " " << y_dim << "\n";
  cout << "255\n";
  for (int x = 0; x < x_dim * y_dim; x++){
    // 0-8 for line rastering
    if (grid[x] == 0){
      // Black
     cout << "0 0 0" << endl;
    } else if (grid[x] == 1) {
      // White
      cout << "225 225 225" << endl;
    } else if (grid[x] == 2) {
      cout << "0 225 225" << endl;
    } else if (grid[x] == 3) {
      cout << "225 0 225" << endl;
    } else if (grid[x] == 4) {
      cout << "225 225 0" << endl;
    }else if (grid[x] == 5) {
      cout << "0 0 225" << endl;
    }else if (grid[x] == 6) {
      cout << "225 0 0" << endl;
    }else if (grid[x] == 7) {
      cout << "0 225 0" << endl;
    }else if (grid[x] == 8) {
      cout << "111 111 0" << endl;
    } else {
      // Other numbers for shader
      // Assume int grid[x] = rrrgggbbb
      //cout << " GOT COLOR:  " <<  grid[x] << endl;
      int red = grid[x] / 1000000;
      int blue = grid[x] % (grid[x] / 1000000) / 1000;
      int green = grid[x] % 1000;
      cout << red << " " << blue << " " << green << endl;
    }
  }
}

void swap(int &a, int &b) {
  int tmp = a;
  a = b;
  b = tmp;
}

void rasterize(
  int * grid, int x_dim, int y_dim, Vertex v1, Vertex v2){
  // Assume range of frustrum = [-1, 1] st v1 lies in this range
  // and range of resolution = [0, dim]

  int x1 = (v1.x + 1)/2.0 * (x_dim);
  int y1 = (v1.y + 1)/2.0 * (y_dim);
  int x2 = (v2.x + 1)/2.0 * (x_dim);
  int y2 = (v2.y + 1)/2.0 * (y_dim);
  
  double dx = x2 - x1;
  double dy = y2 - y1;
  double m;

  int color = 1;
  //int color = 999;

  if (dx == 0){
    // If m = infinity, bucket it with Octant 2/7
    m = dy / 0.000000001;
  } else {
    m = dy/dx;
  }

  double e = 0;
  

  if (m >= 0 && m <= 1){

    // Octant 1: Positive slope + horizonal
    color = 1;
    if (x1 >= x2 && y1 >= y2){
      swap(x1, x2);
      swap(y1, y2);
      color = 2;
    }

    /*cout << "v1 = " << x1 << "," << y1<< endl;
    cout << "v2 = " << x2 << "," << y2<< endl;*/

    // dy = abs(dy);
    // dx = abs(dx);
    dy = y2 - y1;
    dx = x2 - x1;
    int y = y1;
    
    for (int x = x1; x < x2; x++){
      if (y_dim - int(y) >= 0 
          && int(x) >= 0 
          && (y_dim - int(y)) < y_dim 
          && int(x) < x_dim){

        grid[(y_dim - int(y)) * x_dim + int(x)] = color;
      }
      if (2 * (e + dy) < dx){
        e = e + dy;
      } else {
        e = e + dy - dx;
        y = y + 1;
      }
    }
  } else if (m >= 1){
    // Octant 2 / 6
  
    // Flip along y = x so Oct 2 is flipped to Oct 1
    // (and Oct 6 to Oct 5)
    swap(x1, y1);
    swap(x2, y2);

    color = 3;
    // Swap vertices if it's in Oct 5
    // so the line now lies in Oct 1
    if (x1 >= x2 && y1 >= y2){
      swap(x1, x2);
      swap(y1, y2);
      color = 4;
    }

    dy = y2 - y1;
    dx = x2 - x1;

    int y = y1;
    
    for (int x = x1; x < x2; x++){

      if (y_dim - int(x) >= 0 
          && int(y) >= 0 
          && y_dim - int(x) < y_dim 
          && int(y) < x_dim){
        // When chosen pixel at (x,y),
        // color in pixel (y,x) to flip it back
        // to the original octant
        grid[(y_dim - int(x)) * x_dim + int(y)] = color;
      }

      if (2 * (e + dy) < dx){
        e = e + dy;
      } else {
        e = e + dy - dx;
        y = y + 1;
      }
      
    }
  } else if (m < 0 && m >= -1){
    // Octant 4/8

    // Flip along x = 0 from Oct 4 to 1 
    // (or Oct 8 to 5)
    swap(x1, x2);

    color = 5;
    // If this is now in Octant 5, swap vertices
    // so it lies in Octant 1
    if (x1 <= x2 && y1 >= y2){
      swap(x1, x2);
      swap(y1, y2);
      color = 6;
    }

    dy = y2 - y1;
    dx = x2 - x1;
    int y = y1;
    
    for (int x = x1; x < x2; x++){
      // Since slope is drawn downwards,
      // shift the colored pixel up by the vertical distance
      // that the slope traveled (ie dy)
      if (int(y_dim - y - dy) >= 0 
          && int(x) >= 0 
          && int(y_dim - y - dy) < y_dim 
          && int(x) < x_dim){

        grid[int(y_dim - y - dy) * x_dim + int(x)] = color;
      }
      
      if (2 * (e + dy) < dx){
        e = e + dy;
      } else {
        e = e + dy - dx;
        // Decrement slope instead of incrementing
        y = y - 1;
      }

    }
  }else if (m < -1){
    // Octant 3 / 7

    // Flip along x = 0 from Oct 3 to 2
    // (or Oct 7 to 6)
    swap(x1, x2);
    // Flip along y = x from Oct 2 to 1
    // (or Oct 6 to 5)
    swap(x1, y1);
    swap(x2, y2);

    color = 7;

    // If this lies in Octant 5, swap vertices
    // so they both are in Octant 1
    if (x1 >= x2 && y1 >= y2){
      swap(x1, x2);
      swap(y1, y2);
      color = 8;
    }

    dy = y2 - y1;
    dx = x2 - x1;

    int y = y1;
    
    for (int x = x1; x < x2; x++){

      if (int(y_dim - x) >= 0 
          && int(y + dy) >= 0 
          && int(y_dim - x) < y_dim 
          && int(y + dy) < x_dim){

        // Shift upwards to accomodate for
        // slope going down
        // Then swap x and y to flip it back
        // so that its slope goes in the right direction
        grid[int(y_dim - x) * x_dim + int(y + dy)] = color;
      }
      
      if (2 * (e + dy) < dx){
        e = e + dy;
      } else {
        e = e + dy - dx;
        // Decrement slope
        y = y - 1;
      }
    }
  }
  
  // DEBUG : Mark the vertices only
  /*grid[int(y1) * x_dim + int(x1)] = 1;
  cout << "rasterize: marked v1" << endl;
  grid[int(y2) * x_dim + int(x2)] = 1;
  cout << "rasterize: marked v2" << endl;*/
  // DEBUG

}

void draw_lines(int * grid, int x_dim, int y_dim, Obj obj, 
                MatrixXd cam_pos, 
                MatrixXd cam_orientation,
                MatrixXd cam_proj){
  // Convert all vertices to Cartesian NDC 
  // (Copied from main function)
  for (int vertex = 1; vertex < obj.v.size(); vertex++){
    Vertex this_vertex = obj.v[vertex];
    MatrixXd v(4,1); 
    v << this_vertex.x, this_vertex.y, this_vertex.z, 1;
    
    // Vertex after geometric transform
    MatrixXd t = world_to_NDC(v, cam_pos, 
                    cam_orientation,
                    cam_proj);
    
    Vertex v1;
    v1.x = t(0,0);
    v1.y = t(1,0);
    v1.z = t(2,0);
    obj.v[vertex] = v1; // Store transformed vertex*/

    // DEBUG : DRAW THIS VERTEX ONLY
    //rasterize(grid, x_dim, y_dim, v1, v1);
  }

  vector<Vertex> vertices = obj.v;
  vector<Face> faces = obj.f;
  for (int f = 0; f < faces.size(); f++){
    Vertex v1 = vertices[faces[f].x];
    Vertex v2 = vertices[faces[f].y];
    Vertex v3 = vertices[faces[f].z];

    rasterize(grid, x_dim, y_dim, v1, v2);
    rasterize(grid, x_dim, y_dim, v1, v3);
    rasterize(grid, x_dim, y_dim, v2, v3);
  }

}