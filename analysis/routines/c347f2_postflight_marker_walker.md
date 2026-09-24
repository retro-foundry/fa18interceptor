# `$C347F2` postflight marker walker

Static boundary: `$C347F2` begins a distinct linked-frame helper, not a
fall-through continuation of the `$C345A0` byte-pair renderer. Its byte-exact
setup is `prepare_postflight_marker_walker.asm` (`$C347F2-$C34829`).

The following marker-orientation loops begin at `$C3482A`. Linear decoding
through `$C34875` is insufficient to choose an assembly-slice end: an apparent
back edge in the nearby listing belongs to the preceding fallback routine.
Map the full marker-walker control flow before reconstructing that body.
