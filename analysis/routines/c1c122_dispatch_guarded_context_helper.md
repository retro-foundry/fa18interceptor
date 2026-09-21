# `$C1C122` guarded context-helper route

Classification: **structural with byte-exact source**. In context mode 6 with
a zero guard word at `$C458C2`, the route tests bit 6 in three observed record
bytes. The bit pattern permits a helper call with stack arguments `$30` and
`$1C`; it first ORs `$0F` into `$C46200`, then restores the original `D0`
word and enters the common command queue.

The helper at `$C17F8C`, record identities, and higher-level command meaning
need bounded traces before semantic names can be assigned.
