#ifndef __FUNCTIONS__H__
#define __FUNCTIONS__H__
#include "auxiliary.h"

void write_ascii(char s[], current_image p, int **img1, color_image **img2);
void write_binary(char s[], current_image p, int **img1, color_image **img2);
void load(char s[], int ***img1, color_image ***img2, current_image *p);
void select_area(char s[], current_image *p);
void select_whole_image(current_image *p);
void rotate_clockwise_gray(int ***img, current_image *p);
void rotate_clockwise_color(color_image ***img, current_image *p);
void rotate_counterwise_gray(int ***img, current_image *p);
void rotate_counterwise_color(color_image ***img, current_image *p);
void rotate(char s[], current_image *p, int ***img1, color_image ***img2);
void crop_image(int ***img1, color_image ***img2, current_image *p);
void apply(char filter[], color_image **img, current_image p, double f[3][3]);

#endif
