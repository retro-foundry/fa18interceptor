"""Keep validation RAM temporary; retain only an explicitly failed case."""
from pathlib import Path
import shutil
import tempfile


class CaptureWorkspace:
    def __init__(self, output, keep=False):
        self.output = Path(output)
        self.keep = keep
        self.temporary = None

    def __enter__(self):
        self.output.mkdir(parents=True, exist_ok=True)
        if self.keep:
            return self.output
        self.temporary = tempfile.TemporaryDirectory(prefix='ram-', dir=self.output)
        return Path(self.temporary.name)

    def __exit__(self, *error):
        if self.temporary:
            self.temporary.cleanup()


def retain_failure(prefix, output):
    """Copy one before/after/source case, rather than the entire run."""
    prefix, output = Path(prefix), Path(output) / 'failure'
    output.mkdir(parents=True, exist_ok=True)
    for path in prefix.parent.glob(prefix.name + '.*.dat'):
        destination = output / path.name
        if path.resolve() != destination.resolve():
            shutil.copy2(path, destination)
