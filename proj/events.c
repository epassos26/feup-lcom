#include "events.h"
#include "keyboard.h"
#include "mouse.h"
#include "menu.h"
#include "button.h"
#include "video_gr.h"
#include "vbe.h"
#include "proj.h"
#include "i8042.h"

extern mouse_info_t mouse_info;
extern enum game_state state;
extern enum singleplayer_controller controller;
extern menu* start_menu;

unsigned short get_h_res() { //temporary
	return 1024;
}

unsigned short get_v_res() { //temporary
	return 768;
}

void kb_event_handler(unsigned short key) {
	if (key & KB_BREAKCODE)
		key_released(key);
	else
		key_pressed(key);
}

void mouse_event_handler(unsigned char packet[3]) {
	//Middle mouse button pressed/released

	if (!(packet[0] & MOUSE_MB_PRESSED) && mouse_info.mmb_pressed) {
		mouse_info.mmb_pressed = 0;
		mmb_released();
	} else if ((packet[0] & MOUSE_MB_PRESSED) && !mouse_info.mmb_pressed) {
		mouse_info.mmb_pressed = 1;
		mmb_pressed();
	}

	//Right mouse button pressed/released

	if (!(packet[0] & MOUSE_RB_PRESSED) && mouse_info.rmb_pressed) {
		mouse_info.rmb_pressed = 0;
		rmb_released();
	} else if ((packet[0] & MOUSE_RB_PRESSED) && !mouse_info.rmb_pressed) {
		mouse_info.rmb_pressed = 1;
		rmb_pressed();
	}

	//Left mouse button pressed/released

	if (!(packet[0] & MOUSE_LB_PRESSED) && mouse_info.lmb_pressed) {
		mouse_info.lmb_pressed = 0;
		lmb_released();
	} else if ((packet[0] & MOUSE_LB_PRESSED) && !mouse_info.lmb_pressed) {
		mouse_info.lmb_pressed = 1;
		lmb_pressed();
	}

	//Update mouse x position

	if (packet[0] & MOUSE_X_SIGNED) {
		short xdelta = 0xFF00 | packet[1];
		mouse_info.x += xdelta;
		if (mouse_info.x < 0)
			mouse_info.x = 0;
	} else {
		mouse_info.x += packet[1];
		if (mouse_info.x > get_h_res())
			mouse_info.x = get_h_res();
	}

	//Update mouse y position

	if (packet[0] & MOUSE_Y_SIGNED) {
		short ydelta = 0xFF00 | packet[2];
		mouse_info.y -= ydelta;
		if (mouse_info.y > get_v_res())
			mouse_info.y = get_v_res();
	} else {
		mouse_info.y -= packet[2];
		if (mouse_info.y < 0)
			mouse_info.y = 0;
	}
}

void key_pressed(unsigned long key) {
	switch (state) {
	case main_menu: {
		switch (key) {
		case UP_ARROW_MAKECODE:
			previous_button(start_menu);
			break;
		case DOWN_ARROW_MAKECODE:
			next_button(start_menu);
			break;
		case ENTER_MAKECODE:
			press_selected_button(start_menu);
			break;
		case ESC_MAKECODE:
			cancel_button(start_menu);
			break;
		}
		break;
	}
	case singleplayer:

		break;
	case multiplayer:

		break;
	case highscore:

		break;
	case options:

		break;
	}
}

void key_released(unsigned long key) {
	switch (state) {
	case main_menu:

		break;
	case singleplayer:

		break;
	case multiplayer:

		break;
	case highscore:

		break;
	case options:

		break;
	}
}

void mmb_released() {
	//printf("Middle mouse button released.\n");
}

void mmb_pressed() {
	//printf("Middle mouse button pressed.\n");
}

void rmb_released() {
	//printf("Right mouse button released.\n");
}

void rmb_pressed() {
	//printf("Right mouse button pressed.\n");
}

void lmb_released() {
	//printf("Left mouse button released.\n");
	if (state == main_menu) {
		click_button(start_menu, mouse_info.x, mouse_info.y);
	}
}

void lmb_pressed() {
	//printf("Left mouse button pressed.\n");
}

void tick() {
	switch (state) {
	case main_menu:
		menu_draw(start_menu);
		draw_mouse();
		break;
	case singleplayer:
		singleplayer_tick();
		break;
	case multiplayer:

		break;
	case highscore:

		break;
	case options:

		break;
	}

	vg_update_screen();
	//printf("Tick.\n");
}
