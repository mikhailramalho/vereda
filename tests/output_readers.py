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
    ['-m32', '-m64'], [], ['--ub-data-race-check'])
assert import_esbmc.translate('--data-races-check=false', '.') == (
    [], [], ['--ub-data-race-check=false'])
assert import_esbmc.translate('--func work --unlimited-k-step --no-unwinding-assertion '
                             '--no-standard-check', '.') == (
    ['--entry', 'work', '--unlimited-k-steps', '--no-unwinding-assertions'],
    [], ['--no-standard-checks'])
for flag in ('--cheri hybrid', '--goto2lean', '--ts-pdr', '--neurosym',
             '--bitwuzllob',
             '--smtlib --smtlib-oneshot-prog fake-solver.sh',
             '--smtlib-oneshot-model-prog="z3 -in"'):
    with tempfile.TemporaryDirectory() as directory:
        src = Path(directory) / 'source'
        src.mkdir()
        (src / 'main.c').write_text('int main(void) { return 0; }\n')
        (src / 'test.desc').write_text('CORE\nmain.c\n' + flag + '\n^VERIFICATION SUCCESSFUL$\n')
        dest = Path(directory) / 'copy'
        assert not import_esbmc.import_test(str(src), str(dest), 'suite/internal', None).endswith('.c')
        assert not dest.with_suffix('.c').exists()
# ESBMC's frontend switches do not change what the C program means.
with tempfile.TemporaryDirectory() as directory:
    src = Path(directory) / 'source'
    src.mkdir()
    (src / 'main.c').write_text('int main(void) { return 0; }\n')
    (src / 'test.desc').write_text('CORE\nmain.c\n--old-frontend --irep2-bodies '
                                   '--clang-c-irep2-adjust-only --no-irep2-native-body\n'
                                   '^VERIFICATION SUCCESSFUL$\n')
    copy = import_esbmc.import_test(str(src), str(Path(directory) / 'copy'), 'suite/fe', None)
    assert '// RUN: --no-incremental-bmc\n' in Path(copy).read_text(encoding='utf-8')

assert import_esbmc.translate("--no-assertions --no-div-by-zero-check", ".") == (
    ["--no-assertions", "--ub-div-by-zero-check=false"], [], [])
for esbmc, vereda in (
        ("--clz-zero-check", ["--ub-count-zero-check"]),
        ("--uninitialised-vars-check", ["--ub-uninit-check"]),
        ("--no-pointer-check --no-vla-size-check",
         ["--ub-pointer-check=false", "--vla-size-check=false"]),
        ("--overflow-check", ["--ub-signed-overflow-check", "--ub-shift-check",
                              "--ub-float-cast-overflow-check"]),
        ("--no-fp-conversion-check --overflow-check --no-fp-overflow-check",
         ["--ub-signed-overflow-check", "--ub-shift-check"]),
        ("--unsigned-overflow-check", ["--ub-signed-overflow-check", "--unsigned-overflow-check",
                                       "--ub-shift-check"]),
        ("--ub-shift-check", ["--ub-signed-overflow-check", "--ub-shift-check"]),
        ("--overflow-check --unsigned-overflow-check",
         ["--ub-signed-overflow-check", "--ub-shift-check", "--ub-float-cast-overflow-check",
          "--unsigned-overflow-check"])):
    assert import_esbmc.translate(esbmc, ".") == (vereda, [], []), esbmc
assert import_esbmc.translate("--error-label ERROR --unwind 1", ".") == (
    ["--error-label", "ERROR", "--unwind", "1"], [], [])
for flag in ("--memory-leak-check", "--deadlock-check"):
    assert import_esbmc.translate(flag, ".") == ([], [], [flag])

ignored = sorted(import_esbmc.IGNORED_FLAGS)
for flag in ignored:
    for token in (flag, flag + "=true", flag + "=false"):
        assert import_esbmc.translate(token + " --z3", ".") == (["--solver", "z3"], [], [])
        lines = import_esbmc.header("suite/internal_flag", "CORE", token, [], ".", [])
        assert "// RUN: --no-incremental-bmc" in lines, lines
        assert not any(s.startswith(("// Needs:", "// MODE:")) for s in lines), lines
        args = f"'{token}' --k-induction -DMSG='keep {flag} text'"
        lines = import_esbmc.header("suite/unsupported", "CORE", args, [], ".", [])
        assert "// Needs: --k-induction" in lines, lines
        assert f"// RUN: --k-induction -DMSG='keep {flag} text'" in lines, lines
lines = import_esbmc.header("suite/internal_flags", "CORE", " ".join(ignored) + " --z3",
                           [], ".", ["__ESBMC_forall"])
assert "// RUN: --solver z3 --no-incremental-bmc" in lines, lines
assert "// Needs: __ESBMC_forall" in lines, lines
assert "// MODE: FUTURE" in lines, lines
lines = import_esbmc.header("suite/requirement", "CORE", " ".join(ignored),
                           ["REQUIRES uchar_h"], ".", [])
