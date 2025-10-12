#pragma once

class math
{
public:
    static int clamp_int(int value, int min, int max);
    static float clamp_float(float value, float min, float max);
    static float copy_sign(float a, float b);
};
