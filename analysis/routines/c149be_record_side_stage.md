# Record-side stage at `$C149BE`

Classification: **bounded structural stage**. In the sealed run003 frame-6,000
indexed-record packet, `$C25E26` calls `$C149BE` and it returns to `$C25E2C`
after 313 instructions. Future playback is empty. Canonical P-code is
`pcode/raw/run003_6000_c149be/` (277 RAM instruction starts, 1,921 operations,
three observed call targets).

Its observed nested path is `$C14A54 -> $C25754 -> $C1D974`. Record ownership
and arithmetic meaning remain unassigned.
