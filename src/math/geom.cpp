#include "math/geom.h"
#include "common.h"

#include <limits>
#include <assert.h>

tri operator + (tri a, vector b){
    for(int i = 0; i < 3; i++) a.p[i] = a.p[i] + b;
    return a;
}

tri operator + (vector a, tri b){
    return b+a;
}

mesh read_object(const Lib3MF::PObject&  obj, const Lib3MF::PModel& model){// hate that I need to pass the model down into this function. Could just pass the mesh, but I can think about that after it works.
    mesh retMe;
    
    retMe.name = obj->GetName();
    
    Lib3MF::PMeshObject mesh = model->GetMeshObjectByID(obj->GetResourceID());// should not have to reference the model to get the mesh of our object. This is horse shit.
    
    uint64_t num_tris = mesh->GetTriangleCount();
    retMe.tris.reserve(num_tris);
    
    point min = {inf,inf,inf}, max = {-inf,-inf,-inf};
    
    for( uint64_t i = 0; i < num_tris; i++ ){
        tri new_tri;
        
        Lib3MF::sTriangle tri_idxs = mesh->GetTriangle(i);
        for(int i = 0; i < 3; i++){
            Lib3MF::sPosition p = mesh->GetVertex(tri_idxs.m_Indices[i]);
            new_tri.p[i].x = p.m_Coordinates[0];
            new_tri.p[i].y = p.m_Coordinates[1];
            new_tri.p[i].z = p.m_Coordinates[2];
            
            min.x = std::min(min.x,new_tri.p[i].x); max.x = std::max(max.x,new_tri.p[i].x);
            min.y = std::min(min.y,new_tri.p[i].y); max.y = std::max(max.y,new_tri.p[i].y);
            min.z = std::min(min.z,new_tri.p[i].z); max.z = std::max(max.z,new_tri.p[i].z);
        }
        
        new_tri.normal = get_normal(new_tri.p[0],new_tri.p[1],new_tri.p[2]);
        
        // EXTREMELY IMPORTANT - sorting the points by height is an important invariant for our slicing algorithm.
        // since the triangles are now sorted, we can simplify the logic of converting a triangle into segments.
        if(new_tri.p[0].z>new_tri.p[1].z)
            std::swap(new_tri.p[0],new_tri.p[1]);
        if(new_tri.p[1].z>new_tri.p[2].z)
            std::swap(new_tri.p[1],new_tri.p[2]);
        if(new_tri.p[0].z>new_tri.p[1].z)
            std::swap(new_tri.p[0],new_tri.p[1]);
        
        retMe.tris.push_back(new_tri);
    }
    
    retMe.min = min;
    retMe.max = max;
    retMe.offset = {-(min.x+max.x)/2,-(min.y+max.y)/2,-min.z}; //(makes sure the model is above the zplane & roughly centered)
    
    return retMe;
}

bool load(std::string filename,std::vector<mesh>& retMe){
    
    Lib3MF::PModel model = wrapper->CreateModel();
    
    // Create a 3MF reader and read the file
    Lib3MF::PReader reader = model->QueryReader("3mf");
    reader->SetStrictModeActive(false);
    reader->ReadFromFile(filename);
    
    Lib3MF::PObjectIterator objIter = model->GetObjects();
    uint64_t num_obj = 0;// if this overflows, I'd like to talk to you. That's quite a large file system you've got there. (nevermind the fact that it takes a few hundred years to run that many increment instructions lol)
    
    for (;objIter->MoveNext();num_obj++);// if you're frozen here, damn. (either means EXTREMELY large file, or lib3MF generated a cycle in it's objIter, which shouldn't happen.)
    
    retMe.resize(0);
    retMe.reserve(num_obj);
    
    std::cout << "num_obj: " << num_obj << "\n";
    
    objIter = model->GetObjects();
    for(;objIter->MoveNext();){
        Lib3MF::PObject obj = objIter->GetCurrentObject();
        if(!obj->IsMeshObject())// skip processing for non mesh objects. Removed the validity check because even basic models were failing it.
            continue;
        
        retMe.push_back(read_object(obj,model));
    }
    
    if(retMe.size()==0)
        return false;
    return true;
}

