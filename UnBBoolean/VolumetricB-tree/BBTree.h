#pragma once
#include "BoundedObject.h"
#include <vector>
#include <memory>
#include <bitset>
#include <map>
#include <fstream>

template <typename T=BoundedObject, int k=10>
class BBTree : public BoundedObject
{
public:
	BBTree()
		: BoundedObject(BoundedObjectType::BBTree)
		, m_IsLeafLevel(true)
		, m_Parent(nullptr)
	{
		m_Objects.reserve(k);
	}
	virtual ~BBTree() {}

	virtual const BoundingBox& GetBound() const { return m_TotalBounds; };
	
	void add(std::shared_ptr<BoundedObject> object)
	{
		auto& objBound = object->GetBound();
		m_TotalBounds += objBound;

		//if it's a leaf, insert straight away
		if (m_IsLeafLevel)
		{
			m_Objects.push_back(object);
		}
		else
		{
			int foundIdx = 0;
			float minAdditionalVolume = INFINITY;
			int maxChildren = 0;
			for (int idx = 0 ; idx<m_Objects.size() ; ++idx)
			{
				BoundingBox original = m_Objects[idx]->GetBound();
				BoundingBox newBound = original; newBound += objBound;
				float totalAddedIntersection = 0;
				for (int idx1 = 0; idx1 < m_Objects.size(); ++idx1)
				{
					if (idx1 == idx) continue;
					auto res1 = original;
					auto res2 = newBound;
					res1 ^= m_Objects[idx1]->GetBound();
					res2 ^= m_Objects[idx1]->GetBound();
					totalAddedIntersection += res2.size() - res1.size();
				}
				totalAddedIntersection = std::max(totalAddedIntersection, newBound.size() - original.size());
				if ( totalAddedIntersection < minAdditionalVolume || (totalAddedIntersection == 0 && maxChildren < ((BBTree<T, k>*) m_Objects[foundIdx].get())->m_Objects.size()))
				{
					foundIdx = idx;
					minAdditionalVolume = totalAddedIntersection;
					maxChildren = ((BBTree<T, k>*) m_Objects[foundIdx].get())->m_Objects.size();
				}
			}
			((BBTree<T, k>*) m_Objects[foundIdx].get())->add(object);
		}
		//if it's not a leaf, find which child node will accept the current object with minimal additional volume and insert it there

		// if the size isn't k, there's no need to split the current object
		if (m_Objects.size() < k)
		{
			return;
		}
		
		// find the best split of the objects in the current tree node (this alogirhm may not be that good as it's O(2^k)
		float minimalIntersection = INFINITY;
		std::bitset<k> minimalSplit;

		for (int i = 0; i < 1<<(k+1); ++i)
		{
			std::bitset<k> takenElems(i);
			if (takenElems.count() != k / 2) continue;

			BoundingBox ltBox;
			BoundingBox rtBox;

			for (int j = 0; j < k; ++j)
			{
				if (takenElems[j])
				{
					ltBox += m_Objects[j]->GetBound();
				}
				else
				{
					rtBox += m_Objects[j]->GetBound();
				}
			}
			ltBox ^= rtBox;
			if ((ltBox.size()) < minimalIntersection)
			{
				minimalIntersection = ltBox.size();
				minimalSplit = takenElems;
			}
		}

		std::vector<std::shared_ptr<BoundedObject>> ltObjects; ltObjects.reserve(k);
		std::shared_ptr<BoundedObject> rtObject(new BBTree<T, k>());
		std::vector<std::shared_ptr<BoundedObject>>& rtObjects = ((BBTree<T, k>*)rtObject.get())->m_Objects;
		((BBTree<T, k>*)rtObject.get())->m_IsLeafLevel = m_IsLeafLevel;
		((BBTree<T, k>*)rtObject.get())->m_Parent = m_Parent;
		
		//insert objects that are true in minimalSplit to ltObjects and false to the rtObjects (they also get assigned their parent if needed)
		for (int i = 0; i < k; ++i)
		{
			if (minimalSplit[i])
			{
				ltObjects.push_back(m_Objects[i]);
			}
			else
			{
				rtObjects.push_back(m_Objects[i]);
				((BBTree<T, k>*)rtObject.get())->m_TotalBounds += m_Objects[i]->GetBound();
				if (!m_IsLeafLevel)
				{
					((BBTree<T, k>*)m_Objects[i].get())->m_Parent = ((BBTree<T, k>*)rtObject.get());
				}
			}
		}


		// is the root -> current object stays and children are made from the split of the current object
		if (!m_Parent)
		{
			std::shared_ptr<BoundedObject> ltObject(new BBTree<T, k>());
			((BBTree<T, k>*)ltObject.get())->m_IsLeafLevel = m_IsLeafLevel;
			
			for (auto& object : ltObjects)
			{
				((BBTree<T, k>*)ltObject.get())->m_TotalBounds += object->GetBound();
				if (!m_IsLeafLevel) ((BBTree<T, k>*)object.get())->m_Parent = (BBTree<T, k>*)ltObject.get();
			}

			std::swap(((BBTree<T, k>*)ltObject.get())->m_Objects, ltObjects);

			// add objects and change their parent
			((BBTree<T, k>*)rtObject.get())->m_Parent = this;
			((BBTree<T, k>*)ltObject.get())->m_Parent = this;
			m_Objects.clear();
			m_Objects.push_back(ltObject);
			m_Objects.push_back(rtObject);
			m_IsLeafLevel = false;
		}
		else
		{
			std::swap(m_Objects, ltObjects);
			m_TotalBounds = BoundingBox();
			for (auto& object : m_Objects)
			{
				m_TotalBounds += object->GetBound();
			}
			m_Parent->m_Objects.push_back(rtObject);
		}
		// is not the root -> current object is split and the leftover objects are moved to a new child to the parent of this object
	}

