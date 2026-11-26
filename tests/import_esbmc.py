#!/usr/bin/env python3
"""Copies ESBMC's C regression tests into Vereda's format.

    tests/import_esbmc.py ~/tools/esbmc/regression [suite ...]

Each ESBMC test <suite>/<test>/ with a C input becomes tests/<name>/
<test>.c when the source is its only file, and <test>/main.c next to the
other files otherwise, where <name> is the suite, prefixed with "esbmc-"
unless it already starts with "esbmc". The test.desc goes into a comment
header at the end of the C file, so that line numbers in the expected
output still hold. A rerun replaces earlier copies.

__ESBMC_assert, __CPROVER_assert, __ESBMC_assume, __CPROVER_assume,
__ESBMC_atomic_begin, __ESBMC_atomic_end, __ESBMC_memset and
__ESBMC_memcpy become __VERIFIER_assert, __VERIFIER_assume,
__VERIFIER_atomic_begin, __VERIFIER_atomic_end, memset and memcpy in
the copied sources,
__ESBMC_builtin_object_size and __ESBMC_alloca become clang's
__builtin_object_size and __builtin_alloca, __ESBMC_bitcast(tgt, src)
becomes __builtin_memcpy(tgt, src, sizeof *(tgt)), __ESBMC_unroll(N)
right before a loop becomes #pragma unroll N+1, the bound ESBMC gives
it, and --enable-unreachability-intrinsic is dropped: reach_error() is
always an error in Vereda. Setting __ESBMC_rounding_mode becomes
fesetround() where that covers every use of it, and nondet_X becomes
__VERIFIER_nondet_X. ESBMC flags with a Vereda equivalent are
translated. Flags that only turn off a check Vereda does not have are
dropped. A test that needs any other flag keeps its ESBMC flags and
becomes FUTURE, with the missing flags named in a "// Needs:" line;
Vereda then rejects the flags, so the test passes until Vereda learns
them and the script runs again. ESBMC's REQUIRES, CHECK_JSON, CHECK_FILE
and SEED_FILE lines make a test FUTURE the same way, as run.py has none
of them. Vereda supports only the __VERIFIER_ builtins, so a test whose
code still calls an __ESBMC_ or __CPROVER_ function after the renames
becomes FUTURE too, with those functions in a "// Needs:" line of their
own, and run.py skips it without running Vereda. Expectations on ESBMC's
counterexample are rewritten for Vereda's, which prints it as C.

Tests that read files outside their own directory are skipped, and so is
ESBMC's "disabled" suite unless named. So are tests of features only
ESBMC's users need, function contracts, integer/real arithmetic, LTL
monitors and the unreachability intrinsic (ESBMC_ONLY), which Vereda
does not plan to support, and the tests in SKIPPED_TESTS; a rerun
deletes earlier copies.

With --mark, each copied test then runs through run.py, and one that
does not behave as its mode says gets the other mode: a failing CORE or
THOROUGH test becomes FUTURE, and a FUTURE or KNOWNBUG test Vereda
passes becomes CORE; a test that times out keeps its mode. A
"// Vereda:" line says why. The suite then passes as Vereda is now, and
a later Vereda that fixes a FUTURE test fails it until it is promoted."""
import argparse
import concurrent.futures
import os
import re
import shlex
import shutil
import subprocess
import sys

sys.dont_write_bytecode = True  # no __pycache__ next to the tests
import run  # pylint: disable=wrong-import-position

# ESBMC flag -> Vereda flags; a value, if the flag takes one, follows.
SAME = {"--no-unwinding-assertions", "--incremental-bmc", "--multi-property",
        "--force-malloc-success", "--fp2bv", "--overflow-check", "--unsigned-overflow-check",
        "--ub-shift-check", "--unlimited-k-steps", "--no-summarize-array-loops",
        "--no-pointer-check", "--no-vla-size-check", "--no-assertions", "--no-div-by-zero-check"}
SAME_WITH_VALUE = {"--unwind", "--max-k-step", "--k-step", "--std"}
RENAMED = {
    "--unlimited-k-step": ["--unlimited-k-steps"],
    "--no-unwinding-assertion": ["--no-unwinding-assertions"],
    "--32": ["-m32"], "--64": ["-m64"], "--overflow": ["--overflow-check"],
    "--bitwuzla": ["--solver", "bitwuzla"], "--z3": ["--solver", "z3"],
    "--cvc": ["--solver", "cvc5"], "--cvc5": ["--solver", "cvc5"],
}
UNSUPPORTED_ALIASES = {"--data-races-check": "--data-race-check",
                       "--no-standard-check": "--no-standard-checks"}
RENAMED_WITH_VALUE = {"--function": "--entry", "--func": "--entry"}
# Checks and tuning Vereda does not have, so turning them off changes nothing,
# --summarize-array-loops, which Vereda does by default. Ignore ESBMC's internal
# pipeline switches and numeric selectors in both normal and fallback commands.
PROGRAM_OUTPUT_FLAGS = {"--" + name + "-" + suffix
                        for name in ("goto-program", "goto-functions", "program")
                        for suffix in ("only", "too")}
IGNORED_FLAGS = {"--floatbv", "--fixedbv", "--tuple-sym-flattener", "--state-hashing",
                 "--smt-during-symex", "--add-symex-value-sets"} | PROGRAM_OUTPUT_FLAGS
