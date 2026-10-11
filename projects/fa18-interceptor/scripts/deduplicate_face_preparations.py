"""Select one observed face per renderer context and record address."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--context", action="append", required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    report = json.loads(args.input.read_text(encoding="utf-8"))
    selected = []
    seen = set()
    for submission in report["submissions"]:
        context = submission["context"]
        if context["a5"] not in args.context:
            continue
        key = (context["a5"], context["a2"])
        if key in seen:
            continue
        seen.add(key)
        selected.append(submission)
    if not selected:
        raise ValueError("no selected face records")
    output = {
        "scope": "one observed face per renderer context and record address",
        "input": str(args.input), "submission_count": len(selected), "submissions": selected,
        "qualification": "One first-observed transformed coordinate sample is retained per record. Coordinates are mutable renderer-workspace values; the input report's scope determines whether a face was captured before or after an orientation gate.",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(output, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"faces": len(selected), "output": str(args.output)}))


if __name__ == "__main__":
    main()
