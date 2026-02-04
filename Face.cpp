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
Face::Face()
	:v1(0,0,0,{0,0,0})
	,v2(0,0,0,{0,0,0})
	,v3(0,0,0,{0,0,0})
{
	this->status = Face::INVALID;
}
	
/**
 * Constructs a face with unknown status.
 * 
 * @param v1 a face vertex
 * @param v2 a face vertex
 * @param v3 a face vertex
 */
Face::Face(Vertex& v1, Vertex& v2, Vertex& v3)
	:v1(v1)
	,v2(v2)
	,v3(v3)
{
	this->status = Face::UNKNOWN;
}

//-----------------------------------OVERRIDES----------------------------------//

/**
 * Clones the face object
 * 
 * @return cloned face object
 */
Face::Face(Face& other)
	:v1(other.v1)
	,v2(other.v2)
	,v3(other.v3)
	,status(other.status)
{
}

/**
 * Makes a string definition for the Face object
 * 
 * @return the string definition
 */
std::string Face::toString() const
{
	return v1.toString()+"\n"+v2.toString()+"\n"+v3.toString();
}

/**
 * Checks if a face is equal to another. To be equal, they have to have equal
 * vertices in the same order
 * 
 * @param anObject the other face to be tested
 * @return true if they are equal, false otherwise. 
 */
bool Face::equals(Face& other) const
{
	for(int i = 1; i<=3; ++i){
		if(!v1.equals(other.getVertex(i))) continue;
		
		for(int j = 1; j<=3; ++j){
			if(j==i) continue;
			if(!v2.equals(other.getVertex(j))) continue;

			for(int k = 1; k<=3; ++k){
				if(k == i || k == j) continue;
				if(!v3.equals(other.getVertex(k))) continue;

				return true;
			}
			break;
		}
		break;
	}
	return false;
	/*
	bool cond1 = v1.equals(other.v1) && v2.equals(other.v2) && v3.equals(other.v3);
	bool cond2 = v1.equals(other.v2) && v2.equals(other.v3) && v3.equals(other.v1);
	bool cond3 = v1.equals(other.v3) && v2.equals(other.v1) && v3.equals(other.v2);
	
	return cond1 || cond2 || cond3;*/
}

//-------------------------------------GETS-------------------------------------//

/**
 * Gets the face bound
 * 
 * @return face bound 
 */
Bound Face::getBound() const
{
	Bound rval(v1.getPosition(),v2.getPosition(),v3.getPosition());
	return rval;
}

/**
 * Gets the face normal
 * 
 * @return face normal
 */
Vector3f Face::getNormal() const
{
	Point3f p1 = v1.getPosition();
	Point3f p2 = v2.getPosition();
	Point3f p3 = v3.getPosition();
	
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
	Point3f p1 = v1.getPosition();
	Point3f p2 = v2.getPosition();
	Point3f p3 = v3.getPosition();
	Vector3f xy = {p2.x-p1.x, p2.y-p1.y, p2.z-p1.z};
	Vector3f xz = {p3.x-p1.x, p3.y-p1.y, p3.z-p1.z};
	
	double a = p1.distance(p2);
	double c = p1.distance(p3);
	double B = xy.angle(xz);
	
	return (a * c * sin(B))/2.0;
}

//-------------------------------------OTHERS-----------------------------------//

/** Invert face direction (normal direction) */
void Face::invert()
{
	Vertex vertexTemp = v2;
	v2 = v1;
	v1 = vertexTemp;
}
	
//------------------------------------CLASSIFIERS-------------------------------//

/**
 * Classifies the face if one of its vertices are classified as INSIDE or OUTSIDE
 * 
 * @return true if the face could be classified, false otherwise 
 */
bool Face::simpleClassify()
{
	int status1 = v1.getStatus();
	int status2 = v2.getStatus();
	int status3 = v3.getStatus();
		
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
	p0.x = (v1.x + v2.x + v3.x)/3.0;
	p0.y = (v1.y + v2.y + v3.y)/3.0;
	p0.z = (v1.z + v2.z + v3.z)/3.0;
	Line ray(getNormal(),p0);
	
	bool success;
	double dotProduct, distance; 
	Point3f intersectionPoint;
	Face closestFace; //construct invalid face
	double closestDistance; 
								
	do
	{
		success = true;
		closestDistance = __DBL_MAX__;
		//for each face from the other solid...
		for(int i=0;i<object.getNumFaces();i++)
		{
			Face face = object.getFace(i);
			dotProduct = this->getNormal().dot(ray.getDirection());
			intersectionPoint = ray.computePlaneIntersection(this->getNormal(), this->v1.getPosition());
							
			//if ray intersects the plane...  
			if(intersectionPoint.isNAN())
			{
				distance = ray.computePointToPointDistance(intersectionPoint);
				
				//if ray lies in plane...
				if(abs(distance)<TOL && abs(dotProduct)<TOL)
				{
					//disturb the ray in order to not lie into another plane 
					ray.perturbDirection();
					success = false;
					break;
				}
				
				//if ray starts in plane...
				else if(abs(distance)<TOL && abs(dotProduct)>TOL)
				{
					//if ray intersects the Face...
					if(this->hasPoint(intersectionPoint))
					{
						//faces coincide
						closestFace = face;
						closestDistance = 0;
						break;
					}
				}
				
				//if ray intersects plane... 
				else if(abs(dotProduct)>TOL && distance>TOL)
				{
					if(distance<closestDistance)
					{
						//if ray intersects the face;
						if(this->hasPoint(intersectionPoint))
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
		if(abs(closestDistance)<TOL)
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
	if(abs(normal.x)>TOL) 
	{
		//tests on the x plane
		result1 = linePositionInX(point, v1.getPosition(), v2.getPosition());
		result2 = linePositionInX(point, v2.getPosition(), v3.getPosition());
		result3 = linePositionInX(point, v3.getPosition(), v1.getPosition());
	}
	
	//if y is constant...
	else if(abs(normal.y)>TOL)
	{
		//tests on the y plane
		result1 = linePositionInY(point, v1.getPosition(), v2.getPosition());
		result2 = linePositionInY(point, v2.getPosition(), v3.getPosition());
		result3 = linePositionInY(point, v3.getPosition(), v1.getPosition());
	}
	else
	{
		//tests on the z plane
		result1 = linePositionInZ(point, v1.getPosition(), v2.getPosition());
		result2 = linePositionInZ(point, v2.getPosition(), v3.getPosition());
		result3 = linePositionInZ(point, v3.getPosition(), v1.getPosition());
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
	if((abs(pointLine1.y-pointLine2.y)>TOL)&&(((point.y>=pointLine1.y)&&(point.y<=pointLine2.y))||((point.y<=pointLine1.y)&&(point.y>=pointLine2.y))))
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
	if((abs(pointLine1.x-pointLine2.x)>TOL)&&(((point.x>=pointLine1.x)&&(point.x<=pointLine2.x))||((point.x<=pointLine1.x)&&(point.x>=pointLine2.x))))
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
	if((abs(pointLine1.x-pointLine2.x)>TOL)&&(((point.x>=pointLine1.x)&&(point.x<=pointLine2.x))||((point.x<=pointLine1.x)&&(point.x>=pointLine2.x))))
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

Vertex& Face::getVertex(int id)
{
	switch(id){
		case 1:
			return v1;
		case 2:
			return v2;
		case 3:
			return v3;
		default:
			abort();
	}
}

