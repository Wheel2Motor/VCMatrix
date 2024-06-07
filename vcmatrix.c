#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "vcmatrix.h"


#define SAFE_RELEASE(ptr) { if (ptr) free(ptr); ptr = NULL; }
#define MIN2(a, b) ((a) < (b) ? (a) : (b))
#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#define MIN3(r, g, b) (MIN2(MIN2(r, g), b))
#define MAX3(r, g, b) (MAX2(MAX2(r, g), b))
#define IS_1ST_MAX_OF_3(r, g, b) (((r) >= (g)) && ((r) >= (b)))
#define IS_2ND_MAX_OF_3(r, g, b) (((g) >= (r)) && ((g) >= (b)))
#define IS_3RD_MAX_OF_3(r, g, b) (((b) >= (r)) && ((b) >= (g)))


struct HSVA     { Float h, s, v, a; };


inline static void
_vec3_sub(struct Vec3 * out, struct Vec3 * va, struct Vec3 * vb) {
	out->x = va->x - vb->x;
	out->y = va->y - vb->y;
	out->z = va->z - vb->z;
}


inline static Float
_vec3_dist(struct Vec3 * va, struct Vec3 * vb) {
	static Float dx, dy, dz;
	dx = va->x - vb->x;
	dy = va->y - vb->y;
	dz = va->z - vb->z;
	return sqrt(dx * dx + dy * dy + dz * dz);
}


inline static void
_color4f_to_color4u(struct Color4U * out, struct Color4F * in) {
	out->r = (Byte)roundf(in->r * 255.0);
	out->g = (Byte)roundf(in->g * 255.0);
	out->b = (Byte)roundf(in->b * 255.0);
	out->a = (Byte)roundf(in->a * 255.0);
}


inline static void
_color4u_to_color4f(struct Color4F * out, struct Color4U * in) {
	out->r = in->r / 255.0;
	out->g = in->g / 255.0;
	out->b = in->b / 255.0;
	out->a = in->a / 255.0;
}


inline static void
_color4f_to_hsva(struct HSVA * out, struct Color4F * in) {
	/*
	 * Color4F转HSVA
	 * H [0, 360]
	 * S [0.0, 1.0]
	 * V [0.0, 1.0]
	 * A [0.0, 1.0]
	 */
	static Float cmax, cmin, delta;
	cmax = MAX3(in->r, in->g, in->b);
	cmin = MIN3(in->r, in->g, in->b);
	delta = cmax - cmin;
	// H
	if (IS_1ST_MAX_OF_3(in->r, in->g, in->b)) {
		out->h = 60 * fmod((in->g - in->b) / delta, 6.0);
	}
	else if (IS_2ND_MAX_OF_3(in->r, in->g, in->b)) {
		out->h = 60 * (in->b - in->r) + 2.0;
	}
	else if (IS_3RD_MAX_OF_3(in->r, in->g, in->b)) {
		out->h = 60 * (in->r - in->g) + 4.0;
	}
	// S
	if (0.0 == cmax) {
		out->s = 0.0;
	}
	else {
		out->s = delta / cmax;
	}
	// V
	out->v = cmax;
	// A
	out->a = in->a;

	if (isnan(out->h)) out->h = 0.0;
	if (isnan(out->s)) out->s = 0.0;
	if (isnan(out->v)) out->v = 0.0;
	out->h += 180.0;
}


inline static void
_color4u_to_hsva(struct HSVA * out, struct Color4U * in) {
	static struct Color4F cf;
	_color4u_to_color4f(&cf, in);
	_color4f_to_hsva(out, &cf);
}


static bool
_T_color_to_hsva(void * out, void * in, enum EColorType coltyp) {
	switch (coltyp) {
		case EColor4U: {
			struct Color4F cf;
			_color4u_to_color4f(&cf, in);
			_color4f_to_hsva(out, &cf);
			return true;
		}
		case EColor4F: {
			_color4f_to_hsva(out, in);
			return true;
		}
		defaultt:
			return false;
	}
}


static void
_hsva_to_color4f(struct Color4F * out, struct HSVA * in) {
	/*
	 * HSVA转Color4F
	 * H [0, 360]
	 * S [0.0, 1.0]
	 * V [0.0, 1.0]
	 * A [0.0, 1.0]
	 */
	static Float M, m, z;
	M = in->v;
	m = M * (1.0 - in->s);
	z = (M - m) * (1.0 - fabs(fmod(in->h / 60.0, 2.0) - 1.0));
	if ((in->h >= 0.0) && (in->h < 60.0)) {
		out->r = M;
		out->g = z + m;
		out->b = m;
	}
	else if ((in->h >= 60.0) && (in->h < 120.0)) {
		out->r = z + m;
		out->g = M;
		out->b = m;
	}
	else if ((in->h >= 120.0) && (in->h < 180.0)) {
		out->r = m;
		out->g = M;
		out->b = z + m;
	}
	else if ((in->h >= 180.0) && (in->h < 240.0)) {
		out->r = m;
		out->g = z + m;
		out->b = M;
	}
	else if ((in->h >= 240.0) && (in->h < 300.0)) {
		out->r = z + m;
		out->g = m;
		out->b = M;
	}
	else if ((in->h >= 300.0) && (in->h <= 360.0)) {
		out->r = M;
		out->g = m;
		out->b = z + m;
	}
	out->a = in->a;
}


