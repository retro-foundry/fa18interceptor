; Byte-exact post-stream record gate $C1F844-$C1F871.
; Downstream branches remain separately bounded.

                org     $C1F844

POST_STREAM_FLAGS               equ $C4585B
STREAM_RECORD_TABLE             equ $C46184
STREAM_RECORD_INDEX             equ $C459B6

gate_c1f844_post_stream_record:
                btst.b  #6,POST_STREAM_FLAGS.l
                bne.b   $C1F83C
                btst.b  #4,POST_STREAM_FLAGS.l
                beq.b   $C1F83C
                lea     STREAM_RECORD_TABLE.l,a0
                adda.w  STREAM_RECORD_INDEX.l,a0
                dc.w    $3028,0                 ; move.w  0(a0),d0
                andi.w  #$400,d0
                beq.b   $C1F8D8
