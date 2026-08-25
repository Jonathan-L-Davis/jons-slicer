#include <iostream>

#include "common.h"
#include "math/math.h"
#include "math/geom.h"

Lib3MF::PWrapper wrapper;

int main(int argc, char** argv){
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
    
    return 0;
}

