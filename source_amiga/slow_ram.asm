; Byte authority only. Includes game and operating-system allocations.
SLOW_RAM_BASE equ $c00000
    org SLOW_RAM_BASE
    incbin "captures/uae/baseline_menu/slow.bin"
