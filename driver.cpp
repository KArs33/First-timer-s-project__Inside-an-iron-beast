#include "IAIB.h"
#include<iostream>
#include <cstdlib>
#include <string>
#include <ctime>


int main(int argc, char *argv[]){
    // program expects two arguments: loc file and foe file
    
    std::cout<<"test: if you are reading this, main() of driver is running.\n";
        
    // Debug helper: put a marker here to set a breakpoint on the next line
    //volatile int BREAKPOINT_MARKER = 0; // <-- set a breakpoint on this line if you want
    std::cout << "Paused at BREAKPOINT_MARKER. Press Enter to continue (or attach debugger and continue)...\n";
    std::cin.get();
    
    if(argc != 3){
        std::cout<<"Usage: driver <loc.txt> <foe.txt>\n";
        return 1;
    }

    std::cout<<"test: if you are reading this, main of driver got past the first if() check.\n";
    // Debug helper: put a marker here to set a breakpoint on the next line
    //volatile int BREAKPOINT_MARKER = 0; // <-- set a breakpoint on this line if you want
    std::cout << "Paused at BREAKPOINT_MARKER. Press Enter to continue (or attach debugger and continue)...\n";
    std::cin.get();

    std::cout<<"Loading files: "<< argv[1] <<" and "<<argv[2]<<"\n";
    // Debug helper: put a marker here to set a breakpoint on the next line
    //volatile int BREAKPOINT_MARKER = 0; // <-- set a breakpoint on this line if you want
    std::cout << "Paused at BREAKPOINT_MARKER. Press Enter to continue (or attach debugger and continue)...\n";
    std::cin.get();
    srand(time(NULL));
    startGame(string(argv[1]), string(argv[2]));
    
    return 0;
};
