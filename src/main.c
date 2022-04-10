#include "include/game.h"
#include "include/draw2d.h"
#include "include/draw3d.h"
#include "include/libutil.h"

#include <stdlib.h>

geo_t *cow;

void ginit() {
	srand(time(NULL));

	draw_setup(DM_PERSPECTIVE, 500.f);

	cow = geom_from_file("cow.obj");
}

void gupdate() {
	if (game.keys.pressed[KEY_ESCAPE]) game.running = false;

	float x = (game.keys.down[KEY_X]) ? 2 * game.delta_time : 0;
	float y = (game.keys.down[KEY_C]) ? 2 * game.delta_time : 0;
	float z = (game.keys.down[KEY_Z]) ? 2 * game.delta_time : 0;
	rotate_relative(x, y, z);

#define rotspeed 4.f

	float barrel = (game.keys.down[KEY_RIGHT] - game.keys.down[KEY_LEFT]) / rotspeed;
	float pitch  = (game.keys.down[KEY_UP] - game.keys.down[KEY_DOWN]) / rotspeed;
	float yaw    = (game.keys.down[KEY_RIGHTBRACKET] = game.keys.down[KEY_LEFTBRACKET]) / rotspeed;

	rotation_rotate_rel(&game.camera.rotation, v3f(yaw, barrel, pitch));

	int forward = game.keys.down[KEY_W] - game.keys.down[KEY_S];
	int side    = game.keys.down[KEY_D] - game.keys.down[KEY_A];
	int vert    = game.keys.down[KEY_RSHIFT] - game.keys.down[KEY_SPACE];

	vec3 movement = v3f(side, vert, forward);

	game.camera.position._itrn += movement._itrn;
}

void grender() {
	clear_screen(0, 0, 0, 255);
	draw_color(255, 255, 255, 255);	

	//timing_start();
	draw_geometry(cow, v3f(0, 0, 10), true);
	//draw_pickbuf();
	//draw_depthbuf();
	//timing_end("draw_cow");


	//if (get_vertex() != -1) printf("%i\n", get_vertex());

	//printf("Exiting after single frame.\n");
	//exit(0);

}

void gexit() {
	geom_destroy(cow);
}

int main()
{
	return game_create(ginit, gupdate, grender, gexit, "Test Window", 640, 480, GRAPHICS_TYPE_3D);
}
