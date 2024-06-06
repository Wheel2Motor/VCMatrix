import os
import ctypes
from enum import IntEnum


_dll = ctypes.cdll.LoadLibrary(
        os.path.join(os.path.dirname(__file__),
        "..",
        "vcmatrix.dll"))


class EOffsetRGBA(IntEnum):
    EOffsetR = 0
    EOffsetG = 1
    EOffsetB = 2
    EOffsetA = 3


class EOffsetHSVA(IntEnum):
    EOffsetH = 0
    EOffsetS = 1
    EOffsetV = 2
    EOffsetA = 3


class EColorType(IntEnum):
    Color4U = 0
    Color4F = 1


class EMethod:
    EGreaterThan        = 0
    ELessThan           = 1
    EGreaterThanOrEqual = 2
    ELessThanOrEqual    = 3
    EEqual              = 4


class Vec3(ctypes.Structure):
    _pack_ = 8
    _fields_ = [
        ("x", ctypes.c_double),
        ("y", ctypes.c_double),
        ("z", ctypes.c_double),
        ]
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z
    def __repr__(self):
        return "Vec3 (%f, %f, %f) " % (self.x, self.y, self.z)

class Color4F(ctypes.Structure):
    _pack_ = 8
    _fields_ = [
        ("r", ctypes.c_double),
        ("g", ctypes.c_double),
        ("b", ctypes.c_double),
        ("a", ctypes.c_double),
        ]
    def __init__(self, r, g, b, a):
        self.r = r
        self.g = g
        self.b = b
        self.a = a
    def __repr__(self):
        return "Color4F (%.4f, %.4f, %.4f, %.4f) " % (self.r, self.g, self.b, self.a)

class Color4U(ctypes.Structure):
    _pack_ = 1
    _fields_ = [
        ("r", ctypes.c_uint8),
        ("g", ctypes.c_uint8),
        ("b", ctypes.c_uint8),
        ("a", ctypes.c_uint8),
        ]
    def __init__(self, r, g, b, a):
        self.r = r
        self.g = g
        self.b = b
        self.a = a
    def __repr__(self):
        return "Color4U (%d, %d, %d, %d) " % (self.r, self.g, self.b, self.a)


"""
******************************************************************************
                               Match Nearest
******************************************************************************
"""


_match_nearest_color4f = _dll.match_nearest_color4f
_match_nearest_color4f.restype = ctypes.c_bool
_match_nearest_color4f.argtypes = (
    ctypes.POINTER(Color4F), ctypes.POINTER(Vec3), ctypes.c_size_t,
    ctypes.POINTER(Color4F), ctypes.POINTER(Vec3), ctypes.c_size_t,
    )


_match_nearest_color4u = _dll.match_nearest_color4u
_match_nearest_color4u.restype = ctypes.c_bool
_match_nearest_color4u.argtypes = (
    ctypes.POINTER(Color4U),
    ctypes.POINTER(Vec3),
    ctypes.c_size_t,
    ctypes.POINTER(Color4U),
    ctypes.POINTER(Vec3),
    ctypes.c_size_t,
    )


def match_nearest_color4f(color_from, verts_from, verts_to):
    assert len(verts_from) == len(color_from)
    nverts_src = len(verts_from)
    nverts_dst = len(verts_to)
    color_dst = (Color4F * nverts_dst)()
    color_src = (Color4F * nverts_src)(*color_from)
    verts_dst = (Vec3 * nverts_dst)(*verts_to)
    verts_src = (Vec3 * nverts_src)(*verts_from)
    ok = _match_nearest_color4f(
        color_dst, verts_dst, nverts_dst,
        color_src, verts_src, nverts_src,
        )
    return tuple(color_dst)


def match_nearest_color4u(color_from, verts_from, verts_to):
    assert len(verts_from) == len(color_from)
    nverts_src = len(verts_from)
    nverts_dst = len(verts_to)
    color_dst = (Color4U * nverts_dst)()
    color_src = (Color4U * nverts_src)(*color_from)
    verts_dst = (Vec3 * nverts_dst)(*verts_to)
    verts_src = (Vec3 * nverts_src)(*verts_from)
    ok = _match_nearest_color4u(
        color_dst, verts_dst, nverts_dst,
        color_src, verts_src, nverts_src,
        )
    return tuple(color_dst)

"""
******************************************************************************
                               Match HSVA
******************************************************************************
"""


