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

for name in ("ptr_rel_huge_object_alloca", "ptr_rel_huge_object_vla"):
    lines = import_esbmc.header("esbmc/" + name, "CORE", "", [], ".", [])
    assert "// RUN: --no-vla-size-check" in lines, lines

for name, args, flags in (("vla_above_ptrdiff_max", "", []),
                          ("vla_no_size_check_fail", "--32", ["// RUN: -m32"])):
    lines = import_esbmc.header("esbmc/" + name, "CORE", args,
                               ["object size exceeds PTRDIFF_MAX"], ".", [])
    assert "// EXPECT: alloca: size exceeds PTRDIFF_MAX" in lines, lines
    assert all(flag in lines for flag in flags), lines
