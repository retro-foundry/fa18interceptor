"""Read and validate ownership of C entries whose CPU adapter was removed."""
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[2]


def function_body(source: str, name: str) -> str:
    source = re.sub(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"', '', source, flags=re.S)
    match = re.search(r'\b' + re.escape(name) + r'\s*\([^;{}]*\)\s*\{', source)
    if not match:
        raise ValueError(f"native function is missing: {name}")
    depth, position = 1, match.end()
    while position < len(source) and depth:
        depth += (source[position] == '{') - (source[position] == '}')
        position += 1
    if depth:
        raise ValueError(f"unterminated native function: {name}")
    return source[match.end():position - 1]


def native_entries(graph: list[dict], registered: set[str]) -> dict[str, dict]:
    data = json.loads((ROOT / 'port/game/native_call_graph.json').read_text())
    if data['schema'] != 1:
        raise ValueError('unsupported native call graph schema')
    entries = {}
    for row in data['entries']:
        entry = row['entry']
        if not re.fullmatch(r'[0-9A-F]{6}', entry) or entry in entries or entry in registered:
            raise ValueError(f'invalid/duplicate native entry: {entry}')
        entries[entry] = row
    owners = registered | entries.keys()
    for entry, row in entries.items():
        if not any(function['entry'] == entry for function in graph):
            raise ValueError(f'native entry is absent from source graph: {entry}')
        callers = {function['entry'] for function in graph if entry in function['calls']}
        declared = {caller['entry'] for caller in row['callers']}
        if not declared or callers != declared or not callers <= owners:
            raise ValueError(f'native caller ownership is incomplete: {entry}: {callers} != {declared}')
        for owner in [row, *row['callers']]:
            source = (ROOT / owner['source']).resolve()
            if not source.is_relative_to((ROOT / 'port/game').resolve()) or 'glue' in source.parts:
                raise ValueError(f'native behavior belongs in game/: {source}')
            body = function_body(source.read_text(), owner['function'])
            if owner is not row and 'call_site' in owner:
                if not re.search(r'\b' + re.escape(owner['call_site']) + r'\s*\(', body):
                    raise ValueError(f'native owner does not reach its C call site: {owner["function"]}')
                body = function_body(source.read_text(), owner['call_site'])
            if owner is not row and not re.search(r'\b' + re.escape(row['function']) + r'\s*\(', body):
                raise ValueError(f'native caller does not call its C child: {owner["function"]}')
            if re.search(r'\b(?:m68k\w*|REG_\w+|fa18_recomp_\w+)\b', body):
                raise ValueError(f'native body still uses CPU/dispatcher state: {owner["function"]}')
        if not (ROOT / row['source_call']).is_file():
            raise ValueError(f'original call evidence is missing: {entry}')
    return entries
