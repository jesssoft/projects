//
// 2009 jesssoft
//
#ifndef __SURFACE_H__
#define __SURFACE_H__

#include "typedefs.h"
#include "util_macro.h"
#include "vector3.h"

class Surface {
public:
	Surface(int width, int height);
	~Surface(void);

	inline pixel_t	*GetBuffer()		{ return buffer_; };
	inline int	GetWidth()		{ return width_; };
	inline int	GetHeight()		{ return height_; };
	void		ClearBuffer(pixel_t pixel);
	void		PutPixel(int x, int y, color_t &color);
	void		PutPixel(int x, int y, pixel_t pixel);

private:
	pixel_t		*buffer_;
	int		width_;
	int		height_;
};

#endif

