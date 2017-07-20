//
// 2009 jesssoft
//
#include <iostream>
#include "vector3.h"
#include "math.h"

using namespace std;

void 
Vector3::Normalize(void)
{
	real_t length = Length();

	if (length == 0)
		return;

	m_x /= length;
	m_y /= length;
	m_z /= length;
}

real_t 
Vector3::Length(void)
{
	return (real_t)sqrt(m_x*m_x + m_y*m_y + m_z*m_z);;
}

real_t 
Vector3::Dot(Vector3& v)
{
	return m_x*v.m_x + m_y*v.m_y + m_z*v.m_z;
}

void 
Vector3::Print(const char *title)
{
	const char *heading = title;

	if (!heading)
		heading = "";

	cout << heading << "(" 
	    << m_x << ", " 
	    << m_y << ", " 
	    << m_z << ")" << endl;
}

