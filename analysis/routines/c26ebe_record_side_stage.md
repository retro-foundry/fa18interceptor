# Record-side stage at `$C26EBE`

Classification: **bounded structural stage**. In sealed run003 frame 6,000,
`$C2600E` calls `$C26EBE` and it returns to `$C26014` after 815 instructions
with empty future playback. Canonical P-code is
`pcode/raw/run003_6000_c26ebe/` (219 RAM instruction starts, 1,275 operations,
eight observed call targets). Its record and rendering ownership remain
unassigned.

After the bounded entry, the observed game-RAM path continues through the
`$C26xxx`, `$C27xxx`, and `$C279C0` return blocks without a direct game `JSR`.
The exported packet also contains normal-frame operating-system callback work;
the 815-instruction caller-return boundary, rather than those incidental calls,
is the packet authority.
