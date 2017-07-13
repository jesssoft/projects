//
// 2009 jesssoft
//
#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <math.h>
#include "typedefs.h"
#include "vector3.h"

//
//		| x |		| nx |
//	mat	| y |	=	| ny |
//		| z |		| nz |
//
class Matrix {
public:
	Matrix(void);
	~Matrix(void);

	void	Identity(void);
	void	Rotate(Vector3 &pos, real_t rx, real_t ry, real_t rz);
	void	RotateX(real_t rx);
	void	RotateY(real_t ry);
	void	RotateZ(real_t rz);
	void	Translate(const Vector3 &pos);
	void	Concatenate(Matrix &mat);
	bool	Invert(void); 
	Vector3 Transform(const Vector3 &v);
	void	operator=(const Matrix &src);

public:
	real_t	m[16];
};

#endif