static void
_hsva_to_color4u(struct Color4U * out, struct HSVA * in) {
	static struct Color4F cf;
	_hsva_to_color4f(&cf, in);
	_color4f_to_color4u(out, &cf);
}


static bool
_T_match_nearest(
	void * out_color_dst,
	struct Vec3 * in_verts_dst,
	Size nverts_dst,
	void * in_color_src,
	struct Vec3 * in_verts_src,
	Size nverts_src,
	enum EColorType coltyp
	) {
	Float * distlut = NULL;
	Size * idxlut = NULL;
	struct Vec3 * vsrc = NULL;
	struct Vec3 * vdst = NULL;
	Float dist = 0.0, org = 0.0;
	Size color_size = 0;
	assert(nverts_dst == nverts_src);
	switch (coltyp) {
		case EColor4U:
			color_size = sizeof(struct Color4U);
			break;
		case EColor4F:
			color_size = sizeof(struct Color4F);
			break;
		default:
			return false;
	}
	distlut = (Float *)malloc(sizeof(Float) * nverts_dst);
	idxlut = (Size *)malloc(sizeof(Size) * nverts_dst);
	if (!(distlut && idxlut)) {
		SAFE_RELEASE(distlut);
		SAFE_RELEASE(idxlut);
		return false;
	}
	for (Size idx = 0; idx < nverts_dst; idx ++) {
		distlut[idx] = -1.0;
		idxlut[idx] = 0;
	}
	for (Size idxdst = 0; idxdst < nverts_dst; idxdst ++) {
		vdst = in_verts_dst + idxdst;
		for (Size idxsrc = 0; idxsrc < nverts_src; idxsrc ++) {
			vsrc = in_verts_src + idxsrc;
			dist = _vec3_dist(vsrc, vdst);
			org = distlut[idxdst];
			if (org < 0) {
				distlut[idxdst] = dist;
				idxlut[idxdst] = idxsrc;
			}
			else {
				if (org > dist) {
					distlut[idxdst] = dist;
					idxlut[idxdst] = idxsrc;
				}
			}
		}
	}
	for (Size idx = 0; idx < nverts_dst; idx ++) {
		memcpy(
			out_color_dst + (idx * color_size),
			in_color_src + (idxlut[idx] * color_size),
			color_size
			);
	}
	SAFE_RELEASE(distlut);
	SAFE_RELEASE(idxlut);
	return true;
}


bool
match_nearest_color4f(
	struct Color4F * out_color_dst, struct Vec3 * in_verts_dst, Size nverts_dst,
	struct Color4F * in_color_src,  struct Vec3 * in_verts_src, Size nverts_src
	) {
	return _T_match_nearest(
		(void *)out_color_dst, in_verts_dst, nverts_dst,
		(void *)in_color_src, in_verts_src, nverts_src,
		EColor4F
		);
}


bool
match_nearest_color4u(
	struct Color4U * out_color_dst,
	struct Vec3 * in_verts_dst,
	Size nverts_dst,
	struct Color4U * in_color_src,
	struct Vec3 * in_verts_src,
	Size nverts_src
	) {
	return _T_match_nearest(
		(void *)out_color_dst, in_verts_dst, nverts_dst,
		(void *)in_color_src,  in_verts_src, nverts_src,
		EColor4U
		);
}


