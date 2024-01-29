#include "../include/graphics.hpp"
#include "../include/geometry.hpp"


GrphcsGraphics::GrphcsGraphics(SDL_Window * window, SDL_Renderer * renderer, SDL_Texture * renderTexture, int xsize, int ysize, float fov) {
    this->window = window;
    this->renderer = renderer;
    this->renderTexture = renderTexture;

    this->bufferSize = xsize * ysize;

#if USE_PICK_BUFFER
    this->pickbuffer = (grphcs_pickbuffer_t *) calloc(this->bufferSize, sizeof(grphcs_pickbuffer_t));
    if (this->pickbuffer == NULL) {
        fprintf(stderr, "Failed to allocate pickbuffer: %s\n", strerror(errno));
        exit(1);
    }
#endif

    this->depthbuffer = (float *) calloc(this->bufferSize, sizeof(float));
    if (this->depthbuffer == NULL) {
        fprintf(stderr, "Failed to allocate depthbuffer: %s\n", strerror(errno));
        exit(1);
    }

    this->pixelbuffer = (GrphcsPixel *) calloc(this->bufferSize, sizeof(GrphcsPixel));
    if (this->pixelbuffer == NULL) {
        fprintf(stderr, "Failed to allocate pixelbuffer: %s\n", strerror(errno));
        exit(1);
    }

    this->context = GrphcsContext(xsize, ysize, fov, this->depthbuffer, this->pixelbuffer, this->pickbuffer);
}

void GrphcsGraphics::prepareFrame(void) {

    this->zeroBuffers();

}

void GrphcsGraphics::finishFrame(void) {

#if USE_LOCK_TEXTURE
    uint8_t * lock_pixels = NULL;
    int pitch = 0;
    SDL_LockTexture( this->renderTexture, NULL, (void **) &lock_pixels, &pitch );
    // TODO: can probably optimize with intrinsics
    memcpy( lock_pixels, this->pixelbuffer, this->bufferSize * sizeof(GrphcsPixel) );
    SDL_UnlockTexture(  this->renderTexture );
#else
    SDL_UpdateTexture( this->renderTexture, NULL, this->pixelbuffer, this->context.getXSize() * sizeof(GrphcsPixel) );
#endif

    SDL_RenderCopy( this->renderer, this->renderTexture, NULL, NULL );
    SDL_RenderPresent( this->renderer );

}

void GrphcsGraphics::zeroBuffers(void) {

    for (int i = 0; i < this->bufferSize; i++) {
        this->depthbuffer[i] = FARPLANE;
#if USE_PICK_BUFFER
        this->pickbuffer[i] = -1;
#endif
        this->pixelbuffer[i] = GrphcsPixel(0, 0, 0);
    }

}

void GrphcsGraphics::quit(void) {

#if USE_PICK_BUFFER
    if (this->pickbuffer) free(this->pickbuffer);
#endif
    if (this->pixelbuffer) free(this->pixelbuffer);
    if (this->depthbuffer) free(this->depthbuffer);

    SDL_DestroyWindow(this->window);
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyTexture(this->renderTexture);

}
