#ifndef CAMERA_H
#define CAMERA_H

#include "MathGeoLibFwd.h"
#include "Math/float3.h"

#include <QMatrix4x4>

class Camera
{
public:
    Camera();
    explicit Camera(const vec& position);

    void move(float amount);
    void strafe(float amount);
    void elevate(float amount);

    void rotateHeading(float amount);
    void rotatePitch(float amount);
    void rotateYaw(float amount);

    operator QMatrix4x4();

    vec view;
    vec right;
    vec up;
    vec position;

private:
    void rotateAroundAxis(vec& v, const vec& axis, float amount);
};

#endif // CAMERA_H
