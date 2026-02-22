#include "Vertex.hpp"
#include <algorithm>

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

 
std::vector<Vertex> Vertex::emptyVertexVector;

/**
 * Constructs a vertex with unknown status
 * 
 * @param position vertex position
 * @param color vertex color
 */
Vertex::Vertex(std::vector<Vertex>& solidVertices, Point3f position, Colour3f color)
	:solidVertices(solidVertices)
{
	this->color = color;
	
	x = position.x;
	y = position.y;
	z = position.z;
	
	status = UNKNOWN;
}

/**
 * Constructs a vertex with unknown status
 * 
 * @param x coordinate on the x axis
 * @param y coordinate on the y axis
 * @param z coordinate on the z axis
 * @param color vertex color
 */
Vertex::Vertex(std::vector<Vertex>& solidVertices, double x, double y, double z, Colour3f color)
	:solidVertices(solidVertices)
{
	this->color = color;
			
	this->x = x;
	this->y = y;
	this->z = z;
	
	status = UNKNOWN;
}

/**
 * Constructs a vertex with definite status
 * 
 * @param position vertex position
 * @param color vertex color
 * @param status vertex status - UNKNOWN, BOUNDARY, INSIDE or OUTSIDE
 */
Vertex::Vertex(std::vector<Vertex>& solidVertices, Point3f position, Colour3f color, int status)
	:solidVertices(solidVertices)
{
	this->color = color;
	
	x = position.x;
	y = position.y;
	z = position.z;
	
	this->status = status;
}

/**
 * Constructs a vertex with a definite status
 * 
 * @param x coordinate on the x axis
 * @param y coordinate on the y axis
 * @param z coordinate on the z axis
 * @param color vertex color
 * @param status vertex status - UNKNOWN, BOUNDARY, INSIDE or OUTSIDE
 */
Vertex::Vertex(std::vector<Vertex>& solidVertices, double x, double y, double z, Colour3f color, int status)
	:solidVertices(solidVertices)
{
	this->color = color;
	
	this->x = x;
	this->y = y;
	this->z = z;
	
	this->status = status;
}

//-----------------------------------OVERRIDES----------------------------------//

/**
 * Clones the vertex object
 * 
 * @return cloned vertex object
 */
Vertex::Vertex(const Vertex& other)
	:solidVertices(other.solidVertices)
{
	x = other.x;
	y = other.y;
	z = other.z;
	color = other.color;
	status = other.status;
	adjacentVertexIndexes = other.adjacentVertexIndexes;
}

/**
 * Create invalid vertex object
 */
Vertex::Vertex()
	:solidVertices(Vertex::emptyVertexVector)
{
	status = INVALID;
}

/**
 * Makes a string definition for the Vertex object
 * 
 * @return the string definition
 */
std::string Vertex::toString() const
{
	return "["+std::to_string(x)+", "+std::to_string(y)+", "+std::to_string(z)+"]";
}

/**
 * Checks if an vertex is equal to another. To be equal, they have to have the same
 * coordinates(with some tolerance) and color
 * 
 * @param anObject the other vertex to be tested
 * @return true if they are equal, false otherwise. 
 */
bool Vertex::equals(const Vertex& other) const
{
	return 	(std::abs(x-other.x)<TOL) && (std::abs(y-other.y)<TOL) && (std::abs(z-other.z)<TOL) && color.equals(other.color);
}

/**
 * Checks if an vertex isn't identical to another. To be equal, they have to have the same
 * coordinates(with some tolerance) and color
 * 
 * @param anObject the other vertex to be tested
 * @return true if they are equal, false otherwise. 
 */
bool Vertex::operator!=(const Vertex& other) const
{

	return 	(nextafter(x, INFINITY) < other.x)
		  ||(nextafter(x,-INFINITY) > other.x)
		  ||(nextafter(y, INFINITY) < other.y)
		  ||(nextafter(y,-INFINITY) > other.y)
		  ||(nextafter(z, INFINITY) < other.z)
		  ||(nextafter(z,-INFINITY) > other.z);
}

//--------------------------------------SETS------------------------------------//
	
/**
 * Sets the vertex status
 * 
 * @param status vertex status - UNKNOWN, BOUNDARY, INSIDE or OUTSIDE
 */
void Vertex::setStatus(int status)
{
	if(status>=UNKNOWN && status<=BOUNDARY)
	{
		this->status = status;	
	}
}

//--------------------------------------GETS------------------------------------//

/**
 * Gets the vertex position
 * 
 * @return vertex position
 */
Point3f Vertex::getPosition() const
{
	return {(double)x, (double)y, (double)z};
} 

/**
 * Gets an array with the adjacent vertices
 * 
 * @return array of the adjacent vertices 
 */
const std::vector<int>& Vertex::getAdjacentVertices() const
{
	return adjacentVertexIndexes;
}

/**
 * Gets the vertex status
 * 
 * @return vertex status - UNKNOWN, BOUNDARY, INSIDE or OUTSIDE
 */	
int Vertex::getStatus() const
{
	return status;
}

/**
 * Gets the vertex color
 * 
 * @return vertex color
 */
Colour3f Vertex::getColor() const
{
	return color;
}

//----------------------------------OTHERS--------------------------------------//

/**
 * Sets a vertex as being adjacent to it
 * 
 * @param adjacentVertex an adjacent vertex
 */
void Vertex::addAdjacentVertex(int adjacentVertex)
{
	if(std::find_if(adjacentVertexIndexes.begin(),adjacentVertexIndexes.end(),
		[&adjacentVertex](const int& curr_vertex){return curr_vertex == adjacentVertex;}) == adjacentVertexIndexes.end())
	{
		adjacentVertexIndexes.push_back(adjacentVertex);
	} 
}

/**
 * Sets the vertex status, setting equally the adjacent ones
 * 
 * @param status new status to be set
 */
void Vertex::mark(int status)
{
	//mark vertex
	this->status = status;
	
	if(status == Vertex::UNKNOWN) return ;

	if(solidVertices.size() == 0) return;
	
	//mark adjacent vertices
	for(int adjVertexIdx : adjacentVertexIndexes)
	{
		if(solidVertices[adjVertexIdx].getStatus()==Vertex::UNKNOWN)
		{
			solidVertices[adjVertexIdx].mark(status);
		}
	}
}

int Vertex::id()const{
	for (int idx = 0; idx< solidVertices.size(); ++idx){
		if(solidVertices[idx]!=(*this)) continue;
		return idx;
	}
	return 0;
}