static bool
_T_match_hsva(
	void * out_color_dst, struct Vec3 * in_verts_dst, Size nverts_dst,
	void * in_color_src,  struct Vec3 * in_verts_src, Size nverts_src,
	Float range,
	int offset,            // 根据第几个分量进行比较
	enum EMethod meth,     // 比较，看需要某个分量大的还是小的数据
	enum EColorType coltyp // 模板HSVA的分量类型
	) {
	Float dist = 0.0;
	Size sz_col = 0, sz_channel = 0;
	Size idxdst = 0, idxsrc = 0;
	struct Vec3 vdiff = { 0 };
	struct Vec3 * vsrc = NULL;
	struct Vec3 * vdst = NULL;
	struct HSVA * hsva_dst = NULL, * hsva_src = NULL;
	struct HSVA * hsva_cursor = NULL, * hsva_cursorA = NULL, * hsva_cursorB = NULL;
	void * color_cursor = NULL;
	bool ok = false;
	assert(nverts_dst == nverts_src);
	ok = _T_match_nearest(
		out_color_dst, in_verts_dst, nverts_dst,
		in_color_src, in_verts_src, nverts_src,
		coltyp
		);
	if (!ok) {
		return false;
	}
	switch (coltyp) {
		case EColor4U: {
			sz_col = sizeof(struct Color4U);
			sz_channel = sizeof(Byte);
			break;
		}
		case EColor4F: {
			sz_col = sizeof(struct Color4F);
			sz_channel = sizeof(Float);
			break;
		}
		default: {
			return false;
		}
	}
	hsva_dst = (struct HSVA *)malloc(sizeof(struct HSVA) * nverts_dst);
	hsva_src = (struct HSVA *)malloc(sizeof(struct HSVA) * nverts_src);
	if (!(hsva_dst && hsva_src)) {
		SAFE_RELEASE(hsva_dst);
		SAFE_RELEASE(hsva_src);
		return false;
	}
	for (idxdst = 0, hsva_cursor = hsva_dst, color_cursor = out_color_dst;
			 idxdst < nverts_dst;
			 idxdst ++, hsva_cursor ++, color_cursor += sz_col) {
		_T_color_to_hsva(hsva_cursor, color_cursor, coltyp);
	}
	for (idxsrc = 0, hsva_cursor = hsva_src, color_cursor = in_color_src;
			 idxsrc < nverts_src;
			 idxsrc ++, hsva_cursor ++, color_cursor += sz_col) {
		_T_color_to_hsva(hsva_cursor, color_cursor, coltyp);
	}

	/*
	 * 不要怀疑，真的没写错，之所以都用h来进行比较
	 * 主要还是因为可以通过OFFSET成功实现一个函数可以比较hsva四个通道
	 */
	#define __PROC__(__OP__) \
	for (idxdst = 0; idxdst < nverts_dst; idxdst ++) { \
		vdst = in_verts_dst + idxdst; \
		for (idxsrc = 0; idxsrc < nverts_src; idxsrc ++) { \
			vsrc = in_verts_src + idxsrc; \
			_vec3_sub(&vdiff, vsrc, vdst); \
			if (vdiff.x < range && vdiff.y < range && vdiff.z < range) { \
				dist = _vec3_dist(vsrc, vdst); \
				if (dist <= range) { \
					hsva_cursorA = (struct HSVA *)(((void *)(hsva_dst + idxdst)) + offset * sz_channel); \
					hsva_cursorB = (struct HSVA *)(((void *)(hsva_src + idxsrc)) + offset * sz_channel); \
					if (hsva_cursorB->h __OP__ hsva_cursorA->h) { \
						memcpy(out_color_dst + idxdst * sz_col, in_color_src + idxsrc * sz_col, sz_col); \
					} \
				} \
			} \
		} \
	}

	switch (meth) {
		case EGreaterThan:
			__PROC__(>)
			break;
		case ELessThan:
			__PROC__(<)
			break;
		case EGreaterThanOrEqual:
			__PROC__(>=)
			break;
		case ELessThanOrEqual:
			__PROC__(<=)
			break;
		case EEqual:
			__PROC__(==)
			break;
		default:
			SAFE_RELEASE(hsva_dst);
			SAFE_RELEASE(hsva_src);
			return false;
	}

	#undef __PROC__

	SAFE_RELEASE(hsva_dst);
	SAFE_RELEASE(hsva_src);
	return true;
}


bool
match_hsva_color4f(
	struct Color4F * out_color_dst, struct Vec3 * in_verts_dst, Size nverts_dst,
	struct Color4F * in_color_src,  struct Vec3 * in_verts_src, Size nverts_src,
	Float range,
	int offset,
	enum EMethod meth
	) {
	return _T_match_hsva(
		out_color_dst, in_verts_dst, nverts_dst,
		in_color_src, in_verts_src, nverts_src,
		range,
		offset,
		meth,
		EColor4F
	);
}


bool
match_hsva_color4u(
	struct Color4U * out_color_dst, struct Vec3 * in_verts_dst, Size nverts_dst,
	struct Color4U * in_color_src,  struct Vec3 * in_verts_src, Size nverts_src,
	Float range,
	int offset,
	enum EMethod meth
	) {
	return _T_match_hsva(
		out_color_dst, in_verts_dst, nverts_dst,
		in_color_src, in_verts_src, nverts_src,
		range,
		offset,
		meth,
		EColor4U
	);
}


