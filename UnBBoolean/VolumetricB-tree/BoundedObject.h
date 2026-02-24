#pragma once
#include<algorithm>
#include<vector>
#include<cmath>

struct Point {
	float x;
	float y;
	float z;

	bool operator>=(const Point& other) const
	{
		return x >= other.x && y >= other.y && z >= other.z;
	}

	void operator+=(const Point& other) {
		x += other.x;
		y += other.y;
		z += other.z;
	}

	void operator-=(const Point& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
	}

	Point operator-(const Point& other) {
		Point res = *this;
		res -= other;
		return res;
	}

	Point operator-() {
		return { -x, -y, -z };
	}

	Point cross(const Point& other) const {
		return { y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x };
	}

};

struct BoundingBox {
	BoundingBox(bool isZero = true, const Point& low = { INFINITY ,INFINITY ,INFINITY }, const Point& high = { -INFINITY ,-INFINITY ,-INFINITY })
		: m_isZero(isZero)
		, m_Low(low)
		, m_High(high)
	{}

	BoundingBox& operator+=(const BoundingBox& other)
	{
		m_Low.x = std::min(m_Low.x, other.m_Low.x);
		m_Low.y = std::min(m_Low.y, other.m_Low.y);
		m_Low.z = std::min(m_Low.z, other.m_Low.z);

		m_High.x = std::max(m_High.x, other.m_High.x);
		m_High.y = std::max(m_High.y, other.m_High.y);
		m_High.z = std::max(m_High.z, other.m_High.z);

		m_isZero = m_isZero && other.m_isZero;

		return *this;
	}

	BoundingBox& operator^=(const BoundingBox& other)
	{
		if (m_High >= other.m_Low && other.m_High >= m_Low)
		{
			m_Low.x = std::max(m_Low.x, other.m_Low.x);
			m_Low.y = std::max(m_Low.y, other.m_Low.y);
			m_Low.z = std::max(m_Low.z, other.m_Low.z);

			m_High.x = std::min(m_High.x, other.m_High.x);
			m_High.y = std::min(m_High.y, other.m_High.y);
			m_High.z = std::min(m_High.z, other.m_High.z);

			m_isZero = m_isZero || other.m_isZero;
		}
		else
		{
			m_isZero = true;
			m_Low = { INFINITY ,INFINITY ,INFINITY };
			m_High = { -INFINITY ,-INFINITY ,-INFINITY };
		}
		return *this;
	}

	float size() const
	{
		return m_isZero? 0 : (m_High.x - m_Low.x) * (m_High.y - m_Low.y) * (m_High.z - m_Low.z);
	}

	BoundingBox epsilonEnlarge()
	{
		if (m_isZero) return *this;

		BoundingBox rvalue;

		rvalue.m_Low.x = std::nextafter(m_Low.x, -INFINITY);
		rvalue.m_Low.y = std::nextafter(m_Low.y, -INFINITY);
		rvalue.m_Low.z = std::nextafter(m_Low.z, -INFINITY);
		
		rvalue.m_High.x = std::nextafter(m_High.x, INFINITY);
		rvalue.m_High.y = std::nextafter(m_High.y, INFINITY);
		rvalue.m_High.z = std::nextafter(m_High.z, INFINITY);

		return rvalue;
	}
	
	void Move(const Point& direction) {
		m_Low += direction;
		m_High += direction;
	}

	bool m_isZero;
	Point m_Low;
	Point m_High;
};

enum class BoundedObjectType {
	BoundingBox = 0,
	BBTree,
	Polygon,
	Triangle,
	Line,
	Point
};

class BoundedObject {
public:
	BoundedObject(BoundedObjectType type = BoundedObjectType::BoundingBox)
				: m_type(type)
	{}

	virtual ~BoundedObject() {}
	
	virtual const BoundingBox& GetBound() const = 0;

	BoundedObjectType GetType()
	{
		return m_type;
	}

private:
	BoundedObjectType m_type;
};

class Polygon : public BoundedObject
{
public:
	Polygon(std::vector<Point>& inputPoints)
		:BoundedObject(BoundedObjectType::Polygon)
	{
		points = inputPoints;
		for (auto& point : points)
		{
			box += BoundingBox(false, point, point);
		}
	}
	virtual ~Polygon() {}

	virtual const BoundingBox& GetBound() const { return box; };
private:
	BoundingBox box;
	std::vector<Point> points;
};

class Triangle : public BoundedObject
{
public:
	Triangle(Point point0, Point point1, Point point2)
		:BoundedObject(BoundedObjectType::Triangle)
	{
		points[0] = point0;
		points[1] = point1;
		points[2] = point2;

		box += BoundingBox(false, point0, point0);
		box += BoundingBox(false, point1, point1);
		box += BoundingBox(false, point2, point2);
	}
	virtual ~Triangle() {}

	virtual const BoundingBox& GetBound() const { return box; };
	Triangle GetFlipped() const { return Triangle(points[0], points[2], points[1]); }
	Point getPoint(int i) const { return points[i]; }
	void move(Point other) { points[0] += other; points[1] += other; points[2] += other; }
private:
	BoundingBox box;
	Point points[3];
};

class Line : public BoundedObject
{
public:
	Line(Point& fst, Point& snd)
		:BoundedObject(BoundedObjectType::Line)
	{
		points[0] = fst;
		points[1] = snd;

		box += BoundingBox(false, fst, fst);
		box += BoundingBox(false, snd, snd);
	}
	virtual ~Line() {}

	virtual const BoundingBox& GetBound() const { return box; };
private:
	BoundingBox box;
	Point points[2];
};

class BoundedPoint : public BoundedObject
{
public:
	BoundedPoint(Point& inputPoint)
		:BoundedObject(BoundedObjectType::Point)
	{
		point = inputPoint;
		box += BoundingBox(false, inputPoint, inputPoint);
	}
	virtual ~BoundedPoint() {}

	virtual const BoundingBox& GetBound() const { return box; };
private:
	BoundingBox box;
	Point point;
};