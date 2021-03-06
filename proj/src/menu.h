#ifndef MENU_H
#define MENU_H

#include "button.h"

/** @defgroup menu menu
 * @{
 * menu
 */

/** @name Menu Information Struct*/
/**@{
 *
 * Struct that stores info regarding a menu and its buttons
 */
typedef struct{
	button* (*buttons);
	unsigned char buttons_size;
	char selected_button;
	font* f;
} menu;
/** @} end of Menu Information Struct */

/**
 * @brief Initializes menu struct
 *
 * @param font_name Name of the font to use with menu's buttons.
 *
 * @return Returns pointer to initialized menu
 */
menu* create_menu(const char* font_name);

/**
 * @brief Adds button to menu
 *
 * @param m Menu in which to add the button
 * @param b Button to be added
 */
void menu_add_button(menu* m, button* b);

/**
 * @brief Deletes button from menu and updates the array. Frees the deleted button.
 *
 * @param m Menu in which to delete the button
 * @param index Index of button to be destroyed
 */
void menu_delete_button(menu* m, unsigned char index);

/**
 * @brief Draws every button in the menu
 *
 * @param m Menu to draw
 */
void menu_draw(menu* m);

/**
 * @brief Checks if a button contained in menu is clicked and calls its function.
 *
 * @param m Menu in which to check
 * @param x Mouse position in the x axis
 * @param y Mouse position in the y axis
 *
 * @return Returns 1 if a button was clicked. Returns 0 otherwise.
 */
unsigned char click_button(menu* m, short x, short y);

/**
 * @brief Select previous button on screen
 *
 * @param m Menu info
 */
unsigned char previous_button(menu* m);

/**
 * @brief Select next button on screen
 *
 * @param m Menu info
 */
unsigned char next_button(menu* m);

/**
 * @brief Resets button selection
 *
 * @param m Menu info
 */
void cancel_button(menu* m);

/**
 * @brief Pressed the selected button by calling its function
 *
 * @param m Menu info
 */
unsigned char press_selected_button(menu* m);

/**
 * @brief Deletes and frees menu and its buttons.
 *
 * @param m Menu to be deleted
 */
void delete_menu(menu* m);

#endif
