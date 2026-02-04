#ifndef __SEGMENT__
#define __SEGMENT__

#include"Vertex.hpp"
#include"Line.hpp"

class Face;
class Line;

/**
 * Represents a line segment resulting from a intersection of a face and a plane.
 * 
 * <br><br>See: 
 * D. H. Laidlaw, W. B. Trumbore, and J. F. Hughes.  
 * "Constructive Solid Geometry for Polyhedral Objects" 
 * SIGGRAPH Proceedings, 1986, p.161. 
 * 
 * @author Danilo Balby Silva Castanheira (danbalby@yahoo.com)
 * Translated to C++ by akatsia-games on github.com
 */
class Segment
{
public:
	/** define as vertex one of the segment ends */
	constexpr static const int VERTEX = 1;
	/** define as face one of the segment ends */
	constexpr static const int FACE = 2;
	/** define as edge one of the segment ends */
	constexpr static const int EDGE = 3;
	
	//---------------------------------CONSTRUCTORS---------------------------------//
		
	Segment(const Line& line, const Face& face, int sign1, int sign2, int sign3);
	
	Segment(const Segment& other);
	
	//-------------------------------------GETS-------------------------------------//
	Vertex getStartVertex();

	Vertex getEndVertex();

	double getStartDistance();

	double getEndDistance();

	int getStartType();

	int getIntermediateType();

	int getEndType();

	int getNumEndsSet();

	Point3f getStartPosition();

	Point3f getEndPosition();

	bool intersect(Segment& segment);
	
	//---------------------------------PRIVATES-------------------------------------//
private:
	bool setVertex(const Vertex& vertex);
	
	bool setEdge(const Vertex& vertex1, const Vertex& vertex2);
	
	void swapEnds();
	
	/** line resulting from the two planes intersection */
	Line line;
	/** shows how many ends were already defined */
	int index;
	
	/** distance from the segment starting point to the point defining the plane */
	double startDist;
	/** distance from the segment ending point to the point defining the plane */
	double endDist;
	
	/** starting point status relative to the face */
	int startType;
	/** intermediate status relative to the face */
	int middleType;
	/** ending point status relative to the face */
	int endType;
	
	/** nearest vertex from the starting point */
	Vertex startVertex;
	/** nearest vertex from the ending point */
	Vertex endVertex; 
	
	/** start of the intersection point */
	Point3f startPos;
	/** end of the intersection point */
	Point3f endPos;
	
	/** tolerance value to test equalities */
	constexpr static const double TOL = 1e-10f;
};
#endif //__SEGMENT__