/* 
 * Need a better name for this function. I don't know whether to call it project/intersect/interpolate/slice.
 * 
 * 
 * 
 */
std::vector<point> yuh(point a, point b, float layer_height){
    std::vector<point> retMe;
    
    // It's very important for these layers to both be floored. Otherwise we don't get the right amount of points for the edges of the triangle.
    int start_layer = std::floor(a.z/layer_height);
    int end_layer = std::floor(b.z/layer_height);
    
    vector ba = b-a;
    
    assert(ba.z>=0&&"b's height should always be greater than or equal to a's height.");
    if(ba.z==0) return {{a.x,a.y,start_layer*layer_height}};// if there's no z height difference return early to avoid division by 0.
    
    assert(start_layer<=end_layer&&"points not sorted by height?!?!");
    
    for(int layer = start_layer; layer <= end_layer;layer++){// this may produce an extra layer or 1 on top/bottom. Not entirely sure just yet.
        float l = (layer_height*layer-a.z)/b.z;// b.z normalizes. We're lerping from a to b.
        point q = a+l*ba;
        point addMe = {q.x,q.y, layer*layer_height };// it's important to redefine the z coordinate. We rely on the z coordinate being constant for a given layer.
        
        if(layer*layer_height<=a.z)
            addMe = {a.x,a.y,layer*layer_height};
        if(layer*layer_height>=b.z)
            addMe = {b.x,b.y,layer*layer_height};
        
        retMe.push_back(addMe);
    }
    
    
    
    return retMe;
}


// A lot of asserts in this function. This is where I'm enforcing a lot of invariants for slicing.
// It took me a lot of time to figure out the invariants, and they will almost certainly change if you use a different approach to slicing.
slice_t slice_mesh(const mesh& sliceMe, float layer_height){
    slice_t retMe;
    
    vector offset = sliceMe.offset;
    
    for( tri t : sliceMe.tris ){
        t = t+offset;
        
        /** find first segment on triangle, then create them above every layer height. Need to workout the projection with the xy plane. **/
        std::vector<point> L02 = yuh(t.p[0],t.p[2],layer_height);
        std::vector<point> L01 = yuh(t.p[0],t.p[1],layer_height);
        std::vector<point> L12 = yuh(t.p[1],t.p[2],layer_height);
        
        int l0 = std::floor(t.p[0].z/layer_height);
        int l1 = std::floor(t.p[1].z/layer_height);
        int l2 = std::floor(t.p[2].z/layer_height);
        
        assert(L02.size()!=0);
        assert(L12.size()!=0);
        assert(L01.size()!=0);
        
        // should hold for all triangles.
        assert(L02.size()+1==L01.size()+L12.size());
        
        // verify that the points are on the same height.
        assert( std::floor(L01[L01.size()-1].z/layer_height) == std::floor(L12[0].z/layer_height) );// we can delete the first point in L12 & be fine. There are probably "better" approaches, but this'll work for now. Maybe smarter to take the "furthest" of the 2 points on this level.
        assert( std::floor(L02[L02.size()-1].z/layer_height) == std::floor(L12[L12.size()-1].z/layer_height) );
        
        L12.erase(L12.begin());
        
        int i = 0;
        for(; i < L01.size(); i++ )
            if(L01[i]!=L02[i])
                retMe.add_segment({ L02[i], L01[i], t.normal}, i+l0 );
        for(; i < L02.size(); i++ )
            if(L12[i]!=L02[i])
                retMe.add_segment({ L02[i], L12[i-l1], t.normal}, i+l0);
        
    }
    
    return retMe;
}

void slice_t::add_segment(segment_t addMe, int layer){
    if(layers.size()<layer+1) layers.resize(layer+1);
    
    layers[layer].segments.push_back(addMe);
}
