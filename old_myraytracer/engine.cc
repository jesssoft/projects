//
// 2009 jesssoft
//
#include <iostream>
#include "engine.h"
#include "util_macro.h"
#include "math.h"
#include "matrix.h"

using namespace std;

const real_t kSScale = 1.0f / SAMPLES;

Engine::Engine(void)
{
	surface_ = nullptr;
	scene_ = new Scene();
}

Engine::~Engine(void)
{
	if (scene_) {
		delete scene_;
		scene_ = nullptr;
	}

	if (surface_) {
		delete surface_;
		surface_ = nullptr;
	}
}

bool
Engine::Init(int width, int height)
{
	if (surface_)
		return false;

	surface_ = new Surface(width, height);
	scene_->Init();

	return true;
}

void 
Engine::InitRender(const Vector3 &pos, const Vector3 &target)
{
	Vector3 up(0, 1, 0);
	Vector3 zaxis = target - pos;
	Vector3 xaxis = up.Cross(zaxis);
	Vector3 yaxis = xaxis.Cross(-zaxis);

	xaxis.Normalize();
	yaxis.Normalize();
	zaxis.Normalize();

	Matrix mat;
	mat.m[0] = xaxis.x; 
	mat.m[1] = xaxis.y; 
	mat.m[2] = xaxis.z;

	mat.m[4] = yaxis.x; 
	mat.m[5] = yaxis.y; 
	mat.m[6] = yaxis.z;

	mat.m[8] = zaxis.x; 
	mat.m[9] = zaxis.y; 
	mat.m[10] = zaxis.z;

	mat.Invert();
	
	mat.m[3] = pos.x; 
	mat.m[7] = pos.y; 
	mat.m[11] = pos.z;

	float ratio = surface_->GetHeight()/(float)surface_->GetWidth();
	float distance = 1;

	// Local view rectangle
	/*
		p1_	p2_

		p4_	p3_
	 */
	p1_ = Vector3(-1,  ratio, 0);
	p2_ = Vector3( 1,  ratio, 0);
	p3_ = Vector3( 1, -ratio, 0);
	p4_ = Vector3(-1, -ratio, 0);

	// Local origine
	origin_ = Vector3(0, 0, -4);

	//
	// Local coordinates -> Global coordinates
	//
	origin_ = mat.Transform(Vector3(0, 0, -distance));
	p1_ = mat.Transform(p1_);
	p2_ = mat.Transform(p2_);
	p3_ = mat.Transform(p3_);
	p4_ = mat.Transform(p4_);

	//
	// Calcuate the deta values
	//
	dx_ = (p2_ - p1_) / surface_->GetWidth();
	dy_ = (p4_ - p1_) / surface_->GetHeight();

	//
	// Debug information
	//
	origin_.Print("origin: ");
	xaxis.Print("xaxis: ");
	yaxis.Print("yaxis: ");
	zaxis.Print("zaxis: ");
}

bool 
Engine::Render()
{
	for (int y = 0; y < surface_->GetHeight(); y++) {
		Vector3 target_pos = p1_ + y * dy_;
		for (int x = 0; x < surface_->GetWidth(); x++) {
			color_t acc(0.0f, 0.0f, 0.0f);
			real_t dist = 0;

			//
			// Direction
			//
			Vector3 dir = target_pos - origin_;
			dir.Normalize();

			//
			// Cast a ray
			//
			Ray ray(origin_, dir);
			RayTrace(ray, acc, 1, 1.0f, dist);

			//
			// Update the result pixel
			//
			surface_->PutPixel(x, y, acc);

			//
			// Next target position
			//
			target_pos += dx_;
		}
		cout << "." << flush;
	}
	cout << endl;

	return true;
}

int
Engine::FindNearest(Ray &ray, real_t &dist, Primitive *&prim, Primitive *skip)
{
	int result = MISS;

	for (int i = 0; i < scene_->GetPrimitiveCount(); i++) {
		Primitive *cur_prim = scene_->GetPrimitive(i);
		if (skip == cur_prim)
			continue;

		int ret = cur_prim->OnIntersect(ray, dist);
		if (ret != MISS) {
			prim = cur_prim;
			result = ret;
		}
	}

	return result;
}

bool
Engine::IsBlockingObject(Ray &ray, real_t &dist, Primitive *skip)
{
	for (int i = 0; i < scene_->GetPrimitiveCount(); i++) {
		Primitive *pr = scene_->GetPrimitive(i);
		if (skip == pr)
			continue;
		if (pr->OnIntersect(ray, dist))
			return true;		
	}

	return false;
}

real_t
Engine::CalcShade(Primitive *pr, Vector3 &pi, Vector3 &dir)
{
	real_t shade = 1.0f;

	if (!pr->IsLight())
		return shade;

	Primitive *light = pr;

	// shade
	if (light->GetType() == Primitive::SPHERE) {
		dir = ((Sphere *)light)->GetCentre() - pi;
		real_t dist = dir.Length();
		dir.Normalize();
		Vector3 newOrg = pi + dir*EPSILON;
		Ray r(newOrg, dir);

		if (IsBlockingObject(r, dist, light))
			shade = 0.3f;
	}

	return shade;
}

