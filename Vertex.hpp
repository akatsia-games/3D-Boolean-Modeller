#ifndef __VERTEX__
#define __VERTEX__

#include<string>
#include<vector>
#include"Point3f.hpp"

/** 
 * Represents of a 3d face vertex.
 * 
 * <br><br>See: 
 * D. H. Laidlaw, W. B. Trumbore, and J. F. Hughes.  
 * "Constructive Solid Geometry for Polyhedral Objects" 
 * SIGGRAPH Proceedings, 1986, p.161. 
 * 
 * @author Danilo Balby Silva Castanheira (danbalby@yahoo.com)
 * Translated to C++ by akatsia-games on github.com
 */
class Vertex
{
public:
	Vertex(Point3f position, Colour3f color);
	
	Vertex(double x, double y, double z, Colour3f color);
	
	Vertex(Point3f position, Colour3f color, int status);
	
	Vertex(double x, double y, double z, Colour3f color, int status);

	Vertex(const Vertex& other);

	Vertex();

	std::string toString() const;

	bool equals(const Vertex& other) const;

	bool operator!=(const Vertex& other) const;

	void setStatus(int status);

	Point3f getPosition() const;

	const std::vector<Vertex*>& getAdjacentVertices() const;

	int getStatus() const;

	Colour3f getColor() const;

	void addAdjacentVertex(Vertex* adjacentVertex);

	void mark(int status);

	/** vertex coordinate in X */
	double x;
	/** vertex coordinate in Y */
	double y;
	/** vertex coordinate in Z */
	double z;
	
	/** vertex status if it is still unknown */
	static const int UNKNOWN = 1;
	/** vertex status if it is inside a solid */
	static const int INSIDE = 2;
	/** vertex status if it is outside a solid */
	static const int OUTSIDE = 3;
	/** vertex status if it on the boundary of a solid */
	static const int BOUNDARY = 4;
	/** vertex status if it on the boundary of a solid */
	static const int INVALID = 5;

private:
	/** references to vertices conected to it by an edge  */
	std::vector<Vertex*> adjacentVertices;
	/** vertex status relative to other object */
	int status;
	/** vertex color */
	Colour3f color;
	
	/** tolerance value to test equalities */
	constexpr static const double TOL = 1e-5f;
};

#endif //__VERTEX__