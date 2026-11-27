#!/usr/bin/env python3
"""Check defaults, independent opt-outs, and unsupported check diagnostics."""
from pathlib import Path
import subprocess
import sys
import tempfile

frontend, binary, clang = sys.argv[1:]
command = [sys.executable, frontend, "--bin", binary, "--clang", clang]


def check(command, status, message):
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            text=True, check=False, timeout=30)
    assert result.returncode == status, result.stdout
    assert message in result.stdout, result.stdout
    return result.stdout


with tempfile.TemporaryDirectory() as directory:
    tmp = Path(directory)
    source = tmp / "main.c"
    ir = tmp / "main.ll"
    ir.write_text("define i32 @main() { ret i32 0 }\n")
    for flag in ("--memory-leak-check", "--ub-data-race-check", "--deadlock-check"):
        for prefix in ([binary], command):
            output = check(prefix + [str(ir), flag], 6, "is not implemented")
            assert "VERIFICATION" not in output, output
        # Reject the request before trying to load even a nonexistent input.
        check([binary, str(tmp / "missing.ll"), flag], 6, "is not implemented")

    source.write_text("#include <assert.h>\nint main(void) { assert(0); }\n")
    check(command + [str(source)], 1, "__assert_fail")
    check(command + [str(source), "--no-assertions"], 0, "VERIFICATION SUCCESSFUL")
    source.write_text("extern void __VERIFIER_assert(int);\n"
                      "int main(void) { __VERIFIER_assert(0); }\n")
    check(command + [str(source), "--no-assertions"], 0, "VERIFICATION SUCCESSFUL")

    # Disabling assertions keeps pointer checks, and vice versa.
    source.write_text('int main(void) { volatile char *p = "x"; *p = 1; }\n')
    check(command + [str(source), "--no-assertions"], 1, "write access to string constant")
    check(command + [str(source), "--ub-pointer-check=false"], 0, "VERIFICATION SUCCESSFUL")

    # The [INFO] Checks line names the checks the run counts, and only those:
    # a property counts its own and the unwinding assertions.
    source.write_text("int main(void) { return 0; }\n")
    overflow = tmp / "overflow.prp"
    overflow.write_text("CHECK( init(main()), LTL(G ! overflow) )\n")
    defaults = "assertions, unwinding-assertions, pointer, div-by-zero, vla-size"
    for flags, families in (
            ([], defaults),
            (["--no-assertions", "--ub-pointer-check=false", "--vla-size-check=false"],
             "unwinding-assertions, div-by-zero"),
            (["--ub-shift-check", "--overflow-check", "--ub-uninit-check", "--ub-alignment-check"],
             defaults + ", shift, signed-overflow, unsigned-overflow, alignment, uninit"),
            (["--ub-float-cast-overflow-check", "--ub-count-zero-check"],
             defaults + ", float-cast-overflow, count-zero"),
            (["--property", "unreach-call", "--ub-shift-check"], "reach-error, unwinding-assertions"),
            (["--property", "valid-free"], "unwinding-assertions, valid-free"),
            (["--property-file", str(overflow)], "unwinding-assertions, signed-overflow"),
            (["--property", "termination"], "unwinding-assertions")):
        check(command + [str(source)] + flags, 0, "[INFO] Checks: " + families + "\n")
    ir.write_text("define i32 @main() { ret i32 0 }\n")
    check([binary, str(ir), "--no-assertions", "--no-unwinding-assertions",
           "--ub-pointer-check=false", "--ub-div-by-zero-check=false", "--vla-size-check=false"],
          0, "[INFO] Checks: none\n")
    source.write_text("#include <assert.h>\nint main(void) { assert(0); }\n")
    check(command + [str(source), "--ub-pointer-check=false"], 1, "__assert_fail")

    # This unused load/division result is removed by IR preparation without a check.
    source.write_text("int main(int n, char **argv) { int unused = 7 / n; return 0; }\n")
    for flags in ([], ["-m32"], ["--no-assertions"], ["--incremental-solver"], ["--incremental-bmc"]):
        check(command + [str(source)] + flags, 1, "division by zero")
    check(command + [str(source), "--ub-div-by-zero-check=false"], 0, "VERIFICATION SUCCESSFUL")

    # Safe compiler checks leave neither a handler nor diagnostic address constraints.
    source.write_text("extern void __VERIFIER_assert(int);\n"
                      "int main(int n, char **argv) { int d = 1; __VERIFIER_assert(n / d == n); }\n")
    prepared = check(command + [str(source), "--dump-ir-only"], 0, "define")
    assert "__ubsan_handle_divrem_overflow_abort" not in prepared, prepared
    assert "@.src" not in prepared, prepared
    output = check(command + [str(source), "--dump-smt-only"], 0, "set-logic")
    assert "base#" not in output, output

    source.write_text("int main(int n, char **argv) { int unused = 7 / n; return 0; }\n")
    check(command + [str(source), "--ub-div-by-zero-check=true"], 1, "division by zero")
    for flags, status in ((["--ub-div-by-zero-check=false", "--ub-div-by-zero-check"], 1),
                          (["--ub-div-by-zero-check", "--ub-div-by-zero-check=false"], 0)):
        check(command + [str(source)] + flags, status, "VERIFICATION")
        source_ir = tmp / "order.ll"
        source_ir.write_text("define i32 @main(i32 %n) {\n %unused = sdiv i32 7, %n\n ret i32 0\n}\n")
        check([binary, str(source_ir)] + flags, status, "VERIFICATION")
    output = check(command + [str(source)], 1, "[main.division-by-zero.1]")
    assert "main.assertion." not in output and "division-by-zero.2" not in output, output
    assert "Properties: 1 checked" in output, output
    prepared = check(command + [str(source), "--dump-ir-only"], 0,
                     "__ubsan_handle_divrem_overflow_abort")
    assert "__vereda_check_divisor" not in prepared, prepared
    for integer, flags in (("long long", []), ("long long", ["-m32"]), ("__int128", []),
                           ("_BitInt(12)", []), ("_BitInt(129)", [])):
        source.write_text(f"int main(int n, char **p) {{ {integer} d = n; return (int)(7 / d); }}\n")
        output = check(command + [str(source)] + flags, 1, "[main.division-by-zero.1]")
        assert "arithmetic-overflow." not in output, output
    source.write_text("int main(void) { int unused = 1 / 0; return 0; }\n")
    output = check(command + [str(source)], 1, "[main.division-by-zero.1]")
    assert "main.assertion." not in output, output
    check(command + [str(source), "--ub-div-by-zero-check=false"], 0, "VERIFICATION SUCCESSFUL")

    for operation in ("sdiv", "udiv", "srem", "urem"):
        ir.write_text(f"define i32 @main(i32 %n) {{\n %unused = {operation} i32 7, %n\n ret i32 0\n}}\n")
        check([binary, str(ir)], 1, "division by zero")
        check([binary, str(ir), "--ub-div-by-zero-check=false"], 0, "VERIFICATION SUCCESSFUL")
    ir.write_text("define i32 @main() {\n"
                  " %unused = udiv <2 x i32> <i32 4, i32 8>, <i32 2, i32 0>\n ret i32 0\n}\n")
    check([binary, str(ir)], 1, "division by zero")

    ir.write_text("declare void @__VERIFIER_assert(i32)\n"
                  "define i32 @main() {\n call void @__VERIFIER_assert(i32 0)\n"
                  " %unused = sdiv i32 7, 0\n ret i32 0\n}\n")
    output = check([binary, str(ir), "--multi-property"], 1, "division by zero")
    assert "__VERIFIER_assert" in output, output
    # Cleaning a dead handler must preserve the other side of a symbolic branch.
    ir.write_text("declare void @__VERIFIER_error() noreturn\n"
                  "declare void @__ubsan_handle_divrem_overflow_abort(ptr, i64, i64) noreturn\n"
                  "define i32 @main(i1 %which) {\n"
                  " br i1 %which, label %bad, label %safe\n"
                  "bad:\n call void @__VERIFIER_error()\n unreachable\n"
                  "safe:\n br i1 true, label %done, label %dead\n"
                  "dead:\n call void @__ubsan_handle_divrem_overflow_abort(ptr null, i64 7, i64 1)\n"
                  " unreachable\ndone:\n ret i32 0\n}\n")
    check([binary, str(ir)], 1, "__VERIFIER_error")
    ir.write_text('@type = constant {i16, i16, [4 x i8]} {i16 0, i16 13, [4 x i8] c"i64\\00"}\n'
                  '@data = constant {{ptr, i32, i32}, ptr} {{ptr, i32, i32} zeroinitializer, ptr @type}\n'
                  "declare void @__ubsan_handle_divrem_overflow_abort(ptr, i64, i64) noreturn\n"
                  "define i32 @main(i64 %n) {\n"
                  " call void @__ubsan_handle_divrem_overflow_abort(ptr @data, i64 0, i64 %n)\n"
                  " unreachable\n}\n")
    output = check([binary, str(ir), "--multi-property"], 1, "[main.division-by-zero.1]")
    assert "[main.arithmetic-overflow.1]" in output, output

    source.write_text("#include <assert.h>\nint main(void) {\n"
                      " assert(0); volatile int n = 0; return 7 / n; }\n")
    # Preserve the preceding assertion even though continuation ends at zero division.
    check(command + [str(source)], 1, "__assert_fail")
    source.write_text("extern void __VERIFIER_assert(int);\nint main(void) {\n"
                      " __VERIFIER_assert(0); volatile int n = 0; return 7 / n; }\n")
    output = check(command + [str(source), "--multi-property"], 1, "division by zero")
    assert "__VERIFIER_assert" in output, output

    source.write_text("#include <limits.h>\nint main(int n, char **argv) {\n"
                      " if (n == INT_MAX) return n + 1; return 0; }\n")
    check(command + [str(source)], 0, "VERIFICATION SUCCESSFUL")
    check(command + [str(source), "--ub-signed-overflow-check", "--no-assertions"],
          1, "arithmetic overflow on add")
    check(command + [str(source), "--unsigned-overflow-check"], 0, "VERIFICATION SUCCESSFUL")
    check(command + [str(source), "--overflow-check"], 1, "arithmetic overflow on add")
    check(command + [str(source), "--overflow-check", "--ub-signed-overflow-check=false"],
          0, "VERIFICATION SUCCESSFUL")
    check(command + [str(source), "--ub-signed-overflow-check=maybe"], 6, "takes true or false")
    source.write_text("#include <limits.h>\n"
                      "int main(void) { return INT_MIN / -1; }\n")
    output = check(command + [str(source), "--overflow-check"], 1, "VERIFICATION FAILED")
    assert "division-by-zero." not in output, output
    source.write_text("#include <limits.h>\nint main(int n, char **argv) {\n"
                      " if (!n) return 7 / n; return INT_MIN / -1; }\n")
    output = check(command + [str(source), "--overflow-check", "--multi-property"],
                   1, "[main.division-by-zero.1]")
    assert "division-by-zero.2" not in output, output
    source.write_text("int main(int n, char **argv) { return (unsigned)n + 1u; }\n")
    check(command + [str(source)], 0, "VERIFICATION SUCCESSFUL")
    check(command + [str(source), "--unsigned-overflow-check"], 1, "arithmetic overflow on add")
    check(command + [str(source), "--overflow-check"], 1, "arithmetic overflow on add")
    check(command + [str(source), "--ub-signed-overflow-check"], 0, "VERIFICATION SUCCESSFUL")
    source.write_text("int main(int n, char **argv) { return 1 << n; }\n")
    check(command + [str(source)], 0, "VERIFICATION SUCCESSFUL")
    check(command + [str(source), "--ub-shift-check"], 1, "undefined behavior on shift operation shl")
    check(command + [str(source), "--ub-shift-check=false"], 0, "VERIFICATION SUCCESSFUL")
    check(command + [str(source), "--overflow-check"], 0, "VERIFICATION SUCCESSFUL")

    source.write_text("extern void reach_error(void);\n"
                      "int main(int n, char **argv) { int x = 7 / n; reach_error(); return x; }\n")
    prop = tmp / "unreach.prp"
    prop.write_text("CHECK( init(main()), LTL(G ! call(reach_error())) )\n")
    check(command + [str(source), "--property-file", str(prop)], 1, "FALSE_REACH")
    check(command + [str(source), "--property-file", str(prop), "--no-assertions"], 6, "conflicts")
    ir.write_text("define i32 @main() { ret i32 0 }\n")
    check([binary, str(ir), "--property=unreach-call", "--no-assertions"], 6, "conflicts")
    check([binary, str(ir), "--property=valid-free", "--ub-pointer-check=false"], 6, "conflicts")
    check([binary, str(ir), "--property=termination", "--no-unwinding-assertions"], 6, "conflicts")

    # A divisor that becomes constant after mem2reg must not block summaries.
    source.write_text("unsigned a[1024];\nint main(void) { unsigned d = 2;\n"
                      " for (unsigned i = 0; i < 1024; ++i) a[i] = i / d; return 0; }\n")
    for property_name in ("unreach-call", "no-overflow"):
        flags = ["--property", property_name]
        output = check(command + [str(source)] + flags, 0, "Summarised 1 array loop(s)")
        assert "VERIFICATION SUCCESSFUL" in output, output
        prepared = check(command + [str(source), "--dump-ir-only"] + flags, 0, "define")
        assert "__vereda_check_divisor" not in prepared, prepared

    # User assertion selection must not disable completeness checks.
    source.write_text("int main(int n, char **argv) { while (n > 0) { --n; } }\n")
    check(command + [str(source), "--no-assertions", "--unwind", "1", "--no-summarize-array-loops"],
          1, "unwinding assertion")