assert "// RUN: --no-incremental-bmc" in lines, lines
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
assert "// RUN: --std c23 --no-incremental-bmc" in lines, lines
assert not any(s.startswith(("// Needs:", "// MODE:")) for s in lines), lines

for walk_tag, solve_tag in (("", ""), ("", "[PROGRESS] "), ("[INFO] ", "[INFO] ")):
    output = (f"{walk_tag}Walk time: 0.125s (2 checks, 3 symbols, peak memory 40 MB)\n"
              f"{solve_tag}Solving time: 0.250s (peak memory 60 MB)\n")
    assert run.numbers(output) == (0.125, 0.25, 60)

# Literal prefiltering must retain the old anchors and Unicode word boundary.
for prefix in ("", "\n", "prefix ", "prefix\n", "\r"):
    for status in ("SUCCESSFUL", "FAILED", "UNKNOWN", "INVALID"):
        for suffix in ("", "\n", "\r\n", "suffix", "_", "é", ": detail"):
            output = prefix + "VERIFICATION " + status + suffix
            expected = run.VERDICT.findall(output)
            assert run.verdicts(output) == expected, repr(output)
            assert run.verdict_of(output) == (expected[-1] if expected else "none")
output = ("[PROGRESS] Unwinding loop 0 iteration 1\n" * 20000 +
          "VERIFICATION SUCCESSFUL\nVERIFICATION FAILED\nVERIFICATION UNKNOWN: bound\n")
assert run.verdicts(output) == ["SUCCESSFUL", "FAILED", "UNKNOWN"]
assert run.verdict_of(output) == "UNKNOWN"
legacy_oom = re.compile(r"std::bad_alloc|[Oo]ut of memory|Cannot allocate memory|MemoryError")
for message in ("std::bad_alloc", "Out of memory", "out of memory", "Cannot allocate memory",
                "MemoryError", "OUT OF MEMORY", "ordinary memory", "out of\nmemory", ""):
    assert run.out_of_memory("prefix " + message + " suffix") == bool(legacy_oom.search(message))
assert not run.out_of_memory(output)
assert run.out_of_memory(output + "MemoryError")
assert run.numbers("[INFO] Walk time: 0.125s (1 check, 0 symbols, peak memory 20 MB)\n"
                   "[PROGRESS] Solving time: 0.25s (peak memory 30 MB)\n"
                   "Walk time: 0.375s (1 check, 0 symbols, peak memory 25 MB)\n"
                   "Solving time: 0.5s (peak memory 28 MB)\n") == (0.5, 0.75, 30)

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
    (r"^\*\* 1 of 2 properties failed, 1 not checked$",
     "Properties: 1 checked, 0 passed, 1 failed, 1 not checked"),
    (r"^Unwinding loop 0", "[PROGRESS] Unwinding loop 0 iteration 1"),
    (r"^  x = 7 \(00000111\)$", "  x = 7; // 00000111"),
    (r"\(10000000 00000000\)", "  // 10000000 00000000"),
    (r"^State 1 file a.c\n-+\n  x = 7 \(00000111\)$",
     "State 1 file a.c\n  x = 7; // 00000111"),
]
for original, output in cases:
    translated = import_esbmc.as_vereda(original)
    assert re.search(translated, output, re.MULTILINE), (original, translated, output)
for original in (r"^\*\* 0 of 1 properties failed, 1 unknown$",
                 r"^\*\* \x1b\[31m1 of 3 properties failed\x1b\[0m, \x1b\[32m1 passed\x1b\[0m$"):
    assert import_esbmc.esbmc_output(original, set()), original

for name in ("ptr_rel_huge_object_alloca", "ptr_rel_huge_object_vla"):
    lines = import_esbmc.header("esbmc/" + name, "CORE", "", [], ".", [])
    assert "// RUN: --vla-size-check=false --no-incremental-bmc" in lines, lines

for args, requirement, resolved in (("--32", "arch32", True), ("--64", "arch64", True),
                                    ("--64", "arch32", False), ("", "arch32", False),
                                    ("--32", "uchar_h", False)):
    lines = import_esbmc.header("suite/target", "CORE", args, ["REQUIRES " + requirement], ".", [])
    assert ("// MODE: FUTURE" not in lines) == resolved, lines
    if resolved:
        assert "// RUN: -m" + requirement[-2:] + " --no-incremental-bmc" in lines, lines
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
assert "// RUN: --solver z3 -m32 --no-incremental-bmc" in lines, lines
assert not any(line.startswith(("// Needs:", "// MODE:")) for line in lines), lines
lines = import_esbmc.header("z3/quantified", "CORE", "--floatbv --z3", [], ".",
                           ["__ESBMC_forall"])
