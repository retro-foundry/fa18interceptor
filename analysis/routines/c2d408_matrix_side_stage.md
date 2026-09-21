# Matrix-side stage at `$C2D408`

Classification: **bounded structural stage**. In the sealed run003 frame-6,000
indexed-record packet, `$C25D9E` calls `$C2D408` and execution returns to
`$C25DA4` after 872 instructions. Future playback is empty. Canonical P-code
is `pcode/raw/run003_6000_c2d408/` (732 RAM instruction starts, 5,411
operations, eight observed call targets).

This packet is the live matrix-side child reached from `$C25B66`. It includes
an observed direct call `$C2D618 -> $C1342C`; its data ownership and transform
meaning remain unassigned pending narrower child traces.
