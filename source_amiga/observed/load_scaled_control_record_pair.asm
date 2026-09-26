; Byte-exact observed indexed-update continuation $C14A82-$C14AA3.
; It extracts the high-word-scale pair from the selected control record into
; frame locals.  The mode/zero-local path at $C14AA4 may replace that pair
; with a coarser scale; field ownership remains unassigned.

                org     $C14A82

CURRENT_CONTROL_RECORD          equ     $C18210
CONTROL_SCALE_MODE              equ     $C457A0
RECORD_LONG_PAIR_FIRST          equ     $3E
RECORD_LONG_PAIR_SECOND         equ     $46

load_scaled_control_record_pair:
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.l  RECORD_LONG_PAIR_FIRST(a0),d0
                asr.l   #2,d0
                move.l  RECORD_LONG_PAIR_SECOND(a0),d1
                asr.l   #2,d1
                move.w  d0,-$8(a6)
                move.w  d1,-$c(a6)
                tst.b   CONTROL_SCALE_MODE.l
                beq.b   $C14ABE
