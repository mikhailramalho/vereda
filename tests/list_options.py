#!/usr/bin/env python3
"""Count option occurrences in regression RUN headers, including skipped tests.

Each RUN line counts independently. Values attached with '=' are grouped under
the option name; separate values and positional inputs are not counted.
"""
import argparse
from collections import Counter
from pathlib import Path
import shlex

from run import HEADER


def count_options(paths):
    counts = Counter()
    for path in paths:
        for number, line in enumerate(path.read_text(errors="replace").splitlines(), 1):
            match = HEADER.match(line)
            if not match or match[1] != "RUN":
                continue
            try:
                tokens = shlex.split(match[2])
            except ValueError as error:
                raise ValueError(f"{path}:{number}: {error}") from error
            for token in tokens:
                if token == "--":
                    break
                # Exclude negative numeric arguments, e.g. --stack-limit -1.
                if token.startswith("-") and len(token) > 1 and not token[1].isdigit():
                    counts[token.split("=", 1)[0]] += 1
    return counts


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--tests-dir", type=Path, default=Path(__file__).resolve().parent,
                        help="regression root (default: this script's directory)")
    args = parser.parse_args()
    if not args.tests_dir.is_dir():
        parser.error(f"not a directory: {args.tests_dir}")
    paths = set()
    for pattern in ("*/*.c", "*/*.i", "*/*.rs", "*/*.ll", "*/*/main.c"):
        paths.update(args.tests_dir.glob(pattern))
    try:
        counts = count_options(sorted(paths))
    except ValueError as error:
        parser.exit(1, f"{error}\n")
    print("COUNT  OPTION")
    for option, count in sorted(counts.items(), key=lambda item: (-item[1], item[0])):
        print(f"{count:5}  {option}")


if __name__ == "__main__":
    main()
