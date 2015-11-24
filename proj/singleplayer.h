#ifndef SINGLEPLAYER_H
#define SINGLEPLAYER_H

#include "aliens.h"
#include "shield.h"
#include "player_ship.h"

typedef struct {
	alien* aliens;
	shield* shields;
	player play;
} game_t;

game_t singleplayer_game;

#define ALIENS_PER_ROW 10
#define ALIEN_ROWS 4
#define NUMBER_OF_SHIELDS 4

/**
 * @brief Initializes singleplayer game information
 */
void singleplayer_init();

/**
 * @brief Event called when a tick occurs and the game state is singleplayer.
 */
void singleplayer_tick();

/**
 * @brief Deletes singleplayer game information
 */
void singleplayer_destruct();

#endif
