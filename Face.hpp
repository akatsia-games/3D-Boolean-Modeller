#ifndef __FACE__
#define __FACE__

#include"Vertex.hpp"
#include"Point3f.hpp"
#include"Bound.hpp"
#include<string>

class Object3D;

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

	/** first vertex */
	Vertex v1;
	/** second vertex */
	Vertex v2;
	/** third vertex */
	Vertex v3;
	
	Face();

	Face(Vertex& v1, Vertex& v2, Vertex& v3);
	
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