IGNORED_FLAGS |= {
    "--gcse", "--field-sensitive-ssa", "--smt-symex-assume", "--parallel-solving",
    "--scalarize-structs", "--no-propagation", "--no-return-value-opt", "--full-inlining",
    "--no-remove-unreachable", "--no-slice-id", "--no-slice-name", "--little-endian",
    "--yices", "--result-only", "--show-cex", "--color", "--show-claims",
    "--show-call-sites", "--ssa-trace", "--symex-trace", "--parse-tree-too",
    "--symbol-table-too", "--ssa-guards", "--ssa-no-location", "--ssa-smt-trace",
    "--ssa-symbol-table", "--symex-ssa-trace", "--claim-fingerprint-dump",
    "--clang-c-irep2-adjust-only", "--goto-unwind", "--goto2lean",
    "--smt-symex-guard", "--unlimited-goto-unwind", "--compact-trace", "--ts-bmc",
    "--smt-symex-assert", "--cswitch-skip-readonly-globals", "--bitwuzllob",
    "--ts-pdr", "--boolector", "--irep2-bodies", "--symbol-table-only", "--bv",
    "--neurosym", "--smt-formula-only", "--smt-formula-too", "--dump-smt-only",
    "--array-flattener", "--mathsat", "--no-irep2-native-body", "--show-vcc",
    "--tuple-node-flattener", "--no-goto-merge", "--show-goto-value-sets",
    "--ts-push-pop", "--show-symex-value-sets", "--all-runs",
    "--ts-check", "--ts-no-fallback",
}
IGNORED_WITH_VALUE = {
    "--verbosity",
    "--bitwuzllob-prog", "--bitwuzllob-model-prog", "--neurosym-prog",
    "--neurosym-model-prog",
}
DROPPED = {"--summarize-array-loops", "--no-bounds-check",
           "--no-align-check", "--no-pointer-relation-check",
           "--no-simplify", "--no-slice"} | IGNORED_FLAGS


def translate(args, test_dir):
    """Returns (Vereda flags, extra source files, ESBMC flags Vereda lacks)."""
    flags, files, missing = [], [], []
    tokens = iter(shlex.split(args))
    for tok in tokens:
        name, eq, value = tok.partition("=")
        if name in IGNORED_WITH_VALUE:
            if not eq:
                next(tokens, "")
        elif not tok.startswith("-"):
            if tok.endswith(".c") and os.path.exists(os.path.join(test_dir, tok)):
                files.append(tok)
            else:
                missing.append(tok)
        elif name in SAME_WITH_VALUE or name in RENAMED_WITH_VALUE:
            value = value if eq else next(tokens, "")
            flags += [RENAMED_WITH_VALUE.get(name, name), value]
        elif tok in SAME:
            flags.append(tok)
        elif name in UNSUPPORTED_ALIASES:
            missing.append(UNSUPPORTED_ALIASES[name] + ("=" + value if eq else ""))
        elif tok in RENAMED:
            flags += RENAMED[tok]
        elif tok in DROPPED or name in IGNORED_FLAGS or tok.startswith("-Wno-"):
            continue
        elif tok.startswith("-D"):
            flags.append(tok if len(tok) > 2 else tok + next(tokens, ""))
        else:
            missing.append(tok)
    return flags, files, missing


def suite_name(suite):
    """Vereda's directory for an ESBMC suite."""
    return suite if suite.startswith("esbmc") else "esbmc-" + suite


# test.desc lines that are not output regexes.
DIRECTIVES = ("REQUIRES", "CHECK_JSON", "CHECK_FILE", "SEED_FILE")
OUTPUT_MODES = PROGRAM_OUTPUT_FLAGS | {"--smt-formula-only", "--smt-formula-too",
                                      "--claim-fingerprint-dump", "--show-claims",
                                      "--show-call-sites", "--ssa-trace", "--symex-trace",
                                      "--parse-tree-too", "--symbol-table-too",
                                      "--ssa-guards", "--ssa-no-location", "--ssa-smt-trace",
                                      "--ssa-symbol-table", "--symex-ssa-trace",
                                      "--dump-smt-only", "--symbol-table-only",
                                      "--clang-c-irep2-adjust-only", "--irep2-bodies",
                                      "--show-vcc", "--show-goto-value-sets",
                                      "--show-symex-value-sets", "--goto2lean"}


def output_mode(args):
    """Whether the test asks for an ESBMC formula or program dump."""
    return any(token.partition("=")[0] in OUTPUT_MODES for token in shlex.split(args))


def header(origin, mode, args, expected, test_dir, builtins):
    """The comment header Vereda's run.py reads, for one test.desc whose
    sources call the __ESBMC_ and __CPROVER_ functions in builtins."""
    flags, files, missing = translate(args, test_dir)
    regexes = [r for r in expected if r.split()[0] not in DIRECTIVES]
    # These pointer-relation tests rely on ESBMC assuming the symbolic size bound.
    if origin in {"esbmc/ptr_rel_huge_object_alloca", "esbmc/ptr_rel_huge_object_vla"}:
        if "--no-vla-size-check" not in flags:
            flags.append("--no-vla-size-check")
    if origin in {"esbmc/vla_above_ptrdiff_max", "esbmc/vla_no_size_check_fail"}:
        regexes = [r.replace("object size exceeds PTRDIFF_MAX", "alloca: size exceeds PTRDIFF_MAX")
                   for r in regexes]
    # Explicit target flags satisfy these requirements independently of the host.
    target = "-m64" if "-m64" in flags else "-m32" if "-m32" in flags else None
    architectures = {"REQUIRES arch32": "-m32", "REQUIRES arch64": "-m64"}
    missing += [r for r in expected if r.split()[0] in DIRECTIVES
                and not (r in architectures and architectures[r] == target)]
    lines = [f"// Imported from ESBMC regression/{origin}"]
    if builtins:
        lines.append("// Needs: " + ", ".join(builtins))
    if missing or builtins:
        mode = "KNOWNBUG" if mode == "KNOWNBUG" else "FUTURE"
    if missing:
        # Keep unsupported options visible without restoring ignored flags.
        lexer = shlex.shlex(args, posix=True)
        lexer.whitespace_split, lexer.commenters = True, ""
        raw_args, start, skip_value = [], 0, False
        for token in lexer:
            end = lexer.instream.tell()
            name, eq, value = token.partition("=")
            if skip_value:
                skip_value = False
            elif name in IGNORED_WITH_VALUE:
                skip_value = not eq
            elif name in UNSUPPORTED_ALIASES:
                raw_args.append(UNSUPPORTED_ALIASES[name] + ("=" + value if eq else "") + " ")
            elif name in RENAMED:
                renamed = shlex.join(RENAMED[name])
                raw_args.append(renamed + " ")
            elif name in RENAMED_WITH_VALUE:
                raw_args.append(RENAMED_WITH_VALUE[name] + ("=" + value if eq else " "))
            elif name not in IGNORED_FLAGS:
                raw_args.append(args[start:end])
            start = end
        lines += ["// Needs: " + " ".join(missing), ("// RUN: " + "".join(raw_args).strip()).rstrip()]
    elif flags:
        lines.append("// RUN: " + " ".join(flags))
    # Keep both the summarized and unrolled decrement performance regressions.
    if origin in {"esbmc/array_loop_summary_decrement", "esbmc/array_loop_summary_decrement_fail"} \
            and not (missing or builtins) and "--no-summarize-array-loops" not in flags:
        if not flags:
            lines.append("// RUN:")
        lines.append("// RUN: " + " ".join(flags + ["--no-summarize-array-loops"]))
    if files:
        lines.append("// FILES: " + " ".join(files))
    if mode != "CORE":
        lines.append("// MODE: " + mode)
    return lines + ["// EXPECT: " + r for r in regexes]