_match_hsva_color4f = _dll.match_hsva_color4f
_match_hsva_color4f.restype = ctypes.c_bool
_match_hsva_color4f.argtypes = (
    ctypes.POINTER(Color4F), ctypes.POINTER(Vec3), ctypes.c_size_t,
    ctypes.POINTER(Color4F), ctypes.POINTER(Vec3), ctypes.c_size_t,
    ctypes.c_double,
    ctypes.c_int,
    ctypes.c_int,
    )


_match_hsva_color4u = _dll.match_hsva_color4u
_match_hsva_color4u.restype = ctypes.c_bool
_match_hsva_color4u.argtypes = (
    ctypes.POINTER(Color4U), ctypes.POINTER(Vec3), ctypes.c_size_t,
    ctypes.POINTER(Color4U), ctypes.POINTER(Vec3), ctypes.c_size_t,
    ctypes.c_double,
    ctypes.c_int,
    ctypes.c_int,
    )


def match_hsva_color4f(color_from, verts_from, verts_to, rng, offset, meth):
    assert len(verts_from) == len(color_from)
    nverts_src = len(verts_from)
    nverts_dst = len(verts_to)
    color_dst = (Color4F * nverts_dst)()
    color_src = (Color4F * nverts_src)(*color_from)
    verts_dst = (Vec3 * nverts_dst)(*verts_to)
    verts_src = (Vec3 * nverts_src)(*verts_from)
    ok = _match_hsva_color4f(
        color_dst, verts_dst, nverts_dst,
        color_src, verts_src, nverts_src,
        rng,
        offset,
        meth,
        )
    return tuple(color_dst)


def match_hsva_color4u(color_from, verts_from, verts_to, rng, offset, meth):
    assert len(verts_from) == len(color_from)
    nverts_src = len(verts_from)
    nverts_dst = len(verts_to)
    color_dst = (Color4U * nverts_dst)()
    color_src = (Color4U * nverts_src)(*color_from)
    verts_dst = (Vec3 * nverts_dst)(*verts_to)
    verts_src = (Vec3 * nverts_src)(*verts_from)
    ok = _match_hsva_color4u(
        color_dst, verts_dst, nverts_dst,
        color_src, verts_src, nverts_src,
        rng,
        offset,
        meth,
        )
    return tuple(color_dst)


