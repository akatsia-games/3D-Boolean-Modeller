#ifndef __COLOUR3F__
#define __COLOUR3F__

class basic_istream;   

class Colour3f
{
public:
    float r;
    float g;
    float b;

    inline void read(std::basic_istream<char>& file);
    inline void write(std::basic_ostream<char>& file) const;
    inline bool equals(const Colour3f& other) const;

private:
	/** tolerance value to test equalities */
	constexpr static const double TOL = 3e-5f;
};

#endif// __COLOUR3F__