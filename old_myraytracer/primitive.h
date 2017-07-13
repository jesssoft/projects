//
// 2009 jesssoft
//
#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__

#include <string>
#include "typedefs.h"
#include "material.h"
#include "plane.h"
#include "ray.h"

class Vertex {
public:
	Vertex(const Vector3 &pos, const real_t &u, const real_t &v) {
		pos_ = pos;
		u_ = u;
		v_ = v;
	};
	~Vertex(void) {};

	inline Vector3	&GetNormal(void) { return normal_; };
	inline Vector3	&GetPos(void) { return pos_; };
	inline void	SetNormal(const Vector3 &normal) { 
		normal_ = normal; 
	};

private:
	Vector3	pos_;
	Vector3	normal_;
	real_t	u_;
	real_t	v_;
};

///////////////////////////////////////////////////////////////////////////////
class Primitive {
public:
	Primitive(void);
	virtual ~Primitive(void);

	virtual int	GetType(void) = 0;
	virtual int	OnIntersect(Ray &ray, real_t &dist_limit) = 0;
	virtual Vector3	OnGetNormal(Vector3 &pos) = 0;
	virtual color_t	GetColor(Vector3 &pos) {
		return material_.GetColor(); 
	};
	inline Material		*GetMaterial()	{ return &material_; };
	void			SetName(const char *name);
	inline const std::string
				&GetName(void)	{ return name_; };
	inline bool		IsLight(void)	{ return light_flag_; };
	inline void		SetLightFlag(void) { light_flag_ = true; };
public:
	enum {
		SPHERE,
		PLANE,
		AABB,
		TRIANGLE
	};

protected:
	Material	material_;

private:
	std::string	name_;
	bool		light_flag_;
};

///////////////////////////////////////////////////////////////////////////////
class Sphere : public Primitive {
public:
	Sphere(const Vector3 &centre, const real_t &radius) { 
		centre_ = centre; 
		radius_ = radius; 
		ve_ = Vector3(1, 0, 0);
		vn_ = Vector3(0, 1, 0);
		vc_ = vn_.Cross(ve_);

	};
	virtual ~Sphere(void) {};

	virtual int	GetType(void) override { return SPHERE; };
	virtual int	OnIntersect(Ray &ray, real_t &dist_limit) override;
	virtual Vector3	OnGetNormal(Vector3 &pos) override;
	virtual color_t	GetColor(Vector3 &pos) override;

	inline const Vector3	&GetCentre(void) { return centre_; };
private:
	real_t		radius_;
	Vector3		centre_;
	Vector3		ve_;
	Vector3		vn_;
	Vector3		vc_;
};

///////////////////////////////////////////////////////////////////////////////
class PlanePrim : public Primitive {
public:
	PlanePrim(const Vector3 &normal, const real_t &d) { 
		normal_ = normal; 
		d_ = d; 
		uaxis_ = Vector3(normal_.y, normal_.z, -normal_.x);
		vaxis_ = uaxis_.Cross(normal_);
	};
	virtual ~PlanePrim() {};

	virtual int	GetType(void) override { return PLANE; };
	virtual int	OnIntersect(Ray &ray, real_t &dist_limit) override;
	virtual Vector3	OnGetNormal(Vector3 &pos) override;
	virtual color_t	GetColor(Vector3 &pos) override;

private:
	Vector3		normal_;
	real_t		d_;
	Vector3		uaxis_;
	Vector3		vaxis_;
};

///////////////////////////////////////////////////////////////////////////////
class Triangle : public Primitive {
public:
	Triangle(Vertex *v1, Vertex *v2, Vertex *v3);
	virtual ~Triangle(void) {};

	virtual int	GetType(void) override { return TRIANGLE; };
	virtual int	OnIntersect(Ray &ray, real_t &dist_limit) override;
	virtual Vector3	OnGetNormal(Vector3 &pos) override;
	virtual color_t	GetColor(Vector3 &pos) override;

private:
	Vector3	normal_;
	Vertex	*vertex_[3];
	real_t	u_;
	real_t	v_;
	real_t	nu_;
	real_t	nv_;
	real_t	nd_;
	int	k_;
	real_t	bnu_;
	real_t	bnv_;
	real_t	cnu_;
	real_t	cnv_;
};

#endif

