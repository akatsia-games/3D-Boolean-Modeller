#include"Solid.hpp"

/**
 * Class representing a 3D solid.
 *  
 * @author Danilo Balby Silva Castanheira (danbalby@yahoo.com)
 * Translated to C++ by akatsia-games on github.com
 */

//--------------------------------CONSTRUCTORS----------------------------------//

/** Constructs an empty solid. */			
Solid::Solid()
{
	setInitialFeatures();
}

/**
 * Construct a solid based on data arrays. An exception may occur in the case of 
 * abnormal arrays (indices making references to inexistent vertices, there are less
 * colors than vertices...)
 * 
 * @param vertices array of points defining the solid vertices
 * @param indices array of indices for a array of vertices
 * @param colors array of colors defining the vertices colors 
 */
Solid::Solid(std::vector<Point3f>& vertices, std::vector<int>& indices, std::vector<Colour3f>& colors)
	:Solid()
{
	setData(vertices, indices, colors);		
}

/**
 * Constructs a solid based on a coordinates file. It contains vertices and indices, 
 * and its format is like this:
 * 
 * <br><br>4
 * <br>0 -5.00000000000000E-0001 -5.00000000000000E-0001 -5.00000000000000E-0001
 * <br>1  5.00000000000000E-0001 -5.00000000000000E-0001 -5.00000000000000E-0001
 * <br>2 -5.00000000000000E-0001  5.00000000000000E-0001 -5.00000000000000E-0001
 * <br>3  5.00000000000000E-0001  5.00000000000000E-0001 -5.00000000000000E-0001
 * 
 * <br><br>2
 * <br>0 0 2 3
 * <br>1 3 1 0 
 * 
 * @param solidFile file containing the solid coordinates
 * @param color solid color
 */
Solid::Solid(std::basic_istream<char>& solidFile, Colour3f color)
	:Solid()
{
	loadCoordinateFile(solidFile, color);
}

/** Sets the initial features common to all constructors */
void Solid::setInitialFeatures()
{	
	/*setCapability(Shape3D.ALLOW_GEOMETRY_WRITE);
	setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);
	setCapability(Shape3D.ALLOW_APPEARANCE_READ);*/
}
	
//---------------------------------------GETS-----------------------------------//

/**
 * Gets the solid vertices
 * 
 * @return solid vertices
 */	
const std::vector<Point3f>& Solid::getVertices() const
{
	return vertices;
}
std::vector<Point3f>& Solid::getVertices()
{
	return vertices;
}

/** Gets the solid indices for its vertices
 * 
 * @return solid indices for its vertices
 */
const std::vector<int>& Solid::getIndices() const
{
	return indices;
}
std::vector<int>& Solid::getIndices()
{
	return indices;
}

/** Gets the vertices colors
 * 
 * @return vertices colors
 */
const std::vector<Colour3f>& Solid::getColors() const
{
	return colors;
}

std::vector<Colour3f>& Solid::getColors()
{
	return colors;
}

/**
 * Gets if the solid is empty (without any vertex)
 * 
 * @return true if the solid is empty, false otherwise
 */
bool Solid::isEmpty() const
{
	return indices.size()==0;
}	


double Solid::signedTriangleVolume(Point3f p1, Point3f p2, Point3f p3)
{
    double v321 = p3.x*p2.y*p1.z;
    double v231 = p2.x*p3.y*p1.z;
    double v312 = p3.x*p1.y*p2.z;
    double v132 = p1.x*p3.y*p2.z;
    double v213 = p2.x*p1.y*p3.z;
    double v123 = p1.x*p2.y*p3.z;
    return (1.0/6.0)*(-v321 + v231 + v312 - v132 - v213 + v123);
}

/**
 * Gets the volume of the solid
 * 
 * @return the volume of the solid
 */
double Solid::getVolume() const
{
    double volume = 0;
	for(int i = 0; i<indices.size(); i+=3)
	{
		volume += signedTriangleVolume(vertices[indices[i]], vertices[indices[i+1]], vertices[indices[i+2]]);
	}
    return std::abs(volume);
}

//---------------------------------------SETS-----------------------------------//

/**
 * Sets the solid data. Each vertex may have a different color. An exception may 
 * occur in the case of abnormal arrays (e.g., indices making references to  
 * inexistent vertices, there are less colors than vertices...)
 * 
 * @param vertices array of points defining the solid vertices
 * @param indices array of indices for a array of vertices
 * @param colors array of colors defining the vertices colors 
 */
void Solid::setData(const std::vector<Point3f>& vertices, const std::vector<int>& indices, const std::vector<Colour3f>& colors)
{
	this->vertices = vertices;
	this->colors = colors;
	this->indices = indices;

	defineGeometry();
}

/**
 * Sets the solid data. Defines the same color to all the vertices. An exception may 
 * may occur in the case of abnormal arrays (e.g., indices making references to  
 * inexistent vertices...)
 * 
 * @param vertices array of points defining the solid vertices
 * @param indices array of indices for a array of vertices
 * @param color the color of the vertices (the solid color) 
 */
void Solid::setData(const std::vector<Point3f>& vertices, const std::vector<int>& indices, Colour3f color)
{
	std::vector<Colour3f> colors(vertices.size(),color);
	setData(vertices, indices, colors);
}

//-------------------------GEOMETRICAL_TRANSFORMATIONS-------------------------//

/**
 * Applies a translation into a solid
 * 
 * @param dx translation on the x axis
 * @param dy translation on the y axis
 */
