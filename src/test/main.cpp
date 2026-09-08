#include <iostream>
#include "jons-slicer.h"
#include "test/test.h"

Lib3MF::PWrapper wrapper;

int main(int argc, char** argv){
    
    /*
    if(test_epsilon()){
        std::cout << "test_epsilon passed.\n";
    }else{
        std::cout << "test_epsilon failed\n";
    }//*/
    
    try{// Eventually I'll *probably* hide this in an init function since it's ugly.
        wrapper = Lib3MF::CWrapper::loadLibrary();
    }catch (std::exception &e){
        std::cout << "Failed to load the Lib3MF library.\n";
        std::cout << e.what() << "\n";
        return 1;
    }
    
    std::vector<mesh> meshes;
    if(argc>=2)
        load(argv[1],meshes);
    
    std::cout << "File '" << argv[1] << "' contains "<< meshes.size() << " meshes.\n";
    
    for(mesh M:meshes)
        slice_mesh(M,1);
    
    
    return 0;
}

