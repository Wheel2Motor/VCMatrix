#include <stdint.h>
#include <stdbool.h>


typedef double  Float;
typedef uint8_t Byte;
typedef size_t  Size;

struct Vec3     { Float x, y, z; };
struct Color4U  { Byte  r, g, b, a; };
struct Color4F  { Float r, g, b, a; };


enum EOffsetRGBA {
	EOffsetR = 0,
	EOffsetG,
	EOffsetB,
	EOffsetA,
};
enum EOffsetHSVA {
	EOffsetH = 0,
	EOffsetS,
	EOffsetV,
	//EOffsetA,
};
enum EColorType {
	EColor4U,
	EColor4F,
};
enum EMethod    {
	EGreaterThan,
	ELessThan,
	EGreaterThanOrEqual,
	ELessThanOrEqual,
	EEqual,
};

/*
 * Nearest
 */

extern bool
match_nearest_color4f(
	struct Color4F * out_color_dst, struct Vec3 * in_verts_dst, Size nverts_dst,
	struct Color4F * in_color_src,  struct Vec3 * in_verts_src, Size nverts_src
	);

extern bool
match_nearest_color4u(
	struct Color4U * out_color_dst, struct Vec3 * in_verts_dst, Size nverts_dst,
	struct Color4U * in_color_src,  struct Vec3 * in_verts_src, Size nverts_src
	);

/*
 * RGBA
 */

extern bool
match_rgba_color4f(
	struct Color4F * out_color_dst, struct Vec3 * in_verts_dst, Size nverts_dst,
	struct Color4F * in_color_src,  struct Vec3 * in_verts_src, Size nverts_src,
	Float range,
	int offset,
	enum EMethod meth
	);

extern bool
match_rgba_color4u(
	struct Color4U * out_color_dst, struct Vec3 * in_verts_dst, Size nverts_dst,
	struct Color4U * in_color_src,  struct Vec3 * in_verts_src, Size nverts_src,
	Float range,
	int offset,
	enum EMethod meth
	);


/*
 * HSVA
 */

extern bool
match_hsva_color4f(
	struct Color4F * out_color_dst, struct Vec3 * in_verts_dst, Size nverts_dst,
	struct Color4F * in_color_src,  struct Vec3 * in_verts_src, Size nverts_src,
	Float range,
	int offset,
	enum EMethod meth
	);

extern bool
match_hsva_color4u(
	struct Color4U * out_color_dst, struct Vec3 * in_verts_dst, Size nverts_dst,
	struct Color4U * in_color_src,  struct Vec3 * in_verts_src, Size nverts_src,
	Float range,
	int offset,
	enum EMethod meth
	);

