//
// 2009 jesssoft
//
#ifndef __PLANE_H__
#define __PLANE_H__

#include "vector3.h"

class Plane {
public:
	Plane(Vector3 &normal, float d);
	~Plane(void);

	Vector3	normal_;
	float	d_;
};

#endif

