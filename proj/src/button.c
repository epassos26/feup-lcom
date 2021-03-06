#include "button.h"

button* create_button(short x, short y, unsigned short width, unsigned short height, Alignment button_align, void (*function)(), const char* title, unsigned short color, Alignment text_align){
	button* b = (button*) malloc(sizeof(button));
	b->x = x;
	b->y = y;

	if(button_align == ALIGN_CENTER)
		b->x -= (unsigned short) width/2;
	else if(button_align == ALIGN_RIGHT)
		b->x -= width;

	b->width = width;
	b->height = height;
	b->function = function;
	b->color = color;
	if(title == NULL || title == "")
		b->title = NULL;
	else{
		b->title = (char*) malloc(strlen(title)*sizeof(char));
		strcpy(b->title, title);
	}
	b->text_align = text_align;
	b->background = NULL;
	return b;
}

void button_set_background(button* b, bitmap* bmp){
	b->background = bmp;
}

void button_draw(button* b, font* f){
	if(f != NULL){
		switch(b->text_align){
		case ALIGN_LEFT:
			font_draw_string(f, b->x+5, b->y+5, b->title, b->text_align);
			break;
		case ALIGN_CENTER:
			font_draw_string(f, b->x+b->width/2, b->y+5, b->title, b->text_align);
			break;
		case ALIGN_RIGHT:
			font_draw_string(f, b->x+b->width, b->y+5, b->title, b->text_align);
			break;
		}
	}

	if(b->background != NULL)
		bitmap_draw(b->background, b->x, b->y, ALIGN_LEFT);
	else{
#ifdef DEBUG
		vg_draw_frame(b->x, b->y, b->width, b->height, b->color);
#endif
	}

}

char collides_with_button(button* b, short x, short y){
	if(x > b->x && x < (b->x + b->width) && y > b->y && y < (b->y + b->height))
		return 1;
	else
		return 0;
}

void button_delete(button* b){
	if(b->title != NULL)
		free(b->title);
	free(b);
}
