#include "math/math.h"

float inf = INFINITY;



point operator + (point a, vector b){
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

point operator + (vector a, point b){
    return b+a;
}

vector operator - (point a, point b){
    return {a.x-b.x,a.y-b.y,a.z-b.z};
}

vector cross(vector a, vector b){// should double check when I'm not tired.
    return { a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x };
}

vector get_normal(point a, point b, point c){
    vector x1 = b-a;
    vector x2 = c-a;
    
    return cross(x1,x2);
}

