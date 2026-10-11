# Run037 bounded placed-component inventory

Classification: **complete walker coverage for one bounded M-map trace**. All
six `$C1F6F8` walkers in the 20-frame run037 placement/control trace now have
an exact placement, transformed local input, and bounded following primitive
association. This inventories the trace window only; it is not complete world
coverage.

| walker stream | placement `(X,Y,Z)` | local source | bounded output | identity |
| --- | --- | --- | --- | --- |
| `$C35BD2` | `(0,0,-328)` | `$C35BDE` | two `$C35BD4` lines | unknown |
| `$C35598` | `(460,0,172)` | `$C35BAA` | one `$C3559A` line | red-line-family reuse |
| `$C355D0` | `(460,0,148)` | `$C35BC2` | one `$C355D2` line | red-line-family reuse |
| `$C36214` | `(272,0,146)` | `$C36220` | two `$C36216` lines | unknown |
| `$C36210` | `(272,0,146)` | `$C36220` | two `$C36212` lines | unknown |
| `$C3B6AE` | `(368,0,-144)` | `$C3B720` | four `$C3B6B0` polygons | unknown |

The first, fourth, fifth, and sixth rows are exported respectively as
[`c35bd0_bounded_instance_asset.json`](c35bd0_bounded_instance_asset.json),
[`c36212_bounded_instance_asset.json`](c36212_bounded_instance_asset.json),
[`c36208_bounded_instance_asset.json`](c36208_bounded_instance_asset.json),
and [`c3b6a6_bounded_instance_asset.json`](c3b6a6_bounded_instance_asset.json).
The red-line-family rows are retained separately because their landmark name is
verified only in the run003/run033 Golden Gate scenario, not globally in this
reused run037 view.
