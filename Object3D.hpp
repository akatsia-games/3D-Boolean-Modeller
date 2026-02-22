#ifndef __OBJECT3D__
#define __OBJECT3D__

#include<vector>
#include"Vertex.hpp"
#include"Face.hpp"
#include"Bound.hpp"

class Solid;
class Point3f;
class Face;
class Segment;
class Colour3f;

/**
 * Data structure about a 3d solid to apply bool operations in it.
 * 
 * <br><br>Tipically, two 'Object3d' objects are created to apply bool operation. The
 * methods splitFaces() and classifyFaces() are called in this sequence for both objects,
 * always using the other one as parameter. Then the faces from both objects are collected
 * according their status.
 * 
 * <br><br>See: 
 * D. H. Laidlaw, W. B. Trumbore, and J. F. Hughes.  
 * "Constructive Solid Geometry for Polyhedral Objects" 
 * SIGGRAPH Proceedings, 1986, p.161. 
 *  
 * @author Danilo Balby Silva Castanheira (danbalby@yahoo.com)
 * Translated to C++ by akatsia-games on github.com
 */
class Object3D
{
	friend Face;
public:

	Object3D(const Solid& solid);

	Object3D(const Object3D& other);

	int getNumFaces() const;

	const Face& getFace(int index) const;
	Face& getFace(int index);

	const Bound& getBound()const;

	void splitFaces(const Object3D& object);

	void classifyFaces(Object3D& object);
	
	void invertInsideFaces();

private:

	double getTotalArea() const;

	int addFace(int v1, int v2, int v3, int testedUntil = 0);

	int addVertex(Point3f pos, Colour3f color, int status);

	double computeDistance(const Vertex& vertex, const Face& face)const;

	void splitFace(int facePos, Segment& segment1, Segment& segment2, int testedUntil);

	void breakFaceInTwo(int facePos, Point3f newPos, int splitEdge, int testedUntil);

	void breakFaceInTwo(int facePos, Point3f newPos, Vertex endVertex, int testedUntil);

	void breakFaceInThree(int facePos, Point3f newPos1, Point3f newPos2, int splitEdge, int testedUntil);

	void breakFaceInThree(int facePos, Point3f newPos, Vertex endVertex, int testedUntil);

	void breakFaceInThree(int facePos, Point3f newPos1, Point3f newPos2, Vertex startVertex, Vertex endVertex, int testedUntil);

	void breakFaceInThree(int facePos, Point3f newPos, int testedUntil);

	void breakFaceInFour(int facePos, Point3f newPos1, Point3f newPos2, Vertex endVertex, int testedUntil);

	void breakFaceInFive(int facePos, Point3f newPos1, Point3f newPos2, int linedVertex, int testedUntil);

#ifdef _DEBUG
	void checkSplit(Face& original, int count);
#endif

	/** solid vertices  */
	std::vector<Vertex> vertices;
	/** solid faces */
	std::vector<Face> faces;
	/** object representing the solid extremes */
	Bound bound;

	
	static std::vector<Vertex> emptyVertices;
	static Face nullFace;
	
	/** tolerance value to test equalities */
	constexpr static const double TOL = 1e-10f;
};
#endif //__OBJECT3D__