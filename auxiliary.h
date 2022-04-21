#ifndef __AUXILIARY__H__
#define __AUXILIARY__H__

//the neccesary structure for a color image
//with its color channels: red, green, blue
struct color_image {
	int R, G, B;
};

//the structure contains all the necessary details
//regarding a memorized picture
struct current_image {
	int width, height;
	int x1, x2, y1, y2;
	//the number of loaded images
	int prev_images;
	//0 for grayscale images
	//1 for color images
	int image_type;
	int max_value;
	//1 if the whole area is selected; 0 otherwise
	int selected_all;
	//1 if a "load" operation is not successful
	int hide;
};

typedef struct color_image color_image;
typedef struct current_image current_image;

void swap(int *a, int *b);

int number_of_words(char s[]);
int is_number(char s[]);
int string_to_number(char s[]);
int only_numbers(char s[]);
int number_of_words(char s[]);
int correct_command(char s[], char known_commands[8][15]);
int correct_filter(char s[], char known_filters[4][20]);
int correct_coordinate(int x, int y, current_image p);
int correct_pixel(int x, int y, current_image p);
int enough_neighbours(int x, int y, current_image p);
void init_filter(char filtername[], double filter[3][3]);
void init_keywords(char c[8][15], char f[4][20]);

int **alloc_gray_image(int rows, int cols);
color_image **alloc_color_image(int rows, int cols);
void dealloc_gray_image(int **image, int rows);
void dealloc_color_image(color_image **image, int rows);

void read_gray_image_ascii(int **image, int rows, int cols, FILE *f);
void read_color_image_ascii(color_image **p, int rows, int cols, FILE *f);
void read_gray_image_binary(int **image, int rows, int cols, FILE *f);
void read_color_image_binary(color_image **image, int rows, int cols, FILE *f);

void image_info(int type, current_image *p, FILE *f);
void copy_info(current_image source, current_image *destination);

void color_correction(double *value, current_image p);

#endif
