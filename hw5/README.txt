To compile:
 > make
 > ./opengl_shader [path/scene_description_file.txt] [xres] [yres] [h]

Notes:
 To apply implicit fairing: press "e"

 To build the matrix operator F, I split up the formula:
    sum over j of 1/(2A) * (cot(alpha_j) + cot(beta_j)) * (x_j - x_i)
 into:
    sum over j of [1/(2A) * (cot(alpha_j) + cot(beta_j)) * (x_j)]
    - sum over j of [1/(2A) * (cot(alpha_j) + cot(beta_j)) x_i]
 The main idea is that the coefficient of x_i is the negative sum of each
 coefficient of its adjacent vertices x_j.

 For each v_i, I first computed the area of all adjacent faces, A.
 For each v_j, I computed the coefficient 1/(2A) * (cot(alpha_j) + cot(beta_j)
 and subtract it from v_i's coefficient.
 So: F[i][j] = 1/(2A) * (cot(alpha_j) + cot(beta_j)
     F[i][i] = -1 * (sum of all F[i][j]) where v_j is adjacent to v_i
