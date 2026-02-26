#include "Bound.hpp"

/**
 * Representation of a bound - the extremes of a 3d component for each coordinate.
 *
 * <br><br>See: 
 * D. H. Laidlaw, W. B. Trumbore, and J. F. Hughes.  
 * "Constructive Solid Geometry for Polyhedral Objects" 
 * SIGGRAPH Proceedings, 1986, p.161. 
 *  
 * @author Danilo Balby Silva Castanheira (danbalby@yahoo.com)
 * Translated to C++ by akatsia-games on github.com 
 */

//---------------------------------CONSTRUCTORS---------------------------------//

/**
 * Bound NAN constructor
 * 
 */	
Bound::Bound()
{
	xMax = xMin = NAN;
	yMax = yMin = NAN;
	zMax = zMin = NAN;
}

/**
 * Bound constructor for a face
 * 
 * @param p1 point relative to the first vertex
 * @param p2 point relative to the second vertex
 * @param p3 point relative to the third vertex
 */	
Bound::Bound(const Point3d& p1, const Point3d& p2, const Point3d& p3)
{
	xMax = xMin = p1.x;
	yMax = yMin = p1.y;
	zMax = zMin = p1.z;
	
	checkVertex(p2);
	checkVertex(p3);
}

/**
 * Bound constructor for a object 3d
 * 
 * @param vertices the object vertices
 */
Bound::Bound(const std::vector<Point3d>& vertices)
{
	if(vertices.empty()){
		xMax = xMin = NAN;
		yMax = yMin = NAN;
		zMax = zMin = NAN;
		return;
	}
	xMax = xMin = vertices[0].x;
	yMax = yMin = vertices[0].y;
	zMax = zMin = vertices[0].z;
	
	for(const Point3d& vertex: vertices)
	{
		checkVertex(vertex);
	}
}

/**
 * Bound constructor for a object 3d
 * 
 * @param vertices the object vertices
 */
Bound::Bound(const Bound& other)
{
	xMax = other.xMax;
	xMin = other.xMin;

	yMax = other.yMax;
	yMin = other.yMin;

	zMax = other.zMax;
	zMin = other.zMin;
}

//----------------------------------OVERRIDES-----------------------------------//

/**
 * Makes a string definition for the bound object
 * 
 * @return the string definition
 */
std::string Bound::toString() const
{
	return "x: "+std::to_string(xMin)+" .. "+std::to_string(xMax)
		+"\ny: "+std::to_string(yMin)+" .. "+std::to_string(yMax)
		+"\nz: "+std::to_string(zMin)+" .. "+std::to_string(zMax);
}

Bound Bound::translate(double x, double y, double z){
	xMax = xMax+x;
	xMin = xMin+x;


	yMax = yMax+y;
	yMin = yMin+y;


	zMax = zMax+z;
	zMin = zMin+z;

	return *this;
}

//--------------------------------------OTHERS----------------------------------//

/**
 * Checks if a bound overlaps other one
 * 
 * @param bound other bound to make the comparison
 * @return true if they insersect, false otherwise
 */
bool Bound::overlap(const Bound& bound) const
{
	if(std::isnan(xMin)||std::isnan(xMax)||std::isnan(yMin)||std::isnan(yMax)||std::isnan(zMin)||std::isnan(zMax)){
		return false;
	}
	if((xMin>bound.xMax+TOL)||(xMax<bound.xMin-TOL)||(yMin>bound.yMax+TOL)||(yMax<bound.yMin-TOL)||(zMin>bound.zMax+TOL)||(zMax<bound.zMin-TOL))
	{
		return false;
	}
	else
	{
		return true;
	}
}

//-------------------------------------PRIVATES---------------------------------//

/**
 * Checks if one of the coordinates of a vertex exceed the ones found before 
 * 
 * @param vertex vertex to be tested
 */
void Bound::checkVertex(const Point3d& vertex)
{
	if(vertex.x>xMax)
	{
		xMax = vertex.x;
	}
	else if(vertex.x<xMin)
	{
		xMin = vertex.x;
	}
	
	if(vertex.y>yMax)
	{
		yMax = vertex.y;
	}
	else if(vertex.y<yMin)
	{
		yMin = vertex.y;
	}
	
	if(vertex.z>zMax)
	{
		zMax = vertex.z;
	}
	else if(vertex.z<zMin)
	{
		zMin = vertex.z;
	}
}
