//#define _DEBUG
//#define _PREVENT_INFINITE

#include"Object3D.hpp"

#include"Solid.hpp"
#include"Point3d.hpp"
#include"Face.hpp"
#include"Segment.hpp"


#ifndef _DEBUG
#define checkSplit(x,y)
#endif


std::vector<Vertex> Object3D::emptyVertices;
Face Object3D::nullFace(Object3D::emptyVertices);

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

/** 
 * Constructs a Object3d object based on a solid file.
 *
 * @param solid solid used to construct the Object3d object  
 */	
Object3D::Object3D(const Solid& solid)
	:bound(solid.getVertices())
{
	const std::vector<Point3d>& verticesPoints = solid.getVertices();
	const std::vector<int>& indices = solid.getIndices();
	const std::vector<Colour3f>& colors = solid.getColors();
	std::vector<int> indexOfSolidVertices;
	
	//create vertices
	vertices.reserve(verticesPoints.size());
	for(int i=0;i<verticesPoints.size();i++)
	{
		int idx = 0;
		idx = addVertex(verticesPoints[i], colors[i], Vertex::UNKNOWN);
		indexOfSolidVertices.push_back(idx); 
	}
	
	//create faces
	faces.reserve(indices.size()/3); //indices.size / 3 rounded up
	for(int i=0; i<indices.size(); i=i+3)
	{
		int v1 = indexOfSolidVertices[indices[i]];
		int v2 = indexOfSolidVertices[indices[i+1]];
		int v3 = indexOfSolidVertices[indices[i+2]];
		addFace(v1, v2, v3);
	}
}

//-----------------------------------OVERRIDES----------------------------------//

/**
 * Clones the Object3D object
 * 
 * @return cloned Object3D object
 */
Object3D::Object3D(const Object3D& other)
	:vertices(other.vertices)
	,faces(other.faces)
	,bound(other.bound)
{
}

//--------------------------------------GETS------------------------------------//

/**
 * Gets the number of faces
 * 
 * @return number of faces
 */
int Object3D::getNumFaces() const
{
	return faces.size();
}

/**
 * Gets a face reference for a given position
 * 
 * @param index required face position
 * @return face reference , null if the position is invalid
 */
const Face& Object3D::getFace(int index) const
{
	if(index<0 || index>=faces.size())
	{
		return nullFace;
	}
	else
	{
		return faces[index];
	}
}

/**
 * Gets a face reference for a given position
 * 
 * @param index required face position
 * @return face reference , null if the position is invalid
 */
Face& Object3D::getFace(int index)
{
	if(index<0 || index>=faces.size())
	{
		return nullFace;
	}
	else
	{
		return faces[index];
	}
}

/**
 * Gets the solid bound
 * 
 * @return solid bound
 */
const Bound& Object3D::getBound()const
{
	return bound;
}

//------------------------------------ADDS----------------------------------------//
	
/**
 * Method used to add a face properly for internal methods
 * 
 * @param v1 a face vertex
 * @param v2 a face vertex
 * @param v3 a face vertex
 * @param emplace - the position the next face should occupy, or -1
 * 
 * @return -1 if the face was able to be placed. return "emplace" otherwise
 */
