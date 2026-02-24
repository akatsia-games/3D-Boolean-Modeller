#include "Point3f.hpp"

Vector3f::operator Point3f() const
{
    return {x,y,z};
}