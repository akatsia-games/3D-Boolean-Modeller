#ifndef __LINE_HPP__
#define __LINE_HPP__

#include"Point3d.hpp"
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
	
	Line(Vector3d direction, Point3d point);
	
	Line(const Line& other);
	
	Line();

	bool isNAN() const;

	std::string toString() const;

	Point3d getPoint() const;

	Vector3d getDirection() const;

	void setPoint(Point3d point);

	void setDirection(Vector3d direction);

	double computePointToPointDistance(const Point3d& otherPoint) const;

	Point3d computeLineIntersection(const Line& otherLine) const;

	Point3d computePlaneIntersection(const Vector3d& normal, const Point3d& planePoint) const;

	void perturbDirection();
	
private:
	/** a line point */
	Point3d point;
	/** line direction */
	Vector3d direction;
	
	/** tolerance value to test equalities */
	constexpr static const double TOL = 1e-10;
};
#endif //__LINE_HPP__