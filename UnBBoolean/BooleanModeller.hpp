#ifndef __BOOLEAN_MODELLER__
#define __BOOLEAN_MODELLER__

#include <vector>
#include "Helpers/Point3d.hpp"
#include "Internal/Object3D.hpp"
#include "Solid.hpp"


/**
 * Class used to apply bool operations on solids.
 * 
 * <br><br>Two 'Solid' objects are submitted to this class constructor. There is a methods for 
 * each bool operation. Each of these return a 'Solid' resulting from the application
 * of its operation into the submitted solids. 
 *  
 * <br><br>See: D. H. Laidlaw, W. B. Trumbore, and J. F. Hughes.  
 * "Constructive Solid Geometry for Polyhedral Objects" 
 * SIGGRAPH Proceedings, 1986, p.161.
 *  
 * @author Danilo Balby Silva Castanheira (danbalby@yahoo.com)
 * Translated to C++ by akatsia-games on github.com
 */
class BooleanModeller
{
public:
	//--------------------------------CONSTRUCTORS----------------------------------//
	
	BooleanModeller(const Solid& solid1, const Solid& solid2);
				
	//-------------------------------BOOLEAN_OPERATIONS-----------------------------//
	
	Solid getUnion();
	
	Solid getIntersection();
	
	Solid getDifference();
	
private:

	Solid composeSolid(int faceStatus1, int faceStatus2, int faceStatus3);
	
	void groupObjectComponents(Object3D& object, std::vector<Vertex>& vertices, std::vector<int>& indices, std::vector<Colour3f>& colors, int faceStatus1, int faceStatus2);

	/** solid where bool operations will be applied */
	Object3D object1, object2;
};
#endif //__BOOLEAN_MODELLER__