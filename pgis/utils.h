#pragma once

# define M_PI           3.14159265358979323846

static const float RAD_IN_DEGREE = 2 * M_PI / 360.f;

static float degToRad(float deg)
{
	return deg * RAD_IN_DEGREE;
}