#include "include/draw2d.h"
#include "include/game.h"
#include "include/math.h"

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

TTF_Font *font = NULL;

void draw2d_init() {
    
}

// 2D Drawing ---------------------------------------

int point(int x, int y) {
    return SDL_RenderDrawPoint(window.renderer, x, y);
}

int rect(int x, int y, int width, int height, bool centered, bool fill) {
    SDL_Rect rect = {0};

    if (centered) {
        rect.x = x - (width / 2);
        rect.y = y - (height / 2);   
    } else {
        rect.x = x;
        rect.y = y;
    }

    rect.w = width;
    rect.h = height;

    if (fill) return SDL_RenderFillRect(window.renderer, &rect);
    else return SDL_RenderDrawRect(window.renderer, &rect);
}

int image(image_t *img, int x, int y, bool centered) {
    SDL_Rect rect = {0};
    if (centered) {
        rect.x = x - ((img->xsize * fabs(img->xscale)) / 2);
        rect.y = y - ((img->ysize * fabs(img->yscale)) / 2);
    } else {
        rect.x = x;
        rect.y = y;
    }

    rect.w = (int)(img->xsize * fabs(img->xscale));
    rect.h = (int)(img->ysize * fabs(img->yscale));

    SDL_RendererFlip flipping = SDL_FLIP_NONE;
    if (img->xscale < 0) flipping |= SDL_FLIP_HORIZONTAL;
    if (img->yscale < 0) flipping |= SDL_FLIP_VERTICAL;

    return SDL_RenderCopyEx(window.renderer, img->tex, NULL, &rect, img->rotation, NULL, flipping);
}

// Circle code from
// https://gist.gixthub.com/Gumichan01/332c26f6197a432db91cc4327fcabb1c

