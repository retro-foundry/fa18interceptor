"""Expose the original Custom-write definition to the test-only observer."""
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
def prepare():
    original=(ROOT/'port/machine/machine.c').read_text()
    declaration='void fa18_custom_write(FA18Machine *m, uint32_t reg, uint16_t value) {'
    assert original.count(declaration)==1
    # Rename only the definition: internal original bus calls still go through
    # the observer. Macro-renaming also renamed those calls and missed them.
    variant=original.replace(declaration,'void fa18_original_custom_write(FA18Machine *m, uint32_t reg, uint16_t value) {',1)
    path=ROOT/'build/recomp/hud_stream_original_machine.c'
    if not path.exists() or path.read_text()!=variant: path.write_text(variant)
    return path
if __name__=='__main__': print(prepare().relative_to(ROOT))
