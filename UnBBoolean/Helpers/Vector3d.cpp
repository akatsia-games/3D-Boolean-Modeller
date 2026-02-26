#include "Vector3d.hpp"
#include "Point3d.hpp"
#include <string>
#include <cmath>

Vector3d& Vector3d::operator+=(Vector3d other){
    x+=other.x;
    y+=other.y;
    z+=other.z;
    return *this;
}

Vector3d& Vector3d::operator-=(Vector3d other){
    x-=other.x;
    y-=other.y;
    z-=other.z;
    return *this;
}

Vector3d& Vector3d::operator*=(double mult){
    x*=mult;
    y*=mult;
    z*=mult;
    return *this;
}

Vector3d Vector3d::operator+(Vector3d other)const{
    other+= (*this);
    return other;
}

Vector3d Vector3d::operator-(Vector3d other)const{
    other-= (*this);
    return other;
}

Vector3d Vector3d::operator*(double mult)const{
    Vector3d cpy = *this;
    cpy*=mult;
    return cpy;
}

Vector3d::operator Point3d() const
{
    return {x,y,z};
}


double Vector3d::isNAN() const{
    return std::isnan(x)||std::isnan(y)||std::isnan(z);
}

double Vector3d::dot(Vector3d other) const{
    return ((double)x*other.x)+((double)y*other.y)+((double)z*other.z);
}

double Vector3d::length() const{
    double lenSqr = dot(*this);
    return sqrt(lenSqr);
}

void Vector3d::normalize(){
    double len = length();
    x/=len;
    y/=len;
    z/=len;
}

void Vector3d::cross(Vector3d& a, Vector3d& b){
    *this = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

double Vector3d::angle(Vector3d& other) const{
    double this_len = length();
    double other_len = other.length();
    double dot_pr = this->dot(other);
    double cos_res = dot_pr / (this_len*other_len);
    return acos(cos_res);
}

void Vector3d::rotate(Vector3d other, double radians){
    other.normalize();
    double other_len = dot(other);

    //remove the component of "this" that is "other"
    (*this) -= other*other_len;

    //save current length and normalize
    double this_len = length();
    normalize();

    //generate a normalized cross product to the "right" of "this" as viewed from "other"
    Vector3d sin_vec;
    sin_vec.cross(*this,other);

    //rotate the current vector and then resize it accordingly and add back the "other" component
    *this = (*this)*cos(radians) + sin_vec*sin(radians);
    *this = (*this) * this_len + other*other_len;
}

std::string Vector3d::toString() const{
    return "(Vector3d x:"+std::to_string(x)+", y:"+std::to_string(y)+", z:"+std::to_string(z)+")";
}