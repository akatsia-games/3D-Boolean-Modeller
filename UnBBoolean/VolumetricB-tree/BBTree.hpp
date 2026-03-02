#pragma once

#include "UnBBoolean/Helpers/Bound.hpp"

#include <vector>
#include <memory>
#include <bitset>
#include <map>

template <class T, int k=10>
class BBTree
{
    class BBIterator{
        int current_element;
        int current_node;
    protected:
        BBIterator()
    public:
        BBIterator& operator++();
        T& operator*();
        int getElemId();
    }

    class BBNode{
    protected:
        int parent;
        int height;
        int elements[k];
        int size;
        Bound boundingBox;
    public:
        bool isLeaf();
        int size();
        Bound getBound();
    }
public:
    BBNode& root();
    BBNode& getNode();

    BBIterator begin();
    BBIterator end();

    BBIterator find(const T& object);
    BBIterator add(const T& object);

    BBIterator add(const T& object, int nodeHint);
    BBIterator add(const std::vector<T>& object, int nodeHint);

    //id-s are consistent if elements are not removed from the objects list
    T& get(int id);
    
    void remove(const T& object);
    void remove(int id);

    void findAll(Bound search, std::vector<T>& result);
    Point3d findIntersection(Point3d starting, Vector3d direction);
    Point3d findIntersection(Point3d starting, Vector3d direction);

    
    void findAllIntersecting(const T& search, std::vector<T>& result);
    void findAllIntersecting(const BBTree<T>& searchTree, std::vector<T>& result);
private:
    //root id is always 0
    //if a node splits, it stays where it is, 

    std::vector<T> m_objects;
    std::vector<BBNode<T,k>> m_nodes;

};

#include "BBTree.cpp.hpp"