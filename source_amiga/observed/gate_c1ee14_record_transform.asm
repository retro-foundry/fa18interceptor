; Byte-exact record transform gate $C1F000-$C1F025.

                org     $C1F000

STREAM_RECORD_TABLE             equ $C46184
STREAM_RECORD_INDEX             equ $C459B6

gate_c1ee14_record_transform:
                lea     STREAM_RECORD_TABLE.l,a0
                adda.w  STREAM_RECORD_INDEX.l,a0
                dc.w    $3228,0                 ; move.w  0(a0),d1
                btst    #6,d1
                beq.w   $C1EEA0
                move.b  -136(a6),123(a0)
                andi.w  #$400,d1
                beq.b   $C1F078