# Output only ESBMC prints, which no Vereda run can match: its formula
# statistics, its summary with a passed count, and its wording for a failed
# assert(), 'assertion <condition>' (Vereda names the check function, as in
# __assert_fail; 'unwinding assertion loop N' is common to both).
ESBMC_OUTPUT = re.compile(r"VCC\\?\(s\\?\)|properties failed, .*passed"
                          r"|(?<!unwinding)(?<!unwinding )(?<!unwinding\\s)(?<!unwinding\\s\+)"
                          r"\bassertion(?: |\\s)(?!loop)")


def esbmc_output(regex, messages):
    """Whether an expectation matches only ESBMC's output: ESBMC_OUTPUT, or
    the message of an __ESBMC_assert, which the import keeps as a comment only."""
    plain = re.sub(r"\\(.)", r"\1", regex)
    return bool(ESBMC_OUTPUT.search(regex)) or any(len(m) >= 6 and m in plain for m in messages)


def assert_messages(src):
    """The messages of the __ESBMC_assert calls in the C files of test
    directory src, as the import writes them into comments."""
    messages = set()
    for name in os.listdir(src):
        path = os.path.join(src, name)
        if not name.endswith((".c", ".h")) or not os.path.isfile(path):
            continue
        with open(path, encoding="utf-8", errors="replace") as f:
            text = f.read()
        for m in ASSERT.finditer(text):
            parsed = call_arguments(text, m.end() - 1)
            if parsed and len(parsed[0]) == 2:
                start, end = parsed[0][1]
                messages.add(message_comment(text[start:end]))
    return messages


def as_vereda(regex):
    """Translate ESBMC trace separators, values and summary to Vereda's layout."""
    if re.match(r"\^?(?: |\\s[+*]?)*PASSED\b", regex) and \
            (r"\[" in regex or re.search(r"line(?: |\\s)", regex)):
        regex = regex.replace("PASSED", "SUCCESS", 1)
    regex = re.sub(r"\bline +(?=[0-9])", "line +", regex)
    overflow = re.fullmatch(r'\^  !overflow\\\("([^" ]+)", (\w+), (\w+)\\\)\$', regex)
    operations = {r"\+": "add", "-": "sub", r"\-": "sub", r"\*": "mul"}
    if overflow and overflow[1] in operations:
        regex = "^  arithmetic overflow on " + operations[overflow[1]] + "$"
    regex = regex.replace(r"\n-+\n", r"\n")
    regex = regex.replace(r"\[Counterexample\]", r"\[COUNTEREXAMPLE\]")
    regex = regex.replace(r"\*\* Results:", r"\[RESULTS\]")
    regex = regex.replace(r"\*\* ", "")
    count = r"(?:[0-9]+|\\d\+|\[0-9\]\+)"
    regex = re.sub("(" + count + ") of (" + count + ") properties failed",
                   lambda m: "Properties: " + m[2] + r" checked, \d+ passed, " + m[1]
                   + r" failed, \d+ not checked", regex)
    if regex.startswith("^Unwinding loop"):
        regex = regex.replace("^Unwinding loop", r"^\[PROGRESS\] Unwinding loop", 1)
    value = r"( = (?:\\.|[^\s\\])+)"
    regex = re.sub(value + r" \\\(([^\\]*)(?:\\\))?",
                   lambda m: m[1] + "; //" + (f" {m[2]}" if m[2] else ""), regex)
    regex = re.sub(r"\\\(((?:[01]{8} )*[01]{8})\\\)", r"// \1", regex)
    if "\\{" not in regex:
        regex = re.sub(r"^(\^(?:  |\\s\+)\S+)" + value + r"\$$", r"\1\2;$", regex)
    return regex


def expectations(lines, src, args=""):
    """The output regexes of a test.desc, without the ones only ESBMC's
    output matches, made to match Vereda's counterexample."""
    messages = assert_messages(src)
    dump = output_mode(args)
    result = []
    for line in lines:
        regex = line.strip()
        if not regex:
            continue
        # Keep the claim, status and location while dropping ESBMC's label.
        if dump and "assertion" in regex and r"\[" in regex and re.search(r"\b(?:FAILED|PASSED)\b", regex):
            for message in messages:
                for separator in (" +", r"\s+", " "):
                    suffix = separator + re.escape(message) + "$"
                    if regex.endswith(suffix):
                        regex = regex[:-len(suffix)] + r"(?:\s|$)"
        if not esbmc_output(regex, messages) and not (dump and dump_output(regex)):
            result.append(as_vereda(regex))
    return result


