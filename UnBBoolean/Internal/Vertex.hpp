#ifndef __VERTEX__
#define __VERTEX__

#include<string>
#include<vector>
#include"UnBBoolean/Helpers/Point3d.hpp"
#include"UnBBoolean/Helpers/Colour3f.hpp"

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
	Vertex(std::vector<Vertex>& solidVertices, Point3d position, Colour3f color);
	
	Vertex(std::vector<Vertex>& solidVertices, double x, double y, double z, Colour3f color);
	
	Vertex(std::vector<Vertex>& solidVertices, Point3d position, Colour3f color, int status);
	
	Vertex(std::vector<Vertex>& solidVertices, double x, double y, double z, Colour3f color, int status);

	Vertex(const Vertex& other);

	Vertex& operator=(const Vertex& other) = delete;
	const Vertex& operator=(const Vertex& other)const = delete;

	Vertex();

	std::string toString() const;

	bool equals(const Vertex& other) const;

	bool operator!=(const Vertex& other) const;

	void setStatus(int status);

	Point3d getPosition() const;

	const std::vector<int>& getAdjacentVertices() const;

	int getStatus() const;

	Colour3f getColor() const;

	void addAdjacentVertex(int index);

	void mark(int status);

	/** vertex coordinate in X */
	double x;
	/** vertex coordinate in Y */
	double y;
	/** vertex coordinate in Z */
	double z;

	int id() const;
	
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
	std::vector<Vertex>& solidVertices;
	std::vector<int> adjacentVertexIndexes;

	//std::vector<Vertex*> adjacentVertices;
	/** vertex status relative to other object */
	int status;
	/** vertex color */
	Colour3f color;

	static std::vector<Vertex> emptyVertexVector;
	
	/** tolerance value to test equalities */
	constexpr static const double TOL = 1e-5;
};

#endif //__VERTEX__