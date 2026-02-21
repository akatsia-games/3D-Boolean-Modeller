#include "Face.hpp"
#include "Line.hpp"
#include "Object3D.hpp"
#include<cmath>

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

	
//---------------------------------CONSTRUCTORS---------------------------------//
	
/**
 * Constructs a face with unknown status.
 * 
 * @param v1 a face vertex
 * @param v2 a face vertex
 * @param v3 a face vertex
 */
Face::Face(std::vector<Vertex>& solidVertices)
	:v{0,0,0}
	,solidVertices(solidVertices)
	,testedUntil(0)
{
	status = Face::INVALID;
}
	
/**
 * Constructs a face with unknown status.
 * 
 * @param v1 a face vertex
 * @param v2 a face vertex
 * @param v3 a face vertex
 */
Face::Face(std::vector<Vertex>& solidVertices, int v1, int v2, int v3, int testedUntil)
	:v{v1, v2, v3}
	,solidVertices(solidVertices)
	,testedUntil(testedUntil)
{
	status = Face::UNKNOWN;
}

//-----------------------------------OVERRIDES----------------------------------//

/**
 * Clones the face object
 * 
 * @return cloned face object
 */
Face::Face(const Face& other)
	:v{other.v[0], other.v[1], other.v[2]}
	,status(other.status)
	,solidVertices(other.solidVertices)
	,testedUntil(other.testedUntil)
{
}


Face& Face::operator=(const Face& other){
	v[0] = other.v[0];
	v[1] = other.v[1];
	v[2] = other.v[2];
	status = other.status;
	testedUntil = other.testedUntil;
	return *this;
}

/**
 * Makes a string definition for the Face object
 * 
 * @return the string definition
 */
std::string Face::toString() const
{
	return v1().toString()+"\t"+v2().toString()+"\t"+v3().toString();
}

/**
 * Checks if a face is equal to another. To be equal, they have to have equal
 * vertices in the same order
 * 
 * @param anObject the other face to be tested
 * @return true if they are equal, false otherwise. 
 */
bool Face::equals(const Face& other) const
{
	for(int i = 0; i<=2; ++i){

		if(!v1().equals(other.getVertex(i))) continue;

		int j = (i+1)%3;

		if(!v2().equals(other.getVertex(j))) continue;

		int k = (i+2)%3;

		if(!v3().equals(other.getVertex(k))) continue;

		return true;
	}
	return false;
}

/**
 * Checks if a face isn't identical to another. To be equal, they have to have equal
 * vertices in the same order
 * 
 * @param anObject the other face to be tested
 * @return true if they are equal, false otherwise. 
 */
bool Face::operator!=(const Face& other) const
{
	return ((v1()!=other.v1()) || (v2()!=other.v2()) || (v3()!=other.v3()));
}

//-------------------------------------GETS-------------------------------------//

/** first vertex */
Vertex& Face::v1(){
	return solidVertices[v[0]];
}
const Vertex& Face::v1()const{
	return solidVertices[v[0]];
}
/** second vertex */
Vertex& Face::v2(){
	return solidVertices[v[1]];
}
const Vertex& Face::v2()const{
	return solidVertices[v[1]];
}
/** third vertex */
Vertex& Face::v3(){
	return solidVertices[v[2]];
}
const Vertex& Face::v3()const{
	return solidVertices[v[2]];
}

/**
 * Gets the face bound
 * 
 * @return face bound 
 */
Bound Face::getBound() const
{
	Bound rval(v1().getPosition(),v2().getPosition(),v3().getPosition());
	return rval;
}

/**
 * Gets the face normal
 * 
 * @return face normal
 */
Vector3f Face::getNormal() const
{
	Point3f p1 = v1().getPosition();
	Point3f p2 = v2().getPosition();
	Point3f p3 = v3().getPosition();
	
	Vector3f xy = {p2.x-p1.x, p2.y-p1.y, p2.z-p1.z};
	Vector3f xz = {p3.x-p1.x, p3.y-p1.y, p3.z-p1.z};
	
	Vector3f normal;
	
	normal.cross(xy,xz);
	normal.normalize();
	
	return normal;
}

/**
 * Gets the face status
 * 
 * @return face status - UNKNOWN, INSIDE, OUTSIDE, SAME OR OPPOSITE
 */
int Face::getStatus() const
{
	return status;
}

/**
 * Gets the face area
 * 
 * @return face area
 */
float Face::getArea() const
{
	//area = (a * c * sen(B))/2
	Point3f p1 = v1().getPosition();
	Point3f p2 = v2().getPosition();
	Point3f p3 = v3().getPosition();
	Vector3f xy = {p2.x-p1.x, p2.y-p1.y, p2.z-p1.z};
	Vector3f xz = {p3.x-p1.x, p3.y-p1.y, p3.z-p1.z};
	
	double a = p1.distance(p2);
	double c = p1.distance(p3);
	double B = xy.angle(xz);
	
	return (a * c * sin(B))/2.0;
}

