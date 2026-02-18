#include"Line.hpp"
#include<math.h>

/**
 * Representation of a 3d line or a ray (represented by a direction and a point).
 * 
 * <br><br>See: 
 * D. H. Laidlaw, W. B. Trumbore, and J. F. Hughes.  
 * "Constructive Solid Geometry for Polyhedral Objects" 
 * SIGGRAPH Proceedings, 1986, p.161.
 *  
 * @author Danilo Balby Silva Castanheira (danbalby@yahoo.com)
 * Translated to C++ by akatsia-games on github.com
 */


//----------------------------------CONSTRUCTORS---------------------------------//

/**
 * Constructor for a line. The line created is the intersection between two planes 
 * 
 * @param face1 face representing one of the planes 
 * @param face2 face representing one of the planes
 */
Line::Line(const Face& face1, const Face& face2)
{
	Vector3f normalFace1 = face1.getNormal();
	Vector3f normalFace2 = face2.getNormal();
	
	//direction: cross product of the faces normals
	direction = {}; 
	direction.cross(normalFace1, normalFace2);
			
	//if direction lenght is not zero (the planes aren't parallel )...
	if (!(direction.length()<TOL))
	{
		//getting a line point, zero is set to a coordinate whose direction 
		//component isn't zero (line intersecting its origin plan)
		point = {};
		double d1 = -(normalFace1.x*face1.v1().x + normalFace1.y*face1.v1().y + normalFace1.z*face1.v1().z);
		double d2 = -(normalFace2.x*face2.v1().x + normalFace2.y*face2.v1().y + normalFace2.z*face2.v1().z);
		if(std::abs(direction.x)>TOL)
		{
			point.x = 0;
			point.y = (d2*normalFace1.z - d1*normalFace2.z)/direction.x;
			point.z = (d1*normalFace2.y - d2*normalFace1.y)/direction.x;
		}
		else if(std::abs(direction.y)>TOL)
		{
			point.x = (d1*normalFace2.z - d2*normalFace1.z)/direction.y;
			point.y = 0;
			point.z = (d2*normalFace1.x - d1*normalFace2.x)/direction.y;
		}
		else
		{
			point.x = (d2*normalFace1.y - d1*normalFace2.y)/direction.z;
			point.y = (d1*normalFace2.x - d2*normalFace1.x)/direction.z;
			point.z = 0;
		}
	}
			
	direction.normalize();
}

/**
 * Constructor for a ray
 * 
 * @param direction direction ray
 * @param point beginning of the ray
 */
Line::Line(Vector3f direction, Point3f point)
{
	this->direction = direction;
	this->point = point;
	direction.normalize();
}

//---------------------------------OVERRIDES------------------------------------//

/**
 * Clones the Line object
 * 
 * @return cloned Line object
 */
Line::Line(const Line& other)
{
	direction = other.direction;
	point = other.point;
}


	
Line::Line()
	:point({NAN,NAN,NAN})
	,direction({NAN,NAN,NAN})
{

}

bool Line::isNAN() const{
	return point.isNAN()||direction.isNAN();
}

/**
 * Makes a string definition for the Line object
 * 
 * @return the string definition
 */
std::string Line::toString() const
{
	return "Direction: "+direction.toString()+"\nPoint: "+point.toString();
}

//-----------------------------------GETS---------------------------------------//

/**
 * Gets the point used to represent the line
 * 
 * @return point used to represent the line
 */
Point3f Line::getPoint() const
{
	return point;
}

/**
 * Gets the line direction
 * 
 * @return line direction
 */
Vector3f Line::getDirection() const
{
	return direction;
}

//-----------------------------------SETS---------------------------------------//

/**
 * Sets a new point
 * 
 * @param point new point
 */
void Line::setPoint(Point3f point)
{
	this->point = point;
}

/**
 * Sets a new direction
 * 
 * @param direction new direction
 */
void Line::setDirection(Vector3f direction)
{
	this->direction = direction;
}
	
//--------------------------------OTHERS----------------------------------------//
	
