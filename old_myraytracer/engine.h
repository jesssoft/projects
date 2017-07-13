//
// 2009 jesssoft
//
#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "scene.h"
#include "surface.h"
#include "typedefs.h"
#include "ray.h"
#include "twister.h"

class Engine {
public:
	Engine(void);
	~Engine(void);

	bool		Init(int width, int height);
	inline Scene	*GetScene(void) { return scene_; };
	inline Surface	*GetSurface(void) { return surface_; };
	void		InitRender(const Vector3 &pos, const Vector3 &target);
	bool		Render(void);

private:
	Surface		*surface_;
	Scene		*scene_;
	Twister		twister_;

	Vector3		origin_;

	Vector3		p1_;
	Vector3		p2_;
	Vector3		p3_;
	Vector3		p4_;

	Vector3		dx_;
	Vector3		dy_;

private:
	void		RayTrace(Ray ray, color_t &acc, int depth,
			    real_t rIndex, real_t &dist);
	int		FindNearest(Ray &ray, real_t &dist, Primitive *&prim,
			    Primitive *skip = nullptr);
	bool		IsBlockingObject(Ray &ray, real_t &dist,
			    Primitive *skip = nullptr);
	real_t		CalcShade(Primitive *pr, Vector3 &pi, Vector3 &dir);
};

#endif

