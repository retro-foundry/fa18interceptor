# `$C4FFB0` indexed `COPJMP2` trigger

Classification: **structural hardware effect**.

The complete 26-byte leaf reads a longword argument from `SP+$04`, shifts it
by two, uses it to index the `$C4FE28` pointer table, then writes that selected
entry's word at `+$14` to OCS custom register `$DFF09C` (`COPJMP2`). It is
observed in 12 capture packets, including direct command-child paths.

On OCS, writing `COPJMP2` is the Copper jump-2 strobe: it causes a jump to the
address held in the `COP2LC` pointer registers. The source word is still an
ordinary observed bus-write value; this evidence does not prove it is itself a
Copper pointer, list identifier, or record type.

`source_amiga/observed/trigger_copper_jump2_for_indexed_record.asm` preserves
the complete `$C4FFB0-$C4FFC9` entry through `RTS`.
