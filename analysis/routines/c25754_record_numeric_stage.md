# Record numeric stage at `$C25754`

Classification: **bounded structural stage**. In sealed run003 frame 6,000,
`$C14A54` calls `$C25754` and it returns to `$C14A5A` after 84 instructions.
Future playback is empty. Canonical P-code is
`pcode/raw/run003_6000_c25754/` (82 RAM instruction starts, 683 operations,
one observed call target).

The route directly calls `$C1D974`. A separate return-bounded trace of that
child does not return to `$C25784` within 3,000 instructions, so it is retained
as a capped packet and not treated as a completed function.
