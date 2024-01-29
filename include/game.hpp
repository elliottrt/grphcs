#ifndef _GRPHCS_GAME_H
#define _GRPHCS_GAME_H

#include "defs.hpp"
#include "graphics.hpp"
#include "geometry.hpp"

#include <functional>

#if USE_PICK_BUFFER
#define DEFAULT_PB_VAL -1
#endif

typedef std::function<void(float)> GrphcsUpdateFunction;
typedef std::function<void(GrphcsContext*)> GrphcsRenderFunction;

class GrphcsGame {

public:

    GrphcsInput input;

    GrphcsGame(char const * title, int xsize, int ysize, float fov,
               GrphcsUpdateFunction updateFunction, GrphcsRenderFunction renderFunction);
    ~GrphcsGame();

    void run(void);
    void quit(int exitCode = 0);

private:

    float deltaTime;
    GrphcsGraphics graphics;

    GrphcsUpdateFunction updateFunction;
    GrphcsRenderFunction renderFunction;

    void handleEvents(void);
    void prepareFrame(void);
    void finishFrame(void);

};

#endif
