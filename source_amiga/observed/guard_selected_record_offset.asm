; Byte-exact observed guard prefix $C2651E-$C2652B.
; The unequal route returns through adjacent raw stub $C2651C.

                org     $C2651E

SELECTED_RECORD_OFFSET          equ $C459B6
GUARD_RECORD_OFFSET              equ $C4FDD2
RETURN_UNEQUAL_RECORD_OFFSET     equ $C2651C

guard_selected_record_offset:
                move.w  SELECTED_RECORD_OFFSET.l,d0
                cmp.w   GUARD_RECORD_OFFSET.l,d0
                bne.b   RETURN_UNEQUAL_RECORD_OFFSET
