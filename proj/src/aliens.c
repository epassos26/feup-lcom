#include "aliens.h"

static char direction;

void alien_list_init() {
	direction = 1;

	if ((invaders = (alien_list*) malloc(sizeof(alien_list))) == NULL)
		return;

	invaders->head = NULL;
	invaders->last = NULL;
	invaders->movement_frequency = INITIAL_MOV_FREQ;

	ufo = NULL;

	unsigned char row_pos;
	unsigned char line = 0;
	unsigned char counter;
	int x = INITIAL_X_POS;
	int y = INITIAL_Y_POS;
	enum alien_type type;

	for (counter = 0; counter < ALIENS_PER_ROW * ALIEN_ROWS; counter++) {

		if (counter % ALIENS_PER_ROW == 0) {
			y += ALIEN_HEIGHT + ALIEN_SPACEMENT;
			x = INITIAL_X_POS;
			line++;
		}
		else
			x += ALIEN_WIDTH + ALIEN_SPACEMENT;

		switch(line){
		case 1:
			type = SMALL;
			break;
		case 2: case 3:
			type = MEDIUM;
			break;
		case 4:	case 5:
			type = LARGE;
			break;
		}
		alien_add((alien_init(x, y, type)));
	}

	search_new_extreme(1);
	search_new_extreme(2);

	//initialize small aliens animation
	invaders->small_alien = animation_init();
	animation_add_by_filename(invaders->small_alien, "small_invader1.bmp");
	animation_add_by_filename(invaders->small_alien, "small_invader2.bmp");

	//initialize medium aliens animation
	invaders->medium_alien = animation_init();
	animation_add_by_filename(invaders->medium_alien, "medium_invader1.bmp");
	animation_add_by_filename(invaders->medium_alien, "medium_invader2.bmp");

	//initialize large aliens animation
	invaders->large_alien = animation_init();
	animation_add_by_filename(invaders->large_alien, "large_invader1.bmp");
	animation_add_by_filename(invaders->large_alien, "large_invader2.bmp");

	//initialize ufo image
	ufo_img = bitmap_load("ufo.bmp");

	invaders->destroy = animation_init();
	animation_add_by_filename(invaders->destroy, "explosion.bmp");
}

void alien_add(alien *a1) {

	if (invaders->head == NULL) {
		invaders->head = a1;
		invaders->last = a1;
		return;
	}

	invaders->last->next = a1;
	a1->prev = invaders->last;
	invaders->last = a1;
}

int alien_remove(alien *a1) {
	unsigned char flag = 0;

	invaders->movement_frequency -= MOVEMENT_INCREASE;
	if(invaders->movement_frequency < 1)
		invaders->movement_frequency = 1;

	if (invaders->rightmost == a1)
		flag = 1;

	else if (invaders->leftmost == a1)
		flag = 2;

	if (invaders->head == a1) {

		if (invaders->last == a1) { /*if the alien to remove is the last adversary */
			invaders->head = NULL;
			invaders->last = NULL;
			invaders->rightmost = NULL;
			invaders->leftmost = NULL;
			free(a1);
			return 1;
		}

		invaders->head = invaders->head->next;
		free(a1);
		invaders->head->prev = NULL;
		if (!flag)
			return 0;
		else {
			search_new_extreme(flag);
			return 0;
		}
	}

	if (invaders->last == a1) {

		invaders->last = a1->prev;
		a1->prev->next = NULL;
		free(a1);

		if (!flag)
			return 0;
		else {
			search_new_extreme(flag);
			return 0;
		}
	}

	alien* iterator = invaders->head;
	do {
		if (iterator->next == a1) {
			iterator->next = a1->next;
			free(a1);
			iterator->next->prev = iterator;

			if (!flag)
				return 0;

			else {
				search_new_extreme(flag);
				return 0;
			}
		}

		iterator = iterator->next;
	} while (iterator != NULL);

	return 0;
}

int aliens_move() {
	animation_next(invaders->small_alien);
	animation_next(invaders->medium_alien);
	animation_next(invaders->large_alien);

	if(invaders->head == NULL)
		return -1;

	alien* iterator = invaders->head;

	if ((direction == -1
			&& invaders->leftmost->x + direction * ALIEN_X_DELTA
			< PLACEHOLDER_LEFT_BORDER)
			|| (direction == 1
					&& invaders->rightmost->x + invaders->rightmost->width
					+ ALIEN_X_DELTA > PLACEHOLDER_RIGHT_BORDER)) {

		if (invaders->last->y + invaders->last->height + ALIEN_Y_DELTA
				> PLACEHOLDER_SHIELD_LINE) {
			return -1;
		}

		do {
			alien_move(iterator, 0, ALIEN_Y_DELTA);
			iterator = iterator->next;
		} while (iterator != NULL);

		direction = (-1) * direction;

	} else
		do {
			alien_move(iterator, direction * ALIEN_X_DELTA, 0);
			iterator = iterator->next;
		} while (iterator != NULL);

	return 0;
}

