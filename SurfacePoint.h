#ifndef SURFACEPOINT_H
#define SURFACEPOINT_H

#include "MathGeoLibFwd.h"
#include "Math/float3.h"

class SurfacePoint : public vec
{
public:
    SurfacePoint(const vec& pos, const vec& normal);

    vec normal;
};

#endif // SURFACEPOINT_H
