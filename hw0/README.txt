To compile:

Part 1
--------
 > make
 > ./parsing obj_file1.obj obj_file2.obj ... obj_fileN.obj

 Part 2
 ---------
 > make
 > ./parsing_transform file.txt

 Part 3
 ---------
 > make
 > ./parsing_obj_and_transform file.txt

 Part 4
 ---------
 > make
 > ./display_ppm xres yres | display -
       OR
   ./display_ppm xres yres | convert - my_image_name.png

