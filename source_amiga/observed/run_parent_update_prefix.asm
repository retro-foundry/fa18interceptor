; Byte-exact observed parent-update prefix $C0EFD4-$C0F01B.
; It joins the bounded input phase to the complete C1C63E update stage.

                org     $C0EFD4

FRAME_COUNTER_WORD             equ $C458DA
UPDATE_STAGE_MARKER             equ $C45AD4
UPDATE_STAGE_MARKER_PREPARE     equ 8
UPDATE_STAGE_MARKER_INDEXED     equ $10
INPUT_PHASE                     equ $C0F3C4
PRE_INPUT_UPDATE                equ $C0F5F8
PRE_UPDATE_STAGE                equ $C11B44
UPDATE_SKIP_FLAG                equ $C45795
UPDATE_SKIP_TARGET              equ $C0F370
PREPARE_STAGE                   equ $C12098
INDEXED_UPDATE_STAGE            equ $C25B1E
MAIN_UPDATE_STAGE               equ $C1C63E

run_parent_update_prefix:
                link.w  a6,#-2
                move.w  FRAME_COUNTER_WORD.l,-2(a6)
                bsr.w   INPUT_PHASE
                jsr     PRE_INPUT_UPDATE.l
                jsr     PRE_UPDATE_STAGE.l
                tst.b   UPDATE_SKIP_FLAG.l
                beq.w   UPDATE_SKIP_TARGET
                move.w  #UPDATE_STAGE_MARKER_PREPARE,UPDATE_STAGE_MARKER.l
                jsr     PREPARE_STAGE.l
                move.w  #UPDATE_STAGE_MARKER_INDEXED,UPDATE_STAGE_MARKER.l
                jsr     INDEXED_UPDATE_STAGE.l
                jsr     MAIN_UPDATE_STAGE.l
