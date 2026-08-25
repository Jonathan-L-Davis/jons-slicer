#pragma once
#include "math/math.h"
#include <vector>

struct tri{
    point p[3];
    vector normal;
};

struct mesh{
    std::string name;
    std::vector<tri> tris;
    point min,max,offset;
};

bool load(std::string filename,std::vector<mesh>& retMe);

