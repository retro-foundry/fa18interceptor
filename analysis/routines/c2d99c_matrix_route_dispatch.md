# `$C2D99C` matrix-route dispatcher

Classification: **byte-exact static reconstruction**. The parent update calls
this routine at `$C0F02A`. It tests `$C45785`, dispatching to `$C2D9BA` when
nonzero and `$C2DB18` when zero, then returns to `$C0F030`.

The full `$C2D99C-$C2D9AF` range (20 bytes) is
`source_amiga/observed/dispatch_matrix_update_route.asm`. The two route bodies
remain separate structural routines; the state byte has no assigned game-level
meaning.
