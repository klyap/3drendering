#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <math.h>

#include "../Eigen/Dense"

using namespace Eigen;
using namespace std;

void parse_transform(char * filename){

  // READ IN FILE
  ifstream infile(filename);
  char type;
  double x, y, z, a;
  vector<MatrixXd> all_transforms;
  while (infile >> type)
  {
    //cout << type << " " << x << " " << y << " " << z << endl;
    
    if (type == 't'){
    	// TRANSLATE MATRIX
   	  infile >> x >> y >> z;

      MatrixXd m(4,4); // m is constructed as a 4x4 matrix  
			m << 1, 0, 0, x, // row1  
			     0, 1, 0, y, // row2  
			     0, 0, 1, z, // row3  
			     0, 0, 0, 1; // row4

			//cout << m << endl;
			all_transforms.push_back(m);

    } else if (type == 'r'){
    	// ROTATE MATRIX
    	infile >> x >> y >> z >> a;

    	double a1 = x * x + (1 - x * x) * cos(a);
    	double a2 = x * y * (1 - cos(a)) - z * sin(a);
    	double a3 = x * z * (1 - cos(a)) + y * sin(a);
    	double b1 = y * x * (1 - cos(a)) + z * sin(a);
    	double b2 = y * y + (1 - y * y) * cos(a);
    	double b3 = y * z * (1 - cos(a)) - x * sin(a);
    	double c1 = z * x * (1 - cos(a)) - y * sin(a);
    	double c2 = z * y * (1 - cos(a)) + x * sin(a);
    	double c3 = z * z + (1 - z * z) * cos(a);

      MatrixXd m(4,4); // m is constructed as a 4x4 matrix  
			m << a1, a2, a3, 0, // row1  
			     b1, b2, b3, 0, // row2  
			     c1, c2, c3, 0, // row3  
			     0, 0, 0, 1; // row4

			//cout << m << endl;
			all_transforms.push_back(m);

    } else if (type == 's'){
    	// SCALE MATRIX
    	infile >> x >> y >> z;
      MatrixXd m(4,4); // m is constructed as a 4x4 matrix  
			m << x, 0, 0, 0, // row1  
			     0, y, 0, 0, // row2  
			     0, 0, z, 0, // row3  
			     0, 0, 0, 1; // row4

			//cout << m << endl;
			all_transforms.push_back(m);
  	}

    //cout << type << " " << x << " " << y << " " << z << " " << a << endl;
  }

  // COMBINE MATRICES
  MatrixXd m = all_transforms.back();
  all_transforms.pop_back();

  while (all_transforms.size() > 0){
  	m *= all_transforms.back();
  	all_transforms.pop_back();
  }

  // PRINT OUT
  cout << "FINAL:\n" << m.inverse() << endl;
  infile.close();
}

int main (int argc, char* argv[])
{
	char name[500];
	for (int i = 1; i < argc; i++){
		// Each arg is a .txt file
		parse_transform(argv[i]);
	}

	return 0;
}