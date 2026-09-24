; Byte-exact static table-to-bitmask builder $C1C40C-$C1C54D.
; It initializes three 2 KiB buffers then expands the three compact streams at
; C42290, C42390, and C42490.  Table/bitmask ownership remains structural.

                org     $C1C40C

BITMASK_BUFFER_A               equ     $C1929C
BITMASK_BUFFER_B               equ     $C19A9C
BITMASK_BUFFER_C               equ     $C1A29C
BITMASK_BUFFER_SIZE            equ     $0800
BITMASK_BUFFER_STEP            equ     $20
BITMASK_BUFFER_BLOCKS          equ     $3F
BITMASK_STREAM_A               equ     $C42290
BITMASK_STREAM_B               equ     $C42390
BITMASK_STREAM_C               equ     $C42490
BITMASK_STREAM_ROWS            equ     $7F
BITMASK_STREAM_WORD_STEP       equ     2
BITMASK_STREAM_BUFFER_STEP     equ     $10
BITMASK_INDEX_BITS             equ     $1F
BITMASK_ERROR_A                equ     $0043
BITMASK_ERROR_B                equ     $0044
BITMASK_ERROR_C                equ     $0045
BITMASK_ERROR_WORD             equ     $C4599E
BITMASK_ERROR_HELPER           equ     $C06C02

build_table_bitmask_buffers:
                moveq   #0,d0
                move.l  d0,d1
                move.l  d0,d2
                move.l  d0,d3
                move.l  d0,d4
                move.l  d0,d5
                move.l  d0,d6
                movea.l d0,a3
                moveq   #BITMASK_BUFFER_BLOCKS,d7
                lea     BITMASK_BUFFER_A.l,a0
                lea     BITMASK_BUFFER_B.l,a1
                lea     BITMASK_BUFFER_C.l,a2
build_table_bitmask_buffers_clear_loop:
                movem.l d0-d6/a3,(a0)
                movem.l d0-d6/a3,(a1)
                movem.l d0-d6/a3,(a2)
                dc.w    $D0FC,$0020             ; adda.w #BITMASK_BUFFER_STEP,a0
                dc.w    $D2FC,$0020             ; adda.w #BITMASK_BUFFER_STEP,a1
                dc.w    $D4FC,$0020             ; adda.w #BITMASK_BUFFER_STEP,a2
                dbra    d7,build_table_bitmask_buffers_clear_loop
                lea     BITMASK_STREAM_A.l,a0
                lea     BITMASK_BUFFER_A.l,a2
                move.w  #BITMASK_STREAM_ROWS,d2
build_table_bitmask_buffers_stream_a_row:
                move.w  (a0),d3
                ble.w   build_table_bitmask_buffers_error_a
                lea     BITMASK_STREAM_A.l,a1
                adda.w  d3,a1
                move.w  (a1)+,d1
                blt.b   build_table_bitmask_buffers_stream_a_next
                asr.w   #1,d1
                subq.w  #1,d1
build_table_bitmask_buffers_stream_a_bit:
                move.w  (a1)+,d3
                move.w  d3,d5
                asr.w   #5,d5
                add.w   d5,d5
                add.w   d5,d5
                andi.w  #BITMASK_INDEX_BITS,d3
                move.l  (a2,d5.w),d4
                bset    d3,d4
                move.l  d4,(a2,d5.w)
                dbra    d1,build_table_bitmask_buffers_stream_a_bit
build_table_bitmask_buffers_stream_a_next:
                addq.w  #BITMASK_STREAM_WORD_STEP,a0
                dc.w    $D4FC,$0010             ; adda.w #BITMASK_STREAM_BUFFER_STEP,a2
                dbra    d2,build_table_bitmask_buffers_stream_a_row
                lea     BITMASK_STREAM_B.l,a0
                lea     BITMASK_BUFFER_B.l,a2
                move.w  #BITMASK_STREAM_ROWS,d2
build_table_bitmask_buffers_stream_b_row:
                move.w  (a0),d3
                ble.w   build_table_bitmask_buffers_error_b
                lea     BITMASK_STREAM_B.l,a1
                adda.w  d3,a1
                move.w  (a1)+,d1
                blt.b   build_table_bitmask_buffers_stream_b_next
                asr.w   #1,d1
                subq.w  #1,d1
build_table_bitmask_buffers_stream_b_bit:
                move.w  (a1)+,d3
                move.w  d3,d5
                asr.w   #5,d5
                add.w   d5,d5
                add.w   d5,d5
                andi.w  #BITMASK_INDEX_BITS,d3
                move.l  (a2,d5.w),d4
                bset    d3,d4
                move.l  d4,(a2,d5.w)
                dbra    d1,build_table_bitmask_buffers_stream_b_bit
build_table_bitmask_buffers_stream_b_next:
                addq.w  #BITMASK_STREAM_WORD_STEP,a0
                dc.w    $D4FC,$0010             ; adda.w #BITMASK_STREAM_BUFFER_STEP,a2
                dbra    d2,build_table_bitmask_buffers_stream_b_row
                lea     BITMASK_STREAM_C.l,a0
                lea     BITMASK_BUFFER_C.l,a2
                move.w  #BITMASK_STREAM_ROWS,d2
build_table_bitmask_buffers_stream_c_row:
                move.w  (a0),d3
                ble.b   build_table_bitmask_buffers_error_c
                lea     BITMASK_STREAM_C.l,a1
                adda.w  d3,a1
                move.w  (a1)+,d1
                blt.b   build_table_bitmask_buffers_stream_c_next
                asr.w   #1,d1
                subq.w  #1,d1
build_table_bitmask_buffers_stream_c_bit:
                move.w  (a1)+,d3
                move.w  d3,d5
                asr.w   #5,d5
                add.w   d5,d5
                add.w   d5,d5
                andi.w  #BITMASK_INDEX_BITS,d3
                move.l  (a2,d5.w),d4
                bset    d3,d4
                move.l  d4,(a2,d5.w)
                dbra    d1,build_table_bitmask_buffers_stream_c_bit
build_table_bitmask_buffers_stream_c_next:
                addq.w  #BITMASK_STREAM_WORD_STEP,a0
                dc.w    $D4FC,$0010             ; adda.w #BITMASK_STREAM_BUFFER_STEP,a2
                dbra    d2,build_table_bitmask_buffers_stream_c_row
                rts
build_table_bitmask_buffers_error_a:
                move.w  #BITMASK_ERROR_A,d0
                bra.b   build_table_bitmask_buffers_report_error
build_table_bitmask_buffers_error_b:
                move.w  #BITMASK_ERROR_B,d0
                bra.b   build_table_bitmask_buffers_report_error
build_table_bitmask_buffers_error_c:
                move.w  #BITMASK_ERROR_C,d0
build_table_bitmask_buffers_report_error:
                move.w  d0,BITMASK_ERROR_WORD.l
                jsr     BITMASK_ERROR_HELPER.l
                rts
