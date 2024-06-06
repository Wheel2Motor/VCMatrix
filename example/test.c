#include <stdio.h>
#include <vcmatrix.h>

int main() {
	struct Color4F cdst[2] = { 0 };
	struct Color4F csrc[2] = {
		{0.5, 0.5, 0.8, 0.2},
		{0.4, 0.5, 0.5, 0.1},
	};
	struct Vec3 vsrc[2] = {
		{0.0, 0.0, 0.0},
		{1.0, 1.0, 1.0},
	};
	struct Vec3 vdst[2] = {
		{0.0, 0.0, 0.0},
		{1.0, 1.0, 1.0},
	};

	match_hsva_color4f(
			cdst, vdst, 2,
			csrc, vsrc, 2,
			10.0,
			EOffsetA,
			EGreaterThan
			);
	for (size_t i = 0; i < 2; i++) {
		struct Color4F * c = cdst + i;
		printf("%lf %lf %lf %lf\n", c->r, c->g, c->b, c->a);
	}

	return 0;
}
