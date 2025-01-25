#include "camera.hpp"

GrphcsCamera::GrphcsCamera(float fov, int xSize, int ySize) {

    this->position = GrphcsVec();
    this->rotation_v = GrphcsVec();
    this->rotation_m = GrphcsMat();

    // TODO: figure out if we should do y/x or x/y for aspect
    this->perspective_m = GrphcsMat::makeProjectionMatrix(fov, (float) ySize / xSize, NEARPLANE, FARPLANE);
    this->generateViewMatrix();

    this->xSize = xSize;
    this->ySize = ySize;

}

void GrphcsCamera::setRotation(GrphcsVec rotation) {

    rotation.y = fmod(rotation.y, 360.f);
    rotation.x = SDL_clamp(rotation.x, -90 * DEG2RAD, 90 * DEG2RAD);

    this->rotation_v = rotation;
    this->rotation_m = rotation.getRotationMatrix();
    this->generateViewMatrix();
}

void GrphcsCamera::generateViewMatrix(void) {

    //GrphcsVec up_v (0, 1, 0);
	//GrphcsVec target_v (0, 0, 1);

    //target_v *= this->rotation_m;
    //target_v += this->position;

    //this->view_m = GrphcsMat::lookAt(target_v, this->position, up_v);

    this->view_m = GrphcsMat::fpsView(this->position, this->rotation_v.x, this->rotation_v.y);

}
