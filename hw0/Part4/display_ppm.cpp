#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <math.h>

using namespace std;

void display_circle(int x_dim, int y_dim){

  int diameter = min(x_dim, y_dim) / 2;
  int radius = diameter / 2;

  cout << "P3\n";
  cout << x_dim << " " << y_dim << "\n";
  cout << "255\n";

  for (int x_i = 0; x_i < x_dim; x_i++){
    for (int y_i = 0; y_i < y_dim; y_i++){
      int x = x_i - x_dim / 2;
      int y = y_i - y_dim / 2;
      if (x * x + y * y <= radius * radius){
        cout << "0 255 0" << endl;
      } else {
        cout << "255 0 0" << endl;
      }
    }
  }

}

int main (int argc, char* argv[])
{
  if (argc != 3){
    cout << "Usage: ./ppm_test xres yres" << endl;
    return 0;
  }

	display_circle(atoi(argv[1]), atoi(argv[2]));
	

	return 0;
}