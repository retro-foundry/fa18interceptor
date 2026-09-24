; Byte-exact C2F4DE-C2F556 copies two renderer pointer-table layouts.
                org     $C2F4DE
RENDERER_TABLE_A                equ     $C4566E
RENDERER_TABLE_B                equ     $C4568E
RENDERER_SOURCE_0               equ     $C456BE
RENDERER_SOURCE_1               equ     $C456C2
RENDERER_SOURCE_2               equ     $C456C6
RENDERER_SOURCE_3               equ     $C456CA
RENDERER_SOURCE_4               equ     $C456CE
RENDERER_SOURCE_5               equ     $C456D2
RENDERER_SOURCE_6               equ     $C456D6
RENDERER_SOURCE_7               equ     $C456DA
RENDERER_SOURCE_8               equ     $C456DE

copy_projected_renderer_tables:
                lea     RENDERER_TABLE_A.l,a0
                move.l  RENDERER_SOURCE_3.l,(a0)+
                move.l  RENDERER_SOURCE_2.l,(a0)+
                move.l  RENDERER_SOURCE_1.l,(a0)+
                move.l  RENDERER_SOURCE_0.l,(a0)+
                move.l  RENDERER_SOURCE_8.l,(a0)+
                move.l  RENDERER_SOURCE_7.l,(a0)+
                move.l  RENDERER_SOURCE_6.l,(a0)+
                move.l  RENDERER_SOURCE_5.l,(a0)
                lea     RENDERER_TABLE_B.l,a0
                move.l  RENDERER_SOURCE_4.l,(a0)+
                move.l  RENDERER_SOURCE_3.l,(a0)+
                move.l  RENDERER_SOURCE_2.l,(a0)+
                move.l  RENDERER_SOURCE_1.l,(a0)+
                move.l  RENDERER_SOURCE_0.l,(a0)+
                move.l  RENDERER_SOURCE_8.l,(a0)+
                move.l  RENDERER_SOURCE_8.l,(a0)+
                move.l  RENDERER_SOURCE_7.l,(a0)+
                move.l  RENDERER_SOURCE_6.l,(a0)+
                move.l  RENDERER_SOURCE_5.l,(a0)
                rts
