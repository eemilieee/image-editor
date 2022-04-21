#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "auxiliary.h"

#define L_MAX 1000
#define S_MAX 100

void swap(int *a, int *b)
{
	int aux;
	aux = *a;
	*a = *b;
	*b = aux;
}

//the function shows if a given parameter of the input command
//represents a number or not
int is_number(char s[])
{
	unsigned int i;

	for (i = 0; i < strlen(s); i++)
		if (!strchr("-0123456789", s[i]))
			return 0;

	return 1;
}

//the function returns the integer value of a text-type parameter
//given within a certain command
int string_to_number(char s[])
{
	int aux = 0, p = 1, i, sign = 0;

	//if the number is negative
	if (s[0] == '-')
		sign = 1;

	for (i = strlen(s) - 1; i >= 0; i--)
		if (strchr("1234567890", s[i])) {
			aux += p * (s[i] - '0');
			p *= 10;
		}

	if (sign)
		aux = -aux;

	return aux;
}

//the function tells whether all parameters of the imput command
//are numbers or not (otherwise it is invalid based on its given type)
int only_numbers(char s[])
{
	char aux[L_MAX];

	strcpy(aux, s);

	//the parameters are extracted from right to left
	while (strchr(aux, ' ')) {
		if (!is_number(strrchr(aux, ' ') + 1))
			return 0;

		//moving on to the next parameter
		strcpy(strrchr(aux, ' '), "");
	}

	return 1;
}

//the function returns the number of words a command line contains
//it is required to check whether it is valid or not
int number_of_words(char s[])
{
	char aux[L_MAX], *word;
	int nr = 0;

	strcpy(aux, s);

	word = strtok(aux, " ");

	while (word) {
		nr++;
		word = strtok(NULL, " ");
	}

	return nr;
}

//the function checks if a given command fulfills the requirements
//in order to be valid, depending on its type: if so,
//it returns the corresponding position from list of commands
//and -1 otherwise
int correct_command(char s[], char known_commands[8][15])
{
	int i, pos = -1;

	//erasing trailing whitespaces
	while (s[strlen(s) - 1] == ' ')
		strcpy(s + strlen(s) - 1, s + strlen(s));

	//verifying if a command appears in the given list
	for (i = 0; i < 8 && pos < 0; i++)
		if (!(strstr(s, known_commands[i]) - s))
			pos = i;

	//checking a command's structure based on its type:
	//number of parameters and their types (text/number)
	if (pos >= 0) {
		if (pos == 5 || pos == 7) {
			if (number_of_words(s) != 1)
				pos = -1;
		} else if (!(pos % 2)) {
			if (number_of_words(s) != 2)
				pos = -1;
			if (pos == 4)
				if (!only_numbers(s))
					pos = -1;
		} else if (pos == 3) {
			if (number_of_words(s) != 5)
				pos = -1;
			if (!only_numbers(s))
				pos = -1;
		} else if (pos == 1) {
			if (number_of_words(s) > 3 || number_of_words(s) < 2)
				pos = -1;
			if (number_of_words(s) == 3)
				if (strcmp(strrchr(s, ' ') + 1, "ascii"))
					pos = -1;
		}
	}

	return pos;
}

//the functions tells if the filter name given as a second
//paramteter for the "apply" command is found among the given ones;
//if so, it returns the corresponding position from list of filters
//and -1 otherwise
int correct_filter(char s[], char known_filters[4][20])
{
	int i;
	for (i = 0; i < 4; i++)
		if (strstr(s, known_filters[i]))
			return i;

	return -1;
}

//the function checks whether a point in the axis of coordinates XoY
//could represent the corners of a valid selection of the current image or not
int correct_coordinate(int x, int y, current_image p)
{
	if ((x < 0 || x > p.width) || (y < 0 || y > p.height))
		return 0;

	return 1;
}

