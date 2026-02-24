#pragma once
#include "BBTree.h"

class SolidObject {

	SolidObject() {}
	~SolidObject() {}
	void MoveTo(const Point& destination) {
		location = destination;
	}
	void Move(const Point& direction) {
		location += direction;
	}

	//void ReadObject();
	SolidObject Union(const SolidObject& other) const{
		std::vector<Triangle> result;
		Split(other, &result, nullptr, &result, nullptr);

		SolidObject rval;
		rval.location = location;

		for (Triangle triangle : result) {
			rval.objectData.add(std::shared_ptr<BoundedObject>(new Triangle(triangle)));
		}

		return rval;
	}

	SolidObject Intersect(const SolidObject& other)const {
		std::vector<Triangle> result;
		Split(other, nullptr, &result, nullptr, &result);

		SolidObject rval;
		rval.location = location;

		for (Triangle triangle : result) {
			rval.objectData.add(std::shared_ptr<BoundedObject>(new Triangle(triangle)));
		}

		return rval;
	}

	SolidObject Subtract(const SolidObject& other)const {
		std::vector<Triangle> normal;
		std::vector<Triangle> flipped;
		Split(other, &normal, nullptr, nullptr, &flipped);

		SolidObject rval;
		rval.location = location;

		for (Triangle triangle : normal) {
			rval.objectData.add(std::shared_ptr<BoundedObject>(new Triangle(triangle)));
		}
		for (Triangle triangle : flipped) {
			rval.objectData.add(std::shared_ptr<BoundedObject>(new Triangle(triangle.GetFlipped())));
		}

		return rval;
	}

private:
	void Sort(const SolidObject& other,
		std::vector<Triangle>* thisExternal, std::vector<Triangle>* thisInternal, BBTree<Triangle>& thisCrossing,
		std::vector<Triangle>* otherExternal, std::vector<Triangle>* otherInternal, BBTree<Triangle>& otherCrossing)const;

	void Split(const SolidObject& other,
		std::vector<Triangle>* thisExternal, std::vector<Triangle>* thisInternal,
		std::vector<Triangle>* otherExternal, std::vector<Triangle>* otherInternal)const 
	{
		BBTree<Triangle> thisCrossing;
		BBTree<Triangle> otherCrossing;

		Sort(other, thisExternal, thisInternal, thisCrossing, otherExternal, otherInternal, otherCrossing);

		std::vector<std::shared_ptr<BoundedObject>> thisCrossingVector;
		std::vector<std::shared_ptr<BoundedObject>> otherCrossingVector;

		thisCrossing.dump(thisCrossingVector);
		otherCrossing.dump(otherCrossingVector);

		if (thisExternal) {
			for (auto& obj : thisCrossingVector) {
				Triangle triangle = *((Triangle*)obj.get());
				std::vector<std::shared_ptr<BoundedObject>> checkingTriangles;
				otherCrossing.FindAll(triangle.GetBound(), checkingTriangles);
				
				Point offset = triangle.getPoint(0);
				Point plusX = triangle.getPoint(1) - offset;
				Point plusY = triangle.getPoint(2) - offset;
				Point plusZ = plusX.cross(plusY);

				Mat3 forward = Mat3(plusX, plusY, plusZ);
				Mat3 backward = forward.invert();

				Triangle transformed = Triangle({ 0,0,0 }, { 1,0,0 }, { 0,1,0 });
				std::vector<Triangle> transformedChecking;
				for (auto& checkingObj : checkingTriangles) {
					transformedChecking.push_back( *((Triangle*)checkingObj.get()));
					transformedChecking.back().move(-offset);
					transformedChecking.back().transform(backward);
				}

				std::vector<Line> contour;

				for()
			}
		}
	}

	Point location;
	BBTree<Triangle> objectData;
};