def dump_output(regex):
    """ESBMC's GOTO/SSA/SMT representation, not a verdict or counterexample."""
    if regex.split()[0] in ("CHECK_FILE", "CHECK_JSON"):
        return True
    plain = re.sub(r"\\(.)", r"\1", regex)
    return bool(re.search(
        r"\b(?:ASSIGN|ASSERT|DECL|DEAD|GOTO|FUNCTION_CALL|END_FUNCTION|START_THREAD|"
        r"ATOMIC_BEGIN|ATOMIC_END)\b|\bc:@[FS]@|anon_pad#|"
        r"return_value\$|\?\d+!|"
        r"\((?:set-|check-|declare-|define-|assert\b|bv\w+)|"
        r"SMT formula|Solving claim|Encoding to solver time|Field-sensitive SSA|"
        r"Checking (?:forward condition|inductive step)", plain)) or bool(
            re.fullmatch(r"bv\w+", plain.strip("^$")))


def outside(test_dir, path):
    """Whether path, relative to test_dir, leaves it."""
    real = os.path.realpath(os.path.join(test_dir, path))
    return os.path.commonpath([real, os.path.realpath(test_dir)]) != os.path.realpath(test_dir)


def copy_files(src, dest, source, tracked):
    """Copies the test's files from src, renaming source to dest.c when it
    is the only one and to dest/main.c otherwise. Returns that path, or
    None when another file is already main.c."""
    def ignored(directory, names):
        # A test's directory can hold other tests; each is copied on its own.
        def untracked(p):
            return tracked is not None and os.path.isfile(p) and os.path.realpath(p) not in tracked
        paths = {n: os.path.join(directory, n) for n in names}
        return {n for n, p in paths.items() if n == "test.desc" or untracked(p)
                or os.path.exists(os.path.join(p, "test.desc"))}

    shutil.rmtree(dest, ignore_errors=True)
    if os.path.exists(dest + ".c"):
        os.remove(dest + ".c")
    shutil.copytree(src, dest, ignore=ignored)
    for root, _, _ in os.walk(dest, topdown=False):  # what the filters emptied
        if not os.listdir(root):
            os.rmdir(root)
    if os.listdir(dest) == [source]:
        os.replace(os.path.join(dest, source), dest + ".c")
        os.rmdir(dest)
        return dest + ".c"
    if source != "main.c" and os.path.exists(os.path.join(dest, "main.c")):
        shutil.rmtree(dest)
        return None
    os.replace(os.path.join(dest, source), os.path.join(dest, "main.c"))
    return os.path.join(dest, "main.c")


# ESBMC and CBMC builtins with an equivalent that Vereda models. The others
# (quantifiers, pointer predicates, ...) have none and keep their names,
# and a test that calls one becomes FUTURE.
RENAMES = {"__ESBMC_assert": "__VERIFIER_assert", "__ESBMC_assume": "__VERIFIER_assume",
           "__CPROVER_assert": "__VERIFIER_assert", "__CPROVER_assume": "__VERIFIER_assume",
           "__ESBMC_memset": "memset", "__ESBMC_memcpy": "memcpy",
           "__ESBMC_atomic_begin": "__VERIFIER_atomic_begin",
           "__ESBMC_atomic_end": "__VERIFIER_atomic_end",
           # ESBMC defines __builtin_object_size as its own and handles
           # __builtin_alloca like __ESBMC_alloca, so clang's builtins match.
           "__ESBMC_builtin_object_size": "__builtin_object_size",
           "__ESBMC_alloca": "__builtin_alloca"}
# Besides turning __ESBMC_unreachable() into a violation, which Vereda does
# not support, this flag makes reach_error() and __VERIFIER_error() errors,
# as they always are in Vereda; so it is dropped, and a test that uses the
# intrinsic with it is skipped (ESBMC_ONLY).
UNREACHABLE_FLAG = re.compile(r"(?<!\S)--enable-unreachability-intrinsic(?!\S)\s*")


def rename(text):
    """Renames the builtins in text."""
    return re.sub(r"\b(" + "|".join(RENAMES) + r")\b", lambda m: RENAMES[m.group(1)], text)
ASSERT = re.compile(r"\b__(?:ESBMC|CPROVER)_assert\s*\(")
LITERAL = re.compile(r'"((?:[^"\\\n]|\\.)*)"')


def skip_literal(text, i):
    """The index past the string, character literal or comment starting at
    text[i], or None when none starts there."""
    if text.startswith("//", i):
        end = text.find("\n", i)
        return len(text) if end < 0 else end
    if text.startswith("/*", i):
        end = text.find("*/", i + 2)
        return len(text) if end < 0 else end + 2
    if text[i] in "\"'":
        j = i + 1
        while j < len(text) and text[j] not in (text[i], "\n"):
            j += 2 if text[j] == "\\" else 1
        return j + 1
    return None


def call_arguments(text, start):
    """Splits the call whose '(' is at text[start] into its arguments.
    Returns (spans of the arguments, index of the closing ')'), or None when
    the call does not close."""
    spans, arg, depth, i = [], start + 1, 0, start + 1
    while i < len(text):
        past = skip_literal(text, i)
        if past is not None:
            i = past
            continue
        c = text[i]
        if c in "([{":
            depth += 1
        elif c in ")]}" and depth:
            depth -= 1
        elif c == ")" or (c == "," and not depth):
            spans.append((arg, i))
            if c == ")":
                return spans, i
            arg = i + 1
        i += 1
    return None


