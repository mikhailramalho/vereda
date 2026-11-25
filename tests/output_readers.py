#!/usr/bin/env python3
"""The timing reader accepts both sides of a performance comparison."""
import re
import import_esbmc
import run

for walk_tag, solve_tag in (("", ""), ("", "[PROGRESS] "), ("[INFO] ", "[INFO] ")):
    output = (f"{walk_tag}Walk time: 0.125s (2 checks, 3 symbols, peak memory 40 MB)\n"
              f"{solve_tag}Solving time: 0.250s (peak memory 60 MB)\n")
    assert run.numbers(output) == (0.125, 0.25, 60)

cases = [
    (r"^\[Counterexample\]$", "[COUNTEREXAMPLE]"),
    (r"^\*\* Results:$", "[RESULTS]"),
    (r"^\*\* 1 of 2 properties failed$",
     "Properties: 2 checked, 1 passed, 1 failed, 0 not checked"),
    (r"^Unwinding loop 0", "[PROGRESS] Unwinding loop 0 iteration 1"),
    (r"^  x = 7 \(00000111\)$", "  x = 7; // 00000111"),
    (r"\(10000000 00000000\)", "  // 10000000 00000000"),
    (r"^State 1 file a.c\n-+\n  x = 7 \(00000111\)$",
     "State 1 file a.c\n  x = 7; // 00000111"),
]
for original, output in cases:
    translated = import_esbmc.as_vereda(original)
    assert re.search(translated, output, re.MULTILINE), (original, translated, output)
