#!/usr/bin/env python3
"""Front-end vereda.py: compiles C or Rust to LLVM bitcode and runs the
verifier binary on it. Only front-end flags are defined here; every other
argument is forwarded to the binary unchanged, so each flag lives in one
place. Inputs are recognised by extension (.c .i .rs .ll .bc)."""
import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
SOURCE_EXTS = (".c", ".i", ".rs")
# A linemarker, '# 12 "foo.c" 1' or '#line 12 "foo.c"', as gcc -E and clang -E
# write them.
LINEMARKER = re.compile(rb"^[ \t]*#[ \t]*(?:line[ \t]+)?[0-9]+\b.*$", re.MULTILINE)
IR_EXTS = (".ll", ".bc")

CLANG_FLAGS = [
    "-g", "-O0", "-emit-llvm", "-c",
    # C11 lets the compiler assume loops without side effects terminate;
    # clang would mark them mustprogress and the binary rejects that.
    "-fno-finite-loops",
    # The names clang gives blocks (if.then, while.end) tell which source
    # condition a branch decides and which way, for the witness's branching
    # waypoints.
    "-fno-discard-value-names",
    # Old C, as SV-COMP's CIL and kernel sources are: clang 15 and later make
    # these hard errors. ESBMC relaxes the same set (clang_c_language.cpp).
    "-Wno-error=implicit-function-declaration",
    "-Wno-int-conversion",
    "-Wno-incompatible-pointer-types",
    "-Wno-incompatible-function-pointer-types",
    "-Wno-unknown-attributes",
    "-fbracket-depth=1024",
]

# ESBMC's flags for the checks clang's sanitizers insert. They trap into
# llvm.ubsantrap, which the binary takes as a failed check.
SANITIZERS = {
    "--overflow-check": "signed-integer-overflow",
    "--unsigned-overflow-check": "unsigned-integer-overflow",
    "--ub-shift-check": "shift",
}
# SV-COMP property formula -> (the binary's --property, sanitizers). Only
# signed overflow: SV-COMP's no-overflow is a signed result out of range,
# and clang's shift checks also flag negative operands, which are not.
PROPERTIES = {
    "G ! call(reach_error())": ("unreach-call", []),
    "G ! overflow": ("no-overflow", ["signed-integer-overflow"]),
}
CHECK = re.compile(r"CHECK\(\s*init\((\w+)\(\)\)\s*,\s*LTL\((.*)\)\s*\)")


# Exit code of an error rather than a verdict, as the binary and ESBMC use;
# 1 is VERIFICATION FAILED only.
ERROR_EXIT = 6


def fail(message):
    print(message, file=sys.stderr)
    sys.exit(ERROR_EXIT)


def read_property(path):
    """(entry function, the binary's --property, sanitizers) for an SV-COMP
    property file. Exits on a property the binary cannot check."""
    with open(path, encoding="utf-8") as f:
        checks = [CHECK.fullmatch(line.strip()) for line in f if line.strip()]
    formula = " ".join(checks[0].group(2).split()) if len(checks) == 1 and checks[0] else None
    if formula not in PROPERTIES:
        fail(f"vereda.py: unsupported property in {path}; "
             "supported: unreach-call, no-overflow")
    return (checks[0].group(1),) + PROPERTIES[formula]


def find_binary():
    for candidate in (os.path.join(HERE, "vereda"),
                      os.path.join(HERE, "build", "vereda"),
                      shutil.which("vereda")):
        if candidate and os.access(candidate, os.X_OK):
            return candidate
    fail("vereda.py: verifier binary not found; pass --bin")


def find_rustc(binary):
    """Compiler selected by CMake, or rustc on PATH for an unpacked tool."""
    path = os.path.join(os.path.dirname(os.path.abspath(binary)), "rustc-path.txt")
    if os.path.isfile(path):
        with open(path, encoding="utf-8") as f:
            return f.read().strip()
    return "rustc"


def find_clang(binary):
    """Prefer the compiler belonging to the LLVM selected at configure time."""
    binary = shutil.which(binary) or binary
    path = os.path.join(os.path.dirname(os.path.abspath(binary)), "clang-path.txt")
    if os.path.isfile(path):
        with open(path, encoding="utf-8") as f:
            return f.read().strip()
    return "clang"


def check_clang(clang, banner):
    expected = re.search(r"\bLLVM (\d+)\.", banner)
    try:
        version = subprocess.run([clang, "--version"], capture_output=True, text=True, check=False)
    except OSError as error:
        fail(f"vereda.py: cannot run clang {clang}: {error}; pass --clang <path> or set CLANG")
    actual = re.search(r"\bclang version (\d+)\.", version.stdout)
    if version.returncode or not actual or not expected or actual[1] != expected[1]:
        required = expected[1] if expected else "the verifier's LLVM version"
        found = actual[1] if actual else "unknown"
        fail(f"vereda.py: incompatible clang {clang} (version {found}); needs clang {required} "
             "matching the verifier; pass --clang <path> or set CLANG")


