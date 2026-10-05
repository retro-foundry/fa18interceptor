"""Validate native palette imports against the original ADF and sealed game data."""
import hashlib
import json
import subprocess
from audit_command_dispatch import source_decoder
from check_record_region_probe import ROOT


def main():
    state, decoder = source_decoder()
    seal = json.loads((ROOT / "analysis/data/command_dispatch_source_scope.json").read_text())["state_sha256"]
    if hashlib.sha256(state).hexdigest() != seal:
        raise RuntimeError("original state seal differs")
    disk = ROOT / "local/media/fa18.adf"
    disk_hash = hashlib.sha256(disk.read_bytes()).hexdigest()
    authority = json.loads((ROOT / "analysis/disk_graphics_assets.json").read_text())
    if disk_hash != authority["authority_disk_sha256"]:
        raise RuntimeError("original ADF differs from palette authority")
    expected = bytearray()
    for base,count in [(0xc1aa9c,32),(0xc084d0,32)]+[(0xc08510+(15-mode)*32,16) for mode in range(16)]:
        for i in range(count):
            expected.extend(decoder.word(base+2*i).to_bytes(2,"big"))
    for i in range(16):
        expected.extend(decoder.word(0xc3f040+2*i).to_bytes(2,"big"))
    directory = ROOT / "build/recomp"
    directory.mkdir(parents=True,exist_ok=True)
    fixture = directory / "display_palette_expected.bin"
    fixture.write_bytes(expected)  # Reference-only decoded words, never native data.
    paths = ["tools/recomp/display_palette_assets_oracle.c", "port/display_palette_assets.c",
             "port/disk.c", "port/hunk.c", "port/menu_record.c", "port/postflight_text.c",
             "port/hex_field.c", "port/audio_selection.c", "port/voice_selection.c"]
    exe = directory / "display_palette_assets_oracle.exe"
    subprocess.run(["gcc","-std=c11","-O2","-UNDEBUG","-Wall","-Wextra","-Werror",*paths,"-o",str(exe)],cwd=ROOT,check=True)
    result = subprocess.run([str(exe),str(disk),str(fixture)],cwd=ROOT,capture_output=True,text=True,timeout=60)
    (directory / "display_palette_assets.log").write_text(result.stdout+result.stderr)
    if result.returncode:
        raise RuntimeError(result.stderr or result.stdout or "native palette import differs")
    print(result.stdout.strip())
    paths += ["port/display_palette_assets.h", "port/postflight_text.h", "port/stage_callback.h",
              "port/hex_field.h", "port/audio_selection.h", "port/voice_selection.h",
              "tools/recomp/check_display_palette_assets.py"]
    checkpoint = {
        "status":"validated_original_disk_palette_imports_startup_loading_order_pending",
        "resources":["pix/inst5","pix/frnt5"],"words_per_import":320,
        "original_state_sha256":seal,"original_disk_sha256":disk_hash,
        "comparison":"all 32 decoded initial palette words, 32 static words and 256 raw mode words, including the contiguous first 32-word seed; 32 bytes of the real mutable selector-97 text against sealed original state; Hunk 21, Hunk 64 and actual ILBM CMAP imports only",
        "text_descriptor_bytes":32,
        "native_cpu_dependency":False,"report":result.stdout.strip(),
        "source_sha256":{p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},
    }
    (ROOT / "analysis/figures/native_display_palette_assets_checkpoint.json").write_text(json.dumps(checkpoint,indent=2)+"\n")


if __name__ == "__main__":
    main()