/**
 * Computes the distance from the line point to another point
 * 
 * @param otherPoint the point to compute the distance from the line point. The point 
 * is supposed to be on the same line.
 * @return points distance. If the point submitted is behind the direction, the 
 * distance is negative 
 */
double Line::computePointToPointDistance(const Point3f& otherPoint) const
{
	Vector3f vec = {otherPoint.x - point.x, otherPoint.y - point.y, otherPoint.z - point.z};
	double distance = vec.length();
	vec.normalize();
	if (vec.dot(direction)<0)
	{
		return -distance;
	}
	else
	{
		return distance;
	}
}

/**
 * Computes the point resulting from the intersection with another line
 * 
 * @param otherLine the other line to apply the intersection. The lines are supposed
 * to intersect
 * @return point resulting from the intersection. If the point coundn't be obtained, return null
 */
Point3f Line::computeLineIntersection(const Line& otherLine) const
{
	//x = x1 + a1*t = x2 + b1*s
	//y = y1 + a2*t = y2 + b2*s
	//z = z1 + a3*t = z2 + b3*s
	
	Point3f linePoint = otherLine.getPoint(); 
	Vector3f lineDirection = otherLine.getDirection();
			
	double t;
	if(std::abs(direction.y*lineDirection.x-direction.x*lineDirection.y)>TOL)
	{
		t = (-point.y*lineDirection.x+linePoint.y*lineDirection.x+lineDirection.y*point.x-lineDirection.y*linePoint.x)/(direction.y*lineDirection.x-direction.x*lineDirection.y);
	}
	else if (std::abs(-direction.x*lineDirection.z+direction.z*lineDirection.x)>TOL)
	{
		t=-(-lineDirection.z*point.x+lineDirection.z*linePoint.x+lineDirection.x*point.z-lineDirection.x*linePoint.z)/(-direction.x*lineDirection.z+direction.z*lineDirection.x);
	}
	else if (std::abs(-direction.z*lineDirection.y+direction.y*lineDirection.z)>TOL)
	{
		t = (point.z*lineDirection.y-linePoint.z*lineDirection.y-lineDirection.z*point.y+lineDirection.z*linePoint.y)/(-direction.z*lineDirection.y+direction.y*lineDirection.z);
	}
	else return {NAN,NAN,NAN};
	
	double x = point.x + direction.x*t;
	double y = point.y + direction.y*t;
	double z = point.z + direction.z*t;
			
	return {(float)x,(float)y,(float)z};
}

/**
 * Compute the point resulting from the intersection with a plane
 * 
 * @param normal the plane normal
 * @param planePoint a plane point. 
 * @return intersection point. If they don't intersect, return null
 */
Point3f Line::computePlaneIntersection(const Vector3f& normal, const Point3f& planePoint) const
{
	//Ax + By + Cz + D = 0
	//x = x0 + t(x1 � x0)
	//y = y0 + t(y1 � y0)
	//z = z0 + t(z1 � z0)
	//(x1 - x0) = dx, (y1 - y0) = dy, (z1 - z0) = dz
	//t = -(A*x0 + B*y0 + C*z0 )/(A*dx + B*dy + C*dz)
	
	double A = normal.x;
	double B = normal.y;
	double C = normal.z;
	double D = -(normal.x*planePoint.x+normal.y*planePoint.y+normal.z*planePoint.z);
		
	double numerator = A*point.x + B*point.y + C*point.z + D;
	double denominator = A*direction.x + B*direction.y + C*direction.z;
			
	//if line is paralel to the plane...
	if(std::abs(denominator)<TOL)
	{
		//if line is contained in the plane...
		if(std::abs(numerator)<TOL)
		{
			return point;
		}
		else
		{
			return {NAN,NAN,NAN};
		}
	}
	//if line intercepts the plane...
	else
	{
		double t = -numerator/denominator;
		Point3f resultPoint;
		resultPoint.x = point.x + t*direction.x; 
		resultPoint.y = point.y + t*direction.y;
		resultPoint.z = point.z + t*direction.z;
		
		return resultPoint;
	}
}

/** Changes slightly the line direction */
void Line::perturbDirection()
{
	direction.x += 1e-5*random();			
	direction.y += 1e-5*random();
	direction.z += 1e-5*random();
}