if __name__ == "__main__":

    import time
    import random
    from itertools import cycle

    def timeit(func):
        def _(*args, **kwargs):
            print("@ Executing {}".format(func.__name__))
            beg = time.time()
            res = func(*args, **kwargs)
            end = time.time()
            print("- Time elapsed: {}".format(end - beg))
            return res
        return _

    """
    Match nearest
    """

    @timeit
    def test_nearest_color4f(sample):
        pos = cycle((random.random() for i in range(100)))
        col = cycle((random.random() for i in range(100)))
        color_dst = (Color4F * sample)()
        color_src = (Color4F * sample)()
        verts_dst = (Vec3 * sample)()
        verts_src = (Vec3 * sample)()
        for i in range(sample):
            color_src[i] = Color4F(next(col), next(col),next(col),next(col))
            verts_src[i] = Vec3(next(pos), next(pos), next(pos))
            verts_dst[i] = Vec3(next(pos), next(pos), next(pos))
        _match_nearest_color4f(
            color_dst, verts_dst, sample,
            color_src, verts_src, sample,
            )


    @timeit
    def test_nearest_color4u(sample):
        pos = cycle((random.random() for i in range(100)))
        col = cycle((random.randint(0, 255) for i in range(100)))
        color_dst = (Color4F * sample)()
        color_src = (Color4F * sample)()
        verts_dst = (Vec3 * sample)()
        verts_src = (Vec3 * sample)()
        for i in range(sample):
            color_src[i] = Color4F(next(col), next(col),next(col),next(col))
            verts_src[i] = Vec3(next(pos), next(pos), next(pos))
            verts_dst[i] = Vec3(next(pos), next(pos), next(pos))
        _match_nearest_color4f(
            color_dst, verts_dst, sample,
            color_src, verts_src, sample,
            )

    @timeit
    def testpy_nearest_color4f(sample):
        pos = cycle((random.random() for i in range(100)))
        col = cycle((random.random() for i in range(100)))
        color_src = tuple(Color4F(next(col), next(col), next(col), next(col)) for i in range(sample))
        verts_dst = tuple(Vec3(next(pos), next(pos), next(pos)) for i in range(sample))
        verts_src = tuple(Vec3(next(pos), next(pos), next(pos)) for i in range(sample))
        match_nearest_color4f(color_src, verts_src, verts_dst)

    @timeit
    def testpy_nearest_color4u(sample):
        pos = cycle((random.random() for i in range(100)))
        col = cycle((random.randint(0, 255) for i in range(100)))
        color_src = tuple(Color4U(next(col), next(col), next(col), next(col)) for i in range(sample))
        verts_dst = tuple(Vec3(next(pos), next(pos), next(pos)) for i in range(sample))
        verts_src = tuple(Vec3(next(pos), next(pos), next(pos)) for i in range(sample))
        match_nearest_color4u(color_src, verts_src, verts_dst)


    """
    Match hsva
    """

    @timeit
    def test_hsva_color4f(sample, rng, offset, meth):
        pos = cycle((random.random() for i in range(100)))
        col = cycle((random.random() for i in range(100)))
        color_dst = (Color4F * sample)()
        color_src = (Color4F * sample)()
        verts_dst = (Vec3 * sample)()
        verts_src = (Vec3 * sample)()
        for i in range(sample):
            color_src[i] = Color4F(next(col), next(col),next(col),next(col))
            verts_src[i] = Vec3(next(pos), next(pos), next(pos))
            verts_dst[i] = Vec3(next(pos), next(pos), next(pos))
        _match_hsva_color4f(
            color_dst, verts_dst, sample,
            color_src, verts_src, sample,
            rng,
            offset,
            meth,
            )

    @timeit
    def test_hsva_color4u(sample, rng, offset, meth):
        pos = cycle((random.random() for i in range(100)))
        col = cycle((random.randint(0, 255) for i in range(100)))
        color_dst = (Color4F * sample)()
        color_src = (Color4F * sample)()
        verts_dst = (Vec3 * sample)()
        verts_src = (Vec3 * sample)()
        for i in range(sample):
            color_src[i] = Color4F(next(col), next(col),next(col),next(col))
            verts_src[i] = Vec3(next(pos), next(pos), next(pos))
            verts_dst[i] = Vec3(next(pos), next(pos), next(pos))
        _match_hsva_color4f(
            color_dst, verts_dst, sample,
            color_src, verts_src, sample,
            rng,
            offset,
            meth,
            )

    @timeit
    def testpy_hsva_color4f(sample, rng, offset, meth):
        pos = cycle((random.random() for i in range(100)))
        col = cycle((random.random() for i in range(100)))
        color_src = tuple(Color4F(next(col), next(col), next(col), next(col)) for i in range(sample))
        verts_dst = tuple(Vec3(next(pos), next(pos), next(pos)) for i in range(sample))
        verts_src = tuple(Vec3(next(pos), next(pos), next(pos)) for i in range(sample))
        match_hsva_color4f(color_src, verts_src, verts_dst, rng, offset, meth)

    @timeit
    def testpy_hsva_color4u(sample, rng, offset, meth):
        pos = cycle((random.random() for i in range(100)))
        col = cycle((random.randint(0, 255) for i in range(100)))
        color_src = tuple(Color4U(next(col), next(col), next(col), next(col)) for i in range(sample))
        verts_dst = tuple(Vec3(next(pos), next(pos), next(pos)) for i in range(sample))
        verts_src = tuple(Vec3(next(pos), next(pos), next(pos)) for i in range(sample))
        match_hsva_color4u(color_src, verts_src, verts_dst, rng, offset, meth)

    SAMPLE = 5000
    RNG = 0.005

    test_nearest_color4f(SAMPLE)
    test_nearest_color4u(SAMPLE)
    testpy_nearest_color4f(SAMPLE)
    testpy_nearest_color4u(SAMPLE)
    test_hsva_color4f(SAMPLE,   RNG, EOffsetHSVA.EOffsetS, EMethod.ELessThan)
    test_hsva_color4u(SAMPLE,   RNG, EOffsetHSVA.EOffsetS, EMethod.ELessThan)
    testpy_hsva_color4f(SAMPLE, RNG, EOffsetHSVA.EOffsetS, EMethod.ELessThan)
    testpy_hsva_color4u(SAMPLE, RNG, EOffsetHSVA.EOffsetS, EMethod.ELessThan)
