//
// 2009 jesssoft
//
#include "primitive.h"
#include "matrix.h"

using namespace std;

Primitive::Primitive(void)
{
	light_flag_ = false;
}

Primitive::~Primitive(void)
{
}

void
Primitive::SetName(const char *name)
{
	name_ = name;
}

///////////////////////////////////////////////////////////////////////////////
int
Sphere::OnIntersect(Ray &ray, real_t &dist_limit)
{
	int ret = MISS;
	Vector3 vec_centre = centre_ - ray.GetOrigin();
	real_t b = Dot(vec_centre, ray.GetDirection());

	//
	// The object is behind the camera?
	//
	if (b <= 0)
		return ret;

	//
	// a*a = b*b + c*c
	//
	real_t a = vec_centre.Length();
	
	//
	// Check for the collision between this sphere and the ray
	//
	// 1)
	// det*det + c*c = r*r
	// det = root(r*r - c*c)
	//
	// 2)
	// a*a = b*b + c*c
	// c*c = a*a - b*b
	//
	// 3)
	// det = root(r*r - (a*a - b*b))
	// det = root(r*r - a*a + b*b)
	// det = root(r*r + b*b - a*a)
	//
	real_t det = radius_*radius_ + b*b - a*a;
	if (det < 0)
		return ret;

	det = sqrt(det);

	real_t dist1 = b - det; // Near distance
	real_t dist2 = b + det; // Far distance

	if (dist1 <= 0) {
		if (dist2 < dist_limit) {
			dist_limit = dist2;
			ret = INPRIM;
		}
	} else {
		if (dist1 < dist_limit) {
			dist_limit = dist1;
			ret = HIT;
		}
	}
	
	return ret;
}

Vector3
Sphere::OnGetNormal(Vector3 &pos)
{
	Vector3 normal;

	normal = pos - centre_;
	normal.Normalize();

	return normal;
}

color_t
Sphere::GetColor(Vector3 &pos)
{
	if (material_.GetTexture()) {
		Vector3 vp = (pos - centre_) * (1.0f / radius_);
		real_t phi = acos(-Dot(vp, vn_));
		real_t u;
		real_t v = phi * material_.GetVScaleReci() * (1.0f / PI);
		real_t theta = (acos(Dot(ve_, vp) / sin(phi))) * (2.0f / PI);

		if (Dot(vc_, vp) >= 0)
			u = (1.0f - theta) * material_.GetUScaleReci();
		else
			u = theta * material_.GetUScaleReci();

		return material_.GetTexture()->GetTexel(u, v) *
		    material_.GetColor();
	} else
		return material_.GetColor();
}

///////////////////////////////////////////////////////////////////////////////
int
PlanePrim::OnIntersect(Ray &ray, real_t &dist_limit)
{
	real_t denom = Dot(ray.GetDirection(), normal_);
	if (denom >= 0)
		return MISS;

	//
	// 1)
	// p = org + t*dir
	//
	// 2)
	// (p - p0).n = 0
	// (org + t*dir - p0).n = 0
	// t*dir.n + (org - p0).n = 0
	// 
	// 3)
	// d = -p0.n
	// p0.n = -d
	//
	// 4)
	// t = (p0 - org).n/(dir.n) = (p0.n - org.n)/(dir.n)
	// t = (-d - org.n)/(dir.n)
	// t = -(org.n + d)/(dir.n)
	//
	real_t dist = -(Dot(ray.GetOrigin(), normal_) + d_) / denom;

	if (dist > 0) {
		if (dist < dist_limit) {
			dist_limit = dist;
			return HIT;
		}
	}

	return MISS;
}

Vector3
PlanePrim::OnGetNormal(Vector3 &pos)
{
	if (material_.GetNTexture()) {
		Texture *t = material_.GetNTexture();

		real_t u = Dot(pos, uaxis_) * material_.GetUScale();
		real_t v = Dot(pos, vaxis_) * material_.GetVScale();

		color_t s = t->GetTexel(u, v);
		Matrix mat;
		
		// Here: hard coded
		if (normal_.y == -1) {
			// celling
			mat.RotateY(PI/2 * 2);
			s = mat.Transform(s);

			s.y *= -1;
		} else if (normal_.x == -1) {
			// right
			mat.RotateZ(PI/2);
			s = mat.Transform(s);

			s.y *= -1;
		} else if (normal_.x == 1) {
			// left
			mat.RotateZ(-PI/2);
			s = mat.Transform(s);

			s.y *= -1;
		} else if (normal_.z == -1) {
			// back
			mat.RotateX(-PI/2);
			s = mat.Transform(s);
		} else if (normal_.y == 1) {
			// front?
			s.z *= -1;
		}

		s.Normalize();
		return s;
	} else
		return normal_;
}

