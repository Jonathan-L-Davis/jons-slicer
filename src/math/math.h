#pragma once

#include <cmath>

extern float inf;

struct point{
    float x,y,z;
};

struct vector{
    float x,y,z;
};

struct point2{
    float x,y;
};

struct vec2{
    float x,y;
};


point operator + (vector a, point b);
point operator + (point a, vector b);

vector operator - (point a, point b);

vector cross(vector a, vector b);

vector get_normal(point a, point b, point c);

float epsilon(float);

