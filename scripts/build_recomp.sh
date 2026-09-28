#!/bin/sh
# Build build/recomp/fa18_recomp.exe (gcc). Musashi objects are cached.
set -e
cd "$(dirname "$0")/.."
M=tools/musashi
O=build/recomp/obj
mkdir -p $O
CFLAGS="-O2 -w -I$M -Iport/machine -Iport/recomp -Iport/recomp/generated"
for src in $M/m68kcpu.c $M/m68kops.c $M/m68kdasm.c $M/softfloat/softfloat.c port/recomp/generated/*.c; do
  obj=$O/$(basename "$src" .c).o
  if [ ! -f "$obj" ] || [ "$src" -nt "$obj" ]; then gcc $CFLAGS -c "$src" -o "$obj" & fi
done
wait
gcc $CFLAGS -o build/recomp/fa18_recomp.exe port/recomp/recomp_main.c port/recomp/recomp_runtime.c \
  port/machine/machine.c port/machine/blitter.c port/machine/display.c port/machine/input.c $O/*.o