void Solid::translate(double dx, double dy, double dz)
{
	if(dx!=0||dy!=0)
	{
		for(int i=0;i<vertices.size();i++)
		{
			vertices[i].x += dx;
			vertices[i].y += dy;
			vertices[i].z += dz;
		}
		
		defineGeometry();
	}
}

/**
 * Applies a rotation into a solid
 * 
 * @param dx rotation on the x axis
 * @param dy rotation on the y axis
 */
void Solid::rotate(double dx, double dy)
{
	double cosX = cos(dx);
	double cosY = cos(dy);
	double sinX = sin(dx);
	double sinY = sin(dy);
				
	if(dx!=0||dy!=0)
	{
		//get mean
		Point3f mean = getMean();
		
		double newX, newY, newZ;
		for(int i=0;i<vertices.size();i++)
		{
			vertices[i].x -= mean.x; 
			vertices[i].y -= mean.y; 
			vertices[i].z -= mean.z; 
			
			//x rotation
			if(dx!=0)
			{
				newY = vertices[i].y*cosX - vertices[i].z*sinX;
				newZ = vertices[i].y*sinX + vertices[i].z*cosX;
				vertices[i].y = newY;
				vertices[i].z = newZ;
			}
			
			//y rotation
			if(dy!=0)
			{
				newX = vertices[i].x*cosY + vertices[i].z*sinY;
				newZ = -vertices[i].x*sinY + vertices[i].z*cosY;
				vertices[i].x = newX;
				vertices[i].z = newZ;
			}
									
			vertices[i].x += mean.x; 
			vertices[i].y += mean.y; 
			vertices[i].z += mean.z;
		}
	}
	
	defineGeometry();
}

/**
 * Applies a zoom into a solid
 * 
 * @param dz translation on the z axis
 */
void Solid::zoom(double dz)
{
	if(dz!=0)
	{
		for(int i=0;i<vertices.size();i++)
		{
			vertices[i].z += dz;
		}

		defineGeometry();
	}
}

/**
 * Applies a scale changing into the solid
 * 
 * @param dx scale changing for the x axis 
 * @param dy scale changing for the y axis
 * @param dz scale changing for the z axis
 */
void Solid::scale(double dx, double dy, double dz)
{
	for(int i=0;i<vertices.size();i++)
	{
		vertices[i].x*=dx;
		vertices[i].y*=dy;
		vertices[i].z*=dz;
	}
	
	defineGeometry();
}

Vector3f Solid::intersectRay(Vector3f position, Vector3f direction)
{//work in progress
	if(position.y > 1 != direction.y < 0){
		return {NAN,NAN,NAN};
	}
	float k = (1-position.y)/direction.y;
	return position + direction*k;
}

//-----------------------------------PRIVATES--------------------------------//
		
/** Creates a geometry based on the indexes and vertices set for the solid */
void Solid::defineGeometry()
{
	/*GeometryInfo gi = new GeometryInfo(GeometryInfo.TRIANGLE_ARRAY);
	gi.setCoordinateIndices(indices);
	gi.setCoordinates(vertices);
	NormalGenerator ng = new NormalGenerator();
	ng.generateNormals(gi);
	
	gi.setColors(colors);
	gi.setColorIndices(indices);
	gi.recomputeIndices();
	
	setGeometry(gi.getIndexedGeometryArray());*/
}

/**
 * Loads a coordinates file, setting vertices and indices 
 * 
 * @param solidFile file stream used to create the solid
 * @param color solid color
 */
void Solid::loadCoordinateFile(std::basic_istream<char>& solidFile, Colour3f color)
{
	int numVertices;
	solidFile>>numVertices;

	vertices.resize(numVertices);
				
	for(int i=0;i<numVertices;i++)
	{
		float x,y,z;
		solidFile>>x>>y>>z;
		vertices[i]= {(float)x, (float)y, (float)z};
	}
	
	int numTriangles;
	solidFile>>numTriangles;

	indices.resize(numTriangles*3);
				
	for(int i=0,j=0;i<numTriangles*3;i=i+3,j++)
	{
		solidFile>>indices[i];
		solidFile>>indices[i+1];
		solidFile>>indices[i+2];
	}
	
	colors.resize(0);
	colors.resize(vertices.size(),color);
	
	defineGeometry();
}

/**
 * Writes a coordinates file
 * 
 * @param solidFile file stream
 */
void Solid::write(std::basic_ostream<char>& solidFile) const
{
	solidFile<<vertices.size()<<std::endl;
				
	for(int i=0;i<vertices.size();i++)
	{
		auto& vertex = vertices[i];
		solidFile<<vertex.x<<" "<<vertex.y<<" "<<vertex.z<<std::endl;
	}
	
	solidFile<<indices.size()/3<<std::endl;
				
	for(int i=0,j=0;i<(indices.size()/3)*3;i=i+3,j++)
	{
		solidFile<<indices[i]<<" ";
		solidFile<<indices[i+1]<<" ";
		solidFile<<indices[i+2]<<std::endl;
	}
}

/**
 * Gets the solid mean
 * 
 * @return point representing the mean
 */
Point3f Solid::getMean()
{
	Point3f mean({0,0,0});
	for(int i=0;i<vertices.size();i++)
	{
		mean.x += vertices[i].x;
		mean.y += vertices[i].y;
		mean.z += vertices[i].z;
	}
	mean.x /= vertices.size();
	mean.y /= vertices.size();
	mean.z /= vertices.size();
	
	return mean;
}