int Face::getStart() const{
	return testedUntil;
}

//-------------------------------------OTHERS-----------------------------------//

/** Invert face direction (normal direction) */
void Face::invert()
{
	int vertexTemp = v[1];
	v[1] = v[0];
	v[0] = vertexTemp;
}
	
//------------------------------------CLASSIFIERS-------------------------------//

/**
 * Classifies the face if one of its vertices are classified as INSIDE or OUTSIDE
 * 
 * @return true if the face could be classified, false otherwise 
 */
bool Face::simpleClassify()
{
	int status1 = v1().getStatus();
	int status2 = v2().getStatus();
	int status3 = v3().getStatus();
		
	if(status1==Vertex::INSIDE || status1==Vertex::OUTSIDE)
	{
		this->status = status1;
		return true; 
	}
	else if(status2==Vertex::INSIDE || status2==Vertex::OUTSIDE)
	{
		this->status = status2;
		return true;
	}
	else if(status3==Vertex::INSIDE || status3==Vertex::OUTSIDE)
	{
		this->status = status3;
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * Classifies the face based on the ray trace technique
 * 
 * @param object object3d used to compute the face status 
 */
void Face::rayTraceClassify(Object3D& object)
{
	//creating a ray starting starting at the face baricenter going to the normal direction
	Point3f p0;
	p0.x = (v1().x + v2().x + v3().x)/3.0;
	p0.y = (v1().y + v2().y + v3().y)/3.0;
	p0.z = (v1().z + v2().z + v3().z)/3.0;
	Line ray(getNormal(),p0);
	
	bool success;
	double dotProduct, distance; 
	Point3f intersectionPoint;
	Face closestFace(object.vertices); //construct invalid face
	double closestDistance; 
								
	do
	{
		success = true;
		closestDistance = __DBL_MAX__;
		//for each face from the other solid...
		for(int i=0;i<object.getNumFaces();i++)
		{
			Face face = object.getFace(i);
			dotProduct = face.getNormal().dot(ray.getDirection());
			intersectionPoint = ray.computePlaneIntersection(face.getNormal(), face.v1().getPosition());
							
			//if ray intersects the plane...  
			if(!intersectionPoint.isNAN())
			{
				distance = ray.computePointToPointDistance(intersectionPoint);
				
				//if ray lies in plane...
				if(std::abs(distance)<TOL && std::abs(dotProduct)<TOL)
				{
					//disturb the ray in order to not lie into another plane 
					ray.perturbDirection();
					success = false;
					break;
				}
				
				//if ray starts in plane...
				else if(std::abs(distance)<TOL && std::abs(dotProduct)>TOL)
				{
					//if ray intersects the Face...
					if(face.hasPoint(intersectionPoint))
					{
						//faces coincide
						closestFace = face;
						closestDistance = 0;
						break;
					}
				}
				
				//if ray intersects plane... 
				else if(std::abs(dotProduct)>TOL && distance>TOL)
				{
					if(distance<closestDistance)
					{
						//if ray intersects the face;
						if(face.hasPoint(intersectionPoint))
						{
							//this face is the closest face untill now
							closestDistance = distance;
							closestFace = face;
						}
					}
				}
			}
		}
	}while(success==false);
	
	//none face found: outside face
	if(closestFace.status == INVALID)
	{
		status = OUTSIDE;
	}
	//face found: test dot product
	else
	{
		dotProduct = closestFace.getNormal().dot(ray.getDirection());
		
		//distance = 0: coplanar faces
		if(std::abs(closestDistance)<TOL)
		{
			if(dotProduct>TOL)
			{
				status = SAME;
			}
			else if(dotProduct<-TOL)
			{
				status = OPPOSITE;
			}
		}
		
		//dot product > 0 (same direction): inside face
		else if(dotProduct>TOL)
		{
			status = INSIDE;
		}
		
		//dot product < 0 (opposite direction): outside face
		else if(dotProduct<-TOL)
		{
			status = OUTSIDE;
		}
	}
}

//------------------------------------PRIVATES----------------------------------//

/**
 * Checks if the the face contains a point
 * 
 * @param point to be tested
 * @param true if the face contains the point, false otherwise 
 */	
bool Face::hasPoint(Point3f& point)
{
	int result1, result2, result3;
	bool hasUp, hasDown, hasOn;
	Vector3f normal = getNormal(); 

	//if x is constant...	
	if(std::abs(normal.x)>TOL) 
	{
		//tests on the x plane
		result1 = linePositionInX(point, v1().getPosition(), v2().getPosition());
		result2 = linePositionInX(point, v2().getPosition(), v3().getPosition());
		result3 = linePositionInX(point, v3().getPosition(), v1().getPosition());
	}
	
	//if y is constant...
	else if(std::abs(normal.y)>TOL)
	{
		//tests on the y plane
		result1 = linePositionInY(point, v1().getPosition(), v2().getPosition());
		result2 = linePositionInY(point, v2().getPosition(), v3().getPosition());
		result3 = linePositionInY(point, v3().getPosition(), v1().getPosition());
	}
	else
	{
		//tests on the z plane
		result1 = linePositionInZ(point, v1().getPosition(), v2().getPosition());
		result2 = linePositionInZ(point, v2().getPosition(), v3().getPosition());
		result3 = linePositionInZ(point, v3().getPosition(), v1().getPosition());
	}
	
	//if the point is up and down two lines...		
	if(((result1==Face::UP)||(result2==Face::UP)||(result3==Face::UP))&&((result1==Face::DOWN)||(result2==Face::DOWN)||(result3==Face::DOWN)))
	{
		return true;
	}
	//if the point is on of the lines...
	else if ((result1==Face::ON)||(result2==Face::ON)||(result3==Face::ON))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/** 
 * Gets the position of a point relative to a line in the x plane
 * 
 * @param point point to be tested
 * @param pointLine1 one of the line ends
 * @param pointLine2 one of the line ends
 * @return position of the point relative to the line - Face::UP, Face::DOWN, Face::ON, Face::NONE 
 */
int Face::linePositionInX(Point3f point, Point3f pointLine1, Point3f pointLine2)
{
	double a, b, z;
	if((std::abs(pointLine1.y-pointLine2.y)>TOL)&&(((point.y>=pointLine1.y)&&(point.y<=pointLine2.y))||((point.y<=pointLine1.y)&&(point.y>=pointLine2.y))))
	{
		a = (pointLine2.z-pointLine1.z)/(pointLine2.y-pointLine1.y);
		b = pointLine1.z - a*pointLine1.y;
		z = a*point.y + b;
		if(z>point.z+TOL)
		{
			return Face::UP;			
		}
		else if(z<point.z-TOL)
		{
			return Face::DOWN;
		}
		else
		{
			return Face::ON;
		}
	}
	else
	{
		return Face::NONE;
	}
}

/** 
 * Gets the position of a point relative to a line in the y plane
 * 
 * @param point point to be tested
 * @param pointLine1 one of the line ends
 * @param pointLine2 one of the line ends
 * @return position of the point relative to the line - Face::UP, Face::DOWN, Face::ON, Face::NONE 
 */

int Face::linePositionInY(Point3f point, Point3f pointLine1, Point3f pointLine2)
{
	double a, b, z;
	if((std::abs(pointLine1.x-pointLine2.x)>TOL)&&(((point.x>=pointLine1.x)&&(point.x<=pointLine2.x))||((point.x<=pointLine1.x)&&(point.x>=pointLine2.x))))
	{
		a = (pointLine2.z-pointLine1.z)/(pointLine2.x-pointLine1.x);
		b = pointLine1.z - a*pointLine1.x;
		z = a*point.x + b;
		if(z>point.z+TOL)
		{
			return Face::UP;			
		}
		else if(z<point.z-TOL)
		{
			return Face::DOWN;
		}
		else
		{
			return Face::ON;
		}
	}
	else
	{
		return Face::NONE;
	}
}

/** 
 * Gets the position of a point relative to a line in the z plane
 * 
 * @param point point to be tested
 * @param pointLine1 one of the line ends
 * @param pointLine2 one of the line ends
 * @return position of the point relative to the line - Face::UP, Face::DOWN, Face::ON, Face::NONE 
 */

int Face::linePositionInZ(Point3f point, Point3f pointLine1, Point3f pointLine2)
{
	double a, b, y;
	if((std::abs(pointLine1.x-pointLine2.x)>TOL)&&(((point.x>=pointLine1.x)&&(point.x<=pointLine2.x))||((point.x<=pointLine1.x)&&(point.x>=pointLine2.x))))
	{
		a = (pointLine2.y-pointLine1.y)/(pointLine2.x-pointLine1.x);
		b = pointLine1.y - a*pointLine1.x;
		y = a*point.x + b;
		if(y>point.y+TOL)
		{
			return Face::UP;			
		}
		else if(y<point.y-TOL)
		{
			return Face::DOWN;
		}
		else
		{
			return Face::ON;
		}
	}
	else
	{
		return Face::NONE;
	}
}

const Vertex& Face::getVertex(int id)const
{
	switch(id){
		case 0:
			return v1();
		case 1:
			return v2();
		case 2:
			return v3();
		default:
			abort();
	}
}