def physical_lines(source, tmp):
    """The file to compile for a .i source, and the clang flags it needs.
    clang takes a linemarker's file and line as the location of what follows,
    but a witness must name the input's own lines, so a .i that holds
    linemarkers is compiled from a copy in tmp with each one blanked (the
    other lines keep their numbers); the prefix map then names the source in
    the debug info, exactly as given."""
    if not source.endswith(".i"):
        return source, []
    with open(source, "rb") as f:
        text = f.read()
    stripped, count = LINEMARKER.subn(b"", text)
    if not count:
        return source, []
    copy = os.path.join(tmp, os.path.basename(source))
    with open(copy, "wb") as f:
        f.write(stripped)
    return copy, ["-fdebug-prefix-map=" + copy + "=" + source]


def compile_and_link(args, flags, inputs, tmp, run):
    """Compiles C with clang and Rust with rustc, and links several modules with
    llvm-link. Returns the IR file to verify, or None on failure."""
    tag = "\033[36m[PROGRESS]\033[0m" if sys.stdout.isatty() else "[PROGRESS]"
    ext = ".ll" if args.keep_ir else ".bc"
    textual = ["-S"] if args.keep_ir else []
    irs = []
    for source in inputs:
        if not source.endswith(SOURCE_EXTS):
            irs.append(source)
            continue
        print(tag, "Compiling", source)
        ir = os.path.join(tmp, os.path.basename(source) + ext)
        if source.endswith(".rs"):
            cmd = [args.rustc, "--edition=2024", "--crate-type=bin", "-C", "panic=abort",
                   "-C", "opt-level=0", "-C", "overflow-checks=on", "-g",
                   "--emit=" + ("llvm-ir" if args.keep_ir else "llvm-bc"), source, "-o", ir]
        else:
            compiled, mapping = physical_lines(source, tmp)
            cmd = ([args.clang] + CLANG_FLAGS + flags + mapping + ["-I" + d for d in args.I]
                   + ["-D" + d for d in args.D] + textual + [compiled, "-o", ir])
        if run(cmd) != 0:
            return None
        irs.append(ir)
    if len(irs) == 1:
        return irs[0]
    print(tag, "Linking", len(irs), "modules")
    ir = os.path.join(tmp, "linked" + ext)
    resolved_clang = shutil.which(args.clang) or args.clang
    llvm_link = os.path.join(os.path.dirname(os.path.realpath(resolved_clang)), "llvm-link")
    if run([llvm_link] + textual + irs + ["-o", ir]) != 0:
        return None
    return ir


