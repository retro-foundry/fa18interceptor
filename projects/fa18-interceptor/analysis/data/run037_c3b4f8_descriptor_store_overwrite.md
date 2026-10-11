# `$C3B4F8` descriptor store before control-walker overwrite

Classification: **bounded descriptor-store ordering**.

The no-future-input trace starts at `$C1CC70` reading `$C22708`, whose static `+8` field is `$C3B4F8`. Before the next `$C1F6F8` entry, it performs 11 later descriptor-field stores. The walker enters with `A1 = $C3B73E`, not `$C3B4F8`.

Therefore this trace proves `$C3B4F8` is live descriptor-control input, but does not prove it reaches the walker, executes `$C3B4FE`, or identifies a mountain instance.

| Trace index | descriptor `+8` field | stored control target |
| ---: | --- | --- |
| 0 | `$C22708` | `$C3B4F8` |
| 170 | `$C22334` | `$C445DC` |
| 295 | `$C227F8` | `$C44612` |
| 420 | `$C22604` | `$C37EA6` |
| 580 | `$C225F0` | `$C37E56` |
| 1025 | `$C228D4` | `$C44A3A` |
| 1148 | `$C228E8` | `$C44AA4` |
| 1271 | `$C229D8` | `$C45026` |
| 1393 | `$C22898` | `$C44970` |
| 1515 | `$C22A64` | `$C453C4` |
| 1679 | `$C22A78` | `$C4545C` |
| 1801 | `$C224D8` | `$C3B6A6` |

C1CC70 store ordering and the next observed C1F6F8 entry are proven. This does not establish that a stored target is executed, parsed, or a model/terrain pointer.
