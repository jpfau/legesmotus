/*
 * common/Point.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
 * 
 * Leges Motus is free and open source software.  You may redistribute it and/or
 * modify it under the terms of version 2, or (at your option) version 3, of the
 * GNU General Public License (GPL), as published by the Free Software Foundation.
 * 
 * Leges Motus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the full text of the GNU General Public License for
 * further detail.
 * 
 * For a full copy of the GNU General Public License, please see the COPYING file
 * in the root of the source code tree.  You may also retrieve a copy from
 * <http://www.gnu.org/licenses/gpl-2.0.txt>, or request a copy by writing to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 * 
 */

#ifndef LM_COMMON_POINT_HPP
#define LM_COMMON_POINT_HPP

#include <iosfwd>

// A simple class which represents a point or a vector in the Cartesian plane.
// Has x and y components.
namespace LM {
	class Point {
	public:
		double		x;
		double		y;
	
		Point() { clear(); }
		Point(double arg_x, double arg_y) { x = arg_x; y = arg_y; }
	
		void		clear() { x = y = 0; }
		void		scale(double factor);
		void		rotate(double angle);	// Rotate CW given number of _radians_
		double		get_angle() const;	// In radians
		double		get_magnitude() const;
		Point		get_unit_vector() const;
	
		// Given a string of the from "x,y", make a point from it
		static Point	make_from_string(const char* str);
		// Given a magnitude and an angle (in radians), make a point from it
		static Point	make_from_magnitude(double magnitude, double angle);

		inline static double dot_product(Point u, Point v) {
			return u.x * v.x + u.y * v.y;
		}
		inline static double cross_product(Point u, Point v) {
			return u.x * v.y - u.y * v.x;
		}

		void		operator+=(Point other);
		void		operator-=(Point other);
		void		operator*=(double scale_factor);
		void		operator/=(double scale_factor);
	
		static double distance(Point a, Point b);
	};
	
	// Output in the form "x,y"
	std::ostream& operator<< (std::ostream& out, Point point);
	
	// Arithmetic
	Point	operator+(Point a, Point b);
	Point	operator-(Point a, Point b);
	Point	operator*(Point p, double scale_factor);
	Point	operator/(Point p, double scale_factor);
	
	typedef Point Vector;
}

#endif
