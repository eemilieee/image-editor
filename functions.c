#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "functions.h"
#include "auxiliary.h"

#define L_MAX 1000
#define S_MAX 100

//the function prints the content of the current image
//(according to the standard format) to the desired text file
void write_ascii(char s[], current_image p, int **img1, color_image **img2)
{
	FILE *fout;
	int i, j;
	unsigned char r, g, b;

	fout = fopen(s, "wt");
	printf("Saved %s\n", s);

	//printing the magic word based on the image and file type
	if (!p.image_type) {
		if (p.max_value == 1)
			fprintf(fout, "P1\n");
		else
			fprintf(fout, "P2\n");
	} else {
		fprintf(fout, "P3\n");
	}

	fprintf(fout, "%d %d\n", p.width, p.height);

	//printing the max value of the pixels if the image is greyscale/color
	if (p.max_value != 1)
		fprintf(fout, "%d\n", p.max_value);

	//printing the pixels' values
	if (!p.image_type) {
		for (i = 0; i < p.height; i++) {
			for (j = 0; j < p.width; j++)
				fprintf(fout, "%d ", img1[i][j]);
			fprintf(fout, "\n");
		}
	} else {
		for (i = 0; i < p.height; i++) {
			for (j = 0; j < p.width; j++) {
				//the double values resulted from the operations
				//involving the pixels are reverted into the integer type
				r = round(1.0 * img2[i][j].R / 1000);
				g = round(1.0 * img2[i][j].G / 1000);
				b = round(1.0 * img2[i][j].B / 1000);

				fprintf(fout, "%hhu %hhu %hhu ", r, g, b);
			}
			fprintf(fout, "\n");
		}
	}

	fclose(fout);
}

//the function prints the content of the current image
//(according to the standard format) to the desired binary file
void write_binary(char s[], current_image p, int **img1, color_image **img2)
{
	FILE *fout;
	int i, j;
	unsigned char r, g, b;

	fout = fopen(s, "wt");
	printf("Saved %s\n", s);

	//printing the magic word based on the image and file type
	if (!p.image_type) {
		if (p.max_value == 1)
			fprintf(fout, "P4\n");
		else
			fprintf(fout, "P5\n");
	} else {
		fprintf(fout, "P6\n");
	}

	fprintf(fout, "%d %d\n", p.width, p.height);

	//printing the max value of the pixels if the image is greyscale/color
	if (p.max_value != 1)
		fprintf(fout, "%d\n", p.max_value);

	fclose(fout);

	fout = fopen(s, "ab");

	//printing the pixels' values
	if (!p.image_type) {
		for (i = 0; i < p.height; i++)
			for (j = 0; j < p.width; j++)
				fwrite(&img1[i][j], sizeof(char), 1, fout);
	} else {
		for (i = 0; i < p.height; i++)
			for (j = 0; j < p.width; j++) {
				//the double values resulted from the operations
				//involving the pixels are reverted into the integer type
				r = round(1.0 * img2[i][j].R / 1000);
				g = round(1.0 * img2[i][j].G / 1000);
				b = round(1.0 * img2[i][j].B / 1000);

				fwrite(&r, sizeof(char), 1, fout);
				fwrite(&g, sizeof(char), 1, fout);
				fwrite(&b, sizeof(char), 1, fout);
			}
	}

	fclose(fout);
}