int alien_draw(alien *a1) {
	if(a1->state == ALIEN_ALIVE){
		switch (a1->type) {
		case SMALL:
			bitmap_draw(invaders->small_alien->current->bmp, a1->x, a1->y,
					ALIGN_LEFT);
			break;
		case MEDIUM:
			bitmap_draw(invaders->medium_alien->current->bmp, a1->x, a1->y,
					ALIGN_LEFT);
			break;
		case LARGE:
			bitmap_draw(invaders->large_alien->current->bmp, a1->x, a1->y,
					ALIGN_LEFT);
			break;
		case UFO:
			bitmap_draw(ufo_img, a1->x, a1->y, ALIGN_LEFT);
			break;
		}
	} else {
		bitmap_draw(invaders->destroy->current->bmp, a1->x, a1->y, ALIGN_LEFT);
	}


#ifdef DEBUG
	if (invaders->last == a1)
		vg_draw_frame(a1->x + a1->width / 2, a1->y + a1->height / 2, 5, 5,
				rgb(0xCC0000));
	if (invaders->head == a1)
		vg_draw_frame(a1->x + a1->width / 2, a1->y + a1->height / 2, 5, 5,
				rgb(0x00FF00));
	if (invaders->leftmost == a1)
		vg_draw_frame(a1->x + a1->width / 2, a1->y + a1->height / 2, 5, 5,
				rgb(0xCC0066));
	if (invaders->rightmost == a1)
		vg_draw_frame(a1->x + a1->width / 2, a1->y + a1->height / 2, 5, 5,
				rgb(0xFFFF00));
#endif
}

void alien_move(alien* a1, char x, char y) {
	if(a1->state == ALIEN_ALIVE){
		a1->x += x;
		a1->y += y;
	}
}

alien *alien_init(int x, int y, enum alien_type type) {
	alien *et;

	if ((et = (alien *) malloc(sizeof(alien))) == NULL)
		return;

	et->x = x;
	et->y = y;
	et->type = type;

	et->state = ALIEN_ALIVE;
	et->width = ALIEN_WIDTH;
	et->height = ALIEN_HEIGHT;
	et->next = NULL;
	et->prev = NULL;
	et->ticks = 0;

	return et;
}

int aliens_collision_handler(projectile* proj) {
	unsigned short rightmost_collision_point, lowest_collision_point;

	if (invaders->head == NULL)
		return 0;

	rightmost_collision_point = invaders->rightmost->x + ALIEN_WIDTH;
	lowest_collision_point = invaders->last->y + ALIEN_HEIGHT;

	if(ufo != NULL){
		if(ufo->state == ALIEN_ALIVE){
			if(((proj->x >= ufo->x && proj->x <= ufo->x + ufo->width) &&
					(proj->y >= ufo->y && proj->y <= ufo->y + ufo->height)) ||
					((proj->x + proj->width >= ufo->x && proj->x + proj->width <= ufo->x + ufo->width) &&
							(proj->y + proj->height >= ufo->y && proj->y + proj->height <= ufo->y + ufo->height))){
				ufo->state = ALIEN_DESTROYED;
				ufo->ticks = 0;
				proj->shooter->score += UFO_ALIEN_SCORE;
			}
		}
	}

	if (proj->x >= invaders->leftmost->x && proj->x <= rightmost_collision_point
			&& proj->y >= invaders->head->y && proj->y <= lowest_collision_point) {
		alien* iterator = invaders->head;

		do {
			if(iterator->state == ALIEN_ALIVE){
				if ((proj->x > iterator->x && proj->x < iterator->x + ALIEN_WIDTH &&
						proj->y > iterator->y && proj->y < iterator->y + ALIEN_HEIGHT) ||
						(proj->x + proj->width > iterator->x && proj->x + proj->width < iterator->x + ALIEN_WIDTH &&
								proj->y + proj->height > iterator->y && proj->y + proj->height < iterator->y + ALIEN_HEIGHT)) {
					if(proj->shooter != NULL){
						switch(iterator->type){
						case SMALL:
							proj->shooter->score += SMALL_ALIEN_SCORE;
							break;
						case MEDIUM:
							proj->shooter->score += MEDIUM_ALIEN_SCORE;
							break;
						case LARGE:
							proj->shooter->score += LARGE_ALIEN_SCORE;
							break;
						}
					}
					iterator->state = ALIEN_DESTROYED;
					return 1;
				}
			}
			iterator = iterator->next;
		} while (iterator != NULL);
	}

	return 0;
}

