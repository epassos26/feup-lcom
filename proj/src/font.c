#include "font.h"

font* font_init(const char* font_name){
	font* f = (font*) malloc(sizeof(font));
	f->lower_limit = 32;
	f->higher_limit = 126;
	f->letters_per_line = 10;
	f->letter_width = LETTER_WIDTH;
	f->letter_height = LETTER_HEIGHT;

	unsigned char* full_path =  (unsigned char*) malloc((strlen(font_path) + strlen(font_name))*sizeof(unsigned char));

	strcpy(full_path, font_path);
	strcat(full_path, font_name);

	if((f->letters = bitmap_load(full_path)) == NULL){
		free(f);
		return NULL;
	}

	font_recolor(f, rgb(0x000000), rgb(0xFFFFFF));

	return f;
}

void font_recolor(font* f, unsigned short initial_color, unsigned short final_color){
	unsigned short i, j, width = f->letters->bmp_info_header.width, height = f->letters->bmp_info_header.height;

	for(i = 0; i < height; i++){
		for(j = 0; j < width; j++){
			if(*(f->letters->bmp_data + j + i*width) == initial_color)
				*(f->letters->bmp_data + j + i*width) = final_color;
		}
	}
}

void font_draw_int(font* f, short x, short y, int number, Alignment alignment){
	char* temp = (char*) malloc(100*sizeof(char));
	sprintf(temp, "%d", number);
	font_draw_string(f, x, y, temp, alignment);
	free(temp);
}

void font_draw_string(font* f, short x, short y, const char* str, Alignment alignment){
	if (str == NULL || str == "")
		return;

	unsigned short width = f->letters->bmp_info_header.width;
	unsigned short height = f->letters->bmp_info_header.height;
	unsigned short i, j, k;
	unsigned short *letter_position;
	unsigned short string_length = strlen(str);

	if(alignment == ALIGN_CENTER)
		x -= (unsigned short) ((LETTER_SPACEMENT*(string_length-1) + f->letter_width*string_length)/2);
	else if(alignment == ALIGN_RIGHT)
		x -= LETTER_SPACEMENT*(string_length-1) + f->letter_width*strlen(str);

	//prints the string

	/*for(i = 0; i < string_length; i++){ //seems a better approach, but does not work yet
		font_draw_char(f, x + i*LETTER_WIDTH, y, str[i]);
	}*/

	while(str[i] != '\0'){
		if(str[i] > f->lower_limit && str[i] < f->higher_limit){ //if the char is valid, print it, otherwise do nothing
			letter_position = f->letters->bmp_data + ((str[i] - f->lower_limit) / f->letters_per_line)*width*f->letter_height + ((str[i] - f->lower_limit) % f->letters_per_line)*f->letter_width;
			for(j = 0; j < f->letter_height; j++){
				for(k = 0; k < f->letter_width; k++){
					vg_set_pixel(x+k+i*(f->letter_width+LETTER_SPACEMENT), y+j, *(letter_position + j*width + k));
				}
			}
		}
		i++;
	}

	//vg_draw_pixmap(f->letters->bmp_data, x, y, LETTER_WIDTH, LETTER_HEIGHT);
}

void font_draw_char(font *f, short x, short y, char c, Alignment alignment){
	if(c < f->lower_limit || c > f->higher_limit)
		return;

	if(alignment == ALIGN_CENTER)
		x -= f->letter_width/2;
	else if(alignment == ALIGN_RIGHT)
		x -= f->letter_width;

	unsigned short width = f->letters->bmp_info_header.width;
	unsigned short* char_start_position = f->letters->bmp_data + ((c - f->lower_limit) / f->letters_per_line)*width*f->letter_height + ((c - f->lower_limit) % f->letters_per_line)*f->letter_width;

	unsigned short i, j;
	for(i = 0; i < f->letter_height; i++){
		for(j = 0; j < f->letter_width; j++)
			vg_set_pixel(x + j, y + i, *(char_start_position + j));
		char_start_position += width;
	}
}

void font_delete(font* f){
	bitmap_delete(f->letters);
	free(f);
}
