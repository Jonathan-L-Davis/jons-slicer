#pragma once
#include "math/math.h"
#include <vector>

struct segment{
    point2 p[2];
    vec2 normal;
};

struct tri{
    point p[3];
    vector normal;
};

struct mesh{
    std::string name;
    std::vector<tri> tris;
    point min,max;
    vector offset;
};

struct slice{
    std::vector<std::vector<segment>> layers;
    float layer_height;
};

tri operator + (tri a, vector b);
tri operator + (vector a, tri b);

bool load(std::string filename,std::vector<mesh>& retMe);
slice slice_mesh(const mesh&, float layer_height);



