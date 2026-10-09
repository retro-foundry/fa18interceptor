"""Extract unchanged original UAE averaging functions for a test-only oracle."""
import argparse
import hashlib
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    data = args.source.read_bytes()
    source = data.decode()
    functions = []
    for name in ('anti_prehandler', 'samplexx_anti_handler'):
        start = source.index('static void ' + name + ' (')
        body = source.index('{', start)
        depth = 1
        end = body + 1
        while depth:
            if source[end] == '{': depth += 1
            elif source[end] == '}': depth -= 1
            end += 1
        functions.append(source[start:end])
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text('#define ORIGINAL_ANTI_SOURCE_SHA256 "'+hashlib.sha256(data).hexdigest()+'"\n'+
                        '\n\n'.join(functions)+'\n')


if __name__ == '__main__':
    main()
