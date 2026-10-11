; Byte-exact observed active-control-record word-$56 update $C13BDC-$C13C09.
; It subtracts one quarter of the caller word from the record field, preserves
; the record pointer for the C13CDE follow-up, then closes the stack frame.

                org     $C13BDC

CURRENT_CONTROL_RECORD          equ     $C18210
CONTROL_RECORD_FOLLOWUP         equ     $C13CDE

update_control_record_word56:
                movea.l CURRENT_CONTROL_RECORD.l,a0
                ; Preserve the binary's immediate ADDA encoding.
                dc.w    $D0FC,$0056             ; adda.w #$56,a0
                move.w  (a0),d0
                dc.w    $3F40,$0000             ; move.w d0,$0(a7)
                sub.w   $A(a6),d0
                asr.w   #2,d0
                dc.w    $322F,$0000             ; move.w $0(a7),d1
                sub.w   d0,d1
                move.w  d1,(a0)
                move.l  a0,-(a7)
                move.l  a0,-$4(a6)
                bsr.w   CONTROL_RECORD_FOLLOWUP
                addq.l  #4,a7
                unlk    a6
                rts