//the function checks whether a point in the axis of coordinates XoY
//is found within the current image's region or not
int correct_pixel(int x, int y, current_image p)
{
	if ((x < 0 || x >= p.width) || (y < 0 || y >= p.height))
		return 0;

	return 1;
}

//the function tells if a pixel found in the current image
//could be applied a filter within the "apply" command
int enough_neighbours(int x, int y, current_image p)
{
	int row_directions[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
	int column_directions[8] = {0, 1, 1, 1, 0, -1, -1, -1};
	int i, nr = 0;

	//counting the current pixel's neighbours
	for (i = 0; i < 8; i++)
		if (correct_pixel(x + column_directions[i], y + row_directions[i], p))
			nr++;

	if (nr == 8)
		return 1;

	return 0;
}

//the function sets the specific values for the kernel matrix
//depending on the prefered filter
//the kernel's values are multiplied by 10000 in order to improve
//the accuracy of the mathematical operations when the "apply" command
//is being executed (when using double values that have 4 decimals)
//this way, the floating point is omitted up to a point and
//the integer values are processed easier
void init_filter(char filtername[], double filter[3][3])
{
	int i, j;
	if (!strcmp(filtername, "EDGE")) {
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j++)
				if (!i || i == 2 || !j || j == 2)
					filter[i][j] = -1;
		filter[1][1] = 8;
	} else if (!strcmp(filtername, "SHARPEN")) {
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j++)
				if ((!(i % 2) && j % 2) || (i % 2 && !(j % 2)))
					filter[i][j] = -1;
				else
					filter[i][j] = 0;
		filter[1][1] = 5;
	} else if (!strcmp(filtername, "BLUR")) {
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j++)
				filter[i][j] = 0.1111;
	} else if (!strcmp(filtername, "GAUSSIAN_BLUR")) {
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j++)
				if ((!(i % 2) && j % 2) || (i % 2 && !(j % 2)))
					filter[i][j] = 0.1250;
				else
					filter[i][j] = 0.0625;
		filter[1][1] = 0.2500;
	}

	for (int  i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			filter[i][j] *= 10000;
}

//the function initializes the command and filter lists
void init_keywords(char c[8][15], char f[4][20])
{
	strcpy(c[0], "LOAD");
	strcpy(c[1], "SAVE");
	strcpy(c[2], "SELECT ALL");
	strcpy(c[3], "SELECT");
	strcpy(c[4], "ROTATE");
	strcpy(c[5], "CROP");
	strcpy(c[6], "APPLY");
	strcpy(c[7], "EXIT");

	strcpy(f[0], "EDGE");
	strcpy(f[1], "SHARPEN");
	strcpy(f[2], "BLUR");
	strcpy(f[3], "GAUSSIAN_BLUR");
}

//the function returns a pointer to a newly allocated
//memory zone needed to store the grayscale image
int **alloc_gray_image(int rows, int cols)
{
	int **aux;
	int i;

	//allocating the rows
	aux = (int **)calloc(rows, sizeof(int *));

	//allocating each pixel on every row
	for (i = 0; i < rows; i++)
		aux[i] = (int *)calloc(cols, sizeof(int));

	return aux;
}

//the function returns a pointer to a newly allocated
//memory zone needed to store the color image
color_image **alloc_color_image(int rows, int cols)
{
	color_image **aux;
	int i;

	//allocating the rows
	aux = (color_image **)calloc(rows, sizeof(color_image *));

	//allocating each pixel on every row
	for (i = 0; i < rows; i++)
		aux[i] = (color_image *)calloc(cols, sizeof(color_image));

	return aux;
}

//the function deallocates the memory used to store
//a grayscale image that is no longer needed
void dealloc_gray_image(int **image, int rows)
{
	int i;

	//deallocating each row
	for (i = 0; i < rows; i++)
		free(image[i]);

	//deallocating the image's adress
	free(image);
}