assert "// Needs: __ESBMC_forall" in lines, lines
assert "// MODE: FUTURE" in lines, lines
assert "// RUN: --solver z3 --no-incremental-bmc" in lines, lines

for name, args, flags in (("vla_above_ptrdiff_max", "", []),
                          ("vla_no_size_check_fail", "--32", ["// RUN: -m32 --no-incremental-bmc"])):
    lines = import_esbmc.header("esbmc/" + name, "CORE", args,
                               ["object size exceeds PTRDIFF_MAX"], ".", [])
    assert "// EXPECT: alloca: size exceeds PTRDIFF_MAX" in lines, lines
    assert all(flag in lines for flag in flags), lines

for name in ("array_loop_summary_decrement", "array_loop_summary_decrement_fail"):
    for args, flags in (("", "--no-incremental-bmc"), ("--32", "-m32 --no-incremental-bmc")):
        lines = import_esbmc.header("esbmc/" + name, "CORE", args, [], ".", [])
        runs = [line.partition(":")[2].strip() for line in lines if line.startswith("// RUN:")]
        assert runs == [flags, (flags + " --no-summarize-array-loops").strip()], runs

for flag in ('--unwindset f:0:3', '--unwindsetname loop:4', '--data-races-check', '--no-standard-check'):
    lines = import_esbmc.header('suite/semantic', 'CORE', flag, [], '.', [])
    assert '// MODE: FUTURE' in lines, lines
    assert any(line.startswith('// Needs:') for line in lines), lines
assert import_esbmc.translate('--cvc5 -D__builtin_unreachable()', '.') == (
    ['--solver', 'cvc5', '-D__builtin_unreachable()'], [], [])
for args, expected in (('--gcse --z3', '--solver z3'),
                       ('--array-flattener --overflow --bitwuzla',
                        '--ub-signed-overflow-check --ub-shift-check '
                        '--ub-float-cast-overflow-check --solver bitwuzla')):
    lines = import_esbmc.header('suite/last_blocker', 'CORE', args, [], '.', [])
    assert '// RUN: ' + expected + ' --no-incremental-bmc' in lines, lines
    assert not any(line.startswith('// Needs:') for line in lines), lines

# ESBMC checks one bound unless a flag gives a bound or an incremental mode.
for args, single in (("", True), ("--no-unwinding-assertions", True), ("--unwind 3", False),
                     ("--unwind=3", False), ("--incremental-bmc", False),
                     ("--k-induction", False), ("--falsification", False),
                     ("--termination", False), ("--cheri purecap", True)):
    lines = import_esbmc.header("suite/bound", "CORE", args, [], ".", [])
    runs = [line for line in lines if line.startswith("// RUN:")]
    assert any(r.endswith("--no-incremental-bmc") for r in runs) == single, (args, lines)

# A FUTURE test's RUN line keeps its flags, but not those translate() drops.
for args, run in (
        ("--overflow-check --no-fp-conversion-check --no-bounds-check -Wno-error --k-induction",
         "--ub-signed-overflow-check --ub-shift-check --k-induction"),
        ("--k-induction --no-fp-conversion-check --overflow-check --no-slice",
         "--k-induction --ub-signed-overflow-check --ub-shift-check"),
        ("--overflow-check --k-induction",
         "--ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --k-induction")):
    lines = import_esbmc.header("suite/raw_flags", "CORE", args, [], ".", [])
    assert "// RUN: " + run in lines, lines
    assert "// Needs: --k-induction" in lines, lines

# Output-only and scheduling flags are dropped, --i386-macos is -m32, and a bare
# "-" (stdin or stdout) goes with the flag before it, never into a Needs line.
assert import_esbmc.translate("--smt-model --memstats --interactive-ileaves --schedule "
                              "--i386-macos --unwind 3", ".") == (["-m32", "--unwind", "3"], [], [])
assert import_esbmc.translate("--claim-fingerprint-dump - --multi-property", ".") == (
    ["--multi-property"], [], [])
for args, needs, run in (
        ("--witness-output-yaml - --k-induction", "--witness-output-yaml --k-induction",
         "--witness-output-yaml - --k-induction"),
        ("--claim-fingerprint-dump - --k-induction", "--k-induction", "--k-induction"),
        ("--schedule --k-induction --i386-macos", "--k-induction", "--k-induction -m32")):
    lines = import_esbmc.header("suite/dash", "CORE", args, [], ".", [])
    assert "// Needs: " + needs in lines and "// RUN: " + run in lines, lines
# --timeout and its value go: run.py sets the time limit.
assert import_esbmc.translate("--timeout 60s --unwind 2", ".") == (["--unwind", "2"], [], [])
assert import_esbmc.translate("--timeout=30 --k-induction", ".") == ([], [], ["--k-induction"])
lines = import_esbmc.header("suite/timeout", "CORE", "--timeout 60s --k-induction", [], ".", [])
assert "// Needs: --k-induction" in lines and "// RUN: --k-induction" in lines, lines
