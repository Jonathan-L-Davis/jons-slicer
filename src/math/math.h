#pragma once

#include <cmath>

extern float inf;

struct point{
    float x,y,z;
};

struct vector{
    float x,y,z;
};

vector operator - (point a, point b);

vector cross(vector a, vector b);

vector get_normal(point a, point b, point c);

