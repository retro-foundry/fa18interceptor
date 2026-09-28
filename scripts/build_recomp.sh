#!/bin/sh
# Build build/recomp/fa18_recomp.exe (gcc, headless). Musashi and generated
# objects are cached; the CMake build in port/recomp adds the SDL window.
set -e
cd "$(dirname "$0")/.."
# The build runs with -w; catch a global defined twice (a silent redefinition).
dups=$(sed -n 's/^#define \([A-Z0-9_]*\) .*/\1/p' port/game/globals.h | sort | uniq -d)
if [ -n "$dups" ]; then echo "globals.h defines twice: $dups" >&2; exit 1; fi
M=tools/musashi
O=build/recomp/obj
mkdir -p $O
CFLAGS="-O2 -w -I$M -Iport/machine -Iport/recomp -Iport/recomp/generated -Iport/game -Iport/game/glue"
for src in $M/m68kcpu.c $M/m68kops.c $M/m68kdasm.c $M/softfloat/softfloat.c port/recomp/generated/*.c; do
  obj=$O/$(basename "$src" .c).o
  if [ ! -f "$obj" ] || [ "$src" -nt "$obj" ]; then gcc $CFLAGS -c "$src" -o "$obj" & fi
done
wait
gcc $CFLAGS -o build/recomp/fa18_recomp.exe port/recomp/recomp_main.c port/recomp/recomp_runtime.c \
  port/recomp/recomp_ports.c port/machine/machine.c port/machine/bus.c port/machine/blitter.c port/machine/display.c \
  port/machine/input.c $(ls port/game/*.c 2>/dev/null) port/game/glue/*.c $O/*.o