//the function reads and memorizes the details of a new image,
//allocates the neccesary memory zone for it and
//memorizes the values of its pixels,
//all depending on its type
void load(char s[], int ***img1, color_image ***img2, current_image *p)
{
	FILE *fin;
	int **aux1;
	color_image **aux2;
	current_image new;
	char line[L_MAX];
	int type, ok = 1, nr = 0;
	fpos_t pos1, pos2, pos3;

	fin = fopen(s, "rt");
	if (!fin) {
		printf("Failed to load %s\n", s);
		p->hide = 1;
	} else {
		printf("Loaded %s\n", s);
		fgets(line, L_MAX, fin);
		strcpy(line + strlen(line) - 1, line + strlen(line));
		type = line[1] - '0';
		image_info(type, &new, fin);
		fgetpos(fin, &pos1);
		//checking whether there are more comments found in the file
		while (ok) {
			fgets(line, L_MAX, fin);
			if (!(strchr(line, '#') - line)) {
				nr++;
				fgetpos(fin, &pos2);
			} else {
				ok = 0; } }
		//setting the cursor just before the pixels
		if (!nr) {
			fsetpos(fin, &pos1);
			pos3 = pos1;
		} else {
			fsetpos(fin, &pos2);
			pos3 = pos2; }
		//allocating the needed memory for the new image,
		//deallocating the old one and replacing it (if the case)
		if (!p->prev_images) {
			copy_info(new, p);
			if (type == 1 || type == 2 || type == 4 || type == 5) {
				*img1 = alloc_gray_image(p->height, p->width);
				p->image_type = 0;
			} else {
				*img2 = alloc_color_image(p->height, p->width);
				p->image_type = 1; }
		} else {
			if (!p->image_type)
				dealloc_gray_image(*img1, p->height);
			else
				dealloc_color_image(*img2, p->height);
			copy_info(new, p);
			if (type == 1 || type == 2 || type == 4 || type == 5) {
				aux1 = alloc_gray_image(p->height, p->width);
				p->image_type = 0;
				*img1 = aux1;
				aux1 = NULL;
			} else {
				aux2 = alloc_color_image(p->height, p->width);
				p->image_type = 1;
				*img2 = aux2;
				aux2 = NULL; } }
		//reading the pixels' values
		if (type <= 3) {
			if (type == 1 || type == 2)
				read_gray_image_ascii(*img1, p->height, p->width, fin);
			else
				read_color_image_ascii(*img2, p->height, p->width, fin);
		} else {
			fclose(fin);
			fin = fopen(s, "rb");
			//setting the cursor just before the pixels
			fsetpos(fin, &pos3);
			if (type == 4 || type == 5)
				read_gray_image_binary(*img1, p->height, p->width, fin);
			else if (type == 6)
				read_color_image_binary(*img2, p->height, p->width, fin); }

		p->prev_images = p->prev_images + 1;
		fclose(fin); }
}

//the function selects a specific zone of the image
//characterized by the upper-left corner and the lower-right one
void select_area(char s[], current_image *p)
{
	int x1, x2, y1, y2, rows, columns, ok = 1;

	//extracting the coordinates from the given command (from right to left)
	y2 = string_to_number(strrchr(s, ' ') + 1);
	strcpy(strrchr(s, ' '), "");
	x2 = string_to_number(strrchr(s, ' ') + 1);
	strcpy(strrchr(s, ' '), "");
	y1 = string_to_number(strrchr(s, ' ') + 1);
	strcpy(strrchr(s, ' '), "");
	x1 = string_to_number(strrchr(s, ' ') + 1);

	//checking  if the selection is valid (ok remains 1)
	if (!correct_coordinate(x1, y1, *p) || !correct_coordinate(x2, y2, *p))
		ok = 0;
	if (x1 == x2 || y1 == y2)
		ok = 0;

	if (!ok) {
		printf("Invalid set of coordinates\n");
	} else {
		//putting the corners in the right order
		if (x1 > x2)
			swap(&x1, &x2);
		if (y1 > y2)
			swap(&y1, &y2);
		//updating the selection of the current image
		p->x1 = x1;
		p->x2 = x2;
		p->y1 = y1;
		p->y2 = y2;

		rows = p->y2 - p->y1;
		columns = p->x2 - p->x1;

		//checking if the whole image has been selected
		if (rows != p->height || columns != p->width)
			p->selected_all = 0;
		else
			p->selected_all = 1;

		printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
	}
}

//the function selects the current image entirely
//by updating the coordinates of the corners (upper-left and lower-right)
void select_whole_image(current_image *p)
{
	printf("Selected ALL\n");

	p->x1 = 0;
	p->y1 = 0;
	p->x2 = p->width;
	p->y2 = p->height;
	p->selected_all = 1;
}

