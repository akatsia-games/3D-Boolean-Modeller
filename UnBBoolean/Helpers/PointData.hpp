#pragma once
namespace std
{
    template<class type>
    class basic_istream;   
}

/*
EmptyPointData is a class/interface that shows how Solids hold data per vertex

The functions are intentionally not virtual to avoid using the dynamic function dispatcher
*/
class EmptyPointData{
    EmptyPointData(){}
    EmptyPointData(const EmptyPointData& other){}
    ~EmptyPointData(){}
    inline void read(std::basic_istream<char>& file){}
    inline void write(std::basic_istream<char>& file){}
    inline bool equals(const EmptyPointData& other){return true;}
};