#include "Camera.h"

Camera::Camera() : Camera(vec::zero)
{

}

Camera::Camera(const vec& position) : view(0,0,-1), right(-1,0,0), up(0,1,0), position(position)
{

}

void Camera::move(float amount)
{
    position += view * amount;
}

void Camera::strafe(float amount)
{
    position += right * amount;
}

void Camera::elevate(float amount)
{
    position += up * amount;
}

void Camera::rotateHeading(float amount)
{
    rotateAroundAxis(view, right, amount);
    rotateAroundAxis(up, right, amount);
}

void Camera::rotatePitch(float amount)
{
    rotateAroundAxis(view, up, amount);
    rotateAroundAxis(right, up, amount);
}

void Camera::rotateYaw(float amount)
{
    rotateAroundAxis(right, view, amount);
    rotateAroundAxis(up, view, amount);
}

void Camera::rotateAroundAxis(vec& v, const vec& axis, float amount)
{
    vec temp = axis;
    float fX = temp.x;
    float fY;
    float fZ = temp.z;

    const float fA = -atan2(temp.x,temp.z);
    temp.x = sin(atan2(fX,fZ)+fA)*sqrt(fX*fX+fZ*fZ);
    temp.z = cos(atan2(fX,fZ)+fA)*sqrt(fX*fX+fZ*fZ);
    fZ = v.z;
    fX = v.x;
    v.x = sin(atan2(fX,fZ)+fA)*sqrt(fX*fX+fZ*fZ);
    v.z = cos(atan2(fX,fZ)+fA)*sqrt(fX*fX+fZ*fZ);


    const float fB = -atan2(temp.y,temp.z);

    fY = temp.y;
    fZ = temp.z;
    temp.z = cos(atan2(fY,fZ)+fB)*sqrt(fY*fY+fZ*fZ);
    temp.y = sin(atan2(fY,fZ)+fB)*sqrt(fY*fY+fZ*fZ);

    fY = v.y;
    fZ = v.z;
    v.z = cos(atan2(fY,fZ)+fB)*sqrt(fY*fY+fZ*fZ);
    v.y = sin(atan2(fY,fZ)+fB)*sqrt(fY*fY+fZ*fZ);

    fX = v.x;
    fY = v.y;
    v.x = cos(atan2(fY,fX)+amount)*sqrt(fX*fX+fY*fY);
    v.y = sin(atan2(fY,fX)+amount)*sqrt(fX*fX+fY*fY);

    fZ = v.z;
    fY = v.y;
    v.z = cos(atan2(fY,fZ)-fB)*sqrt(fY*fY+fZ*fZ);
    v.y = sin(atan2(fY,fZ)-fB)*sqrt(fY*fY+fZ*fZ);

    fZ = v.z;
    fX = v.x;
    v.z = cos(atan2(fX,fZ)-fA)*sqrt(fX*fX+fZ*fZ);
    v.x = sin(atan2(fX,fZ)-fA)*sqrt(fX*fX+fZ*fZ);
}


Camera::operator QMatrix4x4()
{
    QMatrix4x4 m;
    m.lookAt(QVector3D(position.x, position.y, position.z),
             QVector3D(position.x + view.x, position.y + view.y, position.z + view.z),
             QVector3D(up.x, up.y, up.z));
    return m;
}
