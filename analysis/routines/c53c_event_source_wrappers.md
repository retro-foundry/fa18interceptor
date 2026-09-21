# `$C53C08` and `$C53C8C`: event-source external wrappers

Static, byte-exact sources:
`source_amiga/observed/invoke_external_vector_174.asm` and
`source_amiga/observed/invoke_external_vector_1ce.asm`.

Both preserve `A6`, load the external base at `$C07F64`, pass their stack
argument in an address register, invoke a fixed negative vector, restore `A6`,
and return. `$C16BF2` uses vector `-$174` before consuming an event descriptor
and vector `-$1CE` afterwards. API names are intentionally unassigned.