int Object3D::addFace(int v1, int v2, int v3, int testedUntil)
{
	if(!(vertices[v1].equals(vertices[v2])||vertices[v1].equals(vertices[v3])||vertices[v2].equals(vertices[v3])))
	{
		Face face(vertices, v1, v2, v3, testedUntil);
		
		#ifdef _PREVENT_INFINITE
		for(int i=0; i<faces.size(); i++)
		{
			if(face.equals(faces[i])) break;			
		}
		#endif

		if(face.getArea()>TOL)
		{
			faces.emplace_back(vertices, v1, v2, v3, testedUntil);
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 2;
	}
}

/**
 * Method used to add a vertex properly for internal methods
 * 
 * @param pos vertex position
 * @param color vertex color
 * @param status vertex status
 * @param index - return index
 * @return the vertex inserted (if a similar vertex already exists, this is returned)
 */
int Object3D::addVertex(Point3d pos, Colour3f color, int status)
{
	int i;
	//if already there is an equal vertex, it is not inserted
	Vertex vertex(vertices,pos, color, status);
	for(i=0;i<vertices.size();i++)
	{
		if(vertex.equals(vertices[i]) && vertices[i].getColor().equals(color)) break;			
	}

	if(i==vertices.size())
	{
		vertices.push_back(vertex);
	}
	else
	{
		vertices[i].setStatus(status);
	}
	return i;
}
	
//-------------------------FACES_SPLITTING_METHODS------------------------------//

double Object3D::getTotalArea() const
{
	double sum = 0;
	for(auto& face: this->faces)
	{
		sum += face.getArea();
	}
	return sum;
}

/**
 * Split faces so that none face is intercepted by a face of other object
 * 
 * @param object the other object 3d used to make the split 
 */
void Object3D::splitFaces(const Object3D& object)
{
	Line line;
	std::vector<Segment> segments;
	double distFace1Vert1, distFace1Vert2, distFace1Vert3, distFace2Vert1, distFace2Vert2, distFace2Vert3;
	int signFace1Vert1, signFace1Vert2, signFace1Vert3, signFace2Vert1, signFace2Vert2, signFace2Vert3;
	int numFacesBefore = getNumFaces();
	int numFacesStart = getNumFaces();
	int facesIgnored = 0;
	
	double totalArea = getTotalArea();

	//if the objects bounds overlap...								
	if(getBound().overlap(object.getBound()))
	{			
		//for each object1 face...
		for(int i=0;i<getNumFaces();i++)
		{
			//if object1 face bound and object2 bound overlap ...
			const Face face1 = getFace(i);
			
			if(face1.getBound().overlap(object.getBound()))
			{
				//for each object2 face...
				for(int j=face1.getStart();j<object.getNumFaces();j++)
				{
					//if object1 face bound and object2 face bound overlap...  
					const Face& face2 = object.getFace(j);
					if(face1.getBound().overlap(face2.getBound()))
					{
						//PART I - DO TWO POLIGONS INTERSECT?
						//POSSIBLE RESULTS: INTERSECT, NOT_INTERSECT, COPLANAR
						
						//distance from the face1 vertices to the face2 plane
						distFace1Vert1 = computeDistance(face1.v1(), face2);
						distFace1Vert2 = computeDistance(face1.v2(), face2);
						distFace1Vert3 = computeDistance(face1.v3(), face2);
						
						//distances signs from the face1 vertices to the face2 plane 
						signFace1Vert1 = (distFace1Vert1>TOL? 1 :(distFace1Vert1<-TOL? -1 : 0)); 
						signFace1Vert2 = (distFace1Vert2>TOL? 1 :(distFace1Vert2<-TOL? -1 : 0));
						signFace1Vert3 = (distFace1Vert3>TOL? 1 :(distFace1Vert3<-TOL? -1 : 0));
						
						//if all the signs are zero, the planes are coplanar
						//if all the signs are positive or negative, the planes do not intersect
						//if the signs are not equal...
						if (!(signFace1Vert1==signFace1Vert2 && signFace1Vert2==signFace1Vert3))
						{
							//distance from the face2 vertices to the face1 plane
							distFace2Vert1 = computeDistance(face2.v1(), face1);
							distFace2Vert2 = computeDistance(face2.v2(), face1);
							distFace2Vert3 = computeDistance(face2.v3(), face1);
							
							//distances signs from the face2 vertices to the face1 plane
							signFace2Vert1 = (distFace2Vert1>TOL? 1 :(distFace2Vert1<-TOL? -1 : 0)); 
							signFace2Vert2 = (distFace2Vert2>TOL? 1 :(distFace2Vert2<-TOL? -1 : 0));
							signFace2Vert3 = (distFace2Vert3>TOL? 1 :(distFace2Vert3<-TOL? -1 : 0));
						
							//if the signs are not equal...
							if (!(signFace2Vert1==signFace2Vert2 && signFace2Vert2==signFace2Vert3))
							{
								line = Line(face1, face2);
						
								//intersection of the face1 and the plane of face2
								Segment segment1(line, face1, signFace1Vert1, signFace1Vert2, signFace1Vert3);
																
								//intersection of the face2 and the plane of face1
								Segment segment2(line, face2, signFace2Vert1, signFace2Vert2, signFace2Vert3);
															
								//if the two segments intersect...
								if(segment1.intersect(segment2))
								{
									//PART II - SUBDIVIDING NON-COPLANAR POLYGONS
									int lastNumFaces = getNumFaces();
									this->splitFace(i, segment1, segment2, j+1);

									#ifdef _DEBUG
									int currNumFaces = getNumFaces();
									double currArea = getTotalArea();

									if(std::abs(currArea-totalArea) > TOL)
									{
										fprintf(stderr,"area changed from %f to %f and faces from %d to %d at step (%d,%d) by (%.3e)\n",totalArea, currArea,lastNumFaces, currNumFaces, i, j, std::abs(totalArea-currArea));
									}
									else
									{
										//fprintf(stderr,"face split from %d to %d but area stayed consistent at step (%d,%d)\n",lastNumFaces,currNumFaces,i,j);
									}
									totalArea = currArea;
									#endif
									
																		
									#ifdef _DEBUG
									#ifdef _PREVENT_INFINITE
									//prevent from infinite loop (with a loss of faces...)
									if(numFacesStart*20<getNumFaces())
									{
										fprintf(stderr,"possible infinite loop situation: terminating faces split\n");
										return;
									}
									#endif
									#endif
							
									//if the face in the position isn't the same, there was a break 
									if(i<faces.size() && !face1.equals(getFace(i))) 
									{
										i--;
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

/**
 * Computes closest distance from a vertex to a plane
 * 
 * @param vertex vertex used to compute the distance
 * @param face face representing the plane where it is contained
 * @return the closest distance from the vertex to the plane
 */
double Object3D::computeDistance(const Vertex& vertex, const Face& face) const
{
	Vector3d normal = face.getNormal();
	double a = normal.x;
	double b = normal.y;
	double c = normal.z;
	double d = -(a*face.v1().x + b*face.v1().y + c*face.v1().z);
	return a*vertex.x + b*vertex.y + c*vertex.z + d;
}

/**
 * Split an individual face
 * 
 * @param facePos face position on the array of faces
 * @param segment1 segment representing the intersection of the face with the plane
 * of another face
 * @return segment2 segment representing the intersection of other face with the
 * plane of the current face plane
 */	  
void Object3D::splitFace(int facePos, Segment& segment1, Segment& segment2, int testedUntil)
{
	Vertex startPosVertex, endPosVertex;
	Point3d startPos, endPos;
	int startType, endType, middleType;
	double startDist, endDist;
	
	Face face = (Face)getFace(facePos);
	Vertex& startVertex = segment1.getStartVertex();
	Vertex& endVertex = segment1.getEndVertex();
	
	//starting point: deeper starting point 		
	if (segment2.getStartDistance() > segment1.getStartDistance()+TOL)
	{
		startDist = segment2.getStartDistance();
		startType = segment1.getIntermediateType();
		startPos = segment2.getStartPosition();
		fprintf(stderr,"start_segment2");
	}
	else
	{
		startDist = segment1.getStartDistance();
		startType = segment1.getStartType();
		startPos = segment1.getStartPosition();
		fprintf(stderr,"start_segment1");
	}
	
	//ending point: deepest ending point
	if (segment2.getEndDistance() < segment1.getEndDistance()-TOL)
	{
		endDist = segment2.getEndDistance();
		endType = segment1.getIntermediateType();
		endPos = segment2.getEndPosition();
		fprintf(stderr,"end_segment2");
	}
	else
	{
		endDist = segment1.getEndDistance();
		endType = segment1.getEndType();
		endPos = segment1.getEndPosition();
		fprintf(stderr,"end_segment1");
	}		
	middleType = segment1.getIntermediateType();
	
	//set vertex to BOUNDARY if it is start type		
	if (startType == Segment::VERTEX)
	{
		startVertex.setStatus(Vertex::BOUNDARY);
	}
			
	//set vertex to BOUNDARY if it is end type
	if (endType == Segment::VERTEX)
	{
		endVertex.setStatus(Vertex::BOUNDARY);
	}
	
	//VERTEX-_______-VERTEX 
	if (startType == Segment::VERTEX && endType == Segment::VERTEX)
	{
		return;
	}
	
	//______-EDGE-______
	else if (middleType == Segment::EDGE)
	{
		//gets the edge 
		int splitEdge;
		if ((startVertex.equals(face.v1()) && endVertex.equals(face.v2())) || (startVertex.equals(face.v2()) && endVertex.equals(face.v1())))
		{
			splitEdge = 1;
		}
		else if ((startVertex.equals(face.v2()) && endVertex.equals(face.v3())) || (startVertex.equals(face.v3()) && endVertex.equals(face.v2())))
		{	  
			splitEdge = 2; 
		} 
		else
		{
			splitEdge = 3;
		} 
		
		//VERTEX-EDGE-EDGE
		if (startType == Segment::VERTEX)
		{
			breakFaceInTwo(facePos, endPos, splitEdge, testedUntil);
			return;
		}
		
		//EDGE-EDGE-VERTEX
		else if (endType == Segment::VERTEX)
		{
			breakFaceInTwo(facePos, startPos, splitEdge, testedUntil);
			return;
		}
	
		// EDGE-EDGE-EDGE
		else if (startDist == endDist)
		{
			breakFaceInTwo(facePos, endPos, splitEdge, testedUntil);
		}
		else
		{
			if((startVertex.equals(face.v1()) && endVertex.equals(face.v2())) || (startVertex.equals(face.v2()) && endVertex.equals(face.v3())) || (startVertex.equals(face.v3()) && endVertex.equals(face.v1())))
			{
				breakFaceInThree(facePos, startPos, endPos, splitEdge, testedUntil);
			}
			else
			{
				breakFaceInThree(facePos, endPos, startPos, splitEdge, testedUntil);
			}
		}
		return;
	}
	
	//______-FACE-______
	
	//VERTEX-FACE-EDGE
	else if (startType == Segment::VERTEX && endType == Segment::EDGE)
	{
		breakFaceInTwo(facePos, endPos, endVertex, testedUntil);
	}
	//EDGE-FACE-VERTEX
	else if (startType == Segment::EDGE && endType == Segment::VERTEX)
	{
		breakFaceInTwo(facePos, startPos, startVertex, testedUntil);
	}
	//VERTEX-FACE-FACE
	else if (startType == Segment::VERTEX && endType == Segment::FACE)
	{
		breakFaceInThree(facePos, endPos, startVertex, testedUntil);
	}
	//FACE-FACE-VERTEX
	else if (startType == Segment::FACE && endType == Segment::VERTEX)
	{
		breakFaceInThree(facePos, startPos, endVertex, testedUntil);
	}
	//EDGE-FACE-EDGE
	else if (startType == Segment::EDGE && endType == Segment::EDGE)
	{
		breakFaceInThree(facePos, startPos, endPos, startVertex, endVertex, testedUntil);
	}
	//EDGE-FACE-FACE
	else if (startType == Segment::EDGE && endType == Segment::FACE)
	{
		breakFaceInFour(facePos, startPos, endPos, startVertex, testedUntil);
	}
	//FACE-FACE-EDGE
	else if (startType == Segment::FACE && endType == Segment::EDGE)
	{
		breakFaceInFour(facePos, endPos, startPos, endVertex, testedUntil);
	}
	//FACE-FACE-FACE
	else if (startType == Segment::FACE && endType == Segment::FACE)
	{
		Vector3d segmentVector({startPos.x-endPos.x, startPos.y-endPos.y, startPos.z-endPos.z});
					
		//if the intersection segment is a point only...
		if (std::abs(segmentVector.x)<TOL && std::abs(segmentVector.y)<TOL && std::abs(segmentVector.z)<TOL)
		{
			breakFaceInThree(facePos, startPos, testedUntil);
			return;
		}
		
		//gets the vertex more lined with the intersection segment
		int linedVertex;
		Point3d linedVertexPos;
		Vector3d   vertexVector({(double)(endPos.x-face.v1().x), (double)(endPos.y-face.v1().y), (double)(endPos.z-face.v1().z)});
		vertexVector.normalize();
		double dot1 = std::abs(segmentVector.dot(vertexVector));
		vertexVector = Vector3d({(double)(endPos.x-face.v2().x), (double)(endPos.y-face.v2().y), (double)(endPos.z-face.v2().z)});
		vertexVector.normalize();
		double dot2 = std::abs(segmentVector.dot(vertexVector));
		vertexVector = Vector3d({(double)(endPos.x-face.v3().x), (double)(endPos.y-face.v3().y), (double)(endPos.z-face.v3().z)});
		vertexVector.normalize();
		double dot3 = std::abs(segmentVector.dot(vertexVector));
		if (dot1 > dot2 && dot1 > dot3)
		{
			fprintf(stderr,"dot1_");
			linedVertex = 1;
			linedVertexPos = face.v1().getPosition();
		}
		else if (dot2 > dot3 && dot2 > dot1)
		{
			fprintf(stderr,"dot2_");
			linedVertex = 2;
			linedVertexPos = face.v2().getPosition();
		}
		else
		{
			fprintf(stderr,"dot3_");
			linedVertex = 3;
			linedVertexPos = face.v3().getPosition();
		}
	
		// Now find which of the intersection endpoints is nearest to that vertex.
		if (linedVertexPos.distance(startPos) > linedVertexPos.distance(endPos))
		{
			fprintf(stderr," near1_");
			breakFaceInFive(facePos, startPos, endPos, linedVertex, testedUntil);
		}
		else
		{
			fprintf(stderr," near2_");
			breakFaceInFive(facePos, endPos, startPos, linedVertex, testedUntil);
		}
	}
}

#ifdef _DEBUG
void Object3D::checkSplit(Face& original, int start_faces){
	double originalArea = original.getArea();
	int count = (faces.size()-start_faces)+1;
	double newArea = 0;

	auto it = faces.rbegin();
	for(int i = 0; i<count; ++i, it++)
	{
		newArea+= it->getArea();
	}

	if(std::abs(originalArea - newArea)>1e-5){
		std::cout<<"\noriginal( \t"<<original.toString()<<" )"<<std::endl;
		
		it = faces.rbegin();
		for(int i = 0; i<count; ++i, it++)
		{
			std::cout<<"newTriangle("<<(count-i)<<", "<<it->toString()<<" )"<<std::endl;
		}

		std::cout<<"";

	}
}
#endif

#ifndef _DEBUG
#define REMOVE(faces, facepos)\
int current_faces = 0;\
faces[facepos] = faces.back();\
faces.pop_back();
#elifdef _DEBUG
#define REMOVE(faces, facepos)\
int current_faces = faces.size();\
faces[facepos] = faces.back();\
faces.pop_back();\
std::cerr<<__FUNCTION__<<__LINE__<<":";
#endif

	
/**
 * Face breaker for VERTEX-EDGE-EDGE / EDGE-EDGE-VERTEX
 * 
 * @param facePos face position on the faces array
 * @param newPos new vertex position
 * @param edge that will be split 
 */		
void Object3D::breakFaceInTwo(int facePos, Point3d newPos, int splitEdge, int testedUntil)
{
	Face face = faces[facePos];
	REMOVE(faces,facePos);

	int vertex = addVertex(newPos, face.v1().getColor(), Vertex::BOUNDARY); 
					
	if (splitEdge == 1)
	{
		addFace(face.v[0], vertex, face.v[2], testedUntil);
		addFace(vertex, face.v[1], face.v[2], testedUntil);
	}
	else if (splitEdge == 2)
	{
		addFace(face.v[1], vertex, face.v[0], testedUntil);
		addFace(vertex, face.v[2], face.v[0], testedUntil);
	}
	else
	{
		addFace(face.v[2], vertex, face.v[1], testedUntil);
		addFace(vertex, face.v[0], face.v[1], testedUntil);
	}
	checkSplit(face,current_faces);
}

/**
 * Face breaker for VERTEX-FACE-EDGE / EDGE-FACE-VERTEX
 * 
 * @param facePos face position on the faces array
 * @param newPos new vertex position
 * @param endVertex vertex used for splitting 
 */		
void Object3D::breakFaceInTwo(int facePos, Point3d newPos, Vertex endVertex, int testedUntil)
{
	Face face = faces[facePos];
	REMOVE(faces,facePos);
	
	int vertex = addVertex(newPos, face.v1().getColor(), Vertex::BOUNDARY);
				
	if (endVertex.equals(face.v1()))
	{
		addFace(face.v[0], vertex, face.v[2], testedUntil);
		addFace(vertex, face.v[1], face.v[2], testedUntil);
	}
	else if (endVertex.equals(face.v2()))
	{
		addFace(face.v[1], vertex, face.v[0], testedUntil);
		addFace(vertex, face.v[2], face.v[0], testedUntil);
	}
	else
	{
		addFace(face.v[2], vertex, face.v[1], testedUntil);
		addFace(vertex, face.v[0], face.v[1], testedUntil);
	}
	checkSplit(face,current_faces);
}

/**
 * Face breaker for EDGE-EDGE-EDGE
 * 
 * @param facePos face position on the faces array
 * @param newPos1 new vertex position
 * @param newPos2 new vertex position 
 * @param splitEdge edge that will be split
 */
void Object3D::breakFaceInThree(int facePos, Point3d newPos1, Point3d newPos2, int splitEdge, int testedUntil)
{
	Face face = faces[facePos];
	REMOVE(faces,facePos);
	
	int vertex1 = addVertex(newPos1, face.v1().getColor(), Vertex::BOUNDARY);	
	int vertex2 = addVertex(newPos2, face.v1().getColor(), Vertex::BOUNDARY);
					
	if (splitEdge == 1)
	{
		addFace(face.v[0], vertex1, face.v[2], testedUntil);
		addFace(vertex1, vertex2, face.v[2], testedUntil);
		addFace(vertex2, face.v[1], face.v[2], testedUntil);
	}
	else if (splitEdge == 2)
	{
		addFace(face.v[1], vertex1, face.v[0], testedUntil);
		addFace(vertex1, vertex2, face.v[0], testedUntil);
		addFace(vertex2, face.v[2], face.v[0], testedUntil);
	}
	else
	{
		addFace(face.v[2], vertex1, face.v[1], testedUntil);
		addFace(vertex1, vertex2, face.v[1], testedUntil);
		addFace(vertex2, face.v[0], face.v[1], testedUntil);
	}
	checkSplit(face,current_faces);
}
	
/**
 * Face breaker for VERTEX-FACE-FACE / FACE-FACE-VERTEX
 * 
 * @param facePos face position on the faces array
 * @param newPos new vertex position
 * @param endVertex vertex used for the split
 */
void Object3D::breakFaceInThree(int facePos, Point3d newPos, Vertex endVertex, int testedUntil)
{
	Face face = faces[facePos];
	REMOVE(faces,facePos);
	
	int vertex = addVertex(newPos, face.v1().getColor(), Vertex::BOUNDARY);
					
	if (endVertex.equals(face.v1()))
	{
		addFace(face.v[0], face.v[1], vertex, testedUntil);
		addFace(face.v[1], face.v[2], vertex, testedUntil);
		addFace(face.v[2], face.v[0], vertex, testedUntil);
	}
	else if (endVertex.equals(face.v2()))
	{
		addFace(face.v[1], face.v[2], vertex, testedUntil);
		addFace(face.v[2], face.v[0], vertex, testedUntil);
		addFace(face.v[0], face.v[1], vertex, testedUntil);
	}
	else
	{
		addFace(face.v[2], face.v[0], vertex, testedUntil);
		addFace(face.v[0], face.v[1], vertex, testedUntil);
		addFace(face.v[1], face.v[2], vertex, testedUntil);
	}
	checkSplit(face,current_faces);
}

/**
 * Face breaker for EDGE-FACE-EDGE
 * 
 * @param facePos face position on the faces array
 * @param newPos1 new vertex position
 * @param newPos2 new vertex position 
 * @param startVertex vertex used the new faces creation
 * @param endVertex vertex used for the new faces creation
 */
void Object3D::breakFaceInThree(int facePos, Point3d newPos1, Point3d newPos2, Vertex startVertex, Vertex endVertex, int testedUntil)
{
	Face face = faces[facePos];
	REMOVE(faces,facePos);
	
	int vertex1 = addVertex(newPos1, face.v1().getColor(), Vertex::BOUNDARY);
	int vertex2 = addVertex(newPos2, face.v1().getColor(), Vertex::BOUNDARY);
					
	if (startVertex.equals(face.v1()) && endVertex.equals(face.v2()))
	{
		addFace(face.v[0], vertex1, vertex2, testedUntil);
		addFace(face.v[0], vertex2, face.v[2], testedUntil);
		addFace(vertex1, face.v[1], vertex2, testedUntil);
	}
	else if (startVertex.equals(face.v2()) && endVertex.equals(face.v1()))
	{
		addFace(face.v[0], vertex2, vertex1, testedUntil);
		addFace(face.v[0], vertex1, face.v[2], testedUntil);
		addFace(vertex2, face.v[1], vertex1, testedUntil);
	}
	else if (startVertex.equals(face.v2()) && endVertex.equals(face.v3()))
	{
		addFace(face.v[1], vertex1, vertex2, testedUntil);
		addFace(face.v[1], vertex2, face.v[0], testedUntil);
		addFace(vertex1, face.v[2], vertex2, testedUntil);
	}
	else if (startVertex.equals(face.v3()) && endVertex.equals(face.v2()))
	{
		addFace(face.v[1], vertex2, vertex1, testedUntil);
		addFace(face.v[1], vertex1, face.v[0], testedUntil);
		addFace(vertex2, face.v[2], vertex1, testedUntil);
	}
	else if (startVertex.equals(face.v3()) && endVertex.equals(face.v1()))
	{
		addFace(face.v[2], vertex1, vertex2, testedUntil);
		addFace(face.v[2], vertex2, face.v[1], testedUntil);
		addFace(vertex1, face.v[0], vertex2, testedUntil);
	}
	else
	{
		addFace(face.v[2], vertex2, vertex1, testedUntil);
		addFace(face.v[2], vertex1, face.v[1], testedUntil);
		addFace(vertex2, face.v[0], vertex1, testedUntil);
	}
	checkSplit(face,current_faces);
}
	
/**
 * Face breaker for FACE-FACE-FACE (a point only)
 * 
 * @param facePos face position on the faces array
 * @param newPos new vertex position
 */
void Object3D::breakFaceInThree(int facePos, Point3d newPos, int testedUntil)
{
	Face face = faces[facePos];
	REMOVE(faces,facePos);
	
	int vertex = addVertex(newPos, face.v1().getColor(), Vertex::BOUNDARY);
			
	addFace(face.v[0], face.v[1], vertex, testedUntil);
	addFace(face.v[1], face.v[2], vertex, testedUntil);
	addFace(face.v[2], face.v[0], vertex, testedUntil);
	
	checkSplit(face,current_faces);
}

/**
 * Face breaker for EDGE-FACE-FACE / FACE-FACE-EDGE
 * 
 * @param facePos face position on the faces array
 * @param newPos1 new vertex position
 * @param newPos2 new vertex position 
 * @param endVertex vertex used for the split
 */	
void Object3D::breakFaceInFour(int facePos, Point3d newPos1, Point3d newPos2, Vertex endVertex, int testedUntil)
{
	Face face = faces[facePos];
	REMOVE(faces,facePos);
	
	int vertex1 = addVertex(newPos1, face.v1().getColor(), Vertex::BOUNDARY);
	int vertex2 = addVertex(newPos2, face.v1().getColor(), Vertex::BOUNDARY);
	
	if (endVertex.equals(face.v1()))
	{
		addFace(face.v[0], vertex1, vertex2, testedUntil);
		addFace(vertex1, face.v[1], vertex2, testedUntil);
		addFace(face.v[1], face.v[2], vertex2, testedUntil);
		addFace(face.v[2], face.v[0], vertex2, testedUntil);
	}
	else if (endVertex.equals(face.v2()))
	{
		addFace(face.v[1], vertex1, vertex2, testedUntil);
		addFace(vertex1, face.v[2], vertex2, testedUntil);
		addFace(face.v[2], face.v[0], vertex2, testedUntil);
		addFace(face.v[0], face.v[1], vertex2, testedUntil);
	}
	else
	{
		addFace(face.v[2], vertex1, vertex2, testedUntil);
		addFace(vertex1, face.v[0], vertex2, testedUntil);
		addFace(face.v[0], face.v[1], vertex2, testedUntil);
		addFace(face.v[1], face.v[2], vertex2, testedUntil);
	}
	checkSplit(face,current_faces);

}

/**
 * Face breaker for FACE-FACE-FACE
 * 
 * @param facePos face position on the faces array
 * @param newPos1 new vertex position
 * @param newPos2 new vertex position 
 * @param linedVertex what vertex is more lined with the interersection found
 */		
void Object3D::breakFaceInFive(int facePos, Point3d newPos1, Point3d newPos2, int linedVertex, int testedUntil)
{
	Face face = faces[facePos];
	REMOVE(faces,facePos);
	
	int vertex1 = addVertex(newPos1, face.v1().getColor(), Vertex::BOUNDARY);
	int vertex2 = addVertex(newPos2, face.v1().getColor(), Vertex::BOUNDARY);
	
	double cont = 0;		
	if (linedVertex == 1)
	{
		addFace(face.v[1], face.v[2], vertex1, testedUntil);
		addFace(face.v[1], vertex1, vertex2, testedUntil);
		addFace(face.v[2], vertex2, vertex1, testedUntil);
		addFace(face.v[1], vertex2, face.v[0], testedUntil);
		addFace(face.v[2], face.v[0], vertex2, testedUntil);
	}
	else if(linedVertex == 2)
	{
		addFace(face.v[2], face.v[0], vertex1, testedUntil);
		addFace(face.v[2], vertex1, vertex2, testedUntil);
		addFace(face.v[0], vertex2, vertex1, testedUntil);
		addFace(face.v[2], vertex2, face.v[1], testedUntil);
		addFace(face.v[0], face.v[1], vertex2, testedUntil);
	}
	else
	{
		fprintf(stderr,"_else");
		addFace(face.v[0], face.v[1], vertex1, testedUntil);
		addFace(face.v[0], vertex1, vertex2, testedUntil);
		addFace(face.v[1], vertex2, vertex1, testedUntil);
		addFace(face.v[0], vertex2, face.v[2], testedUntil);
		addFace(face.v[1], face.v[2], vertex2, testedUntil);
	}
	
	checkSplit(face,current_faces);
}

//-----------------------------------OTHERS-------------------------------------//

/**
 * Classify faces as being inside, outside or on boundary of other object
 * 
 * @param object object 3d used for the comparison
 */
void Object3D::classifyFaces(Object3D& object)
{
	//calculate adjacency information
	for(int i=0;i<this->getNumFaces();i++)
	{
		Face& face = getFace(i); // this needs to be rewritten for c++
		face.v1().addAdjacentVertex(face.v[1]);
		face.v1().addAdjacentVertex(face.v[2]);
		face.v2().addAdjacentVertex(face.v[0]);
		face.v2().addAdjacentVertex(face.v[2]);
		face.v3().addAdjacentVertex(face.v[0]);
		face.v3().addAdjacentVertex(face.v[1]);
	}
	
	//for each face
	for(int i=0;i<getNumFaces();i++)
	{
		Face& face = getFace(i);
		
		//if the face vertices aren't classified to make the simple classify
		if(face.simpleClassify()==false)
		{
			//makes the ray trace classification
			face.rayTraceClassify(object);
			
			//mark the vertices
			if(face.v1().getStatus()==Vertex::UNKNOWN) 
			{
				face.v1().mark(face.getStatus());
			}
			if(face.v2().getStatus()==Vertex::UNKNOWN) 
			{
				face.v2().mark(face.getStatus());
			}
			if(face.v3().getStatus()==Vertex::UNKNOWN) 
			{
				face.v3().mark(face.getStatus());
			}
		}
	}
}

/** Inverts faces classified as INSIDE, making its normals point outside. Usually
 *  used into the second solid when the difference is applied. */
void Object3D::invertInsideFaces()
{
	for(int i=0;i<getNumFaces();i++)
	{
		Face& face = getFace(i);
		if(face.getStatus()==Face::INSIDE)
		{
			face.invert();
		}
	}
}