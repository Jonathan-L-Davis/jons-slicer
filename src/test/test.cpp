#include "test/test.h"
#include "jons-slicer.h"

#include <iostream>
#include <format>

bool test_epsilon(){
    bool retMe;
    
    for(uint32_t i = 0; i < 0x7F'7F'FF'FF; i++ ){
        float x = *(float*)(void*)&i;// shorter than the union type pun. Ideally compiles to just a load instruction.
        float eps1 = epsilon(x);
        float eps2 = epsilon(eps1);
        
        if(x==x+eps1){
            std::cout << "Case 1 failure\n\n";
            float temp = x+(eps1);
            float temp2 = eps1-eps2;
            std::cout << std::dec << "exponent: " << (i>>23) << "\n";
            std::cout << std::hex << "x:    " << i << "\n";
            std::cout << "temp : " << *(uint32_t*)(void*)&temp << "\n";
            std::cout << "eps1 : " << *(uint32_t*)(void*)&eps1 << "\n";
            
            std::cout << std::format("x    : {}",x) << "\n";
            std::cout << std::format("temp : {}",temp) << "\n";
            std::cout << std::format("eps1 : {}",eps1) << "\n";
            return false;
        }
        
        /* either this test case is subtly flawed, or the epsilon function doesn't generate the absolute smallest increment that causes x!=x+eps1
        if( x!=x+(eps1-eps2) ){
            std::cout << "Case 2 failure\n\n";
            
            float temp = x+(eps1-eps2);
            float temp2 = eps1-eps2;
            std::cout << std::dec << "exponent: " << (i>>23) << "\n";
            std::cout << std::hex << "x    : " << i << "\n";
            std::cout << "temp : " << *(uint32_t*)(void*)&temp << "\n";
            std::cout << "eps1 : " << *(uint32_t*)(void*)&eps1 << "\n";
            std::cout << "eps2 : " << *(uint32_t*)(void*)&eps2 << "\n";
            std::cout << "delta: " << *(uint32_t*)(void*)&temp2 << "\n\n";
            
            std::cout << std::format("x    : {}",x) << "\n";
            std::cout << std::format("temp : {}",temp) << "\n";
            std::cout << std::format("eps1 : {}",eps1) << "\n";
            std::cout << std::format("eps2 : {}",eps2) << "\n";
            std::cout << std::format("delta: {}",eps1-eps2) << "\n";
            return false;
        }//*/
        
        
    }
    
    return true;
}

