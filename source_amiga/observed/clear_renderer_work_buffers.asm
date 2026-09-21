; Byte-exact static-only renderer work-buffer clear $C2FD22-$C2FD8B.

                org     $C2FD22

RENDERER_WORK_0                 equ $C456BE
RENDERER_WORK_1                 equ $C456C2
RENDERER_WORK_2                 equ $C456C6
RENDERER_WORK_3                 equ $C456CA
RENDERER_WORK_4                 equ $C456CE
RENDERER_WORK_5                 equ $C456D2
RENDERER_WORK_6                 equ $C456D6
RENDERER_WORK_7                 equ $C456DA
RENDERER_WORK_8                 equ $C456DE
RENDERER_WORK_9                 equ $C456E2
RENDERER_FIFTH_CLEAR_ENABLE     equ $C457D6
RENDERER_CLEAR_ITERATIONS       equ $07CF

clear_renderer_work_buffers:
                movea.l RENDERER_WORK_0.l,a0
                movea.l RENDERER_WORK_1.l,a1
                movea.l RENDERER_WORK_2.l,a2
                movea.l RENDERER_WORK_3.l,a3
                movea.l RENDERER_WORK_4.l,a4
                move.w  #RENDERER_CLEAR_ITERATIONS,d0
.first_loop:
                clr.l   (a0)+
                clr.l   (a1)+
                clr.l   (a2)+
                clr.l   (a3)+
                tst.b   RENDERER_FIFTH_CLEAR_ENABLE.l
                beq.s   .first_loop_next
                clr.l   (a4)+
.first_loop_next:
                dbra    d0,.first_loop
                movea.l RENDERER_WORK_5.l,a0
                movea.l RENDERER_WORK_6.l,a1
                movea.l RENDERER_WORK_7.l,a2
                movea.l RENDERER_WORK_8.l,a3
                movea.l RENDERER_WORK_9.l,a5
                move.w  #RENDERER_CLEAR_ITERATIONS,d0
.second_loop:
                clr.l   (a0)+
                clr.l   (a1)+
                clr.l   (a2)+
                clr.l   (a3)+
                clr.l   (a5)+
                dbra    d0,.second_loop
                rts