def main():
    # No abbreviations: --property, the binary's, is not --property-file.
    parser = argparse.ArgumentParser(add_help=False, description=__doc__, allow_abbrev=False)
    parser.add_argument("-h", "--help", action="store_true")
    parser.add_argument("--bin", help="verifier binary (default: next to this script or build/)")
    parser.add_argument("--clang", default=os.environ.get("CLANG"),
                        help="clang matching the binary's LLVM (default: $CLANG, CMake's compiler, or clang)")
    parser.add_argument("--rustc", default=os.environ.get("RUSTC"),
                        help="rustc 1.100.0 (default: $RUSTC, CMake's compiler, or rustc)")
    parser.add_argument("-m32", action="store_true", help="32-bit target, forwarded to clang")
    parser.add_argument("-m64", action="store_true", help="64-bit target, forwarded to clang")
    parser.add_argument("--std", "-std", help="C language standard, forwarded to clang")
    parser.add_argument("-I", action="append", default=[], help="include directory for clang")
    parser.add_argument("-D", action="append", default=[], help="macro definition for clang")
    parser.add_argument("--keep-ir", action="store_true",
                        help="write the IR handed to the binary next to the input as <input>.ll")
    parser.add_argument("--property-file",
                        help="SV-COMP property file: unreach-call or no-overflow")
    for flag, check in SANITIZERS.items():
        parser.add_argument(flag, dest="sanitize", action="append_const", const=check,
                            default=[], help=f"check with clang's -fsanitize={check}")
    parser.add_argument("-v", "--verbose", action="store_true", help="echo every command run")

    # argparse exits with 2, the UNKNOWN code, on a bad option.
    def error(message):
        parser.print_usage(sys.stderr)
        fail(f"{parser.prog}: error: {message}")
    parser.error = error
    args, rest = parser.parse_known_args()
    flags = ["-m32"] * args.m32 + ["-m64"] * args.m64
    if args.std is not None:
        flags.append("-std=" + args.std)
    if args.property_file:
        entry, prop, sanitize = read_property(args.property_file)
        args.sanitize += sanitize
        rest += ["--entry", entry, "--property", prop]
        # SV-COMP's expected verdicts, its validators and ESBMC round a * b and
        # the + c apart, as x86-64 without FMA runs it; clang's default
        # -ffp-contract=on lets a * b + c fuse into one rounding. An explicit
        # fma() still fuses.
        flags.append("-ffp-contract=off")
        # As ESBMC's SV-COMP runs (esbmc-wrapper.py): no largest bound, so the
        # time limit ends the run, and a step of 2. A step of 8 answered more
        # of the tasks that stopped at bound 50 but skipped the small bounds
        # where others fail (docs/survey/2026-11-24-k-step-2.md).
        if "--incremental-bmc" in rest and not {"--max-k-step", "--unlimited-k-steps"} & set(rest):
            rest += ["--unlimited-k-steps"] + ([] if "--k-step" in rest else ["--k-step", "2"])
    if args.sanitize:
        flags += [f + ",".join(sorted(set(args.sanitize)))
                  for f in ("-fsanitize=", "-fsanitize-trap=")]
        # Since clang 22, a trap's location is line 0 of an artificial
        # function inlined at the check, which names the reason; without it,
        # the trap is at the check itself.
        flags.append("-fsanitize-debug-trap-reasons=none")

    binary = args.bin or find_binary()
    if args.help:
        parser.print_help()
        print("\nBinary options:\n")
        sys.stdout.flush()
        return subprocess.call([binary, "--help"])

    inputs = [a for a in rest if a.endswith(SOURCE_EXTS + IR_EXTS)]
    forwarded = [a for a in rest if a not in inputs]
    if not inputs and "--version" in forwarded:
        return subprocess.call([binary, "--version"])
    if not inputs:
        fail("vereda.py: no input file (.c .i .rs .ll .bc)")
    if any(i.endswith(".rs") for i in inputs):
        if args.std is not None:
            fail("vereda.py: --std selects a C language standard and does not apply to Rust")
        if args.m32:
            fail("vereda.py: Rust source currently supports the host 64-bit target only")
        if args.sanitize or args.property_file or args.I or args.D:
            fail("vereda.py: Rust source does not support C sanitizer, property-file, -I or -D flags")
        args.rustc = args.rustc or find_rustc(binary)
        version = subprocess.run([args.rustc, "--version"], capture_output=True, text=True,
                                 check=False)
        if version.returncode or not version.stdout.startswith("rustc 1.100.0 "):
            fail("vereda.py: Rust input needs rustc 1.100.0; pass --rustc or reconfigure CMake")
    if args.std is not None and not any(i.endswith((".c", ".i")) for i in inputs):
        fail("vereda.py: --std needs C inputs, not .ll or .bc")
    if any(f.startswith("-fsanitize=") for f in flags) and any(i.endswith(IR_EXTS) for i in inputs):
        fail("vereda.py: clang compiles the overflow checks in, so they need C inputs, "
             "not .ll or .bc")

    # Clang can fold a literal zero division before emitting IR. The binary
    # validates property names; inspect its selection here only for instrumentation.
    selection = argparse.ArgumentParser(add_help=False, allow_abbrev=False)
    selection.add_argument("--property", default="all")
    selection.error = error
    selected, _ = selection.parse_known_args(forwarded)
    no_divisor = False
    for flag in forwarded:
        name, eq, value = flag.partition("=")
        if name == "--no-div-by-zero-check":
            no_divisor = not eq or value in ("", "true", "1")
    if any(i.endswith((".c", ".i")) for i in inputs) and selected.property == "all" and not no_divisor:
        # The nonreturning handler retains the operands, unlike ubsantrap's
        # shared code for zero division and signed division overflow.
        flags += ["-fsanitize=integer-divide-by-zero", "-fno-sanitize-recover=integer-divide-by-zero",
                  "-fsanitize-debug-trap-reasons=none"]

    def run(cmd):
        if args.verbose:
            print("+", " ".join(cmd), file=sys.stderr)
        sys.stdout.flush()
        return subprocess.call(cmd)

    version = subprocess.run([binary, "--version"], capture_output=True, text=True, check=False)
    print(version.stdout, end="", flush=True)
    if version.stderr:
        print(version.stderr, end="", file=sys.stderr)
    if version.returncode != 0:
        return ERROR_EXIT
    args.clang = args.clang or find_clang(binary)
    if any(i.endswith((".c", ".i")) for i in inputs):
        check_clang(args.clang, version.stdout)
    with tempfile.TemporaryDirectory() as tmp:
        ir = compile_and_link(args, flags, inputs, tmp, run)
        if ir is None:
            return ERROR_EXIT
        if args.keep_ir:
            shutil.copy(ir, inputs[0] + ".ll")
        return run([binary, "--no-banner"] + forwarded + [ir])


if __name__ == "__main__":
    try:
        sys.exit(main())
    except OSError as e:
        # A clang, llvm-link or binary that cannot be run, or an input that
        # cannot be read: an error, not the traceback's exit code 1 (FAILED).
        fail(f"vereda.py: {e}")
