#ifndef __BOUND__
#define __BOUND__

#include<vector>
#include"Point3d.hpp"
#include<string>

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
class Bound
{
public:
	//---------------------------------CONSTRUCTORS---------------------------------//
	
	Bound();

	/**
	 * Bound constructor for a face
	 * 
	 * @param p1 point relative to the first vertex
	 * @param p2 point relative to the second vertex
	 * @param p3 point relative to the third vertex
	 */	
	Bound(const Point3d& p1, const Point3d& p2, const Point3d& p3);
	
	/**
	 * Bound constructor for a object 3d
	 * 
	 * @param vertices the object vertices
	 */
	Bound(const std::vector<Point3d>& vertices);
	
	/**
	 * Bound constructor for a object 3d
	 * 
	 * @param vertices the object vertices
	 */
	Bound(const Bound& other);
	
	//----------------------------------OVERRIDES-----------------------------------//
	
	/**
	 * Makes a string definition for the bound object
	 * 
	 * @return the string definition
	 */
	std::string toString() const;

	Bound translate(double x, double y, double z);
	
	//--------------------------------------OTHERS----------------------------------//
	
	/**
	 * Checks if a bound overlaps other one
	 * 
	 * @param bound other bound to make the comparison
	 * @return true if they insersect, false otherwise
	 */
	bool overlap(const Bound& bound) const;
	
	//-------------------------------------PRIVATES---------------------------------//
	

	
private:
	
	/**
	 * Checks if one of the coordinates of a vertex exceed the ones found before 
	 * 
	 * @param vertex vertex to be tested
	 */
	void checkVertex(const Point3d& vertex);

	/** maximum from the x coordinate */
	double xMax;
	/** minimum from the x coordinate */
	double xMin;
	/** maximum from the y coordinate */
	double yMax;
	/** minimum from the y coordinate */
	double yMin;
	/** maximum from the z coordinate */
	double zMax;
	/** minimum from the z coordinate */
	double zMin;
	
	/** tolerance value to test equalities */
	constexpr static const double TOL = 1e-10f;
};
#endif //__BOUND__