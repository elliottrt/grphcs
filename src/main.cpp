#include "../include/game.hpp"
#include "../include/util.hpp"

GrphcsVec p0 (0, -1, 5),
          p1 (-1, 1, 5),
          p2 (1, 1, 5);

GrphcsPixel r (255, 0, 0),
            g (0, 255, 0),
            b (0, 0, 255);  

GrphcsGeometry * geo;
GrphcsGame * game;

float movespeed = 5.f;
float rotspeed = 1 / 3.f;
GrphcsVec movement = GrphcsVec(0),
          rotation = GrphcsVec(0);

void update(float dt) {
    
    // TODO: rotation is broken
    geo->setRotationRelative(GrphcsVec(
        0.05f * game->input.keyDown(GrphcsKey::KEY_X), 
        0.05f * game->input.keyDown(GrphcsKey::KEY_Y), 
        0.05f * game->input.keyDown(GrphcsKey::KEY_Z)
    ));

    if (game->input.keyDown(GrphcsKey::KEY_ESCAPE)) {
        game->quit(0);
    }

    int forwards = game->input.keyDown(GrphcsKey::KEY_W) - game->input.keyDown(GrphcsKey::KEY_S);
    int strafe = game->input.keyDown(GrphcsKey::KEY_D) - game->input.keyDown(GrphcsKey::KEY_A);
    int up = game->input.keyDown(GrphcsKey::KEY_LSHIFT) - game->input.keyDown(GrphcsKey::KEY_SPACE);

    movement = GrphcsVec(strafe, up, forwards) * movespeed * dt;

    int ud = game->input.keyDown(GrphcsKey::KEY_DOWN) - game->input.keyDown(GrphcsKey::KEY_UP);
    int lr = game->input.keyDown(GrphcsKey::KEY_RIGHT) - game->input.keyDown(GrphcsKey::KEY_LEFT);

    rotation = GrphcsVec(ud, lr) * rotspeed * dt;

}

void render(GrphcsContext *ctx) {

    //ctx->triangleBlend(p0, p1, p2, r, g, b);

    //ctx->camera.setPositionRelative(movement);
    //ctx->camera.setRotationRelative(rotation);

    // ctx->drawGeometry(geo, GrphcsVec(0, 0, 50), true);
    // ctx->drawGeometry(geo, GrphcsVec(5, 0, -25), true);
    // ctx->drawGeometry(geo, GrphcsVec(7, 5, 17), true);
    // ctx->drawGeometry(geo, GrphcsVec(-10, -5, 0), true);
    // ctx->drawGeometry(geo, GrphcsVec(30, -25, -3), true);

    grphcsTimingStart();
    ctx->drawGeometry(geo, GrphcsVec(0, 0, 10), true);
    grphcsTimingEnd(NULL);

}

// TODO: add geometry presets

int main (void) {

    geo = new GrphcsGeometry("cow.obj");
    //geo = GrphcsGeometry::sphere(10, 10, 5);
    geo->setRotation(GrphcsVec(M_PI, 0, M_PI));

    game = new GrphcsGame("Hello, World!", 640, 480, 90.f, update, render);
    game->run();

    delete game;
    delete geo;

    return 0;

}
