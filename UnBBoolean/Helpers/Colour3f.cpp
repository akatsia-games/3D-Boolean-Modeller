#include"Colour3f.hpp"

#include<iostream>
#include<cmath>



inline void Colour3f::read(std::basic_istream<char>& file){
    file>>r>>g>>b;
}
inline void Colour3f::write(std::basic_ostream<char>& file)const{
    file<<r<<" "<<g<<" "<<b<<"\n";
}
bool Colour3f::equals(const Colour3f& other)const{
    return (std::abs(r-other.r)+std::abs(g-other.g)+std::abs(b-other.b)) < TOL;
}