//the function rotates the whole grayscale image/selection
//90 degrees to the right
void rotate_clockwise_gray(int ***img, current_image *p)
{
	int i, j, n = p->y2 - p->y1;
	int **aux, **x;

	if (!p->selected_all) {
		//allocating 2 auxiliary images for the selection's rotation
		aux = alloc_gray_image(n, n);
		x = alloc_gray_image(n, n);

		//copying the pixels' values
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				x[i][j] = (*img)[i + p->y1][j + p->x1];

		//rotating the selection (based on a mathematical identity)
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				aux[i][j] = x[n - j - 1][i];

		//replacing the newly rotated pixels within the given image
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				(*img)[i + p->y1][j + p->x1] = aux[i][j];

		//deallocating the auxiliary images
		dealloc_gray_image(x, n);
		dealloc_gray_image(aux, n);
	} else {
		//allocating memory for the rotated image
		aux = alloc_gray_image(p->width, p->height);

		//rotating the whole image (based on a mathematical identity)
		for (i = 0; i < p->height; i++)
			for (j = 0; j < p->width; j++)
				aux[j][p->height - i - 1] = (*img)[i][j];

		//deallocating the memory for the old image
		dealloc_gray_image(*img, p->height);

		//updating some of the current's image details:
		//dimensions and the selection corners (upper-left and lower-right)
		swap(&p->height, &p->width);
		swap(&p->x2, &p->y2);

		//replacing the old image with the newly rotated one by changing
		//the old one's address:
		//it now points to the recently allocated memory zone
		*img = aux;
		aux = NULL;
	}
}

//the function rotates the whole color image/selection
//90 degrees to the right
void rotate_clockwise_color(color_image ***img, current_image *p)
{
	int i, j, n = p->y2 - p->y1;
	color_image **aux, **x;

	if (!p->selected_all) {
		//allocating 2 auxiliary images for the selection's rotation
		aux = alloc_color_image(n, n);
		x = alloc_color_image(n, n);

		//copying the pixels' values
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++) {
				x[i][j].R = (*img)[i + p->y1][j + p->x1].R;
				x[i][j].G = (*img)[i + p->y1][j + p->x1].G;
				x[i][j].B = (*img)[i + p->y1][j + p->x1].B;
			}

		//rotating the selection (based on a mathematical identity)
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++) {
				aux[i][j].R = x[n - j - 1][i].R;
				aux[i][j].G = x[n - j - 1][i].G;
				aux[i][j].B = x[n - j - 1][i].B;
			}

		//replacing the newly rotated pixels within the given image
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++) {
				(*img)[i + p->y1][j + p->x1].R = aux[i][j].R;
				(*img)[i + p->y1][j + p->x1].G = aux[i][j].G;
				(*img)[i + p->y1][j + p->x1].B = aux[i][j].B;
			}

		//deallocating the auxiliary images
		dealloc_color_image(x, n);
		dealloc_color_image(aux, n);
	} else {
		//allocating memory for the rotated image
		aux = alloc_color_image(p->width, p->height);

		//rotating the whole image (based on a mathematical identity)
		for (i = 0; i < p->height; i++)
			for (j = 0; j < p->width; j++) {
				aux[j][p->height - i - 1].R = (*img)[i][j].R;
				aux[j][p->height - i - 1].G = (*img)[i][j].G;
				aux[j][p->height - i - 1].B = (*img)[i][j].B;
			}

		//deallocating the memory for the old image
		dealloc_color_image(*img, p->height);

		//updating some of the current's image details:
		//dimensions and the selection corners (upper-left and lower-right)
		swap(&p->height, &p->width);
		swap(&p->x2, &p->y2);

		//replacing the old image with the newly rotated one by changing
		//the old one's address:
		//it now points to the recently allocated memory zone
		*img = aux;
		aux = NULL;
	}
}

//the function rotates the whole grayscale image/selection
//90 degrees to the left
void rotate_counterwise_gray(int ***img, current_image *p)
{
	int i, j, n = p->y2 - p->y1;
	int **aux, **x;

	if (!p->selected_all) {
		//allocating 2 auxiliary images for the selection's rotation
		aux = alloc_gray_image(n, n);
		x = alloc_gray_image(n, n);

		//copying the pixels' values
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				x[i][j] = (*img)[i + p->y1][j + p->x1];

		//rotating the selection (based on a mathematical identity)
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				aux[i][j] = x[j][n - i - 1];

		//replacing the newly rotated pixels within the given image
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				(*img)[i + p->y1][j + p->x1] = aux[i][j];

		//deallocating the auxiliary images
		dealloc_gray_image(x, n);
		dealloc_gray_image(aux, n);
	} else {
		//allocating memory for the rotated image
		aux = alloc_gray_image(p->width, p->height);

		//rotating the whole image (based on a mathematical identity)
		for (i = 0; i < p->height; i++)
			for (j = 0; j < p->width; j++)
				aux[p->width - 1 - j][i] = (*img)[i][j];

		//deallocating the memory for the old image
		dealloc_gray_image(*img, p->height);

		//updating some of the current's image details:
		//dimensions and the selection corners (upper-left and lower-right)
		swap(&p->height, &p->width);
		swap(&p->x2, &p->y2);

		//replacing the old image with the newly rotated one by changing
		//the old one's address:
		//it now points to the recently allocated memory zone
		*img = aux;
		aux = NULL;
	}
}

