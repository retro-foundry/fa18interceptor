# `$C347F2` postflight marker walker

Static boundary: `$C347F2` begins a distinct linked-frame helper, not a
fall-through continuation of the `$C345A0` byte-pair renderer. Its byte-exact
setup is `prepare_postflight_marker_walker.asm` (`$C347F2-$C34829`).

The following marker-orientation loops occupy `$C3482A-$C34875`; they join the
shared filter at `$C34876`, which conditionally calls `$C2F66E`, then returns
at `$C348AC-$C348AE`. `$C348B0` is an adjacent return stub. The earlier
apparent back edge belongs to the preceding fallback routine, not this helper.
