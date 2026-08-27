#include <iostream>
#include "jons-slicer.h"
#include "test/test.h"

int main(){
    
    if(test_epsilon()){
        std::cout << "test_epsilon passed.\n";
    }else{
        std::cout << "test_epsilon failed\n";
    }
    
    return 0;
}

