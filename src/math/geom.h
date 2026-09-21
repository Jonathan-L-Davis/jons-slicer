#pragma once
#include "math/math.h"
#include <vector>

struct segment_t{
    point p[2];
    vector normal;
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

struct layer_t{
    std::vector<segment_t> segments;// this should not be the data structure. It forces an n^2 look up time for constructing real paths later.
    
};

struct slice_t{// rethinking this struct. I want an intermediate struct for the layers vector & I want it to have fast look up for path segments. Going to be tricky. Needs a multimap & who knows what else.
    std::vector<layer_t> layers;
    float layer_height;
    
    void add_segment(segment_t,int);
};

tri operator + (tri a, vector b);
tri operator + (vector a, tri b);

bool load(std::string filename,std::vector<mesh>& retMe);
slice_t slice_mesh(const mesh&, float layer_height);



