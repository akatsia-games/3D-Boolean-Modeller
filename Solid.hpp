#ifndef __SOLID__
#define __SOLID__

#include<istream>
#include<vector>
#include"Point3f.hpp"

/*import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.StringTokenizer;
import javax.media.j3d.Shape3D;
import com.sun.j3d.utils.geometry.GeometryInfo;
import com.sun.j3d.utils.geometry.NormalGenerator;*/

/**
 * Class representing a 3D solid.
 *  
 * @author Danilo Balby Silva Castanheira (danbalby@yahoo.com)
 * Translated to C++ by akatsia-games on github.com
 */
class Solid 
{
public:
	Solid();
	
	Solid(std::vector<Point3f>& vertices, std::vector<int>& indices, std::vector<Colour3f>& colors);

	Solid(std::basic_istream<char>& solidFile, Colour3f color);

	const std::vector<Point3f>& getVertices();

	const std::vector<int>& getIndices();

	const std::vector<Colour3f>& getColors();

	bool isEmpty();

	void setData(const std::vector<Point3f>& vertices, const std::vector<int>& indices, const std::vector<Colour3f>& colors);

	void setData(const std::vector<Point3f>& vertices, const std::vector<int>& indices, Colour3f color);

	void translate(double dx, double dy);

	void rotate(double dx, double dy);

	void zoom(double dz);

	void scale(double dx, double dy, double dz);

protected:

	void setInitialFeatures();

	void defineGeometry();

	void loadCoordinateFile(std::basic_istream<char>& solidFile, Colour3f color);

	Point3f getMean();
	
	/** array of indices for the vertices from the 'vertices' attribute */
	std::vector<int> indices;
	/** array of points defining the solid's vertices */
	std::vector<Point3f> vertices;
	/** array of color defining the vertices colors */
	std::vector<Colour3f> colors;
};

#endif //__SOLID__