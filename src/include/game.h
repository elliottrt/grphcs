#ifndef _GAME_H
#define _GAME_H

#include "io.h"
#include "math.h"

typedef void (*game_func_t)(void);

typedef struct _CAMERA_s {
    vec3 position;
    rotation_t rotation;
} camera_t;

struct _game_t {
 	bool running;
 	int xsize, ysize;

 	float delta_time;

	mouse_t mouse;
	keyboard_t keys;

	camera_t camera;
} game;

typedef enum graphics_type_e {
	GRAPHICS_TYPE_2D = 0,
	GRAPHICS_TYPE_3D = 1,
} graphics_type_t;

int game_create(game_func_t init_f, game_func_t update_f, \
	game_func_t render_f, game_func_t exit_f, \
	char const *title, int xsize, int ysize, \
	graphics_type_t graphics_type);

void clear_screen(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void draw_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

#endif
