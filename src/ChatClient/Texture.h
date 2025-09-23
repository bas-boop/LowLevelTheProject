#pragma once
#include <iostream>

class Texture
{
public:
    Texture();
    virtual ~Texture();
    Texture (const Texture& other);
    Texture& operator=(Texture& other);

    std::string name;
    int size = 2;
    int* data;

    friend std::ostream& operator <<(std::ostream& os, const Texture& t);
};