def message_comment(message):
    """The comment text for __ESBMC_assert's message argument: the contents
    of its string literals, or its source text when it is something else."""
    parts = LITERAL.findall(message)
    text = "".join(parts) if parts and not LITERAL.sub("", message).strip() else message
    # No '*/' to end a block comment, no trailing '\' to splice the next line.
    return " ".join(text.split()).replace("*/", "* /").rstrip("\\ ")


def rewrite_asserts(text):
    """Turns __ESBMC_assert(cond, message) and __CPROVER_assert(cond,
    message) into __VERIFIER_assert(cond) with the message as a comment:
    '// message' at the end of the line the call ends on, or '/* message */'
    right after the call when the rest of that line continues a macro or
    holds a comment or literal. Line numbers stay as they were. In comments
    and strings the name changes and nothing else. The other builtins are
    renamed as rename() says."""
    out, pending, i = [], [], 0
    while i < len(text):
        past = skip_literal(text, i)
        if past is not None:
            out.append(text[i:past])
            i = past
            continue
        m = ASSERT.match(text, i) if text[i] == "_" else None
        parsed = m and call_arguments(text, m.end() - 1)
        if not parsed or len(parsed[0]) != 2:
            if text[i] == "\n" and pending:
                out.insert(len(out) - (out[-1] == "\r"), " // " + "; ".join(pending))
                pending = []
            out.append(text[i])
            i += 1
            continue
        (_, cond_end), (msg_start, msg_end) = parsed[0]
        close = parsed[1]
        message = message_comment(text[msg_start:msg_end])
        out.append(text[i:cond_end])
        # The message's newlines stay, so the lines after it keep their numbers.
        out.append("\n" * text.count("\n", cond_end, close) + ")")
        i = close + 1
        if message:
            eol = text.find("\n", i)
            rest = text[i:len(text) if eol < 0 else eol]
            if rest.rstrip().endswith("\\") or any(c in rest for c in ("//", "/*", '"', "'")):
                out.append(f" /* {message} */")
            else:
                pending.append(message)
    if pending:
        out.append(" // " + "; ".join(pending))
    return rename("".join(out))


BITCAST = re.compile(r"\b__ESBMC_bitcast\s*\(")


def rewrite_bitcasts(text):
    """Turns __ESBMC_bitcast(tgt, src), which copies *src's bits into *tgt,
    into __builtin_memcpy(tgt, src, sizeof *(tgt)). Calls in comments and
    strings, and ones that do not take two arguments, are left alone."""
    out, i = [], 0
    while i < len(text):
        past = skip_literal(text, i)
        if past is not None:
            out.append(text[i:past])
            i = past
            continue
        m = BITCAST.match(text, i) if text[i] == "_" else None
        parsed = m and call_arguments(text, m.end() - 1)
        if not parsed or len(parsed[0]) != 2:
            out.append(text[i])
            i += 1
            continue
        (tgt_start, tgt_end), (_, close) = parsed[0][0], (None, parsed[1])
        tgt = text[tgt_start:tgt_end]
        out.append("__builtin_memcpy(" + text[tgt_start:close]
                   + f", sizeof *({' '.join(tgt.split())}))")
        i = close + 1
    return "".join(out)


UNROLL = re.compile(r"^([ \t]*)__ESBMC_unroll\s*\(([^;\n]*)\)\s*;[ \t]*$", re.M)
LOOP_START = re.compile(r"\s*(for|while|do)\b")


def rewrite_unroll(text):
    """Turns a line holding only __ESBMC_unroll(N); into #pragma unroll N+1
    when the next code is a loop, as the pragma must be; a call anywhere
    else is left alone. ESBMC reads clang's #pragma unroll M as the loop's
    unwinding bound and implements __ESBMC_unroll(N) as that bound with
    M = N + 1 (loop_unroll.cpp), so the two mean the same to it."""
    # Comments and literals blanked out, newlines kept, so offsets match.
    code = "".join(c if is_code else re.sub(r"[^\n]", " ", c) for is_code, c in segments(text))
    out, i = [], 0
    for m in UNROLL.finditer(code):
        if not LOOP_START.match(code, m.end()):
            continue
        n = m.group(2).strip()
        after = code.index(";", m.end(2)) + 1  # a comment after the call stays
        out.append(text[i:m.start()] + m.group(1) + "#pragma unroll "
                   + (str(int(n) + 1) if n.isdigit() else f"(({n}) + 1)")
                   + text[after:m.end()].rstrip())
        i = m.end()
    out.append(text[i:])
    return "".join(out)


# ESBMC's rounding mode is a variable its fenv.h model reads; each value it
# can be set to, and the C rounding mode for it. 1, ties away from zero, has
# none.
ROUNDING_MODES = {"0": "FE_TONEAREST", "2": "FE_UPWARD", "3": "FE_DOWNWARD",
                  "4": "FE_TOWARDZERO"}
ROUNDING_DECL = re.compile(r"^extern int __ESBMC_rounding_mode;[ \t]*$", re.M)
ROUNDING_SET = re.compile(r"\b__ESBMC_rounding_mode\s*=\s*([0-4])\s*;")


def rewrite_rounding(text):
    """Turns __ESBMC_rounding_mode = <mode>; into fesetround(FE_<mode>);
    and the variable's declaration line into #include <fenv.h>, keeping the
    line count. Only when that covers every use outside comments and
    strings: a file that sets mode 1 or reads the variable is unchanged."""
    if "__ESBMC_rounding_mode" not in text:
        return text
    def replace(m):
        mode = ROUNDING_MODES.get(m.group(1))
        return None if mode is None else f"fesetround({mode});" + "\n" * m.group(0).count("\n")
    out, i, uses = [], 0, 0
    while i < len(text):
        past = skip_literal(text, i)
        if past is not None:
            out.append(text[i:past])
            i = past
            continue
        decl = ROUNDING_DECL.match(text, i) if text[i] == "e" and (i == 0 or text[i - 1] == "\n") \
            else None
        assign = ROUNDING_SET.match(text, i) if text[i] == "_" else None
        if decl:
            out.append("#include <fenv.h>")
            i = decl.end()
        elif assign and replace(assign):
            out.append(replace(assign))
            i = assign.end()
        else:
            uses += text.startswith("__ESBMC_rounding_mode", i)
            out.append(text[i])
            i += 1
    return text if uses else "".join(out)


