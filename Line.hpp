#ifndef __LINE_HPP__
#define __LINE_HPP__

#include"Point3f.hpp"
#include"Face.hpp"
#include<string>

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
class Line
{	
public:
	Line(const Face& face1, const Face& face2);
	
	Line(Vector3f direction, Point3f point);
	
	Line(const Line& other);

	std::string toString() const;

	Point3f getPoint() const;

	Vector3f getDirection() const;

	void setPoint(Point3f point);

	void setDirection(Vector3f direction);

	double computePointToPointDistance(const Point3f& otherPoint) const;

	Point3f computeLineIntersection(const Line& otherLine) const;

	Point3f computePlaneIntersection(const Vector3f& normal, const Point3f& planePoint) const;

	void perturbDirection();
	
private:
	/** a line point */
	Point3f point;
	/** line direction */
	Vector3f direction;
	
	/** tolerance value to test equalities */
	constexpr static const double TOL = 1e-10;
};
#endif //__LINE_HPP__