//the function deallocates the memory used to store
//a color image that is no longer needed
void dealloc_color_image(color_image **image, int rows)
{
	int i;

	//deallocating each row
	for (i = 0; i < rows; i++)
		free(image[i]);

	//deallocating the image's adress
	free(image);
}

//the function reads the values contained by a grayscale image
//from the corresponding text file
void read_gray_image_ascii(int **image, int rows, int cols, FILE *f)
{
	int i, j;

	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++)
			fscanf(f, "%d", &image[i][j]);
}

//the function reads the values contained by a color image
//from the corresponding text file
//each pixel's values are multiplied by 1000 in order to improve
//the accuracy of the mathematical operations when the "apply" command
//is being executed (when using double values that have 4 decimals)
//this way, the floating point is omitted up to a point and
//the integer values are processed easier
void read_color_image_ascii(color_image **p, int rows, int cols, FILE *f)
{
	int i, j;

	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++) {
			fscanf(f, "%d", &p[i][j].R);
			fscanf(f, "%d", &p[i][j].G);
			fscanf(f, "%d", &p[i][j].B);

			p[i][j].R *= 1000;
			p[i][j].G *= 1000;
			p[i][j].B *= 1000;
		}
}

//the function reads the values contained by a grayscale image
//from the corresponding binary file
void read_gray_image_binary(int **image, int rows, int cols, FILE *f)
{
	int i, j;

	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++)
			fread(&image[i][j], sizeof(char), 1, f);
}

//the function reads the values contained by a color image
//from the corresponding binary file
//each pixel's values are multiplied by 1000 in order to improve
//the accuracy of the mathematical operations when the "apply" command
//is being executed (when using double values that have 4 decimals)
//this way, the floating point is omitted up to a point and
//the integer values are processed easier
void read_color_image_binary(color_image **image, int rows, int cols, FILE *f)
{
	int i, j;

	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++) {
			fread(&image[i][j].R, sizeof(char), 1, f);
			fread(&image[i][j].G, sizeof(char), 1, f);
			fread(&image[i][j].B, sizeof(char), 1, f);

			image[i][j].R *= 1000;
			image[i][j].G *= 1000;
			image[i][j].B *= 1000;
		}
}

//the function memorizes the newly read image details
//found in the recently opened file
void image_info(int type, current_image *p, FILE *f)
{
	int rows = 0, columns = 0, max = 0;
	char line[L_MAX];

	//the max value for black and white images
	if (type == 1 || type == 4)
		max = 1;

	//extraxting the image's width, height and max value
	//while omitting the comments
	while (!rows || !columns || !max) {
		fgets(line, L_MAX, f);
		strcpy(line + strlen(line) - 1, line + strlen(line));
		if (!strchr(line, '#')) {
			if (number_of_words(line) == 2) {
				rows = string_to_number(strrchr(line, ' ') + 1);
				strcpy(strchr(line, ' '), "");
				columns = string_to_number(line);
			} else {
				if (number_of_words(line) == 1)
					max = string_to_number(line);
			}
		}
	}

	p->width = columns;
	p->height = rows;

	p->max_value = max;

	p->x1 = 0;
	p->x2 = columns;
	p->y1 = 0;
	p->y2 = rows;

	p->selected_all = 1;

	p->hide = 0;
}

//the function updates the current image's details
void copy_info(current_image source, current_image *destination)
{
	destination->width = source.width;
	destination->height = source.height;

	destination->max_value = source.max_value;

	destination->x1 = source.x1;
	destination->x2 = source.x2;
	destination->y1 = source.y1;
	destination->y2 = source.y2;

	destination->selected_all = source.selected_all;

	destination->hide = source.hide;
}

//the implementation of the clamp() function
//adapted to the applied mathematical operations that ensure the accuracy
void color_correction(double *value, current_image p)
{
	if (*value < 0)
		*value = 0;
	if (*value > p.max_value * 1000)
		*value = p.max_value * 1000;
}
