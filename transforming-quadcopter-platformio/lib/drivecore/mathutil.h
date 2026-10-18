#pragma once

// helper funcs shared by every module

inline float clamp_value(float value, float low, float high)
{
    if (value < low)
    {
        return low;
    }
    if (value > high)
    {
        return high;
    }
    return value;
}

inline float absolute_value(float value)
{
    if (value < 0.0f)
    {
        return -value;
    }
    return value;
}
