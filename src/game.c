#include <assert.h>
#include <time.h>

#include <game.h>
#include <misc.h>
#include <window.h>
#include <sprite.h>
#include <stdlib.h>          // à partir de la truc rajouté
#include <monster.h>
#include <map.h>

struct game {
	struct level* curr_level; // current level
	struct player* player;
	struct monster* monster;       //ligne rajoutée
};

struct game* game_new(void) {
	sprite_load(); // load sprites into process memory

	struct game* game = malloc(sizeof(*game));
	game->curr_level = level_get_level(0); // get maps of the first level

	game->player = player_init(4, 8);
	player_from_map(game->player, level_get_map(game->curr_level, 0)); // get x,y of the player on the first map


	game->monster=monster_init(level_get_map(game->curr_level, 0));
	monster_from_map(game->monster, level_get_map(game->curr_level, 0));  //initialise x et y ac les données de la carte

	return game;
}

void game_free(struct game* game) {
	assert(game);

	player_free(game->player);
	monster_free(game->monster);
	level_free(game->curr_level);
}

struct player* game_get_player(struct game* game) {
	assert(game);
	return game->player;
}

struct monster* game_get_monster(struct game* game) {
	assert(game);
	return game->monster;
}

struct level* game_get_curr_level(struct game* game) {
	return game->curr_level;
}

void game_banner_display(struct game* game) {
	assert(game);

	struct map* map = level_get_curr_map(game_get_curr_level(game));

	int y = (map_get_height(map)) * SIZE_BLOC;
	for (int i = 0; i < map_get_width(map); i++)
		window_display_image(sprite_get_banner_line(), i * SIZE_BLOC, y);

	int white_bloc = ((map_get_width(map) * SIZE_BLOC) - 6 * SIZE_BLOC) / 4;
	int x = white_bloc;
	y = (map_get_height(map) * SIZE_BLOC) + LINE_HEIGHT;
	window_display_image(sprite_get_banner_life(), x, y);

	x = white_bloc + SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_nb_life(game->player)), x, y);

	x = 2 * white_bloc + 2 * SIZE_BLOC;
	window_display_image(sprite_get_banner_bomb(), x, y);

	x = 2 * white_bloc + 3 * SIZE_BLOC;
	window_display_image(
			sprite_get_number(player_get_nb_bomb(game_get_player(game))), x, y);

	x = 3 * white_bloc + 4 * SIZE_BLOC;
	window_display_image(sprite_get_banner_range(), x, y);

	x = 3 * white_bloc + 5 * SIZE_BLOC;
	window_display_image(sprite_get_number(1), x, y);
}

void game_display(struct game* game) {
	assert(game);

	window_clear();

	game_banner_display(game);
	level_display(game_get_curr_level(game));
	player_display(game->player);
	monster_display(game->monster);
	window_refresh();
}

short input_keyboard(struct game* game) {
	SDL_Event event;
	struct player* player = game_get_player(game);
	struct map* map = level_get_curr_map(game_get_curr_level(game));

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return 1;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				return 1;
			case SDLK_UP:
				player_set_current_way(player, NORTH);
				player_move(player, map);
				break;
			case SDLK_DOWN:
				player_set_current_way(player, SOUTH);
				player_move(player, map);
				break;
			case SDLK_RIGHT:
				player_set_current_way(player, EAST);
				player_move(player, map);
				break;
			case SDLK_LEFT:
				player_set_current_way(player, WEST);
				player_move(player, map);
				break;
			case SDLK_SPACE:
				break;
			default:
				break;
			}

			break;
		}
	}
	return 0;
}

// fonction rajoutée pour paramétrer déplacements monstre

void monster_update(struct game* game)  {

	struct monster* monster = game_get_monster(game);

	struct map* map = level_get_curr_map(game_get_curr_level(game));

	struct player* player = game_get_player(game);

	random_direction_monster (map, monster, player);
}

/*void player_update(struct game* game)  {

	struct map* map = level_get_curr_map(game_get_curr_level(game));

	while
			if ((map_get_cell_type(map, i, j) == CELL_PLAYER) &&
				((map_get_cell_type(map, i, j) == CELL_MONSTER )))  {

					player_dec_nb_life(game->player);
			}
		}
	}
}*/

/*void player_update(struct game* game)  {

	struct map* map = level_get_curr_map(game_get_curr_level(game));

	int i, j;
	for (i = 0; i < map_get_width(map); i++)  {
		for (j = 0; j < map_get_height(map); j++)  {
			if ((map_get_cell_type(map, i, j) == CELL_PLAYER) &&
				((map_get_cell_type(map, i, j) == CELL_MONSTER )|| (map_get_cell_type(map, i, j) == CELL_BOMB)))  {

					player_dec_nb_life(game->player);
			}
		}
	}
}*/

int game_update(struct game* game) {
	void player_update(struct game* game); //mise à jour vie player (bientot ce sera aussi mise à jour nb bomb joueur)

	monster_update(game);  //mise à jour des monstres

	if (input_keyboard(game))
		return 1; // exit game

	return 0;
}