int SDL_RenderDrawCircle(SDL_Renderer * renderer, int x, int y, int radius)
{
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius -1;
    status = 0;

    while (offsety >= offsetx) {
        status += SDL_RenderDrawPoint(renderer, x + offsetx, y + offsety);
        status += SDL_RenderDrawPoint(renderer, x + offsety, y + offsetx);
        status += SDL_RenderDrawPoint(renderer, x - offsetx, y + offsety);
        status += SDL_RenderDrawPoint(renderer, x - offsety, y + offsetx);
        status += SDL_RenderDrawPoint(renderer, x + offsetx, y - offsety);
        status += SDL_RenderDrawPoint(renderer, x + offsety, y - offsetx);
        status += SDL_RenderDrawPoint(renderer, x - offsetx, y - offsety);
        status += SDL_RenderDrawPoint(renderer, x - offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

int SDL_RenderFillCircle(SDL_Renderer * renderer, int x, int y, int radius)
{
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius -1;
    status = 0;

    while (offsety >= offsetx) {

        status += SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
                                     x + offsety, y + offsetx);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
                                     x + offsetx, y + offsety);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
                                     x + offsetx, y - offsety);
        status += SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
                                     x + offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

int circle(int x, int y, int r, bool fill) {
    if (fill) return SDL_RenderFillCircle(window.renderer, x, y, r);
    else return SDL_RenderDrawCircle(window.renderer, x, y, r);
}

int line(int x1, int y1, int x2, int y2) {
    return SDL_RenderDrawLine(window.renderer, x1, y1, x2, y2);
}


int fill_bottom_tri(vec2 v1, vec2 v2, vec2 v3) {
    float islope1 = (v2.x - v1.x) / (v2.y - v1.y);
    float islope2 = (v3.x - v1.x) / (v3.y - v1.y);

    float cx1 = v1.x;
    float cx2 = v1.x;

    for (int scanlineY = v1.y; scanlineY <= v2.y; scanlineY++)
    {
        line((int)cx1, scanlineY, (int)cx2, scanlineY);
        cx1 += islope1;
        cx2 += islope2;
    }

    return 0;
}

int fill_top_tri(vec2 v1, vec2 v2, vec2 v3) {
    float islope1 = (v3.x - v1.x) / (v3.y - v1.y);
    float islope2 = (v3.x - v2.x) / (v3.y - v2.y);

    float cx1 = v3.x;
    float cx2 = v3.x;

    for (int scanlineY = v3.y; scanlineY > v1.y; scanlineY--)
    {
        line((int)cx1, scanlineY, (int)cx2, scanlineY);
        cx1 -= islope1;
        cx2 -= islope2;
    }

    return 0;
}

void swap_vec2(vec2 *a, vec2 *b) {
    vec2 tmp = *a;
    *a = *b;
    *b = tmp;
}

int triangle(int x1, int y1, int x2, int y2, int x3, int y3, bool fill) {
    if (fill) {
        vec2 vecs[3] = {{x1, y1}, {x2, y2}, {x3, y3}};
        
        int midx;
        for (int i = 0; i < 2; i++) {
            midx = i;
            for (int j = i + 1; j < 3; j++)
                if (vecs[j].y < vecs[midx].y)
                    midx = j;
    
            swap_vec2(&vecs[midx], &vecs[i]);
        }

        if (vecs[1].y == vecs[2].y) {
            return fill_bottom_tri(vecs[0], vecs[1], vecs[2]);
        } else if (vecs[0].y == vecs[1].y) {
            return fill_top_tri(vecs[0], vecs[1], vecs[2]);
        } else {
            vec2 v4 = (vec2){(vecs[0].x + ((float)(vecs[1].y - vecs[0].y) / (float)(vecs[2].y - vecs[0].y)) * (vecs[2].x - vecs[0].x)), vecs[1].y};
            fill_bottom_tri(vecs[0], vecs[1], v4);
            fill_top_tri(vecs[1], v4, vecs[2]);
        }
        return 0;
    } else {
        return line(x1, y1, x2, y2) + line(x2, y2, x3, y3) + line(x3, y3, x1, y1);
    }
}

int trianglev(vec2 a, vec2 b, vec2 c, bool fill) {
    return triangle(a.x, a.y, b.x, b.y, c.x, c.y, fill);
}

int text(int x, int y, const char *txt) {
    if (!font) return -1;
    
    SDL_Color dc = {0, 0, 0, 0};
    SDL_GetRenderDrawColor(window.renderer, &dc.r, &dc.g, &dc.b, &dc.a);
    SDL_Surface *surf = TTF_RenderText_Solid(font, txt, dc);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(window.renderer, surf);
    SDL_Rect bounds = { .x = x, .y = y, .w = surf->w, .h = surf->h };
    SDL_FreeSurface(surf);

    int result = SDL_RenderCopy(window.renderer, tex, NULL, &bounds);

    SDL_DestroyTexture(tex);
    return result;
}

// General Drawing ---------------------------------------

int text_init(const char *font_path, int size) {
    TTF_Init();

    if (atexit(TTF_Quit) != 0) {
        fprintf(stderr, "Unable to register TTF_Quit().\n");
        exit(1);
    }
    text_font(font_path, size);
    return 0;
}

int text_font(const char *font_path, int size) {
    if (font) TTF_CloseFont(font);
    if (!(font = TTF_OpenFont(font_path, size))) {
        fprintf(stderr, "Unable to load font '%s'.\n", font_path);
        exit(1);
    }
    return 0;
}

image_t *image_open(const char *path) {
    SDL_Texture *texture = IMG_LoadTexture(window.renderer, path);
    image_t *img = malloc(sizeof(image_t));
    img->tex = texture;
    SDL_QueryTexture(texture, NULL, NULL, &img->xsize, &img->ysize);
    img->xscale = 1;
    img->yscale = 1;
    img->rotation = 0;
    return img;
}

void image_free(image_t *img) {
    SDL_DestroyTexture(img->tex);
    free(img);
}

void image_scale_to_size(image_t *img, int xsize, int ysize) {
    img->xscale = xsize / (float) img->xsize;
    img->yscale = ysize / (float) img->ysize;
}
