#ifndef _GRPHCS_INPUT_H
#define _GRPHCS_INPUT_H

#include <SDL2/SDL.h>
#include "math.hpp"

#define GRPHCS_INPUT inline

typedef enum {
    MOUSE_BUTTON_LEFT = SDL_BUTTON_LEFT,
    MOUSE_BUTTON_MIDDLE = SDL_BUTTON_MIDDLE,
    MOUSE_BUTTON_RIGHT = SDL_BUTTON_RIGHT,
    MOUSE_BUTTON_X1 = SDL_BUTTON_X1,
    MOUSE_BUTTON_X2 = SDL_BUTTON_X2,
    MOUSE_BUTTON_COUNT = 6,
} GrphcsMouseButton;

typedef enum {
    KEY_A = SDL_SCANCODE_A,
    KEY_B = SDL_SCANCODE_B,
    KEY_C = SDL_SCANCODE_C,
    KEY_D = SDL_SCANCODE_D,
    KEY_E = SDL_SCANCODE_E,
    KEY_F = SDL_SCANCODE_F,
    KEY_G = SDL_SCANCODE_G,
    KEY_H = SDL_SCANCODE_H,
    KEY_I = SDL_SCANCODE_I,
    KEY_J = SDL_SCANCODE_J,
    KEY_K = SDL_SCANCODE_K,
    KEY_L = SDL_SCANCODE_L,
    KEY_M = SDL_SCANCODE_M,
    KEY_N = SDL_SCANCODE_N,
    KEY_O = SDL_SCANCODE_O,
    KEY_P = SDL_SCANCODE_P,
    KEY_Q = SDL_SCANCODE_Q,
    KEY_R = SDL_SCANCODE_R,
    KEY_S = SDL_SCANCODE_S,
    KEY_T = SDL_SCANCODE_T,
    KEY_U = SDL_SCANCODE_U,
    KEY_V = SDL_SCANCODE_V,
    KEY_W = SDL_SCANCODE_W,
    KEY_X = SDL_SCANCODE_X,
    KEY_Y = SDL_SCANCODE_Y,
    KEY_Z = SDL_SCANCODE_Z,

    KEY_1 = SDL_SCANCODE_1,
    KEY_2 = SDL_SCANCODE_2,
    KEY_3 = SDL_SCANCODE_3,
    KEY_4 = SDL_SCANCODE_4,
    KEY_5 = SDL_SCANCODE_5,
    KEY_6 = SDL_SCANCODE_6,
    KEY_7 = SDL_SCANCODE_7,
    KEY_8 = SDL_SCANCODE_8,
    KEY_9 = SDL_SCANCODE_9,
    KEY_0 = SDL_SCANCODE_0,

    KEY_RETURN = SDL_SCANCODE_RETURN,
    KEY_ESCAPE = SDL_SCANCODE_ESCAPE,
    KEY_BACKSPACE = SDL_SCANCODE_BACKSPACE,
    KEY_TAB = SDL_SCANCODE_TAB,
    KEY_SPACE = SDL_SCANCODE_SPACE,

    KEY_MINUS = SDL_SCANCODE_MINUS,
    KEY_EQUALS = SDL_SCANCODE_EQUALS,
    KEY_LEFTBRACKET = SDL_SCANCODE_LEFTBRACKET,
    KEY_RIGHTBRACKET = SDL_SCANCODE_RIGHTBRACKET,
    KEY_BACKSLASH = SDL_SCANCODE_BACKSLASH,
    KEY_NONUSHASH = SDL_SCANCODE_NONUSHASH,
    KEY_SEMICOLON = SDL_SCANCODE_SEMICOLON,
    KEY_APOSTROPHE = SDL_SCANCODE_APOSTROPHE,
    KEY_GRAVE = SDL_SCANCODE_GRAVE, 
    KEY_COMMA = SDL_SCANCODE_COMMA,
    KEY_PERIOD = SDL_SCANCODE_PERIOD,
    KEY_SLASH = SDL_SCANCODE_SLASH,

    KEY_CAPSLOCK = SDL_SCANCODE_CAPSLOCK,

    KEY_F1 = SDL_SCANCODE_F1,
    KEY_F2 = SDL_SCANCODE_F2,
    KEY_F3 = SDL_SCANCODE_F3,
    KEY_F4 = SDL_SCANCODE_F4,
    KEY_F5 = SDL_SCANCODE_F5,
    KEY_F6 = SDL_SCANCODE_F6,
    KEY_F7 = SDL_SCANCODE_F7,
    KEY_F8 = SDL_SCANCODE_F8,
    KEY_F9 = SDL_SCANCODE_F9,
    KEY_F10 = SDL_SCANCODE_F10,
    KEY_F11 = SDL_SCANCODE_F11,
    KEY_F12 = SDL_SCANCODE_F12,

    KEY_PRINTSCREEN = SDL_SCANCODE_PRINTSCREEN,

    KEY_RIGHT = SDL_SCANCODE_RIGHT,
    KEY_LEFT = SDL_SCANCODE_LEFT,
    KEY_DOWN = SDL_SCANCODE_DOWN,
    KEY_UP = SDL_SCANCODE_UP,

    KEY_LCTRL = SDL_SCANCODE_LCTRL,
    KEY_LSHIFT = SDL_SCANCODE_LSHIFT,
    KEY_LALT = SDL_SCANCODE_LALT, /**< alt, option */
    KEY_LGUI = SDL_SCANCODE_LGUI, /**< windows, command (apple), meta */
    KEY_RCTRL = SDL_SCANCODE_RCTRL,
    KEY_RSHIFT = SDL_SCANCODE_RSHIFT,
    KEY_RALT = SDL_SCANCODE_RALT, /**< alt gr, option */
    KEY_RGUI = SDL_SCANCODE_RGUI, /**< windows, command (apple), meta */

    KEY_COUNT = SDL_NUM_SCANCODES
} GrphcsKey;

