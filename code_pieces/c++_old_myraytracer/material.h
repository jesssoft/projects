//
// 2009 jesssoft
//
#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "typedefs.h"
#include "vector3.h"

class Texture {
public:
	Texture(const char *filename, bool normalMap = false);
	~Texture(void);

	color_t	GetTexel(real_t u, real_t v);

private:
	color_t		*bitmap_;
	int		width_;
	int		height_;
};

class Material {
public:
	Material(void);
	~Material(void);

	inline void SetRefrIndex(real_t val)	{ refrIndex_ = val; };
	inline void SetRefraction(real_t val)	{ refraction_ = val; };
	inline void SetReflection(real_t val)	{ reflection_ = val; };
	inline void SetDiffuse(real_t val)	{ diffuse_ = val; };
	inline void SetSpecular(real_t val)	{ specular_ = val; };
	inline void SetDiffuseRefl(real_t val)	{ diffuseRefl_ = val; };
	inline void SetTexture(Texture *tex)	{ texture_ = tex; };
	inline void SetNTexture(Texture *ntex)	{ ntexture_ = ntex; };
	inline void SetColor(const color_t &color) { color_ = color; };
	void SetUVScale(real_t uscale, real_t vscale);

	inline real_t	GetRefrIndex(void)	{ return refrIndex_; };
	inline real_t	GetRefraction(void)	{ return refraction_; };
	inline real_t	GetReflection(void)	{ return reflection_; };
	inline real_t	GetDiffuse(void)	{ return diffuse_; };
	inline real_t	GetSpecular(void)	{ return specular_; };
	inline real_t	GetDiffuseRefl(void)	{ return diffuseRefl_; };
	inline real_t	GetUScale(void)		{ return uscale_; };
	inline real_t	GetVScale(void)		{ return vscale_; };
	inline real_t	GetUScaleReci(void)	{ return ruscale_; };
	inline real_t	GetVScaleReci(void)	{ return rvscale_; };
	inline color_t	&GetColor(void)		{ return color_; };
	inline Texture	*GetTexture(void)	{ return texture_; };
	inline Texture	*GetNTexture(void)	{ return ntexture_; };

private:
	real_t		refrIndex_;
	real_t		refraction_;
	real_t		reflection_;
	real_t		diffuse_;
	real_t		specular_;
	real_t		diffuseRefl_;
	real_t		uscale_;
	real_t		vscale_;
	real_t		ruscale_;
	real_t		rvscale_;
	color_t		color_;
	Texture		*texture_;	
	Texture		*ntexture_;
};

#endif

