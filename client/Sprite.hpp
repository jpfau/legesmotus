/*
 * client/Sprite.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_SPRITE_HPP
#define LM_CLIENT_SPRITE_HPP

#include "Graphic.hpp"

class Sprite : public Graphic {
private:
	int		m_width;
	int		m_height;

	double		m_alpha;
	double		m_red;
	double		m_green;
	double		m_blue;

	void		init();

public:
	Sprite(SDL_Surface* image);
	Sprite(const char* filename);
	Sprite(const Sprite& other);

	int	get_width() const;
	int	get_height() const;

	double	get_alpha() const;
	double	get_red_intensity() const;
	double	get_green_intensity() const;
	double	get_blue_intensity() const;

	void	set_alpha(double alpha);
	void	set_red_intensity(double r);
	void	set_green_intensity(double g);
	void	set_blue_intensity(double b);

	void	draw(const GameWindow* window) const;
};

#endif
