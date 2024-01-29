#ifndef _GRPHCS_GRAPHICS_H
#define _GRPHCS_GRAPHICS_H

#include <SDL2/SDL.h>

#include "defs.hpp"
#include "context.hpp"

class GrphcsGraphics {

public:

    GrphcsContext context;

    GrphcsGraphics(SDL_Window * window, SDL_Renderer * renderer, SDL_Texture * texture, int xsize, int ysize, float fov);
    GrphcsGraphics() = default;

    void prepareFrame(void);
    void finishFrame(void);

    void quit(void);

private:

    SDL_Window * window;
    SDL_Renderer * renderer;
    SDL_Texture * renderTexture;

    int bufferSize;
    float * depthbuffer;
    grphcs_pickbuffer_t * pickbuffer;
    GrphcsPixel * pixelbuffer;

    void zeroBuffers(void);

};

#endif
