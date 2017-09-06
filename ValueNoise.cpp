#include "ValueNoise.h"

#include <math.h>

// Source: http://stackoverflow.com/a/15341154/2471991
inline int ffloor(double x)
{
  int i = (int)x;
  return i - ( i > x );
}

// Source: http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
float noise(int x)
{
    const int x1 = (x << 13) ^ x;
    return 0.5f + ( 0.5f - ( (x1 * (x1 * x1 * 15731 + 789221) + 1376312589) & 0x7fffffff) / 2147483648.0);
}

#define YPRIME 57
#define ZPRIME 1337

float noise1(float x)
{
    const int x1 = (int)ffloor(x);
    const int x2 = x1 + 1;
    const float xt = x - x1;

    return noise(x1) * xt + noise(x2) * (1.0f - xt);
}

float noise2(float x, float y)
{
    const int x1 = (int)ffloor(x);
    const int y1 = (int)ffloor(y);
    const int x2 = x1 + 1;
    const int y2 = y1 + 1;
    const float xt = x - x1;
    const float yt = y - y1;

    return (noise(x1 + y1 * YPRIME) * xt + noise(x2 + y1 * YPRIME) * (1.0f - xt)) * (       yt)
          +(noise(x1 + y2 * YPRIME) * xt + noise(x2 + y2 * YPRIME) * (1.0f - xt)) * (1.0f - yt);
}



float noise3(float x, float y, float z)
{
    const int x1 = (int)ffloor(x);
    const int y1 = (int)ffloor(y);
    const int z1 = (int)ffloor(z);
    const int x2 = x1 + 1;
    const int y2 = y1 + 1;
    const int z2 = z1 + 1;
    const float xt = x - x1;
    const float yt = y - y1;
    const float zt = z - z1;

    return ( (noise(x1 + y1 * YPRIME + z1 * ZPRIME) * (1.0f - xt) + noise(x2 + y1 * YPRIME + z1 * ZPRIME) * (xt)) * ( 1.0f -       yt)
            +(noise(x1 + y2 * YPRIME + z1 * ZPRIME) * (1.0f - xt) + noise(x2 + y2 * YPRIME + z1 * ZPRIME) * (xt)) * (yt)) * (1.0f -     zt)
            +((noise(x1 + y1 * YPRIME + z2 * ZPRIME) * (1.0f - xt) + noise(x2 + y1 * YPRIME + z2 * ZPRIME) * (xt)) * (  1.0f -      yt)
            +(noise(x1 + y2 * YPRIME + z2 * ZPRIME) * (1.0f - xt) + noise(x2 + y2 * YPRIME + z2 * ZPRIME) * (xt)) * (yt)) * (zt);
}

