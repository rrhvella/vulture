#include "./math-helpers.hpp"

float Clamp(const float &v, const float &from, const float &to)
{
    if (v < from)
    {
        return from;
    }
    else if (v > to)
    {
        return to;
    }

    return v;
}

bool WithinBounds(const float &v, const float &from, const float &to)
{
    return v >= from && v <= to;
}