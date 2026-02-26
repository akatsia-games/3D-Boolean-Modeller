#include "Point3d.hpp"
#include "Vector3d.hpp"
#include <cmath>
#include <string>

Point3d::Point3d(double x, double y, double z):x(x),y(y),z(z){}
Point3d::Point3d(int x, int y, int z):x((double)x),y((double)y),z((double)z){}
Point3d::Point3d():x(0),y(0),z(0){}

Point3d::operator Vector3d() const{
    return {x,y,z};
}

bool Point3d::isNAN() const{
    return std::isnan(x)||std::isnan(y)||std::isnan(z);
}

double Point3d::distance(Point3d other) const{
    Vector3d distanceVect = {x-other.x, y-other.y, z-other.z};
    return distanceVect.length();
}

std::string Point3d::toString() const{
    return "(Point3d x:"+std::to_string(x)+", y:"+std::to_string(y)+", z:"+std::to_string(z)+")";
}