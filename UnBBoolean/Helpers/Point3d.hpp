#ifndef __POINT3F__
#define __POINT3F__

#include<string>

class Vector3d;

/**
 * Classes to replace Java3D-specific classes.
 * 
 * @author akatsia-games on github.com
 */
class Point3d;

class Point3d
{
public:
    double x;
    double y;
    double z;

    Point3d(double x, double y, double z);
    Point3d(int x, int y, int z);
    Point3d();

    double distance(Point3d other) const;

    bool isNAN() const;

    operator Vector3d() const;

    std::string toString() const;
};


#endif //__POINT3F__