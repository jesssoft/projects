//
// 2016.05.18 jesssoft
//
#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Sprite {
public:
	Sprite(SDL_Texture *tex);
	~Sprite();

	void	Draw(SDL_Renderer *ren, int x, int y, int degree);
private:
	SDL_Texture	*tex_;
	int		w_;
	int		h_;
	float		pivot_x;
	float		pivot_y;
};

#endif

