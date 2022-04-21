#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "functions.h"
#include "auxiliary.h"

#define L_MAX 1000
#define S_MAX 100

int main(void)
{
	int **image1;
	color_image **image2;
	current_image info;
	char command[L_MAX], filename[S_MAX], filter[S_MAX];
	char known_commands[8][15], known_filters[4][20];
	double ker[3][3];
	int c;

	//initializing the total number of memorized images
	info.prev_images = 0;

	//reading the first command from the console
	init_keywords(known_commands, known_filters);
	fgets(command, L_MAX, stdin);

	//reading the commands one by one until the exit point is reached
	while (strcmp(command, "EXIT")) {
		strcpy(command + strlen(command) - 1, command + strlen(command));
		c = correct_command(command, known_commands);
		//checking if the command is valid or not
		if (!c) {
			//extracting the input file name from the command
			strcpy(filename, strrchr(command, ' ') + 1);
			load(filename, &image1, &image2, &info);
		} else {
			//checking if the command can be applied (an image is memorized)
			if (info.prev_images && !info.hide) {
				if (c == 1) {
					//extracting the output file name from the command
					if (number_of_words(command) == 3) {
						strcpy(strrchr(command, ' '), "");
						strcpy(filename, strrchr(command, ' ') + 1);
						write_ascii(filename, info, image1, image2);
					} else if (number_of_words(command) == 2) {
						strcpy(filename, strrchr(command, ' ') + 1);
						write_binary(filename, info, image1, image2);
					}
				}
				if (c == 2)
					select_whole_image(&info);
				if (c == 3)
					select_area(command, &info);
				if (c == 4)
					rotate(command, &info, &image1, &image2);
				if (c == 5)
					crop_image(&image1, &image2, &info);
				if (c == 6) {
					//extracting the filter name from the command
					strcpy(filter, strrchr(command, ' ') + 1);
					if (correct_filter(filter, known_filters) >= 0) {
						init_filter(filter, ker);
						apply(filter, image2, info, ker);
					} else {
						printf("APPLY parameter invalid\n"); }
				}
		} else {
			printf("No image loaded\n"); }
		}
		if (c < 0) {
			if (info.prev_images && !info.hide)
				printf("Invalid command\n");
		}
		fgets(command, L_MAX, stdin);
	}

	//deallocating the used memory (if the case)
	if (info.prev_images) {
		if (!info.image_type)
			dealloc_gray_image(image1, info.height);
		else
			dealloc_color_image(image2, info.height);
	} else {
		printf("No image loaded\n"); }

	if (info.hide)
		printf("No image loaded\n");
}
