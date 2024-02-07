#ifndef DRAW_HPP_BA97BA20_4B0E_45D8_97D4_65267FFA2EA6
#define DRAW_HPP_BA97BA20_4B0E_45D8_97D4_65267FFA2EA6

#include "forward.hpp"

#include "../vmlib/vec2.hpp"

void draw_rectangle_solid(
	Surface&,
	Vec2f aMinCorner, Vec2f aMaxCorner,
	ColorU8_sRGB
);

void draw_rectangle_outline(
	Surface&,
	Vec2f aMinCorner, Vec2f aMaxCorner,
	ColorU8_sRGB
);

#endif // DRAW_HPP_BA97BA20_4B0E_45D8_97D4_65267FFA2EA6
