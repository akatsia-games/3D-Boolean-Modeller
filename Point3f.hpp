#ifndef __POINT3F__
#define __POINT3F__
#include<cmath>
#include<string>

/**
 * Classes to replace Java3D-specific classes.
 * 
 * @author akatsia-games on github.com
 */

class Vector3f
{
public:
    float x;
    float y;
    float z;

    Vector3f& operator+=(Vector3f other){
        x+=other.x;
        y+=other.y;
        z+=other.z;
        return *this;
    }

    Vector3f& operator-=(Vector3f other){
        x-=other.x;
        y-=other.y;
        z-=other.z;
        return *this;
    }

    Vector3f& operator*=(double mult){
        x*=mult;
        y*=mult;
        z*=mult;
        return *this;
    }

    Vector3f operator+(Vector3f other){
        other+= (*this);
        return other;
    }

    Vector3f operator-(Vector3f other){
        other-= (*this);
        return other;
    }

    Vector3f operator*(double mult){
        Vector3f cpy = *this;
        cpy*=mult;
        return cpy;
    }


    double isNAN() const{
        return std::isnan(x)||std::isnan(y)||std::isnan(z);
    }

    double dot(Vector3f other) const{
        return ((double)x*other.x)+((double)y*other.y)+((double)z*other.z);
    }

    double length() const{
        double lenSqr = dot(*this);
        return sqrt(lenSqr);
    }

    void normalize(){
        double len = length();
        x/=len;
        y/=len;
        z/=len;
    }

    void cross(Vector3f& a, Vector3f& b){
        *this = {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    float angle(Vector3f& other) const{
        double this_len = length();
        double other_len = other.length();
        double dot_pr = this->dot(other);
        double cos_res = dot_pr / (this_len*other_len);
        return acos(cos_res);
    }

    void rotate(Vector3f other, double radians){
        other.normalize();
        double other_len = dot(other);

        //remove the component of "this" that is "other"
        (*this) -= other*other_len;

        //save current length and normalize
        double this_len = length();
        normalize();

        //generate a normalized cross product to the "right" of "this" as viewed from "other"
        Vector3f sin_vec;
        sin_vec.cross(*this,other);

        //rotate the current vector and then resize it accordingly and add back the "other" component
        *this = (*this)*cos(radians) + sin_vec*sin(radians);
        *this = (*this) * this_len + other*other_len;
    }

    std::string toString() const{
        return "(Vector3f x:"+std::to_string(x)+", y:"+std::to_string(y)+", z:"+std::to_string(z)+")";
    }
};

class Point3f
{
public:
    float x;
    float y;
    float z;

    operator Vector3f() const{
        return {x,y,z};
    }

    double isNAN() const{
        return std::isnan(x)||std::isnan(y)||std::isnan(z);
    }

    double distance(Point3f other) const{
        Vector3f distanceVect = {x-other.x, y-other.y, z-other.z};
        return distanceVect.length();
    }

    std::string toString() const{
        return "(Point3f x:"+std::to_string(x)+", y:"+std::to_string(y)+", z:"+std::to_string(z)+")";
    }
};

class Colour3f
{
public:
    float r;
    float g;
    float b;

    bool equals(const Colour3f& other)const{
        return (std::abs(r-other.r)+std::abs(g-other.g)+std::abs(b-other.b)) < TOL;
    }

private:
	/** tolerance value to test equalities */
	constexpr static const double TOL = 3e-5f;
};


#endif //__POINT3F__