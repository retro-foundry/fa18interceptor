# Indexed update-transform setup at `$C149BE`

Classification: **dataflow/behavioural transform caller**.  This routine is
called from `$C25E26` in the repeatedly observed active update path.

The byte-exact entry setup through `$C14A81` is in
`source_amiga/observed/prepare_indexed_update_transform.asm`.  It selects a
512-byte record at `$C46184 + (sign_extend(word[$C459B4]) << 9)`, publishes
that address to `$C18210`, and derives three adjacent pointers from it.

It copies record words at offsets `$96`, `$9C`, `$A2`, and a negated `$6E` into
the argument setup for `$C25754`, then reads three resulting words from
`$C45A4C-$C45A50` into locals.  A bit-7 test of the first derived pointer's
word controls entry into the raw continuation at `$C14AF2`.

The record's gameplay ownership and the callee's coordinate convention are not
yet assigned.  The selector stride, publication, transform call, output words,
and branch condition are direct static/runtime dataflow evidence.
