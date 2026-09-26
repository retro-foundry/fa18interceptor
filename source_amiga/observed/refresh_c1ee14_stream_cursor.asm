; Byte-exact observed C1EE14 stream-cursor refresh gate $C1ED84-$C1EDAB.
; It compares the auxiliary descriptor pointer to the published stream cursor,
; publishes a change, then gates the indirect stage on two shared bytes.

                org     $C1ED84

STREAM_STAGE_POINTER             equ     $C45A36
STREAM_STAGE_AUX_POINTER         equ     $C45A3A
STREAM_REFRESH_LATCH             equ     $C45835
FLIGHT_UPDATE_MODE_BYTE          equ     $C4586B
INDIRECT_STREAM_STAGE            equ     $C1EE14

refresh_c1ee14_stream_cursor:
                move.l  STREAM_STAGE_AUX_POINTER.l,d0
                cmp.l   STREAM_STAGE_POINTER.l,d0
                beq.w   INDIRECT_STREAM_STAGE
                move.l  d0,STREAM_STAGE_POINTER.l
                tst.b   STREAM_REFRESH_LATCH.l
                bne.w   INDIRECT_STREAM_STAGE
                move.b  FLIGHT_UPDATE_MODE_BYTE.l,d1
                beq.b   $C1EDCC
