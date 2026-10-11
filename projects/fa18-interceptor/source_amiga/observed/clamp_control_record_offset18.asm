; Byte-exact observed active-control-record +$18 clamp $C15112-$C15125.
; The shared long at $C456FA caps the record's long +$18; values already at or
; below that bound use the routine epilogue, while larger values are replaced
; by the bound in the following block.

                org     $C15112

CONTROL_RECORD_CLAMP_VALUE      equ     $C456FA
CURRENT_CONTROL_RECORD          equ     $C18210

clamp_control_record_offset18:
                move.l  CONTROL_RECORD_CLAMP_VALUE,d0
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.l  $18(a0),d1
                cmp.l   d0,d1
                ble.b   $C15130