	void FillVolume(std::map<int, float>& totalVolumes, int level = 1) const
	{
		for (auto& object : m_Objects)
		{
			totalVolumes[level] += object->GetBound().size();
			if (!m_IsLeafLevel) ((BBTree<T, k>*)object.get())->FillVolume(totalVolumes, level + 1);
		}
	}
	
	void write(std::map<int, std::pair<int, std::ofstream>>& streams, int level = 1) const
	{
		for (auto& object : m_Objects)
		{
			if (!m_IsLeafLevel) ((BBTree<T, k>*)object.get())->write(streams, level + 1);
			int& firstNode = streams[level].first;
			std::ofstream& file = streams[level].second;
			int a(firstNode+1), 
				a1(firstNode+2), 
				b( firstNode+3), 
				b1(firstNode+4), 
				d( firstNode+5), 
				d1(firstNode+6), 
				c( firstNode+7),
				c1(firstNode+8);
			firstNode += 8;
			
			auto& bound = object->GetBound();
			bound.m_High;
			bound.m_Low;
			for (int i = 0; i < 8; ++i)
			{
				file << "v "<<(i&1? bound.m_Low : bound.m_High).x 
					 << " " << (i & 2 ? bound.m_Low : bound.m_High).y 
					 << " " << (i & 4 ? bound.m_Low : bound.m_High).z <<" " << std::endl;
			}

			int exportType = 2;

			if (exportType == 1)
			{
				file << "f " << a << " " << b << " " << d << " " << std::endl;
				file << "f " << a << " " << b << " " << a1 << " " << std::endl;
				file << "f " << a << " " << a1 << " " << d << " " << std::endl;

				file << "f " << b1 << " " << c1 << " " << d1 << " " << std::endl;
				file << "f " << b1 << " " << c << " " << c1 << " " << std::endl;
				file << "f " << c1 << " " << d1 << " " << c << " " << std::endl;
			}
			else if (exportType == 2)
			{
				file << "f " << a << " " << b << " " << c << " " << d << " " << std::endl;
				file << "f " << a1 << " " << b1 << " " << c1 << " " << d1 << " " << std::endl;
				file << "f " << a << " " << b << " " << b1 << " " << a1 << " " << std::endl;
				file << "f " << b << " " << c << " " << c1 << " " << b1 << " " << std::endl;
				file << "f " << c << " " << d << " " << d1 << " " << c1 << " " << std::endl;
				file << "f " << d << " " << a << " " << a1 << " " << d1 << " " << std::endl;
			}
			else if (exportType == 3)
			{
				file << "l " << a << " " << b << std::endl;
				file << "l " << b << " " << c << std::endl;
				file << "l " << c << " " << d << std::endl;
				file << "l " << d << " " << a << std::endl;

				file << "l " << a1 << " " << b1 << std::endl;
				file << "l " << b1 << " " << c1 << std::endl;
				file << "l " << c1 << " " << d1 << std::endl;
				file << "l " << d1 << " " << a1 << std::endl;

				file << "l " << a << " " << a1 << std::endl;
				file << "l " << b << " " << b1 << std::endl;
				file << "l " << c << " " << c1 << std::endl;
				file << "l " << d << " " << d1 << std::endl;
			}
		}
	}

	void FindAll(const BoundingBox& searchBound, std::vector<std::shared_ptr<BoundedObject>>& out) const {
		for (auto& obj : m_Objects) {
			BoundingBox box = obj->GetBound();
			box ^= searchBound;
			if (!box.m_isZero) {
				if (m_IsLeafLevel) {
					out.push_back(obj);
				}
				else {
					((BBTree<T, k>*)obj)->FindAll(searchBound, out);
				}
			}
		}
	}

	void dump(std::vector<std::shared_ptr<BoundedObject>>& output) const {
		if (m_IsLeafLevel) {
			for (auto& obj : m_Objects) {
				output.push_back(obj);
			}
			return;
		}
		
		for (auto& obj : m_Objects) {
			((BBTree<T, k>*)obj)->dump(output);
		}
	}
private:
	std::vector<std::shared_ptr<BoundedObject>> m_Objects;
	BBTree<T, k>* m_Parent;
	bool m_IsLeafLevel;
	BoundingBox m_TotalBounds;
};