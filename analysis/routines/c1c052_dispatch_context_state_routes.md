# `$C1C052` context-state command routes

Classification: **structural with byte-exact source**. This contiguous block
contains a mode-`$7D` route that toggles bit `$1000` at `$C46986`, a guarded
context-state route with a four-byte `$FF` fill through `$C4FDAC`, and an
alternate entry that writes 1 to `$C457AE` while clearing the low three bits
of `$C458DA`.

At `$C1C0E0`, the flare route first tests `D6`: nonzero enters the guarded
helper at `$C1C122`; zero falls through to the flare handler at `$C1C0E4`.
The field and helper roles are structural pending bounded command traces.
