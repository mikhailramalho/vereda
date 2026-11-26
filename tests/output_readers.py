#!/usr/bin/env python3
"""The timing reader accepts both sides of a performance comparison."""
import re
from pathlib import Path
import tempfile
import import_esbmc
import run

for flag in import_esbmc.IGNORED_WITH_VALUE:
    for argument in (flag + ' "solver -in"', flag + '="solver -in"'):
        assert import_esbmc.translate(argument + ' --z3', '.') == (
            ['--solver', 'z3'], [], [])
        lines = import_esbmc.header('suite/value', 'CORE',
                                    argument + ' --k-induction --32', [], '.', [])
        assert '// RUN: --k-induction -m32' in lines, lines
assert import_esbmc.translate('--32 --64 --data-races-check', '.') == (
    ['-m32', '-m64', '--data-race-check'], [], [])
assert import_esbmc.translate('--data-races-check=false', '.') == (
    ['--data-race-check=false'], [], [])
assert import_esbmc.translate('--func work --unlimited-k-step --no-unwinding-assertion '
                             '--no-standard-check', '.') == (
    ['--entry', 'work', '--unlimited-k-steps', '--no-unwinding-assertions',
     '--no-standard-checks'], [], [])
for flag in ('--cheri hybrid', '--goto2lean', '--ts-pdr', '--neurosym',
             '--bitwuzllob', '--clang-c-irep2-adjust-only'):
    with tempfile.TemporaryDirectory() as directory:
        src = Path(directory) / 'source'
        src.mkdir()
        (src / 'main.c').write_text('int main(void) { return 0; }\n')
        (src / 'test.desc').write_text('CORE\nmain.c\n' + flag + '\n^VERIFICATION SUCCESSFUL$\n')
        dest = Path(directory) / 'copy'
        assert not import_esbmc.import_test(str(src), str(dest), 'suite/internal', None).endswith('.c')
        assert not dest.with_suffix('.c').exists()

ignored = sorted(import_esbmc.IGNORED_FLAGS)
for flag in ignored:
    for token in (flag, flag + "=true", flag + "=false"):
        assert import_esbmc.translate(token + " --z3", ".") == (["--solver", "z3"], [], [])
        lines = import_esbmc.header("suite/internal_flag", "CORE", token, [], ".", [])
        assert not any(s.startswith(("// RUN:", "// Needs:", "// MODE:")) for s in lines), lines
        args = f"'{token}' --k-induction -DMSG='keep {flag} text'"
        lines = import_esbmc.header("suite/unsupported", "CORE", args, [], ".", [])
        assert "// Needs: --k-induction" in lines, lines
        assert f"// RUN: --k-induction -DMSG='keep {flag} text'" in lines, lines
lines = import_esbmc.header("suite/internal_flags", "CORE", " ".join(ignored) + " --z3",
                           [], ".", ["__ESBMC_forall"])
assert "// RUN: --solver z3" in lines, lines
assert "// Needs: __ESBMC_forall" in lines, lines
assert "// MODE: FUTURE" in lines, lines
lines = import_esbmc.header("suite/requirement", "CORE", " ".join(ignored),
                           ["REQUIRES uchar_h"], ".", [])
assert "// RUN:" in lines, lines
assert "// Needs: REQUIRES uchar_h" in lines, lines

with tempfile.TemporaryDirectory() as directory:
    src = Path(directory) / "source"
    src.mkdir()
    (src / "main.c").write_text("int main(void) { return 0; }\n")
    expected = [r"^VERIFICATION SUCCESSFUL$", r"^  division by zero$",
                r"^ +ASSIGN x=1;$", r"^\(check-sat\)$", "SMT formula written to out.smt2",
                r"return_value\$_more\$1\?1!", "CHECK_FILE out.smt2 contains bvneg"]
    for name in ("smt-formula", "goto-program", "goto-functions", "program"):
        for suffix in ("only", "too"):
            mode = "--" + name + "-" + suffix
            assert import_esbmc.expectations(expected, src, mode) == expected[:2], mode
    assert r"^ +ASSIGN x=1;$" in import_esbmc.expectations(expected, src)
    assert not import_esbmc.output_mode("-DMSG='--program-only'")
    dest = Path(directory) / "copied"
    old = dest.with_suffix(".c")
    old.write_text("stale fixture\n")
    (src / "test.desc").write_text("CORE\nmain.c\n--program-only\n^ +ASSIGN x=1;$\n")
    assert import_esbmc.import_test(str(src), str(dest), "suite/dump", None) == (
        "ESBMC-specific dump output only")
    assert not old.exists()
    (src / "test.desc").write_text("CORE\nmain.c\n--program-too\n"
                                   "^VERIFICATION SUCCESSFUL$\n^ +ASSIGN x=1;$\n")
    assert import_esbmc.import_test(str(src), str(dest), "suite/verdict", None) == str(old)
    copied = old.read_text()
    assert "// EXPECT: ^VERIFICATION SUCCESSFUL$" in copied, copied
    assert "ASSIGN" not in copied and "// MODE:" not in copied, copied
    (src / "main.c").write_text('int main(void) { __ESBMC_assert(0, "b"); }\n')
    row = r"^ +FAILED +\[main\.assertion\.2\] +line 10 +b$"
    filtered = import_esbmc.expectations([row, "^VERIFICATION FAILED$"], src, "--smt-formula-too")
    assert len(filtered) == 2, filtered
    assert re.search(filtered[0], "  FAILED  [main.assertion.2]  line 10  __VERIFIER_assert"), filtered
    assert not re.search(filtered[0], "  FAILED  [main.assertion.1]  line 10  __VERIFIER_assert"), filtered

