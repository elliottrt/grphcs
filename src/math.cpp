#include "math.hpp"

GrphcsMat GrphcsMat::quickInverse(void) const {
    GrphcsMat matrix = GrphcsMat();
    matrix.m[0][0] = this->m[0][0]; matrix.m[0][1] = this->m[1][0]; matrix.m[0][2] = this->m[2][0]; matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = this->m[0][1]; matrix.m[1][1] = this->m[1][1]; matrix.m[1][2] = this->m[2][1]; matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = this->m[0][2]; matrix.m[2][1] = this->m[1][2]; matrix.m[2][2] = this->m[2][2]; matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = -(this->m[3][0] * matrix.m[0][0] + this->m[3][1] * matrix.m[1][0] + this->m[3][2] * matrix.m[2][0]);
    matrix.m[3][1] = -(this->m[3][0] * matrix.m[0][1] + this->m[3][1] * matrix.m[1][1] + this->m[3][2] * matrix.m[2][1]);
    matrix.m[3][2] = -(this->m[3][0] * matrix.m[0][2] + this->m[3][1] * matrix.m[1][2] + this->m[3][2] * matrix.m[2][2]);
    matrix.m[3][3] = 1.0f;
	return matrix;
}

// See https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_olcEngine3D_Part3.cpp
GrphcsMat GrphcsMat::makeProjectionMatrix(float fov, float aspect, float near, float far) {

    float scaling = 1.f / tanf(fov * M_PI / 360.f);

    GrphcsMat matrix = GrphcsMat();

    matrix.m[0][0] = aspect * scaling;
    matrix.m[1][1] = scaling;
    matrix.m[2][2] = far / (far - near);
    matrix.m[2][3] = 1.0f;
    matrix.m[3][2] = (near * -far) / (far - near);
    matrix.m[3][3] = 0;

    return matrix;

}

GrphcsMat GrphcsMat::pointAt(const GrphcsVec &pos, const GrphcsVec &target, const GrphcsVec &up) {

    GrphcsVec nForward = (target - pos).normalized();

    GrphcsVec nUp = nForward * up.dot(nForward);
    nUp = (up - nUp).normalized();

    GrphcsVec nRight = nUp.cross(nForward);

    GrphcsMat matrix = GrphcsMat();

    matrix.m[0][0] = nRight.x;
    matrix.m[0][1] = nRight.y;
    matrix.m[0][2] = nRight.z;
    matrix.m[1][0] = nUp.x;	
    matrix.m[1][1] = nUp.y;	
    matrix.m[1][2] = nUp.z;
    matrix.m[2][0] = nForward.x;
    matrix.m[2][1] = nForward.y;
    matrix.m[2][2] = nForward.z;
    matrix.m[3][0] = pos.x;
    matrix.m[3][1] = pos.y;
    matrix.m[3][2] = pos.z;
    matrix.m[3][3] = 1.f;

    return matrix;

}

// See https://www.3dgep.com/understanding-the-view-matrix/
GrphcsMat GrphcsMat::lookAt(const GrphcsVec &pos, const GrphcsVec &target, const GrphcsVec &up) {
    //return GrphcsMat::pointAt(pos, target, up).quickInverse();

    GrphcsVec zaxis = (pos - target).normalized();    // The "forward" vector.
    GrphcsVec xaxis = up.cross(zaxis).normalized();// The "right" vector.
    GrphcsVec yaxis = zaxis.cross(xaxis);     // The "up" vector.

    GrphcsMat viewMatrix = GrphcsMat();
    viewMatrix.m[0][0] = xaxis.x;
    viewMatrix.m[0][1] = yaxis.x;
    viewMatrix.m[0][2] = zaxis.x;
    viewMatrix.m[0][3] = 0;
    viewMatrix.m[1][0] = xaxis.y;
    viewMatrix.m[1][1] = yaxis.y;
    viewMatrix.m[1][2] = zaxis.y;
    viewMatrix.m[1][3] = 0;
    viewMatrix.m[2][0] = xaxis.z;
    viewMatrix.m[2][1] = yaxis.z;
    viewMatrix.m[2][2] = zaxis.z;
    viewMatrix.m[2][3] = 0;
    viewMatrix.m[3][0] = -xaxis.dot(pos);
    viewMatrix.m[3][1] = -yaxis.dot(pos);
    viewMatrix.m[3][2] = -zaxis.dot(pos);
    viewMatrix.m[3][2] = 1;

    return viewMatrix;

}

GrphcsMat GrphcsMat::fpsView(const GrphcsVec &pos, float pitch, float yaw) {

    float cosPitch = cosf(pitch);
    float sinPitch = sinf(pitch);
    float cosYaw = cosf(yaw);
    float sinYaw = sinf(yaw);

    GrphcsVec xaxis = { cosYaw, 0, -sinYaw };
    GrphcsVec yaxis = { sinYaw * sinPitch, cosPitch, cosYaw * sinPitch };
    GrphcsVec zaxis = { sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw };

    GrphcsMat viewMatrix = GrphcsMat();
    viewMatrix.m[0][0] = xaxis.x;
    viewMatrix.m[0][1] = yaxis.x;
    viewMatrix.m[0][2] = zaxis.x;
    viewMatrix.m[0][3] = 0;
    viewMatrix.m[1][0] = xaxis.y;
    viewMatrix.m[1][1] = yaxis.y;
    viewMatrix.m[1][2] = zaxis.y;
    viewMatrix.m[1][3] = 0;
    viewMatrix.m[2][0] = xaxis.z;
    viewMatrix.m[2][1] = yaxis.z;
    viewMatrix.m[2][2] = zaxis.z;
    viewMatrix.m[2][3] = 0;  
    viewMatrix.m[3][0] = -xaxis.dot(pos);
    viewMatrix.m[3][1] = -yaxis.dot(pos);
    viewMatrix.m[3][2] = -zaxis.dot(pos);
    viewMatrix.m[3][2] = 1;

    return viewMatrix;  

}
