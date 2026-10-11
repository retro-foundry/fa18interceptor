"""Build the project's native-port submodule through its standalone entry."""
import subprocess
import sys
from amiga_decomp.project import selected_project

STANDALONE = selected_project().path('native_checkout')

def main():
    script = STANDALONE / "scripts/build_native.py"
    if not script.is_file():
        raise SystemExit("Native submodule is missing; run git submodule update --init projects/fa18-interceptor/native from the framework root")
    subprocess.run([sys.executable, str(script)], cwd=STANDALONE, check=True)

if __name__ == "__main__":
    main()
