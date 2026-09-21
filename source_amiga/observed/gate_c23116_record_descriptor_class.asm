; Byte-exact record descriptor-class gate $C23116-$C23133.
; The shared classification targets remain separately bounded.

                org     $C23116

RECORD_DESCRIPTOR_STATE         equ $C459C2
RECORD_BANK_BASE                equ $C46184

gate_c23116_record_descriptor_class:
                move.l  a2,-(sp)
                tst.w   RECORD_DESCRIPTOR_STATE.l
                bne.b   $C23160
                lea     RECORD_BANK_BASE.l,a2
                move.b  $7C(a2),d0
                andi.b  #$F,d0
                cmpi.b  #$9,d0
                bne.b   $C23142
