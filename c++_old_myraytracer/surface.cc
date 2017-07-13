//
// 2009 jesssoft
//
#include "surface.h"

Surface::Surface(int width, int height)
: width_(width), height_(height)
{
	buffer_ = new pixel_t[width * height];
	ClearBuffer(0);
}

Surface::~Surface(void)
{
	delete [] buffer_;
}

void
Surface::ClearBuffer(pixel_t pixel)
{
	int size = GetWidth() * GetHeight();

	for (int i = 0; i < size; i++)
		buffer_[i] = pixel;
}

void 
Surface::PutPixel(int x, int y, color_t &color)
{
	real_t color_r;
	real_t color_g;
	real_t color_b;

	color_r = color.r;
	color_g = color.g;
	color_b = color.b;

	if (color_r > 1.0f) color_r = 1.0f;
	if (color_g > 1.0f) color_g = 1.0f;
	if (color_b > 1.0f) color_b = 1.0f;

	unsigned char r = static_cast<unsigned char>(color_r * 255);
	unsigned char g = static_cast<unsigned char>(color_g * 255);
	unsigned char b = static_cast<unsigned char>(color_b * 255);

	PutPixel(x, y, COLOR_RGB(r,g,b));
}

void
Surface::PutPixel(int x, int y, pixel_t pixel)
{
	if (x < 0 || y < 0 || x >= GetWidth() || y >= GetHeight())
		return;

	int index = y * GetWidth() + x;
	buffer_[index] = pixel;
}
