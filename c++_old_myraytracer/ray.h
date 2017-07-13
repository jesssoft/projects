//
// 2009 jesssoft
//
#ifndef __RAY_H__
#define __RAY_H__

#include "vector3.h"

class Ray {
public:
	Ray(void);
	Ray(Vector3 org, Vector3 &dir);
	~Ray(void);

	inline Vector3	&GetDirection(void)	{ return dir_; };
	inline Vector3	&GetOrigin(void)	{ return org_; };
private:
	Vector3		org_;
	Vector3		dir_;
};

#endif

