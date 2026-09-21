# Matrix-side leaf at `$C1342C`

Classification: **bounded structural leaf**. In the sealed run003 frame-6,000
matrix-side packet, `$C2D618` calls `$C1342C` and it returns to `$C2D61E`
after 103 instructions. Future playback is empty. Canonical P-code is
`pcode/raw/run003_6000_c1342c/` (103 RAM instruction starts, 487 operations,
no nested calls). Its arithmetic and record ownership remain unassigned.

The observed route publishes `$C46184 + ($C459B4 << 9)` at `$C18210`, selects
the static table base `$C3D690` when the index is zero, and returns through
`$C13A22`. Its enclosing static routine is substantially wider than the live
packet, so no partial source reconstruction is claimed.
