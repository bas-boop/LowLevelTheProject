#include "Texture.h"

Texture::Texture()
{
    
}

Texture::~Texture()
{
    
}

Texture::Texture(const Texture& other)
{
    this->data = new int[size];
    std::copy(other.data, other.size + other.data, data);
}

Texture& Texture::operator=(Texture& other)
{
    if (this == &other) return *this;

    delete[] data;

    this->name = other.name;
    this->size = other.size;
    this->data = other.data;
    std::copy(other.data, other.size + other.data, data);
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Texture& t)
{
    std::cout << t.name << " " << &t << " " << t.size << '\n';
    return os;
}
