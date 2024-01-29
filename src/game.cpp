#include "../include/game.hpp"

GrphcsGame::GrphcsGame(char const * title, int xsize, int ysize, float fov,
    GrphcsUpdateFunction updateFunction, GrphcsRenderFunction renderFunction) {

    if (xsize <= 0 || ysize <= 0) {
        fprintf(stderr, "Invalid window size '%ix%i'.\n", xsize, ysize);
        this->quit(1);
    }

    if (updateFunction == NULL) {
        fprintf(stderr, "GrphcsGame renderFunction is null.\n");
        this->quit(1);
    }
    if (renderFunction == NULL) {
        fprintf(stderr, "GrphcsGame updateFunction is null.\n");
        this->quit(1);
    }
    this->updateFunction = updateFunction;
    this->renderFunction = renderFunction;

    // Make sure SDL loads correctly
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        fprintf(stderr, "Failed to load SDL: %s\n", SDL_GetError());
        this->quit(1);
    }

    int wflags = 0;
	// TODO: vsync limits frames, remove this for smoother
    int rflags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    int rtflags = SDL_TEXTUREACCESS_STREAMING;

    SDL_Window * window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, xsize, ysize, wflags);
    if (window == NULL) {
        fprintf(stderr, "Failed to open SDLWindow: %s\n", SDL_GetError());
        this->quit(1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, rflags);
    if (renderer == NULL) {
        fprintf(stderr, "Failed to create SDLRenderer: %s\n", SDL_GetError());
        this->quit(1);
    }

    //SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);

    // TODO: find other pixel formats
    SDL_Texture * renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, rtflags, xsize, ysize);
    if (renderTexture == NULL) {
        fprintf(stderr, "Failed to create SDLTexture: %s\n", SDL_GetError());
        this->quit(1);
    }

    this->input = GrphcsInput();

    this->graphics = GrphcsGraphics(window, renderer, renderTexture, xsize, ysize, fov);

}

GrphcsGame::~GrphcsGame() {
    this->quit(0);
}

void GrphcsGame::run(void) {

    this->deltaTime = 0;

    memset(this->input.kDown, false, sizeof(bool) * GrphcsKey::KEY_COUNT);
    memset(this->input.mDown, false, sizeof(bool) * GrphcsMouseButton::MOUSE_BUTTON_COUNT);

    while (true) {

        Uint64 frame_start = SDL_GetPerformanceCounter();

        this->handleEvents();

        this->updateFunction(this->deltaTime);

        this->graphics.prepareFrame();

        this->renderFunction(&this->graphics.context);

        this->graphics.finishFrame();

        Uint64 frame_end = SDL_GetPerformanceCounter();
		this->deltaTime = (frame_end - frame_start) / (float) SDL_GetPerformanceFrequency();

    }

}

void GrphcsGame::quit(int exitCode) {

    printf("Call to GrphcsGame::quit(%i)\n", exitCode);

    this->graphics.quit();

    // TODO: explore this
    //SDL_RenderGeometry()

    SDL_Quit();

    exit(exitCode);

}

void GrphcsGame::handleEvents(void) {

    this->input.prepare();

    SDL_Event event;	
	while (SDL_PollEvent(&event)) {
		
        switch(event.type) {

            case SDL_QUIT: {
                this->quit(0);
            } break;
            case SDL_MOUSEWHEEL: {
                this->input.mWheel = event.wheel.y;
            } break;
            case SDL_MOUSEBUTTONDOWN: {
                if (!this->input.mDown[event.button.button]) {
                    this->input.mPressed[event.button.button] = true;
                }
                this->input.mDown[event.button.button] = true;
            } break;
            case SDL_MOUSEBUTTONUP: {
                this->input.mDown[event.button.button] = false;
                this->input.mReleased[event.button.button] = true;
            } break;
            case SDL_KEYDOWN: {
                if (!this->input.kDown[event.key.keysym.scancode]) {
                    this->input.kPressed[event.key.keysym.scancode] = true;
                }
                this->input.kDown[event.key.keysym.scancode] = true;
            } break;
            case SDL_KEYUP: {
                this->input.kDown[event.key.keysym.scancode] = false;
                this->input.kReleased[event.key.keysym.scancode] = true;
            } break;
            //case SDL_WINDOWEVENT: {
                //printf("Hit 512\n");
            //} break; 
            default: break;

        }

	}

}