color_t	
PlanePrim::GetColor(Vector3 &pos)
{ 
	if (material_.GetTexture()) {
		Texture *t = material_.GetTexture();

		real_t u = Dot(pos, uaxis_) * material_.GetUScale();
		real_t v = Dot(pos, vaxis_) * material_.GetVScale();

		return t->GetTexel(u, v) * material_.GetColor();
	} else
		return material_.GetColor(); 
}

////////////////////////////////////////////////////////////////////////////
Triangle::Triangle(Vertex *v1, Vertex *v2, Vertex *v3)
{
	vertex_[0] = v1;
	vertex_[1] = v2;
	vertex_[2] = v3;

	Vector3 A = vertex_[0]->GetPos();
	Vector3 B = vertex_[1]->GetPos();
	Vector3 C = vertex_[2]->GetPos();
	Vector3 c = B - A;
	Vector3 b = C - A;
	normal_ = b.Cross(c);

	int u;
	int v;
	if (fabs(normal_.x) > fabs(normal_.y)) {
		if (fabs(normal_.x) > fabs(normal_.z))
			k_ = 0;
		else
			k_ = 2;
	} else {
		if (fabs(normal_.y) > fabs(normal_.z))
			k_ = 1;
		else
			k_ = 2;
	}

	u = (k_ + 1)%3;
	v = (k_ + 2)%3;

	real_t krec = 1.0f/normal_.cell[k_];
	nu_ = normal_.cell[u]*krec;
	nv_ = normal_.cell[v]*krec;
	nd_ = normal_.Dot(A)*krec;

	real_t reci = 1.0f/(b.cell[u]*c.cell[v] - b.cell[v]*c.cell[u]);
	bnu_ = b.cell[u] * reci;
	bnv_ = -b.cell[v] * reci;

	cnu_ = c.cell[v]*reci;
	cnv_ = -c.cell[u]*reci;

	normal_.Normalize();
	vertex_[0]->SetNormal(normal_);
	vertex_[1]->SetNormal(normal_);
	vertex_[2]->SetNormal(normal_);
}

unsigned int modulo[] = { 0, 1, 2, 0, 1 };

int
Triangle::OnIntersect(Ray &ray, real_t &dist_limit)
{
#define ku modulo[k_ + 1]
#define kv modulo[k_ + 2]

	Vector3 o = ray.GetOrigin();
	Vector3 d = ray.GetDirection();
	Vector3 A = vertex_[0]->GetPos();
	const real_t lnd = 1.0f/(d.cell[k_] + nu_*d.cell[ku] +
	    nv_ * d.cell[kv]);
	const real_t t = (nd_ - o.cell[k_] - nu_*o.cell[ku] -
	    nv_*o.cell[kv]) * lnd;
	if (!(dist_limit > t && t > 0))
		return MISS;

	real_t hu = o.cell[ku] + t*d.cell[ku] - A.cell[ku];
	real_t hv = o.cell[kv] + t*d.cell[kv] - A.cell[kv];
	real_t beta = u_ = hv*bnu_ + hu*bnv_;
	if (beta < 0)
		return MISS;

	real_t gamma = v_ = hu*cnu_ + hv*cnv_;
	if (gamma < 0)
		return MISS;

	if ((u_ + v_) > 1)
		return MISS;

	dist_limit = t;
	int ret = (Dot(d, normal_) > 0) ? INPRIM : HIT;
	return ret;
}

Vector3
Triangle::OnGetNormal(Vector3 &pos)
{
	Vector3 n1 = vertex_[0]->GetNormal();
	Vector3 n2 = vertex_[1]->GetNormal();
	Vector3 n3 = vertex_[2]->GetNormal();
	Vector3 n = n1 + u_*(n2 - n1) + v_*(n3 - n1);
	n.Normalize();
	return n;
}

color_t
Triangle::GetColor(Vector3 &pos) 
{ 
	return material_.GetColor(); 
}

