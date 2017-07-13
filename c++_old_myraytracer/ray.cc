//
// 2009 jesssoft
//
#include "ray.h"

Ray::Ray(void)
{
}

Ray::Ray(Vector3 org, Vector3 &dir)
{
	org_ = org;
	dir_ = dir;
};

Ray::~Ray(void)
{
}
