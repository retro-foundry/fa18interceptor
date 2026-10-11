; Byte-exact observed coordinate-update local-term setup $C12D2E-$C12D5F.
; It initializes a local selector, converts one local signed word through the
; shared stack-argument helper, then derives a signed high-nibble term from
; active-record word +$5A.

                org     $C12D2E

ACTIVE_CONTROL_RECORD           equ     $C18210
STACK_ARGUMENT_GATE             equ     $C131BE

prepare_coordinate_update_local_terms:
                move.w  #$3D,-$6(a6)
                move.w  -$2(a6),d0
                ext.l   d0
                move.l  d0,-(a7)
                bsr.w   STACK_ARGUMENT_GATE
                addq.l  #4,a7
                move.w  d0,-$4(a6)
                tst.w   d0
                beq.w   $C13112
                movea.l ACTIVE_CONTROL_RECORD.l,a0
                move.w  $5A(a0),d0
                asr.w   #4,d0
                move.w  d0,-$C(a6)
                tst.w   d0
                bpl.b   $C12D64
