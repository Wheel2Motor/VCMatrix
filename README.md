VCMatrix
===

### Introduction

Some simple function written in C for Vertex Color passing between LODs.

### Usage

See example/test.c.

* `match_nearest_[coltyp]` matches verts by distance.

* `match_hsva_[coltyp]` matches verts by one channel of HSVA,
you can specify range to prompt for comparasion within some distance,
specify EOffsetH, EOffsetS, EOffsetV, EOffsetA to compare one channel,
and specify EGreaterThan to select bigger one, ELessThan to select smaller one.

### Test

* For test on example/test.c, use `make testc`.

* For test on example/test.py, use `make testpy`.
