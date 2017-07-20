//
// 2016.05.18 jesssoft
//

#include "sprite.h"

Sprite::Sprite(SDL_Texture *tex)
{
	tex_ = tex;
	pivot_x = 0.5f;
	pivot_y = 0.5f;
	SDL_QueryTexture(tex, NULL, NULL, &w_, &h_); 
}

Sprite::~Sprite()
{
	if (tex_) {
		SDL_DestroyTexture(tex_);
		tex_ = NULL;
	}
}

void
Sprite::Draw(SDL_Renderer *ren, int x, int y, int degree)
{
	SDL_Rect rect;
	SDL_Point pivot;

	pivot.x = w_ * pivot_x;
	pivot.y = h_ * pivot_y;

	rect.x = x - pivot.x;
	rect.y = y - pivot.y;
	rect.w = w_;
	rect.h = h_;

	SDL_RenderCopyEx(ren, tex_, NULL, &rect, degree, &pivot, SDL_FLIP_NONE);
}

