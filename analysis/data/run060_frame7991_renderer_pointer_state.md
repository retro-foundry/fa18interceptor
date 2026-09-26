# run060 frame 7991 renderer pointer state

At the live `$C304F4` breakpoint in replay frame 7991, Engine9000 memory reads
show the selected active page table:

| semantic source | values |
|---|---|
| `$C4567E` active plane table | `$018980`, `$016A40`, `$014B00`, `$012BC0` |
| `$C4566E` alternate renderer table | `$0538F0`, `$0519B0`, `$04FA70`, `$04DB30` |
| `$C456B6` table selectors | `$C4567E`, `$C456A2`, `$04DB30`, `$04FA70` |
| `$C45960` live lane pointer | `$0139CE` |
| `$C4596E` setup size | `$1694` |
| `$C456E7/$C456E8` lane controls | `$0100/$0001` |

The selected `$C4567E` table agrees with the known display page order: its
entries are lanes 4, 3, 2, and 1, which map to semantic planes 3, 2, 1, and
0. The changed `$012BC0` range in the settled capture is therefore semantic
plane 0 under the port's low-bit-first page representation. The earlier
description of this as “active plane 1” referred to the original hardware
lane numbering and should not be used as the native plane index.

The capture was made by `scripts/capture_renderer_state.py` after restoring
the run060 checkpoint and replaying the recorded controls through frame 7991.
It reads the pointer tables without copying Amiga memory into the port.
