; Byte-exact bounded renderer entry $C2F66E-$C2F687.
                org $C2F66E
RENDER_LIMIT equ $C45984
RENDER_POINTERS equ $C456B6
enter_bounded_renderer_table_helper:
 cmp.w RENDER_LIMIT.l,d1
 bge.b $C2F60A
 movea.l RENDER_POINTERS.l,a1
 lea $C2F7C6.l,a3
 lea $C2F7E6.l,a4
