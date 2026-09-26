; Byte-exact observed wrapped-long state comparison $C24FF0-$C25001.
; It compares a shared long against a mutable long: equality revisits the
; preceding continuation, while ordering selects the following wrap adjustment.

                org     $C24FF0

WRAP_REFERENCE_LONG             equ     $C45668
WRAP_MUTABLE_LONG               equ     $C4FF26

compare_wrapped_long_state:
                move.l  WRAP_REFERENCE_LONG,d0
                move.l  WRAP_MUTABLE_LONG,d1
                cmp.l   d0,d1
                beq.b   $C24FE6
                bgt.b   $C25012
