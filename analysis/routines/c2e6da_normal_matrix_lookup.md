# `$C2E6DA` normal matrix trigonometry lookup (run003)

## Observed boundary

The single-angle matrix constructor calls this routine through the direct edge
`$C2E348 -> $C2E6DA -> $C2E34C`.  A return-bounded capture restored at run003
frame 6,000 hit the breakpoint on frame 5 and reached `$C2E34C` after 2,397
executed instructions.  No input was delivered after the breakpoint armed.

Canonical P-code is in `pcode/raw/run003_6000_c2e6da_matrix_lookup/`.

## Reconstructed code

The entry block itself is already reconstructed byte-for-byte as
`source_amiga/observed/lookup_sine_cosine.asm` (`$C2E6DA-$C2E74F`).  It maps
the native angle in `D4.w` to sine (`D4.w`) and cosine (`D5.w`) using the
quadrant-symmetric table at `$C3E5E8`.

## Scope boundary

The return-bounded packet contains normal frame and callback activity beyond
the small static lookup block.  Its importer-visible call targets are therefore
not evidence that the source-level lookup has those children.  The evidence
here establishes the constructor edge and return; the byte-exact routine file
is the authority for the lookup's local semantics.
