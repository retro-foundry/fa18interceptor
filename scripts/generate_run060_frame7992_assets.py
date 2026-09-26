"""Generate semantic source byte streams for the captured frame 7992 blits."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CHIP = ROOT / 'build/run060_chip_frames/frame7992.chip'
OUT = ROOT / 'port/run060_frame7992_area_assets.h'
POINTERS = [0xB038, 0xAE88, 0xACD8, 0xAB28]
ROWS, ROW_BYTES = 12, 37

def read_asset(chip: bytes, start: int) -> list[list[int]]:
    return [list(chip[start + row * ROW_BYTES:start + (row + 1) * ROW_BYTES])
            for row in range(ROWS)]

def emit(name: str, rows: list[list[int]]) -> str:
    return 'static const uint8_t %s[%d][%d] = {\n%s\n};\n' % (
        name, ROWS, ROW_BYTES,
        ',\n'.join('    {' + ', '.join(f'0x{x:02x}' for x in row) + '}' for row in rows))

def main() -> None:
    chip = CHIP.read_bytes()
    assets = [read_asset(chip, 0xA230)] + [read_asset(chip, p) for p in POINTERS]
    text = ('#ifndef FA18_RUN060_FRAME7992_AREA_ASSETS_H\n'
            '#define FA18_RUN060_FRAME7992_AREA_ASSETS_H\n\n'
            '#include <stdint.h>\n\n'
            'enum { FA18_RUN060_AREA_ROWS = 12, FA18_RUN060_AREA_ROW_BYTES = 37 };\n\n'
            '/* Extracted source rows; the odd row stride is the captured BLTAMOD/BLTBMOD. */\n')
    text += emit('fa18_run060_frame7992_a_source', assets[0])
    text += 'static const uint8_t fa18_run060_frame7992_b_source[4][12][37] = {\n'
    text += ',\n'.join('    {\n' + ',\n'.join('        {' + ', '.join(f'0x{x:02x}' for x in row) + '}' for row in asset) + '\n    }' for asset in assets[1:])
    text += '\n};\n\n#endif\n'
    OUT.write_text(text, encoding='ascii')

if __name__ == '__main__': main()
