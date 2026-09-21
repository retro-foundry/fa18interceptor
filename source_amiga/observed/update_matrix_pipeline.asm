; Byte-exact reconstruction of $C2D9BA-$C2DADF (Hunk 32 +$5B2).
; Observed caller: $C2D9A4 -> $C2D9BA -> $C2D9A8.
; Builds and scales two matrix caches, then builds a one-angle matrix cache.

                org     $C2D9BA

ACTIVE_RECORD_BASE              equ $C46184
ACTIVE_RECORD_OFFSET             equ $C458DE
FALLBACK_RECORD_OFFSET           equ $C459C2
PIPELINE_ENABLE_STATE             equ $C457B4
PIPELINE_MODE                    equ $C458AE
PIPELINE_FLAG_BYTE                equ $C458B2
PIPELINE_SKIP_UPDATE              equ $C457AE
PIPELINE_SIGN_STATE               equ $C457B5
PIPELINE_SELECTION_CACHE          equ $C45A60
PIPELINE_INPUT_X                  equ $C45A94
PIPELINE_INPUT_Z                  equ $C45A96
PIPELINE_OUTPUT_X                 equ $C45AC0
PIPELINE_OUTPUT_Z                 equ $C45AC2
PIPELINE_ORIGIN_X                 equ $C45C3E
PIPELINE_ORIGIN_Y                 equ $C45C42
PIPELINE_ORIGIN_Z                 equ $C45C46
RECORD_FLAG_OFFSET                equ 1
RECORD_TYPE_OFFSET                equ $62
RECORD_TYPE_MASK                  equ $F0
SPECIAL_RECORD_TYPE               equ $30
RECORD_SELECT_BIT                 equ 6
PIPELINE_MODE_SPECIAL             equ 6
PIPELINE_DEFAULT_SELECTOR         equ 1
PIPELINE_SPECIAL_SELECTOR         equ 5
PIPELINE_SPECIAL_ARGUMENT         equ $FC
PIPELINE_ACTIVE_ARGUMENT          equ $0230
PIPELINE_STABLE_ARGUMENT          equ $07D0
PIPELINE_NEGATIVE_ARGUMENT        equ $FFFFFFFF

matrix_pipeline_fallback          equ $C2D9B0
transform_record_position         equ $C091E0
update_pipeline_coordinates       equ $C123FA
build_two_angle_matrix            equ $C2E38E
scale_matrix_rows                 equ $C2E5AC
build_single_angle_matrix         equ $C2E346

update_matrix_pipeline:
                tst.b   PIPELINE_ENABLE_STATE.l
                beq.b   matrix_pipeline_fallback
                lea.l   ACTIVE_RECORD_BASE.l,a1
                move.w  ACTIVE_RECORD_OFFSET.l,d2
                bne.b   .select_record
                move.w  FALLBACK_RECORD_OFFSET.l,d2
                btst.b  #RECORD_SELECT_BIT,RECORD_FLAG_OFFSET(a1,d2.w)
                beq.b   .read_record_type
                cmpi.b  #SPECIAL_RECORD_TYPE,RECORD_TYPE_OFFSET(a1,d2.w)
                bne.b   .read_record_type
.select_record:
                adda.w  d2,a1
.read_record_type:
                move.b  RECORD_TYPE_OFFSET(a1),d0
                andi.b  #RECORD_TYPE_MASK,d0
                cmpi.b  #SPECIAL_RECORD_TYPE,d0
                beq.b   .special_record
                moveq   #0,d3
                moveq   #0,d4
                moveq   #PIPELINE_DEFAULT_SELECTOR,d5
                cmpi.b  #PIPELINE_MODE_SPECIAL,PIPELINE_MODE.l
                bne.b   .transform
                moveq   #PIPELINE_SPECIAL_SELECTOR,d5
                bra.b   .transform
.special_record:
                moveq   #0,d3
                moveq   #0,d4
                moveq   #PIPELINE_SPECIAL_ARGUMENT,d5
.transform:
                jsr     transform_record_position.l
                move.l  d2,d4
                move.l  d1,d3
                move.l  d0,d2
                sub.l   PIPELINE_ORIGIN_X.l,d2
                sub.l   PIPELINE_ORIGIN_Y.l,d3
                sub.l   PIPELINE_ORIGIN_Z.l,d4
                tst.b   PIPELINE_ENABLE_STATE.l
                blt.b   .negative_argument
                tst.b   PIPELINE_MODE.l
                beq.b   .build_selection_key
                move.l  #PIPELINE_ACTIVE_ARGUMENT,d5
                bra.b   .update_coordinates
.build_selection_key:
                moveq   #0,d5
                move.w  ACTIVE_RECORD_OFFSET.l,d5
                or.b    PIPELINE_FLAG_BYTE.l,d5
                cmp.w   PIPELINE_SELECTION_CACHE.l,d5
                beq.b   .stable_argument
                move.w  d5,PIPELINE_SELECTION_CACHE.l
.negative_argument:
                moveq   #-1,d5
                bra.b   .update_coordinates
.stable_argument:
                move.l  #PIPELINE_STABLE_ARGUMENT,d5
.update_coordinates:
                tst.b   PIPELINE_SKIP_UPDATE.l
                bne.b   .build_matrices
                move.w  PIPELINE_INPUT_X.l,d0
                move.w  PIPELINE_INPUT_Z.l,d1
                ext.l   d0
                ext.l   d1
                tst.b   PIPELINE_SIGN_STATE.l
                bne.b   .push_coordinate_call
                moveq   #-1,d5
.push_coordinate_call:
                movem.l d0-d5,-(a7)
                jsr     update_pipeline_coordinates.l
                ; ADDA.W #$18,A7; retain original immediate encoding.
                dc.w    $DEFC,$0018
                move.w  PIPELINE_OUTPUT_X.l,PIPELINE_INPUT_X.l
                move.w  PIPELINE_OUTPUT_Z.l,PIPELINE_INPUT_Z.l
.build_matrices:
                move.w  PIPELINE_INPUT_X.l,d0
                move.w  PIPELINE_INPUT_Z.l,d2
                lea.l   $C45BD8,a1
                bsr.w   build_two_angle_matrix
                lea.l   $C45BD8,a1
                bsr.w   scale_matrix_rows
                move.w  PIPELINE_INPUT_Z.l,d4
                lea.l   $C45BFC,a1
                bsr.w   build_single_angle_matrix