//the function rotates the whole color image/selection
//90 degrees to the left
void rotate_counterwise_color(color_image ***img, current_image *p)
{
	int i, j, n = p->y2 - p->y1;
	color_image **aux, **x;

	if (!p->selected_all) {
		//allocating 2 auxiliary images for the selection's rotation
		aux = alloc_color_image(n, n);
		x = alloc_color_image(n, n);

		//copying the pixels' values
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++) {
				x[i][j].R = (*img)[i + p->y1][j + p->x1].R;
				x[i][j].G = (*img)[i + p->y1][j + p->x1].G;
				x[i][j].B = (*img)[i + p->y1][j + p->x1].B;
			}

		//rotating the selection (based on a mathematical identity)
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++) {
				aux[i][j].R = x[j][n - i - 1].R;
				aux[i][j].G = x[j][n - i - 1].G;
				aux[i][j].B = x[j][n - i - 1].B;
			}

		//replacing the newly rotated pixels within the given image
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++) {
				(*img)[i + p->y1][j + p->x1].R = aux[i][j].R;
				(*img)[i + p->y1][j + p->x1].G = aux[i][j].G;
				(*img)[i + p->y1][j + p->x1].B = aux[i][j].B;
			}

		//deallocating the auxiliary images
		dealloc_color_image(x, n);
		dealloc_color_image(aux, n);
	} else {
		//allocating memory for the rotated image
		aux = alloc_color_image(p->width, p->height);

		//rotating the whole image (based on a mathematical identity)
		for (i = 0; i < p->height; i++)
			for (j = 0; j < p->width; j++) {
				aux[p->width - 1 - j][i].R = (*img)[i][j].R;
				aux[p->width - 1 - j][i].G = (*img)[i][j].G;
				aux[p->width - 1 - j][i].B = (*img)[i][j].B;
			}

		//deallocating the memory for the old image
		dealloc_color_image(*img, p->height);

		//updating some of the current's image details:
		//dimensions and the selection corners (upper-left and lower-right)
		swap(&p->height, &p->width);
		swap(&p->x2, &p->y2);

		//replacing the old image with the newly rotated one by changing
		//the old one's address:
		//it now points to the recently allocated memory zone
		*img = aux;
		aux = NULL;
	}
}

//the function rotates a certain selection of the image/the whole one
//for a certain number of degrees
void rotate(char s[], current_image *p, int ***img1, color_image ***img2)
{
	int angle, i, rows, columns;

	//the selection's dimensions
	rows = p->y2 - p->y1;
	columns = p->x2 - p->x1;

	//extracting the angle from the given command
	angle = string_to_number(strrchr(s, ' ') + 1);

	if (!(angle % 90)) {
		if ((!p->selected_all && rows == columns) || p->selected_all) {
			printf("Rotated %d\n", angle);

			//rotating repetitively by 90 degrees at a time (to left/right)
			for (i = 0; i < abs(angle / 90); i++)
				if (angle < 0) {
					if (!p->image_type)
						rotate_counterwise_gray(img1, p);
					else
						rotate_counterwise_color(img2, p);
				} else {
					if (!p->image_type)
						rotate_clockwise_gray(img1, p);
					else
						rotate_clockwise_color(img2, p);
				}
		} else {
			if (!p->selected_all && rows != columns)
				printf("The selection must be square\n");
		}
	} else {
		printf("Unsupported rotation angle\n");
	}
}

