#pragma once
#include<iostream>
#include<vector>

//class to have data in the points for the solid processing
class EmptyPointData{
    EmptyPointData();
    EmptyPointData(const EmptyPointData& other);
    ~EmptyPointData();
    inline void read(std::basic_istream<char>& solidFile);
    inline void write(std::basic_istream<char>& solidFile);
    inline bool equals(const EmptyPointData& other);
};