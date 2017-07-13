//
// 2009 jesssoft
//
#include <cstdio>
#include <cmath>
#include "material.h"

using namespace std;

Texture::Texture(const char *filename, bool normalMap)
{
	bitmap_ = nullptr;
	
	FILE *f = fopen(filename, "rb");
	if (f) {
		unsigned char buf[20];
		fread(buf, 1, 20, f);
		width_ = *(buf + 12) + 256 * *(buf + 13);
		height_ = *(buf + 14) + 256 * *(buf + 15);
		fclose(f);

		f = fopen(filename, "rb");
		unsigned char *t = 
		    new unsigned char[width_ * height_ * 3 + 1024];
		fread(t, 1, width_ * height_ * 3 + 1024, f);
		fclose(f);

		int size = width_ * height_;
		bitmap_ = new color_t[size];
		if (normalMap) {
			for (int i = 0; i < size; i++) {
				real_t r = t[i*3+20];
				real_t g = t[i*3+19];
				real_t b = t[i*3+18];

				r = (r - 128)/127;
				g = (g - 128)/127;
				b = (b - 0)/255;

				Vector3 x(1, 0, r);
				Vector3 y(0, 1, g);
				x.Normalize();
				y.Normalize();
				Vector3 ret = y.Cross(x);

				Vector3 result(ret.x, b, ret.y);
				result.Normalize();
				bitmap_[i] = result;
			}
		} else {
			real_t rec = 1.0f / 256;
			for (int i = 0; i < size; i++)
				bitmap_[i] = color_t(t[i*3+20]*rec,
				    t[i*3+19]*rec, t[i*3+18]*rec);
		}
		delete[] t;
	}
}

Texture::~Texture(void)
{
	if (bitmap_) {
		delete[] bitmap_;
		bitmap_ = nullptr;
	}
}

color_t	Texture::GetTexel(real_t u, real_t v)
{
/*
	int x = u * width_;
	int y = v * height_;

	x = x % width_;
	y = y % height_;

	if (x < 0)
		x = -x;
	if (y < 0)
		y = -y;

	int index = x + y * width_;
	color_t c = bitmap_[index];
	return c;
*/

	real_t fu = (u + 1000.5f) * width_;
	real_t fv = (v + 1000.0f) * width_;
	int u1 = ((int)fu) % width_;
	int v1 = ((int)fv) % height_;
	int u2 = (u1 + 1) % width_;
	int v2 = (v1 + 1) % height_;

	real_t fracu = fu - floor(fu);
	real_t fracv = fv - floor(fv);

	real_t w1 = (1 - fracu) * (1 - fracv);
	real_t w2 = fracu * (1 - fracv);
	real_t w3 = (1 - fracu) * fracv;
	real_t w4 = fracu * fracv;

	color_t c1 = bitmap_[u1 + v1 * width_];
	color_t c2 = bitmap_[u2 + v1 * width_];
	color_t c3 = bitmap_[u1 + v2 * width_];
	color_t c4 = bitmap_[u2 + v2 * width_];

	return c1 * w1 + c2 * w2 + c3 * w3 + c4 * w4;
	
}

///////////////////////////////////////////////////////////////////////////////
Material::Material(void)
{
	reflection_	= 0.0f;
	diffuse_	= 0.2f;
	specular_	= 0.6f;
	refraction_	= 0.0f;
	diffuseRefl_	= 0.0f;
	refrIndex_	= 1.5f;
	uscale_		= 1.0f;
	vscale_		= 1.0f;
	ruscale_	= 1.0f;
	rvscale_	= 1.0f;
	color_		= color_t(0.2f, 0.2f, 0.2f);
	texture_	= nullptr;
	ntexture_	= nullptr;
}

Material::~Material(void)
{
	if (texture_) {
		delete texture_;
		texture_ = nullptr;
	}

	if (ntexture_) {
		delete ntexture_;
		ntexture_ = nullptr;
	}
}

void Material::SetUVScale(real_t uscale, real_t vscale)
{
	uscale_ = uscale;
	vscale_ = vscale;

	ruscale_ = 1.0f / uscale_;
	rvscale_ = 1.0f / vscale_;
}