class GrphcsInput {
    friend class GrphcsGame;

public:

    GRPHCS_INPUT GrphcsVec mousePosition(void) { return GrphcsVec(this->mX, this->mY); }
    GRPHCS_INPUT int mouseX(void) { return this->mX; }
    GRPHCS_INPUT int mouseY(void) { return this->mY; }
    GRPHCS_INPUT int mouseWheel(void) { return this->mWheel; }
    GRPHCS_INPUT bool mousePressed(GrphcsMouseButton button) { return this->mPressed[button]; }
    GRPHCS_INPUT bool mouseDown(GrphcsMouseButton button) { return this->mDown[button]; }
    GRPHCS_INPUT bool mouseReleased(GrphcsMouseButton button) { return this->mReleased[button]; }

    GRPHCS_INPUT bool keyPressed(GrphcsKey key) { return this->kPressed[key]; }
    GRPHCS_INPUT bool keyDown(GrphcsKey key) { return this->kDown[key]; }
    GRPHCS_INPUT bool keyReleased(GrphcsKey key) { return this->kReleased[key]; }

private:

    int mX, mY;
    int mWheel;
    bool mDown[GrphcsMouseButton::MOUSE_BUTTON_COUNT];
    bool mPressed[GrphcsMouseButton::MOUSE_BUTTON_COUNT];
    bool mReleased[GrphcsMouseButton::MOUSE_BUTTON_COUNT];
    
    bool kPressed[GrphcsKey::KEY_COUNT];
	bool kDown[GrphcsKey::KEY_COUNT];
	bool kReleased[GrphcsKey::KEY_COUNT];

    GrphcsInput(void) {
        memset(this->mDown, 0, sizeof(bool) * GrphcsMouseButton::MOUSE_BUTTON_COUNT);
        memset(this->kDown, 0, sizeof(bool) * GrphcsKey::KEY_COUNT);
        this->prepare();
    }

    void prepare() {
        memset(this->kPressed,  0, sizeof(bool) * GrphcsKey::KEY_COUNT);
        memset(this->kReleased, 0, sizeof(bool) * GrphcsKey::KEY_COUNT);

        memset(this->mPressed,  0, sizeof(bool) * GrphcsMouseButton::MOUSE_BUTTON_COUNT);
        memset(this->mReleased, 0, sizeof(bool) * GrphcsMouseButton::MOUSE_BUTTON_COUNT);

        SDL_GetMouseState(&this->mX, &this->mY);
    }

};

#endif
