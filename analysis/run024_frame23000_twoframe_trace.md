# Run024 frame-23,000 qualification-flight sample

`pcode/raw/run024_frame23000_twoframe_trace_validated/` is a two-frame,
no-input trace restored from the normal replay checkpoint at run024 frame
23,000.  The checkpoint corresponds to the user-identified San Francisco
cockpit view with the Golden Gate Bridge distant in the background.

The trace records 3,779 RAM instruction starts (13,630 bytes) and 129
structural call targets.  It adds 1,814 distinct bytes to the union of
trace-observed instructions.  This is a flight-frame execution sample, not an
identification of any individual landmark, qualification status, or LOD rule.

The recorder retains CPU-confirmed shorter sequential instruction lengths when
they disagree with Capstone's 68000 length.  This capture contains repeated
`SBCD -(A2),-(A1)` forms for which Capstone 5.0.7 reports four bytes while the
emulated CPU advances two; the corrected byte ranges import cleanly in Ghidra.

As with every stepped trace, its timing is bounded to this no-input sample and
does not establish held-input/autorepeat equivalence to an ordinary replay.
