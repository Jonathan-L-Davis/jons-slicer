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

float epsilon(float input){
    union{
        uint32_t fixMe;
        float retMe;
    };
    
    retMe = input;
    
    uint32_t e = (fixMe>>23)&0xFF;
    uint32_t b = 0;
    
    if( e>23 ){
        e -= 23;
        b = 0;
    }else{
        
        if(e==0) b = 1;
        else b = 1<<(e-1);
    }
    
    fixMe = (e<<23) | (b&0x7F'FFFF);
    
    return retMe;
}