//the function reduces the current image to the selected area
void crop_image(int ***img1, color_image ***img2, current_image *p)
{
	int **aux1;
	color_image **aux2;
	int i, j;

	printf("Image cropped\n");

	if (!p->image_type) {
		//allocating memory for the cropped grayscale image
		aux1 = alloc_gray_image(p->y2 - p->y1, p->x2 - p->x1);

		//copying the pixels' values
		for (i = 0; i < p->y2 - p->y1; i++)
			for (j = 0; j < p->x2 - p->x1; j++)
				aux1[i][j] = (*img1)[i + p->y1][j + p->x1];

		//deallocating the memory for the old image
		dealloc_gray_image(*img1, p->height);

		//updating some of the current's image details:
		//dimensions and the selection corners (upper-left and lower-right)
		p->width = p->x2 - p->x1;
		p->height = p->y2 - p->y1;
		p->x1 = 0;
		p->y1 = 0;
		p->x2 = p->width;
		p->y2 = p->height;
		p->selected_all = 1;

		//replacing the old image with the newly cropped one by changing
		//the old one's address:
		//it now points to the recently allocated memory zone
		*img1 = aux1;
		aux1 = NULL;
	} else {
		//allocating memory for the cropped color image
		aux2 = alloc_color_image(p->y2 - p->y1, p->x2 - p->x1);

		//copying the pixels' values
		for (i = 0; i < p->y2 - p->y1; i++)
			for (j = 0; j < p->x2 - p->x1; j++) {
				aux2[i][j].R = (*img2)[i + p->y1][j + p->x1].R;
				aux2[i][j].G = (*img2)[i + p->y1][j + p->x1].G;
				aux2[i][j].B = (*img2)[i + p->y1][j + p->x1].B;
			}

		//deallocating the memory for the old image
		dealloc_color_image(*img2, p->height);

		//updating some of the current's image details:
		//dimensions and the selection corners (upper-left and lower-right)
		p->width = p->x2 - p->x1;
		p->height = p->y2 - p->y1;
		p->x1 = 0;
		p->y1 = 0;
		p->x2 = p->width;
		p->y2 = p->height;
		p->selected_all = 1;

		//replacing the old image with the newly cropped one by changing
		//the old one's address:
		//it now points to the recently allocated memory zone
		*img2 = aux2;
		aux2 = NULL;
	}
}

//the function applies the desired filter to the current color image
void apply(char filter[], color_image **img, current_image p, double f[3][3])
{
	//the rows and columns directions that help accessing a pixel's neighbours
	int r[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
	int c[8] = {0, 1, 1, 1, 0, -1, -1, -1};
	int i, j, k;
	double sr, sg, sb;
	color_image **x;

	//checking if the current image is colored or not
	if (!p.image_type) {
		printf("Easy, Charlie Chaplin\n");
	} else {
		printf("APPLY %s done\n", filter);

		//allocating memory for an auxiliary image
		x = alloc_color_image(p.height, p.width);

		//copying the pixel's values
		for (i = 0; i < p.height; i++)
			for (j = 0; j < p.width; j++) {
				x[i][j].R = img[i][j].R;
				x[i][j].G = img[i][j].G;
				x[i][j].B = img[i][j].B;
			}

		//applying the filter to a pixel if it has enough neighbours (all 8)
		for (i = p.y1; i < p.y2; i++)
			for (j = p.x1; j < p.x2; j++)
				if (enough_neighbours(j, i, p)) {
					//initializing the sums for all 3 color channels
					sr = x[i][j].R * f[1][1];
					sg = x[i][j].G * f[1][1];
					sb = x[i][j].B * f[1][1];

					//adding the product between the kernel and the neighbours'
					//pixel values to all 3 sums
					for (k = 0; k < 8; k++) {
						sr += x[i + r[k]][j + c[k]].R * f[1 + r[k]][1 + c[k]];
						sg += x[i + r[k]][j + c[k]].G * f[1 + r[k]][1 + c[k]];
						sb += x[i + r[k]][j + c[k]].B * f[1 + r[k]][1 + c[k]];
					}

					//calculating the precise values of the 3 color channels
					//of the current pixel
					sr /= 10000;
					sg /= 10000;
					sb /= 10000;

					//correcting the values if necessary
					color_correction(&sr, p);
					color_correction(&sg, p);
					color_correction(&sb, p);

					//replacing the current's pixel value
					img[i][j].R = sr;
					img[i][j].G = sg;
					img[i][j].B = sb;
				}

		//deallocating the auxiliary image
		dealloc_color_image(x, p.height);
	}
}