for args in ("--std c11", "--std=c11"):
    assert import_esbmc.translate(args + " --z3", ".") == (
        ["--std", "c11", "--solver", "z3"], [], [])
lines = import_esbmc.header("suite/c_standard", "CORE", "--std=c23", [], ".", [])
assert "// RUN: --std c23" in lines, lines
assert not any(s.startswith(("// Needs:", "// MODE:")) for s in lines), lines

for walk_tag, solve_tag in (("", ""), ("", "[PROGRESS] "), ("[INFO] ", "[INFO] ")):
    output = (f"{walk_tag}Walk time: 0.125s (2 checks, 3 symbols, peak memory 40 MB)\n"
              f"{solve_tag}Solving time: 0.250s (peak memory 60 MB)\n")
    assert run.numbers(output) == (0.125, 0.25, 60)

cases = [
    (r"^  PASSED .*line   7  arithmetic overflow on add$",
     "  SUCCESS      [main.assertion.1]  line 7  arithmetic overflow on add"),
    (r"^\s*PASSED\s+\[main.assertion.1\]", "  SUCCESS      [main.assertion.1]"),
    (r'^  !overflow\("\+", a, b\)$', "  arithmetic overflow on add"),
    (r'^  !overflow\("-", x, y\)$', "  arithmetic overflow on sub"),
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

for args, requirement, resolved in (("--32", "arch32", True), ("--64", "arch64", True),
                                    ("--64", "arch32", False), ("", "arch32", False),
                                    ("--32", "uchar_h", False)):
    lines = import_esbmc.header("suite/target", "CORE", args, ["REQUIRES " + requirement], ".", [])
    assert ("// MODE: FUTURE" not in lines) == resolved, lines
    if resolved:
        assert "// RUN: -m" + requirement[-2:] in lines, lines
    else:
        assert "// Needs: REQUIRES " + requirement in lines, lines

assert import_esbmc.as_vereda("PASSED message") == "PASSED message"

assert import_esbmc.translate("--floatbv --fixedbv --z3 --32", ".") == (
    ["--solver", "z3", "-m32"], [], [])
lines = import_esbmc.header("suite/unsupported", "CORE", "--floatbv --fixedbv --k-induction",
                           [], ".", [])
assert "// RUN: --k-induction" in lines, lines
lines = import_esbmc.header("suite/macro", "CORE", "-DMSG='keep --floatbv text' --k-induction",
                           [], ".", [])
assert "// RUN: -DMSG='keep --floatbv text' --k-induction" in lines, lines
assert "// Needs: --k-induction" in lines, lines
lines = import_esbmc.header("suite/quoted", "CORE", "'--floatbv' \"--fixedbv\" --k-induction",
                           [], ".", [])
assert "// RUN: --k-induction" in lines, lines
lines = import_esbmc.header("floats/precision", "CORE", "--floatbv --z3 --32",
                           ["REQUIRES arch32"], ".", [])
assert "// RUN: --solver z3 -m32" in lines, lines
assert not any(line.startswith(("// Needs:", "// MODE:")) for line in lines), lines
lines = import_esbmc.header("z3/quantified", "CORE", "--floatbv --z3", [], ".",
                           ["__ESBMC_forall"])
assert "// Needs: __ESBMC_forall" in lines, lines
assert "// MODE: FUTURE" in lines, lines
assert "// RUN: --solver z3" in lines, lines

for name, args, flags in (("vla_above_ptrdiff_max", "", []),
                          ("vla_no_size_check_fail", "--32", ["// RUN: -m32"])):
    lines = import_esbmc.header("esbmc/" + name, "CORE", args,
                               ["object size exceeds PTRDIFF_MAX"], ".", [])
    assert "// EXPECT: alloca: size exceeds PTRDIFF_MAX" in lines, lines
    assert all(flag in lines for flag in flags), lines

for name in ("array_loop_summary_decrement", "array_loop_summary_decrement_fail"):
    for args, flags in (("", ""), ("--32", "-m32")):
        lines = import_esbmc.header("esbmc/" + name, "CORE", args, [], ".", [])
        runs = [line.partition(":")[2].strip() for line in lines if line.startswith("// RUN:")]
        assert runs == [flags, (flags + " --no-summarize-array-loops").strip()], runs
