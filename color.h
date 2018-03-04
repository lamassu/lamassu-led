#include <stdlib.h>

enum color_type
{
	COLOR_NONE,
	COLOR_RGB8,
	COLOR_RGB,
	COLOR_LINEAR_RGB,
	COLOR_HSL,
	COLOR_HSV,
	COLOR_YUV,
	COLOR_YCBCR,
	COLOR_YDBDR,
	COLOR_YIQ,
	COLOR_XYZ,
	COLOR_XYY,
	COLOR_LAB,
	COLOR_LUV,
	COLOR_LCHAB,
	COLOR_LCHUV,
	COLOR_LSHUV,
	COLOR_DUMMY_END
};

struct led_color  {
	unsigned char r, g, b, a;
};

typedef struct led_color led_color;

struct color
{
	uint8_t type, extra;
	union
	{
		struct { uint8_t R, G, B; } RGB8;
		struct { double R, G, B; } RGB, LinearRGB;
		struct { double H, S, L; } HSL; // hue is in [0, 6)
		struct { double H, S, V; } HSV; // hue is in [0, 6)
		struct { double Y, U, V; } YUV; // Y, U, V are in [0, 1], [-0.436,0.436], [-0.615,0.615]
		struct { uint8_t Y, Cb, Cr; } YCbCr;
		struct { double Y, Db, Dr; } YDbDr;
		struct { double Y, I, Q; } YIQ;
		struct { double X, Y, Z; } XYZ;
		struct { double x, y, Y; } xyY;
		struct { double L, a, b; } Lab;
		struct { double L, u, v; } Luv;
		struct { double L, C, h; } LCHab, LCHuv;  // hue is in [0, pi*2)
		struct { double L, S, h; } LSHuv;  // hue is in [0, pi*2)
	};
};

typedef struct color color;

void color_HSL_to_RGB(struct color *c);
void color_RGB_to_HSL(struct color *c);
