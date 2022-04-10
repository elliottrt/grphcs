#include "include/game.h"
#include "include/draw2d.h"
#include "include/draw3d.h"

void handle_event(SDL_Event *event) {
	switch (event->type) {
		case SDL_QUIT: {
			exit(0);
		} break;
		case SDL_MOUSEWHEEL: {
			game.mouse.wheel = event->wheel.y;
		} break;
		case SDL_MOUSEBUTTONDOWN: {
			game.mouse.button[event->button.button] = true;
		} break;
		case SDL_MOUSEBUTTONUP: {
			game.mouse.button[event->button.button] = false;
		} break;
		case SDL_KEYDOWN: {
			if (!game.keys.down[event->key.keysym.scancode]) {
				game.keys.pressed[event->key.keysym.scancode] = true;
			}
			game.keys.down[event->key.keysym.scancode] = true;
		} break;
		case SDL_KEYUP: {
			game.keys.down[event->key.keysym.scancode] = false;
			game.keys.released[event->key.keysym.scancode] = true;
		} break;
		default: break;
	}
}

void handle_input_and_events() {
	SDL_GetMouseState(&game.mouse.x, &game.mouse.y);

	memset(game.keys.released, false, sizeof(bool) * KEYCOUNT);
	memset(game.keys.pressed, false, sizeof(bool) * KEYCOUNT);

	SDL_Event event;	
	while (SDL_PollEvent(&event)) {
		handle_event(&event);
	}
}

void game_init(char const *title, int xsize, int ysize, game_func_t exit_f) {
	int flags_w = 0;
	int flags_r = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	window.window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, xsize, ysize, flags_w);

	if (!window.window) {
		fprintf(stderr, "Failed to open %dx%d window: %s\n", xsize, ysize, SDL_GetError());
		exit(1);
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	window.renderer = SDL_CreateRenderer(window.window, -1, flags_r);

	if (!window.renderer)
	{
		fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
		exit(1);
	}

	if (atexit(exit_f) != 0) {
		fprintf(stderr, "Unable to register on_exit function.");
		exit(1);
	}
}

void game_loop(game_func_t update_f, game_func_t render_f, graphics_type_t graphics_type) {
	while (game.running) {
		Uint64 frame_start = SDL_GetPerformanceCounter();

		handle_input_and_events();

		update_f();
		if (graphics_type == GRAPHICS_TYPE_3D) zero_buffers();
		render_f();

		SDL_RenderPresent(window.renderer);

		Uint64 frame_end = SDL_GetPerformanceCounter();
		game.delta_time = (frame_end - frame_start) / (float) SDL_GetPerformanceFrequency();
	}
}

int game_create(game_func_t init_f, game_func_t update_f, \
	game_func_t render_f, game_func_t exit_f, \
	char const *title, int xsize, int ysize, \
	graphics_type_t graphics_type) {

	game.running = true;
	game.xsize = xsize;
	game.ysize = ysize;

	game.camera = (camera_t) {
		.position = v3f(0, 0, 0), 
		.rotation = {
			.rotm = MAT4_IDENTITY, 
			.angles = v3f(0, 0, 0)
		}
	};

	game_init(title, xsize, ysize, exit_f);
	init_f();

	switch (graphics_type) {
		case GRAPHICS_TYPE_2D: { draw2d_init(); } break;
		case GRAPHICS_TYPE_3D: { draw3d_init(); } break;
		default: {
			fprintf(stderr, "Invalid graphics_type '%i'.\n", graphics_type);
			exit(1);
		} break;
	}

	SDL_SetRenderDrawBlendMode(window.renderer, SDL_BLENDMODE_BLEND);

	game_loop(update_f, render_f, graphics_type);

	SDL_Quit();

	return 0;
}

void clear_screen(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    draw_color(r, g, b, a);
	SDL_RenderClear(window.renderer);
}

void draw_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(window.renderer, r, g, b, a);
}
