# `$C2E370` one-angle native-trig matrix builder

Classification: **byte-exact static reconstruction**.  The direct Hunk-32
caller `$C2DB12` invokes this helper after loading an angle from the active
512-byte control record.  It shifts the native angle by three, calls the
sine/cosine lookup at `$C2E6DA`, and writes nine words through `A1`.

Unlike `$C2E346`, this form retains the trigonometry values in their native
scale and writes `$4000` as the fixed axis.  The full `$C2E370-$C2E38D` range
(30 bytes) is byte-exact source in
`source_amiga/observed/build_single_angle_trig_matrix.asm`.
