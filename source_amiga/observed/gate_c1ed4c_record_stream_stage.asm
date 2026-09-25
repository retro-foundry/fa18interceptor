; Byte-exact record-stream stage gate $C1ED4C-$C1ED6F.
; This is a short entry wrapper, not the 1,484-byte Ghidra function body that
; follows its branches into the already reconstructed $C1EE14 stage.

                org     $C1ED4C

POST_STREAM_FLAGS               equ     $C4585B
CONTEXT_SELECTION               equ     $C45785
CONTROL_ACTIVITY_WORD           equ     $C459B6
CURRENT_RECORD_OFFSET            equ     $C458DE
RECORD_STREAM_PREDECESSOR        equ     $C1ED38
INDIRECT_STAGE                   equ     $C1EE14

gate_c1ed4c_record_stream_stage:
                btst    #6,POST_STREAM_FLAGS.l
                bne.b   RECORD_STREAM_PREDECESSOR
                tst.b   CONTEXT_SELECTION.l
                bne.w   INDIRECT_STAGE
                move.w  CONTROL_ACTIVITY_WORD.l,d0
                cmp.w   CURRENT_RECORD_OFFSET.l,d0
                bne.w   INDIRECT_STAGE
