# `$C1F6F8`: record-walker control-stream setup

Classification: **runtime-backed control/dataflow**; stream owner and record
semantics remain unassigned.

`source_amiga/observed/initialize_record_walker_from_pointer.asm` is byte
exact for `$C1F6F8-$C1F79F`.

The setup resets the line-emitter masks, loads `A5` from `$C45A36`, and enters
the control/record walker. Ordinary control words select an `$C48390` triple
plus two further triples into `$C4BF94` before calling `$C1FB8C`; another
control bit selects `$C1FB9C`, and a third class calls `$C1FC42`.

The completed run060 `$C1EE14` invocation at replay frame 301 provides a
same-invocation producer/consumer fixture: `$C1EF10` writes
`$C45A36=$C34A56`, and this setup loads that value into `A5`. Its first fetch
at `$C1F716` reads `$FFFF` from `$C34A56`; the negative-word selector takes
the terminal exit, so no record-dispatch routine is entered in this walker
visit. The nearby `$C35132` is instead the `$C45A32` value published by the
same producer, not the walker cursor.

Negative control words enter the previously reconstructed `$C1F7A0` stream
selector. In the frame-602 projected-edge packet, the live walker had
`A5=$C392A8` and its dispatched `$C212B0` callee received
`A2=$C3985A`. This establishes the control-stream ancestry of the stable edge
list, while leaving the writer/owner of `$C45A36` for the next step.

## Frame-boundary qualification

The sealed run024 frame-23,000 checkpoint has `$C45A36=$C4553E`, but a separate
no-input replay from that checkpoint hits `$C1F6F8` on its next chipset frame
and returns to `$C1CFA8` after only 14 instructions. Its first fetched control
word is terminal `$FFFF`. Thus the pointer visible in a completed-frame
snapshot is not automatically the input of the next invocation: preceding
normal-frame work may replace it. This terminal packet is useful negative
evidence only; it neither identifies nor excludes the Golden Gate Bridge's
geometry stream.
