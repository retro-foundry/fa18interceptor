; Byte-exact observed callback-stage setup $C096D6-$C096F5.
; It allocates the stage frame, initializes the shared long to $000FFFFF,
; retains the shared word locally, then selects the direct or scaled path.
; The caller/scene role is not assigned.

                org     $C096D6

SCENE_STAGE_LONG_LIMIT          equ     $C456E6
SCENE_STAGE_SHARED_WORD         equ     $C45B40
SCENE_STAGE_SCALE_MODE          equ     $C457DD

initialize_c096d6_scene_stage:
                link.w  a6,#-$98
                move.l  #$000FFFFF,SCENE_STAGE_LONG_LIMIT.l
                move.w  SCENE_STAGE_SHARED_WORD.l,d2
                move.w  d2,-$96(a6)
                tst.b   SCENE_STAGE_SCALE_MODE.l
                bne.b   $C09708
