#ifndef __VECTOR3D__
#define __VECTOR3D__

class Point3d;

class string;

class Vector3d
{
public:
    double x;
    double y;
    double z;

    Vector3d& operator+=(Vector3d other);
    Vector3d& operator-=(Vector3d other);
    Vector3d& operator*=(double mult);


    Vector3d operator+(Vector3d other) const;
    Vector3d operator-(Vector3d other) const;
    Vector3d operator*(double mult) const;


    double dot(Vector3d other) const;
    void cross(Vector3d& a, Vector3d& b);

    double length() const;
    void normalize();

    double angle(Vector3d& other) const;
    void rotate(Vector3d other, double radians);

    operator Point3d() const;

    double isNAN() const;

    std::string toString() const;
};

#endif //__VECTOR3D__