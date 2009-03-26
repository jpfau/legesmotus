/*
 * client/GraphicGroup.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_GRAPHICGROUP_HPP
#define LM_CLIENT_GRAPHICGROUP_HPP

#include "Graphic.hpp"
#include <list>

class GraphicGroup : public Graphic {
private:
	std::list<Graphic*>	m_graphics;
public:
	GraphicGroup();

	void	add_graphic(Graphic *graphic);
	void	remove_graphic(Graphic *graphic);

	void	draw(const GameWindow* window) const;
};

#endif