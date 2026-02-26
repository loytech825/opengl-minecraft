#include "Chunk.hpp"

#include <iostream>

Chunk::Chunk(int X, int Y, int Z) : x(X), y(Y), z(Z)
{
    std::fill(blocks.begin(), blocks.end(), (unsigned char)1);
}

Chunk::Chunk() : Chunk(0, 0, 0) {}

//DEBUG
void Chunk::print(){

    for(const auto& blk : blocks){
        std::cout << (unsigned short)blk.type << "\n";
    }
}