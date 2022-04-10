#ifndef _DRAW2D_H
#define _DRAW2D_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct {
    SDL_Texture *tex;
    int xsize, ysize;
    float xscale, yscale;
    float rotation;
} image_t;

void draw2d_init();

// 2D Drawing ---------------------------------------

int point(int x, int y);
int rect(int x, int y, int width, int height, bool centered, bool fill);
int image(image_t *img, int x, int y, bool centered);
int circle(int x, int y, int r, bool fill);
int line(int x1, int y1, int x2, int y2);

int triangle(int x1, int y1, int x2, int y2, int x3, int y3, bool fill);
int quad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, bool fill);

int text(int x, int y, const char *txt);

// General Drawing ---------------------------------------

int text_init(const char *font_path, int size);
int text_font(const char *font_path, int size);

image_t *image_open(const char *path);
void image_free(image_t *img);
void image_scale_to_size(image_t *img, int xsize, int ysize);

#endif