int search_new_extreme(unsigned char side) {

	unsigned short left = get_h_res();
	unsigned short right = 0;

	alien* new_extreme = NULL; //extreme

	alien* iterator = invaders->head;

	if (side == 1) { //searches rightmost
		do {
			if (iterator->x + iterator->width > right) {
				right = iterator->x + iterator->width;
				new_extreme = iterator;
			}

			iterator = iterator->next;
		} while (iterator != NULL);

		invaders->rightmost = new_extreme;

		return 0;
	}

	else if (side == 2) { //searches leftmost
		do {
			if (iterator->x < left) {
				left = iterator->x;
				new_extreme = iterator;
			}

			iterator = iterator->next;
		} while (iterator != NULL);

		invaders->leftmost = new_extreme;

		return 0;
	}

	return 1;

}

void aliens_draw() {
	if (invaders->head == NULL)
		return;

	if(ufo != NULL)
		alien_draw(ufo);

	alien* iterator = invaders->head;

	do {
		alien_draw(iterator);
		iterator = iterator->next;
	} while (iterator != NULL);
}

int alien_fire(alien *a1) {

	if (a1 == NULL) {
		return 1;
	}

	projectile_init(NULL, (unsigned short) (a1->x + ALIEN_WIDTH / 2),
			a1->y + ALIEN_HEIGHT+ALIEN_Y_DELTA, ALIEN_PROJECTILE_VELOCITY);
	//TODO remove + ALIEN_Y_DELTA AFTER THE DIMENSIONS ARE SORTED OUT
	return 0;
}

int is_on_last_row(alien *a1) {

	alien *iterator = a1->next;

	while (iterator != NULL) {
		if ((int) (a1->x + ALIEN_WIDTH / 2) >= iterator->x  && (int) (a1->x + ALIEN_WIDTH / 2) <= iterator->x + ALIEN_WIDTH)
			return 0;

		iterator = iterator->next;
	}

	return 1;
}

void aliens_tick(){
	alien* iterator = invaders->head;

	if(ufo != NULL){
		if(ufo->state == ALIEN_DESTROYED){
			ufo->ticks++;
			if(ufo->ticks > 60)
				ufo_destruct();
		}
	}

	while(iterator != NULL){
		if(iterator->state == ALIEN_DESTROYED){
			iterator->ticks++;
			if(iterator->ticks > 30){
				if(alien_remove(iterator) == 1)
					return;
			}
		}
		iterator = iterator->next;
	}

	if(invaders->alien_num != 0){
		aliens_draw();
	}
}

void aliens_destruct() {
	animation_destruct(invaders->small_alien);
	animation_destruct(invaders->medium_alien);
	animation_destruct(invaders->large_alien);
	animation_destruct(invaders->destroy);
	//bitmap_delete(ufo_img);

	while (invaders->head != NULL)
		alien_remove(invaders->head);

	ufo_destruct();

	//free(invaders); //crashes here
}

void ufo_create(){
	if(ufo != NULL)
		return;
	ufo = (alien*) malloc(sizeof(alien));
	ufo->ticks = 0;
	ufo->type = UFO;
	ufo->state = ALIEN_ALIVE;
	ufo->x = UFO_INITIAL_X;
	ufo->y = UFO_INITIAL_Y;
	ufo->width = UFO_WIDTH;
	ufo->height = UFO_HEIGHT;
	ufo->next = NULL;
	ufo->prev = NULL;
}

void ufo_destruct(){
	if(ufo == NULL)
		return;

	free(ufo);
	ufo = NULL;
}

void ufo_move(){
	if(ufo != NULL){
		if (ufo->x + ufo->width + UFO_X_DELTA > PLACEHOLDER_RIGHT_BORDER){
			ufo_destruct();
		} else {
			if(ufo->state != ALIEN_DESTROYED)
				alien_move(ufo, UFO_X_DELTA, 0);
		}
	}
}
