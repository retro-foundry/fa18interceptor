# `$C3925A`: external-view control-stream words

Classification: **runtime-backed static control data inside original Hunk 52 CODE**. These words are not model coordinates.

Segment 52 is verified at `$C39168-$C39E3B`; `$C3925A` is segment 52 payload offset `$F2`. The exact six bytes are stable original payload bytes:

| Address | Word |
| --- | --- |
| `$C3925A` | `$0ADA` |
| `$C3925C` | `$80F4` |
| `$C3925E` | `$80FE` |

Two independent external-camera polygon collectors enter `$C2FF48` with `A5=$C3925C` and `A5=$C3925E`:

- frame 7,500, where the player aircraft is visibly external;
- frame 12,600, where the external player aircraft is beneath the Golden Gate Bridge.

This proves the addresses are a static external-view control-stream fragment consumed by the renderer path. It narrows the `$C3925x` polygon groups to a stable upstream control family, but does not by itself identify the player model or locate immutable source vertices.
