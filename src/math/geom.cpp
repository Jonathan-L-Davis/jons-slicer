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
            
            min.x = std::min(min.x,new_tri.p[i].x); max.x = std::min(max.x,new_tri.p[i].x);
            min.y = std::min(min.y,new_tri.p[i].y); max.y = std::min(max.y,new_tri.p[i].y);
            min.z = std::min(min.z,new_tri.p[i].z); max.z = std::min(max.z,new_tri.p[i].z);
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
    reader->ReadFromFile(filename);
    
    Lib3MF::PObjectIterator objIter = model->GetObjects();
    uint64_t num_obj = 0;// if this overflows, I'd like to talk to you. That's quite a large file system you've got there. (nevermind the fact that it takes a few hundred years to run that many increment instructions lol)
    
    for (;objIter->MoveNext();num_obj++);// if you're frozen here, damn. (either means EXTREMELY large file, or lib3MF generated a cycle in it's objIter, which shouldn't happen.)
    
    retMe.resize(0);
    retMe.reserve(num_obj);
    
    objIter = model->GetObjects();
    for(;objIter->MoveNext();){
        Lib3MF::PObject obj = objIter->GetCurrentObject();
        if(!obj->IsMeshObject()||!obj->IsValid())// skip processing for invalid or non mesh object
            continue;
        
        retMe.push_back(read_object(obj,model));
    }
    
    if(retMe.size()==0)
        return false;
    return true;
}

std::vector<point2> yuh(point a, point b, float layer_height){
    std::vector<point2> retMe;
    
    int start_layer = std::ceil(a.z/layer_height);
    int end_layer = std::floor(b.z/layer_height);
    
    vector ba = b-a;
    
    for(int layer = start_layer; layer <= end_layer;layer++){// this may produce an extra layer or 1 on top/bottom. Not entirely sure just yet.
        point2 addMe = {0,0};
        
        if(layer*layer_height<=a.z)
            addMe = {a.x,a.y};
        if(layer*layer_height>=b.z)
        
            addMe = {b.x,b.y};
        retMe.push_back(addMe);
    }
    
    
    
    return retMe;
}

slice slice_mesh(const mesh& sliceMe, float layer_height){
    slice retMe;
    
    vector offset = sliceMe.offset;
    
    for( tri t : sliceMe.tris ){
        t = t+offset;
        
        /** find first segment on triangle, then create them above every layer height. Need to workout the projection with the xy plane. **/
        
        std::vector<point2> L02 = yuh(t.p[0],t.p[2],layer_height);
        std::vector<point2> L01 = yuh(t.p[0],t.p[1],layer_height);
        std::vector<point2> L12 = yuh(t.p[1],t.p[2],layer_height);
        
        /* Pretty sure all of these can break. Only been able to trigger L01 so far.
        assert(L02.size()!=0);
        assert(L12.size()!=0);
        assert(L01.size()!=0);//*/
        
        /// L02 sometimes is 1 less than L01 + L12 & sometimes equal to them.
        if(L02.size()+1==L01.size()+L12.size())
            L01.resize(L01.size()-1);
        
    }
    
    return retMe;
}

