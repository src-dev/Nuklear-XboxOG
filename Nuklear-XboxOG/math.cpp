#include "math.h"

#include <xtl.h>

int math::clamp_int(int value, int min, int max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float math::clamp_float(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float math::copy_sign(float a, float b)
{
    return (b >= 0.0f) ? fabsf(a) : -fabsf(a);
}