#include "IAIB.h"
#include<iostream>
#include <cstdlib>
#include <string>
#include <ctime>


int main(int argc, char *argv[]){
    // program expects two arguments: loc file and foe file
    
        
    // startup helper removed so the program launches immediately under a debugger or terminal
    
    if(argc != 3){
        std::cout<<"Usage: driver <loc.txt> <foe.txt>\n";
        return 1;
    }

    std::cout<<"Starting IAIB — loading files: "<< argv[1] <<" and "<<argv[2]<<"\n";
    srand(time(NULL));
    // startGame expects (foeFile, locFile) — pass argv[2] (foe) first and argv[1] (loc) second
    startGame(string(argv[2]), string(argv[1]));
    
    return 0;
};
