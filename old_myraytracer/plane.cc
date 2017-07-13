//
// 2009 jesssoft
//
#include "plane.h"

Plane::Plane(Vector3 &normal, float d)
{
	normal_ = normal;
	d_ = d;
}

Plane::~Plane(void)
{
}
