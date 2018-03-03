#include <math.h>
#include <assert.h>

#include "color.h"

static void finish_HSL_to_RGB(struct color *c, double h, double C, double m)
{
	static const char rgb_tbl[][3] =
	{
		{ 0, 2, 1 },
		{ 2, 0, 1 },
		{ 1, 0, 2 },
		{ 1, 2, 0 },
		{ 2, 1, 0 },
		{ 0, 1, 2 }
	};

	double absh, h2, vars[3];
	int idx;

	// clamps hue to [0, 2), and returns (1.0 - fabs(hue - 1.0))

	absh = fabs(h);

	h2 =
		absh >= 2.0 ? floor(h * 0.5) * -2.0 + h - 1.0 :
		h < 0.0 ? h + 1.0 :
		h - 1.0;
	h2 = 1.0 - fabs(h2);

	// clamps hue to [0, 6), for indexing into rgb_tbl.

	idx = (int)
		(absh >= 6.0 ? floor(h * (1.0 / 6.0)) * -6.0 + h :
		h < 0.0 ? h + 6.0 :
		h);

	assert(idx >= 0 && idx <= 5);

	// finish HSL->RGB.

	vars[0] = C + m;
	vars[1] = m;
	vars[2] = C * h2 + m;

	c->RGB.R = vars[rgb_tbl[idx][0]];
	c->RGB.G = vars[rgb_tbl[idx][1]];
	c->RGB.B = vars[rgb_tbl[idx][2]];
}

void color_HSL_to_RGB (struct color *c) {
	double H, S, L, C, m;

	assert(c != NULL);
	assert(c->type == COLOR_HSL);

	H = c->HSL.H;
	S = c->HSL.S;
	L = c->HSL.L;

	c->type = COLOR_RGB;

	if(fabs(S) > 0.0)
	{
		C = (1.0 - fabs(L * 2.0 - 1.0)) * S;
		m = C * -0.5 + L;

		finish_HSL_to_RGB(c, H, C, m);
		return;
	}

	c->RGB.R = L; c->RGB.G = L; c->RGB.B = L;
}

void color_RGB_to_HSL(struct color *c) {
	double R, G, B, min, max, delta, L;

	assert(c != NULL);
	assert(c->type == COLOR_RGB);

	R = c->RGB.R;
	G = c->RGB.G;
	B = c->RGB.B;

	min = R < G ? R : G;
	if(B < min) min = B;

	max = R > G ? R : G;
	if(B > max) max = B;

	delta = max - min;

	L = (max + min) * 0.5;

	c->HSL.L = L;
	c->type = COLOR_HSL;

	if(fabs(delta) > 0.0)
	{
		c->HSL.S =
			L < 0.5 ? delta / (max + min) :
			delta / (2.0 - max - min);

		c->HSL.H =
			max == R ? (G - B) / delta :
			max == G ? (B - R) / delta + 2.0 :
			(R - G) / delta + 4.0;

		return;
	}

	c->HSL.S = 0.0;
	c->HSL.H = 0.0;
}