void
Engine::RayTrace(Ray ray, color_t &acc, int depth, real_t rIndex,
    real_t &dist_limit)
{
	int result;
	Primitive *prim = nullptr;

	if (depth > TRACEDEPTH)
		return;

	dist_limit = 1000000.0f;

	result = FindNearest(ray, dist_limit, prim);
	if (result == MISS)
		return;

	if (prim->IsLight()) {
		acc = Vector3(1, 1, 1);
		return;
	} 

	Vector3 pi = ray.GetOrigin() + ray.GetDirection() * dist_limit;
	color_t color = prim->GetColor(pi);

	for (int i = 0; i < scene_->GetPrimitiveCount(); i++) {
		Primitive *light = scene_->GetPrimitive(i);
		Vector3 L;
		real_t shade = CalcShade(light, pi, L);
		if (shade > 0.0f) {
			//
			// diffuse
			//
			Vector3 N = prim->OnGetNormal(pi);
			if (prim->GetMaterial()->GetDiffuse() > 0) {
				real_t dot = Dot(L, N);
				if (dot > 0) {
					real_t diffuse = 
					    prim->GetMaterial()->GetDiffuse();
					real_t diff = 
					    dot * diffuse * shade;

					acc += diff * color * 
					    light->GetMaterial()->GetColor();
				}
			}

			//
			// specular
			//
			if (prim->GetMaterial()->GetSpecular() > 0) {
				Vector3 V = ray.GetDirection();
				Vector3 R = L - 2.0f*Dot(L, N)*N;
				real_t dot = Dot(V, R);

				if (dot > 0) {
					real_t spec = pow(dot, 10) *
					    prim->GetMaterial()->GetSpecular() *
					        shade;
					acc += spec * 
					    light->GetMaterial()->GetColor();
				}
			}
		}
	}

	//
	// reflection
	//
	real_t refl = prim->GetMaterial()->GetReflection();
	if ((refl > 0) && (depth < TRACEDEPTH)) {
		real_t drefl = prim->GetMaterial()->GetDiffuseRefl();
		if ((drefl > 0) && (depth < 2)) {
			Vector3 N = prim->OnGetNormal(pi);
			Vector3 RP = ray.GetDirection() - 
			    2.0f * Dot(ray.GetDirection(), N)*N;
			Vector3 RN1 = Vector3(RP.z, RP.y, -RP.x);
			Vector3 RN2 = RP.Cross(RN1);
			refl *= kSScale;
			for (int i = 0; i < SAMPLES; i++) {
				real_t xoffs;
				real_t yoffs;
				do {
					xoffs = (twister_.Rand()) * drefl;
					yoffs = (twister_.Rand()) * drefl;
				} while ((xoffs * xoffs + yoffs * yoffs) > 
				    (drefl * drefl));

				Vector3 R = RP + RN1 * xoffs + RN2 * yoffs * 
				    drefl;
				R.Normalize();
				real_t dist;
				color_t rcol(0, 0, 0);
				RayTrace(Ray(pi + R * EPSILON, R), rcol,
				    depth + 1, rIndex, dist);
				acc += refl * rcol * color;
			}
		} else {
			Vector3 N = prim->OnGetNormal(pi);
			Vector3 R = ray.GetDirection() - 
			    2.0f*Dot(ray.GetDirection(), N)*N;

			color_t rcol;
			real_t dist;
			RayTrace(Ray(pi + R*EPSILON, R), rcol, depth + 1, 1.0f,
			    dist);
			acc += refl * rcol * color;
		}
	}

	//
	// refraction
	//
	real_t refr = prim->GetMaterial()->GetRefraction();
	if ((refr > 0) && (depth < TRACEDEPTH)) {
		real_t objRIndex = prim->GetMaterial()->GetRefrIndex();
		
		if (result == -1 && rIndex > 1.0f)
			objRIndex = 1.0f;

		real_t n = rIndex / objRIndex;
		Vector3 N = prim->OnGetNormal(pi) * (real_t)result;

		real_t cosI = Dot(N, ray.GetDirection());
		real_t sinT2 = n*n*(1.0f - cosI*cosI);
		if (sinT2 <= 1.0f) {
			Vector3 T = n*ray.GetDirection() -
			    (n*cosI + sqrt(1.0f - sinT2))*N;
			T.Normalize();

			color_t rcolor;
			real_t dist;
			RayTrace(Ray(pi + T * EPSILON, T), rcolor, depth + 1,
			    objRIndex, dist);

			color_t absorbance = prim->GetMaterial()->GetColor() *
			    0.08f * -dist;
			color_t transparency = color_t(exp(absorbance.m_x),
			    exp(absorbance.m_y), exp(absorbance.m_z));
			acc += rcolor * transparency;
		}
	}
}

