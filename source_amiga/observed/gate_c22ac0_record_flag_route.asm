; Byte-exact record-flag routing gate $C22AC0-$C22ADD.
; Alternate branches remain separately bounded.

                org     $C22AC0

STREAM_RECORD_TABLE             equ $C46184
STREAM_RECORD_INDEX             equ $C459B6

gate_c22ac0_record_flag_route:
                lea     STREAM_RECORD_TABLE.l,a1
                adda.w  STREAM_RECORD_INDEX.l,a1
                dc.w    $3229,0                 ; move.w  0(a1),d1
                move.w  d1,d2
                andi.w  #$40,d1
                beq.b   $C22B04
                andi.w  #$200,d2
                beq.b   $C22AFE
