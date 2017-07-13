//
// 2009 jesssoft
//
#include "matrix.h"

Matrix::Matrix(void)
{
	Identity();
}

Matrix::~Matrix(void)
{
}

void
Matrix::operator=(const Matrix &src)
{
	for (int i = 0; i < 16; ++i)
		m[i] = src.m[i];
}

void	
Matrix::Identity(void) 
{
	m[ 0] = 1; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0;
	m[ 4] = 0; m[ 5] = 1; m[ 6] = 0; m[ 7] = 0;
	m[ 8] = 0; m[ 9] = 0; m[10] = 1; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}

void	
Matrix::Rotate(Vector3 &pos, real_t rx, real_t ry, real_t rz) 
{
	Matrix t;
	t.RotateX(rz);
	RotateY(ry);
	Concatenate(t);
	t.RotateZ(rx);
	Concatenate(t);
	Translate(pos);
}

void	
Matrix::RotateX(real_t rx) 
{
	real_t	sx = (real_t)sin(rx);
	real_t	cx = (real_t)cos(rx);
	Identity();
	m[5] = cx;
	m[6] = -sx;
	m[9] = sx;
	m[10] = cx;
}

void	
Matrix::RotateY(real_t ry) 
{
	real_t sy = (real_t)sin(ry);
	real_t cy = (real_t)cos(ry);
	Identity();
	m[0] = cy;
	m[2] = -sy;
	m[8] = sy;
	m[10] = cy;
}

void	
Matrix::RotateZ(real_t rz) 
{
	real_t sz = (real_t)sin(rz);
	real_t cz = (real_t)cos(rz);
	Identity();
	m[0] = cz;
	m[1] = -sz;
	m[4] = sz;
	m[5] = cz;
}

void	
Matrix::Translate(const Vector3 &pos) 
{
	m[3] += pos.x;
	m[7] += pos.y;
	m[11] += pos.z;
}

void	
Matrix::Concatenate(Matrix &mat) 
{
	Matrix tmp;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++) {
			tmp.m[r*4 + c] = m[r*4 + 0] * mat.m[c + 0] +
			    m[r*4 + 1] * mat.m[c + 4] +
			    m[r*4 + 2] * mat.m[c + 8] +
			    m[r*4 + 3] * mat.m[c + 12];
		}

	for (int c = 0; c < 16; c++)
		m[c] = tmp.m[c];
}

bool
Matrix::Invert(void) 
{
	/* I think this routine is something wrong.
	Matrix t;
	real_t tx = -m[3];
	real_t ty = -m[7];
	real_t tz = -m[11];

	for (int h = 0; h < 3; h++)
		for (int v = 0; v < 3; v++)
			t.m[h+v*4] = m[v+h*4];

	for (int i = 0; i < 11; i++)
		m[i] = t.m[i];

	m[3]  = tx*m[0] + ty*m[1] + tz*m[2];
	m[7]  = tx*m[4] + ty*m[5] + tz*m[6];
	m[11] = tx*m[8] + ty*m[9] + tz*m[10];
	*/
	float detInv;
	const real_t (*pm)[4];
	real_t (*dm)[4];
	Matrix t;

	dm = (real_t (*)[4])t.m;
	pm = (real_t (*)[4])m;
	if (fabs(pm[3][3] - 1.0f) > 0.001f) { /* No inverse matrix */
		return false;
	}

	if (fabs(pm[3][0]) > 0.001f || fabs(pm[3][1]) > 0.001f || 
	    fabs(pm[3][2]) > 0.001f) { /* No inverse matrix */
		return false;
	}

	detInv = 1.0f / (pm[0][0]*(pm[1][1]*pm[2][2] - pm[2][1]*pm[1][2]) - 
			pm[1][0]*(pm[0][1]*pm[2][2] - pm[2][1]*pm[0][2]) + 
			pm[2][0]*(pm[0][1]*pm[1][2] - pm[1][1]*pm[0][2]));

	dm[0][0] = 	detInv*(pm[1][1]*pm[2][2] - pm[2][1]*pm[1][2]);
	dm[1][0] = -detInv*(pm[1][0]*pm[2][2] - pm[2][0]*pm[1][2]);
	dm[2][0] = 	detInv*(pm[1][0]*pm[2][1] - pm[2][0]*pm[1][1]);
	dm[3][0] = 	0.0f;

	dm[0][1] =  -detInv*(pm[0][1]*pm[2][2] - pm[2][1]*pm[0][2]);
	dm[1][1] =   detInv*(pm[0][0]*pm[2][2] - pm[2][0]*pm[0][2]);
	dm[2][1] =  -detInv*(pm[0][0]*pm[2][1] - pm[2][0]*pm[0][1]);
	dm[3][1] = 	0.0f;

	dm[0][2] = 	detInv*(pm[0][1]*pm[1][2] - pm[1][1]*pm[0][2]);
	dm[1][2] = -detInv*(pm[0][0]*pm[1][2] - pm[1][0]*pm[0][2]);
	dm[2][2] = 	detInv*(pm[0][0]*pm[1][1] - pm[1][0]*pm[0][1]);
	dm[3][2] = 	0.0f;

	dm[0][3] = -(pm[0][3]*dm[0][0] + pm[1][3]*dm[0][1] + 
			pm[2][3]*dm[0][2]);
	dm[1][3] = -(pm[0][3]*dm[1][0] + pm[1][3]*dm[1][1] + 
			pm[2][3]*dm[1][2]);
	dm[2][3] = -(pm[0][3]*dm[2][0] + pm[1][3]*dm[2][1] + 
			pm[2][3]*dm[2][2]);
	dm[3][3] = 1.0f;

	*this = t;
	return true;
}

Vector3 
Matrix::Transform(const Vector3 &v) 
{
	real_t x = m[0]*v.x + m[1]*v.y + m[2]*v.z + m[3];
	real_t y = m[4]*v.x + m[5]*v.y + m[6]*v.z + m[7];
	real_t z = m[8]*v.x + m[9]*v.y + m[10]*v.z + m[11];
	return Vector3(x, y, z);
}