NONDET = re.compile(r"\bnondet_\w+")
LITERAL_START = re.compile(r"//|/\*|[\"']")


def segments(text):
    """Splits text into (is_code, chunk) pieces: comments, strings and
    character literals are the pieces that are not code."""
    i = 0
    for m in LITERAL_START.finditer(text):
        if m.start() < i:
            continue
        yield True, text[i:m.start()]
        i = skip_literal(text, m.start())
        yield False, text[m.start():i]
    yield True, text[i:]


def code_sub(pattern, repl, text):
    """pattern.sub(repl, text) outside comments, strings and character
    literals."""
    return "".join(pattern.sub(repl, c) if code else c for code, c in segments(text))


def nondets_to_rename(texts):
    """The nondet_ names in texts, a test's sources, to rename to
    __VERIFIER_nondet_: ESBMC and Vereda both treat a bodyless function so
    named as returning a nondet value and call one with a body, so renaming
    every use keeps the meaning, unless the test defines nondet_X with a
    body while also using __VERIFIER_nondet_X."""
    code = " ".join(c for t in texts for is_code, c in segments(t) if is_code)
    names = set(NONDET.findall(code))
    verifier = set(re.findall(r"__VERIFIER_(nondet_\w+)", code))
    defined = set(re.findall(r"\b(nondet_\w+)\s*\([^;{)]*\)\s*\{", code))
    return names - (verifier & defined)


# An __ESBMC_ or __CPROVER_ name before '(', with the type before it when
# it has one: then it is declared or defined, not called.
BUILTIN_CALL = re.compile(r"(?:\b(\w+)\s+|(\*)\s*)?\b((?:__ESBMC|__CPROVER)_\w+)\s*\(")


def rename_builtins(path):
    """Renames ESBMC builtins to what Vereda models in the C files at path,
    a file or a directory; see rewrite_asserts, rename, rewrite_rounding and
    nondets_to_rename. Returns the nondet_ names renamed and the __ESBMC_
    and __CPROVER_ functions the code still calls."""
    files = [path] if os.path.isfile(path) else [
        os.path.join(root, n) for root, _, names in os.walk(path) for n in names
        if n.endswith((".c", ".h", ".i"))]
    texts = {}
    for name in files:
        with open(name, encoding="latin-1") as f:  # any bytes round-trip
            texts[name] = f.read()
    nondets, calls = nondets_to_rename(texts.values()), set()
    for name, text in texts.items():
        renamed = rewrite_unroll(rewrite_bitcasts(rewrite_rounding(
            rewrite_asserts(text))))
        if nondets:
            renamed = code_sub(NONDET, lambda m: "__VERIFIER_" + m.group(0)
                               if m.group(0) in nondets else m.group(0), renamed)
        calls.update(called for is_code, chunk in segments(renamed) if is_code
                     for before, star, called in BUILTIN_CALL.findall(chunk)
                     if before in ("", "return", "else", "do") and not star)
        if renamed != text:
            with open(name, "w", encoding="latin-1", newline="") as f:
                f.write(renamed)
    return nondets, sorted(calls)


# Features ESBMC has for its own users that SV-COMP never uses, so Vereda
# leaves them to ESBMC: (reason, flags, builtins, both). A test that names
# any of them, or with both a flag and a builtin when both is set, is not
# imported.
ESBMC_ONLY = [
    ("ESBMC frontend internals",
     re.compile(r"--(?:clang-c-irep2-adjust(?:-only|-writeback-all)?|irep2-bodies|no-irep2-native-body|old-frontend)\b"), None, False),
    ("Lean exporter", re.compile(r"--goto2lean\b"), None, False),
    ("transition-system engine", re.compile(r"--ts-(?:bmc|pdr|check|no-fallback|push-pop|dump|fresh-solver)\b"), None, False),
    ("experimental solver wrappers",
     re.compile(r"--(?:(?:bitwuzllob|neurosym)(?:-\w+)*|smtlib-oneshot(?:-model)?-prog)\b"),
     None, False),
    ("CHERI pointer semantics", re.compile(r"--cheri\b"), None, False),
    ("function contracts",
     re.compile(r"--(enforce-contract|replace-call-with-contract"
                r"|enforce-all-contracts|replace-all-contracts)\b"),
     re.compile(rb"\b__ESBMC_(requires|ensures|assigns|old|old_raw|return_value"
                rb"|is_fresh|contract)\b"), False),
    ("integer/real arithmetic",
     re.compile(r"(?<!\S)--ir(-ieee)?(?!\S)"),
     None, False),
    ("LTL monitors",
     re.compile(r"--ltl\b"),
     re.compile(rb"\b__ESBMC_(register_monitor|switch_to_monitor|switch_from_monitor"
                rb"|kill_monitor|ltl_sample|property__ltl2ba\w*|really_atomic_\w+)\b"), False),
    ("unreachability intrinsic", UNREACHABLE_FLAG, re.compile(rb"\b__ESBMC_unreachable\b"), True),
]


# Single tests left out, by ESBMC path, with the reason.
SKIPPED_TESTS = {
    "esbmc/github_243": "64-bit CIL Linux driver benchmark run under -m32; OOMs over 4 GiB in CI",
    "esbmc/vasprintf_unbounded_s_no_overflow": "tests ESBMC's INT_MAX/2 vasprintf cap on an unconstrained symbolic string",
    "esbmc/vasprintf_unbounded_s_overflow_fail": "tests ESBMC's INT_MAX/2 vasprintf cap on an unconstrained symbolic string",
    "z3/github_10316": "rounds ties away from zero, which C cannot set",
    "z3/github_10316_fail": "rounds ties away from zero, which C cannot set",
}

