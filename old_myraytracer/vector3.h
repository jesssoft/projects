//
// 2009 jesssoft
//
#ifndef __VECTOR3_H__
#define __VECTOR3_H__

#include "typedefs.h"

class Vector3 {
public:
	union {
		struct { real_t m_x, m_y, m_z; };
		struct { real_t x, y, z; };
		struct { real_t r, g, b; };
		struct { real_t cell[3]; };
		struct { real_t v[3]; };
	};

	Vector3(void) : m_x(0.0f), m_y(0.0f), m_z(0.0f) {};
	Vector3(const real_t &x, const real_t &y, const real_t &z) { 
		m_x = x; 
		m_y = y; 
		m_z = z; 
	};
	~Vector3(void) {};

	Vector3 &operator=(const Vector3 &src) {
		m_x = src.m_x; 
		m_y = src.m_y; 
		m_z = src.m_z; 
		return *this;
	};

	void operator+=(const Vector3 &v) { 
		m_x += v.m_x; 
		m_y += v.m_y; 
		m_z += v.m_z; 
	};
	void operator-=(const real_t &val) { 
		m_x -= val; 
		m_y -= val; 
		m_z -= val; 
	};
	void operator+=(const real_t &val) { 
		m_x += val; 
		m_y += val; 
		m_z += val; 
	};
	void operator*=(const real_t &val) { 
		m_x *= val; 
		m_y *= val; 
		m_z *= val; 
	};

	Vector3 Cross(const Vector3 &v) { 
		return Vector3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x); 
	};
	Vector3 operator-() const { 
		return Vector3(-x, -y, -z); 
	};

	friend Vector3 operator-(const Vector3 &v1, const Vector3 &v2) { 
		return Vector3(v1.m_x - v2.m_x, v1.m_y - v2.m_y,
		    v1.m_z - v2.m_z); 
	};
	friend Vector3 operator+(const Vector3 &v1, const Vector3 &v2) { 
		return Vector3(v1.m_x + v2.m_x, v1.m_y + v2.m_y,
		    v1.m_z + v2.m_z); 
	};
	friend Vector3 operator*(const Vector3 &v1, const Vector3 &v2) { 
		return Vector3(v1.m_x * v2.m_x, v1.m_y * v2.m_y,
		    v1.m_z * v2.m_z); 
	};
	friend Vector3 operator*(const real_t &val, const Vector3 &v) { 
		return Vector3(v.m_x * val, v.m_y * val, v.m_z * val); 
	};
	friend Vector3 operator*(const Vector3 &v, const real_t &val) { 
		return Vector3(v.m_x * val, v.m_y * val, v.m_z * val); 
	};
	friend Vector3 operator/(const Vector3 &v, const real_t &val) { 
		return Vector3(v.m_x / val, v.m_y / val, v.m_z / val); 
	};

	void Normalize(void);
	real_t	Length(void);
	real_t	Dot(Vector3 &v);

	friend real_t Dot(Vector3 &v1, Vector3 &v2) { 
		return v1.m_x*v2.m_x + v1.m_y*v2.m_y + v1.m_z*v2.m_z; 
	};
	friend real_t Dot(Vector3 &v, real_t val) { 
		return v.m_x*val + v.m_y*val + v.m_z*val; 
	};
	friend real_t Dot(real_t val, Vector3 &v) { 
		return v.m_x*val + v.m_y*val + v.m_z*val; 
	};

	void Print(const char *title);
};

typedef Vector3 color_t;

#endif

