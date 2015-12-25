#ifndef FONT_H
#define FONT_H

#include "bitmap.h"

#define LETTER_HEIGHT 44
#define LETTER_WIDTH 32

static char font_path[] = "";

typedef struct{
	unsigned char lower_limit, higher_limit, letters_per_line;
	unsigned short letter_width, letter_height;
	bitmap* letters;
} font; 

/**
* @brief Initializes font stored in filename, in the font_path directory.
*
* @return Returns pointer to font
*/
font* font_init(const char* filename);

/**
* @brief Draws string on x,y position
*
* @param x Position on the x axis to print the string
* @param y Position on the y axis to print the string
* @param str String to print
*/
void font_draw_string(font* f, short x, short y, const char* str, Alignment alignment);

/**
 * @brief Changes the color in the font
 *
 * @param f Font to recolor
 * @param initial_color Color to replace
 * @param final_color Color to replace with
 */
void font_recolor(font* f, unsigned short initial_color, unsigned short final_color);

/**
 * @brief Draws char on specified position
 *
 * @param f Font with which to draw
 * @param x Position in the x axis
 * @param y Position in the y axis
 * @param c Character to draw
 */
void font_draw_char(font *f, short x, short y, char c);

/**
 * @brief Delete font
 *
 * @param f Font to be deleted
 */
void font_delete(font* f);

#endif