# Imported tests kept with local edits (such as smaller bounds) so a rerun
# skips them without overwriting or deleting the checked-in copy.
PRESERVED_TESTS = {
    "cstd/scanf-false-3": "uses --max-k-step 3 locally so it does not spend ~45 s reaching bound 50",
}


# Tests Vereda gets wrong in a way to fix, by ESBMC path: they are imported
# as KNOWNBUG, with the reason.
KNOWN_BUGS = {
    "esbmc/array_loop_summary_counter": "out of memory, over 16 GB",
    "esbmc/array_loop_summary_decrement": "out of memory, over 16 GB",
    "esbmc/array_loop_summary_store": "out of memory, over 16 GB",
    "esbmc/array_loop_summary_store_fail": "out of memory, over 16 GB",
    "esbmc/array_loop_summary_stride": "over 300 s",
    "esbmc/array_loop_summary_stride_fail": "over 300 s",
    "esbmc/github_9785_int_base_stores": "out of memory, needs 9.5 GB",
    "esbmc/github_123": "over 300 s in CI",
    "esbmc/github_2602_1": "over 300 s in CI",
    "esbmc/github_9356": "over 300 s in CI",
    "esbmc/github_9356_success": "over 300 s in CI",
    "z3/github_6100_assume_noninline": "over 300 s in CI",
    "z3/github_6100_noninline": "over 300 s in CI",
}


def esbmc_only(src, args):
    """The ESBMC-only feature the test in src, run with args, uses, or None.
    Directories of other tests inside src are not looked at."""
    texts = []
    for root, dirs, names in os.walk(src):
        dirs[:] = [d for d in dirs if not os.path.exists(os.path.join(root, d, "test.desc"))]
        for n in names:
            if n.endswith((".c", ".h", ".i")):
                with open(os.path.join(root, n), "rb") as f:
                    texts.append(f.read())
    for reason, flags, builtins, both in ESBMC_ONLY:
        flag = bool(flags.search(args))
        builtin = bool(builtins) and any(builtins.search(t) for t in texts)
        if (flag and builtin) if both else (flag or builtin):
            return reason
    return None


def import_test(src, dest, origin, tracked):
    """Copies one test from src to dest, or to dest.c when it has one file.
    Returns the C file written, or why there is none. When tracked is a set
    of paths, files outside it are left behind."""
    with open(os.path.join(src, "test.desc"), encoding="utf-8", errors="replace") as f:
        lines = f.read().split("\n")
    if len(lines) < 3 or not lines[1].strip().endswith(".c"):
        return "not C"
    mode, source, args = (line.strip() for line in lines[:3])
    try:
        paths = [source] + [t for t in shlex.split(args) if not t.startswith("-")]
    except ValueError:
        return "unreadable flags"
    if any(outside(src, p) for p in paths):
        return "files outside the test"
    if origin in PRESERVED_TESTS:
        return PRESERVED_TESTS[origin]
    regexes = expectations(lines[3:], src, args)
    reason = SKIPPED_TESTS.get(origin) or esbmc_only(src, args)
    if output_mode(args) and not any(run.EXPECTED.fullmatch(r) for r in regexes):
        reason = reason or "ESBMC-specific dump output only"
    if reason:
        shutil.rmtree(dest, ignore_errors=True)  # an earlier copy
        if os.path.exists(dest + ".c"):
            os.remove(dest + ".c")
        return reason

    entry = copy_files(src, dest, source, tracked)
    if not entry:
        return "a main.c besides the source"
    nondets, builtins = rename_builtins(entry if entry == dest + ".c" else dest)
    # -Dldv_assume=__ESBMC_assume and the like name builtins too.
    args = rename(UNREACHABLE_FLAG.sub("", args).strip())
    for nondet in nondets:  # e.g. an assertion's text in the results list
        regexes = [re.sub(r"(?<!\w)" + nondet + r"\b", "__VERIFIER_" + nondet, r) for r in regexes]
    name = os.path.basename(entry)
    if source != name:
        # The output names the source file, so the expectations follow it.
        old = re.compile(r"(?<![\w-])" + "".join(r"\\?\." if c == "." else re.escape(c)
                                                 for c in source))
        regexes = [old.sub(lambda _: re.escape(name), r) for r in regexes]

    with open(entry, "rb") as f:
        newline = b"" if f.read().endswith(b"\n") else b"\n"
    with open(entry, "ab") as f:
        lines = header(origin, mode, args, regexes, src, builtins)
        if origin in KNOWN_BUGS:
            lines += [f"// Vereda: {KNOWN_BUGS[origin]}", "// MODE: KNOWNBUG"]
        f.write(newline + ("\n".join(lines) + "\n").encode())
    return entry


def record_verdict(test, verdict):
    """Records verdict as the test's `// Vereda: verdict` line, or drops the
    line when verdict is None (a CORE test needs none)."""
    # surrogateescape: a few imported sources are not UTF-8; their bytes stay.
    with open(test, encoding="utf-8", errors="surrogateescape") as f:
        lines = [l for l in f.read().splitlines(keepends=True)
                 if not run.RECORDED.match(l)]
    if verdict:
        lines.append(f"// Vereda: verdict {verdict}\n")
    with open(test, "w", encoding="utf-8", errors="surrogateescape") as f:
        f.write("".join(lines))


# A header line set_mode replaces: a mode, or why a mode was set (not the
# recorded verdict, which record_verdict owns).
MODE_LINE = re.compile(r"^\s*//\s*(MODE:|Vereda: (?!verdict ))")


