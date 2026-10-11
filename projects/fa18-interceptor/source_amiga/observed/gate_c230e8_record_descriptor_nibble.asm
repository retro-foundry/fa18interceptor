; Byte-exact record descriptor-nibble gate $C230E8-$C23107.
; Post-classification branches remain separately bounded.

                org     $C230E8

RECORD_DESCRIPTOR_STATE         equ $C459C2
RECORD_BANK_BASE                equ $C46184

gate_c230e8_record_descriptor_nibble:
                move.l  a2,-(sp)
                tst.w   RECORD_DESCRIPTOR_STATE.l
                bne.b   $C23160
                lea     RECORD_BANK_BASE.l,a2
                move.b  $7C(a2),d0
                move.b  d0,d1
                andi.b  #$F,d0
                cmpi.b  #$E,d0
                blt.b   $C2312E
