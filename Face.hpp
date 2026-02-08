#ifndef __FACE__
#define __FACE__

#include"Vertex.hpp"
#include"Point3f.hpp"
#include"Bound.hpp"
#include<string>

class Object3D;
class Segment;

/**
 * Representation of a 3D face (triangle).
 *
 * <br><br>See: 
 * D. H. Laidlaw, W. B. Trumbore, and J. F. Hughes.  
 * "Constructive Solid Geometry for Polyhedral Objects" 
 * SIGGRAPH Proceedings, 1986, p.161. 
 * 
 * @author Danilo Balby Silva Castanheira (danbalby@yahoo.com)
 * Translated to C++ by akatsia-games on github.com
 */
class Face
{
	friend Segment;
public:
	/** face status if it is still unknown */
	static const int UNKNOWN = 1;
	/** face status if it is inside a solid */
	static const int INSIDE = 2;
	/** face status if it is outside a solid */
	static const int OUTSIDE = 3;
	/** face status if it is coincident with a solid face */
	static const int SAME = 4;
	/** face status if it is coincident with a solid face with opposite orientation*/
	static const int OPPOSITE = 5;

	static const int INVALID = 6;

	Face& operator=(const Face& other);
	
	/** first vertex */
	Vertex& v1();
	const Vertex& v1()const;
	/** second vertex */
	Vertex& v2();
	const Vertex& v2()const;
	/** third vertex */
	Vertex& v3();
	const Vertex& v3()const;
	
	int v[3];
	
	Face(std::vector<Vertex>& solidVertices);

	Face(std::vector<Vertex>& solidVertices, int v1, int v2, int v3);
	
	Face(const Face& other);
	
	std::string toString() const;
	
	bool equals(const Face& other) const;

	bool operator!=(const Face& other)const;
	
	Bound getBound() const;
	
	Vector3f getNormal() const;
	
	int getStatus() const;
	
	float getArea() const;
	
	void invert();
		
	//------------------------------------CLASSIFIERS-------------------------------//
	
	bool simpleClassify();
	
	void rayTraceClassify(Object3D& object);
	
private:
	bool hasPoint(Point3f& point);

	static int linePositionInX(Point3f point, Point3f pointLine1, Point3f pointLine2);

	static int linePositionInY(Point3f point, Point3f pointLine1, Point3f pointLine2);

	static int linePositionInZ(Point3f point, Point3f pointLine1, Point3f pointLine2);

	const Vertex& getVertex(int id)const;

	std::vector<Vertex>& solidVertices;
	

	static std::vector<Vertex> emptyVertexVector;

	/** face status relative to a solid  */
	int status;
	
	/** point status if it is up relative to an edge - see linePositionIn_ methods */
	static const int UP = 6;
	/** point status if it is down relative to an edge - see linePositionIn_ methods */
	static const int DOWN = 7;
	/** point status if it is on an edge - see linePositionIn_ methods */
	static const int ON = 8;
	/** point status if it isn't up, down or on relative to an edge - see linePositionIn_ methods */
	static const int NONE = 9;
	
	/** tolerance value to test equalities */
	constexpr static const double TOL = 1e-10;
};
#endif //__FACE__