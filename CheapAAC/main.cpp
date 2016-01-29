#include "CheapAAC.h"
#include <iostream>

int main(){
    CheapAAC music;
    CheapAAC output;
    music.readFile("/home/kachan/sandbox/CheapAAC/SampleAAC.aac");
    std::cout<<"\n";
    output.readFile("/home/kachan/sandbox/CheapAAC/output2.aac");
    music.writeFile("/home/kachan/sandbox/CheapAAC/SampleAAC.aac", "/home/kachan/sandbox/CheapAAC/output.aac", 0, 1763/2);
}