def set_mode(test, mode, why=()):
    """Sets the test's mode, with the reasons in why, in place of the mode
    and reasons it had, so a flipped test does not keep the old ones. CORE,
    the default, is written as no MODE line, and a mode that passes (CORE,
    THOROUGH) needs no reason."""
    with open(test, encoding="utf-8", errors="surrogateescape") as f:
        lines = [l for l in f.read().splitlines(keepends=True) if not MODE_LINE.match(l)]
    if mode in run.FAIL_MODES:
        lines += [f"// Vereda: {w[:150]}\n" for w in why]
    if mode != "CORE":
        lines.append(f"// MODE: {mode}\n")
    with open(test, "w", encoding="utf-8", errors="surrogateescape") as f:
        f.write("".join(lines))


def mark(tests, runner):
    """Runs each test with runner, the run.py command line without the test,
    and flips the mode of those that do not behave as their mode says; one
    that runs out of memory is listed instead. A test left KNOWNBUG or FUTURE
    gets its verdict recorded, which run.py then checks; one whose recorded
    verdict changed is listed, since this accepts it. Returns how many were
    flipped."""
    def check(test):
        return test, subprocess.run(runner + [test], stdout=subprocess.PIPE,
                                    stderr=subprocess.STDOUT, text=True, check=False,
                                    env={**os.environ, "REGRESS_REMARK": "1"})

    flipped = 0
    with concurrent.futures.ThreadPoolExecutor(os.cpu_count()) as pool:
        for test, proc in pool.map(check, tests):
            lines = proc.stdout.splitlines()
            now = next((l[len("verdict: "):] for l in reversed(lines)
                        if l.startswith("verdict: ")), None)
            if proc.returncode == run.OUT_OF_MEMORY:  # a failure to fix, not a mode
                print(f"out of memory: {test}")
                continue
            desc = run.parse(test)
            if now and desc["VERDICT"] not in (None, now) and desc["MODE"] in run.FAIL_MODES:
                print(f"verdict {desc['VERDICT']} -> {now}: {test}")
            if proc.returncode not in (0, run.SKIPPED):
                mode = "CORE" if desc["MODE"] in run.FAIL_MODES else "FUTURE"
                why = ([l for l in lines if l.startswith("unmet")] or lines[-1:])[:1]
                set_mode(test, mode, why)
                flipped += 1
            if now:  # a timeout tells nothing, so it keeps the earlier record
                record_verdict(test, now if run.parse(test)["MODE"] in run.FAIL_MODES else None)
    return flipped


def main():
    """Imports the suites named on the command line, or all of them."""
    here = os.path.dirname(os.path.abspath(__file__))
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("esbmc", help="ESBMC's regression directory")
    parser.add_argument("suites", nargs="*", help="suites to import (default: all but disabled)")
    parser.add_argument("--out", default=here, help="where suites go (default: tests/)")
    parser.add_argument("--mark", action="store_true", help="run each test and flip its mode "
                        "when Vereda disagrees; needs --bin and --clang")
    parser.add_argument("--bin", help="the vereda binary, for --mark")
    parser.add_argument("--clang", help="clang matching the binary, for --mark")
    parser.add_argument("--timeout", default="300", help="seconds per test, for --mark")
    parser.add_argument("--memory-limit", default="4096", help="MB per test, for --mark")
    args = parser.parse_args()
    if args.mark and not (args.bin and args.clang):
        parser.error("--mark needs --bin and --clang")

    copied_tests = import_suites(args)
    if args.mark:
        runner = [sys.executable, os.path.join(here, "run.py"),
                  "--vereda", os.path.join(os.path.dirname(here), "vereda.py"),
                  "--bin", os.path.abspath(args.bin), "--clang", os.path.abspath(args.clang),
                  "--timeout", args.timeout, "--memory-limit", args.memory_limit]
        print(f"{mark(copied_tests, runner)} of {len(copied_tests)} tests changed mode")
    return 0


def tracked_files(esbmc):
    """The real paths of the files git tracks under esbmc, or None when it is
    not a checkout. A working tree also has what earlier runs wrote."""
    git = subprocess.run(["git", "-C", esbmc, "ls-files", "-z", "--full-name"],
                         stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=False)
    if git.returncode != 0:
        return None
    top = subprocess.run(["git", "-C", esbmc, "rev-parse", "--show-toplevel"],
                         stdout=subprocess.PIPE, text=True, check=True).stdout.strip()
    return {os.path.realpath(os.path.join(top, p.decode())) for p in git.stdout.split(b"\0") if p}


def import_suites(args):
    """Copies the suites args names, printing a line per suite. Returns the
    main.c of each copied test."""
    suites = args.suites or sorted(d for d in os.listdir(args.esbmc) if d != "disabled"
                                   and os.path.isdir(os.path.join(args.esbmc, d)))
    tracked = tracked_files(args.esbmc)
    copied_tests = []
    for suite in suites:
        counts = {}
        for root, _, names in os.walk(os.path.join(args.esbmc, suite)):
            if "test.desc" not in names:
                continue
            rel = os.path.relpath(root, os.path.join(args.esbmc, suite))
            name = "root" if rel == "." else rel.replace(os.sep, "__")
            dest = os.path.join(args.out, suite_name(suite), name)
            result = import_test(root, dest, os.path.join(suite, rel), tracked)
            if result.endswith(".c"):
                copied_tests.append(result)
                result = "copied"
            counts[result] = counts.get(result, 0) + 1
        copied = counts.pop("copied", 0)
        counts.pop("not C", None)
        if copied or counts:
            skipped = "".join(f", {n} skipped: {why}" for why, n in sorted(counts.items()))
            print(f"{suite}: {copied} copied to {suite_name(suite)}{skipped}")
    return copied_tests


if __name__ == "__main__":
    sys.exit(main())
