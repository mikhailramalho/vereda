#!/usr/bin/env python3
"""C standards reach every C compiler invocation and stay out of the verifier."""
from pathlib import Path
import os
import subprocess
import sys
import tempfile

frontend, binary, clang = sys.argv[1:]
command = [sys.executable, frontend, "--bin", binary, "--clang", clang]


def check(args, status, message):
    result = subprocess.run(command + args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            text=True, check=False, timeout=30)
    assert result.returncode == status, result.stdout
    assert message in result.stdout, result.stdout
    return result.stdout


with tempfile.TemporaryDirectory() as directory:
    tmp = Path(directory)
    main, helper = tmp / "main.c", tmp / "helper.c"
    requires_c11 = "#if __STDC_VERSION__ != 201112L\n#error Wrong C standard\n#endif\n"
    main.write_text(requires_c11 + "extern int helper(void);\n"
                    "int main(void) { __VERIFIER_assert(helper() == 7); return 0; }\n")
    helper.write_text(requires_c11 + "int helper(void) { return 7; }\n")
    check(["--std", "c11", str(main), str(helper)], 0, "VERIFICATION SUCCESSFUL")

    ir = tmp / "helper.ll"
    ir.write_text("define i32 @helper() { ret i32 7 }\n")
    check(["--std=c11", str(main), str(ir)], 0, "VERIFICATION SUCCESSFUL")
    check(["--std", "c11", str(ir)], 6, "--std needs C inputs")
    check([str(main), "--std"], 6, "expected one argument")
    check(["--std=invalid-standard", str(main)], 6, "invalid-standard")
    check(["--std=", str(main)], 6, "invalid value")

    # A repeated option selects the last standard, as clang does.
    check(["--std", "gnu89", "--std=c11", str(main), str(helper)],
          0, "VERIFICATION SUCCESSFUL")
    rust = tmp / "main.rs"
    rust.write_text("fn main() {}\n")
    check(["--std=c11", "--rustc", str(tmp / "missing-rustc"), str(rust)],
          6, "does not apply to Rust")
    for flag in ("--overflow-check", "--ub-shift-check=false"):
        check([flag, "--rustc", str(tmp / "missing-rustc"), str(rust)],
              6, "Rust source does not support C sanitizer")

    # CMake's compiler wins over PATH, while explicit overrides remain authoritative.
    selected_binary = tmp / "vereda"
    selected_binary.symlink_to(Path(binary).resolve())
    configured = tmp / "clang-path.txt"
    configured.write_text(str(Path(clang).resolve()) + "\n")
    path_dir = tmp / "bin"
    path_dir.mkdir()
    old_clang = path_dir / "clang"
    old_clang.write_text('#!/bin/sh\nif [ "$1" = --version ]; then\n'
                         'echo "clang version 21.0.0"\nelse\n'
                         'echo "unexpected compilation"\nexit 1\nfi\n')
    old_clang.chmod(0o755)
    env = {**os.environ, "PATH": str(path_dir) + os.pathsep + os.environ["PATH"]}
    env.pop("CLANG", None)
    base = [sys.executable, frontend, "--bin", str(selected_binary)]

    def selection(extra, environment, status, message):
        result = subprocess.run(base + extra, env=environment, capture_output=True,
                                text=True, check=False, timeout=30)
        output = result.stdout + result.stderr
        assert result.returncode == status, output
        assert message in output, output
        assert "unexpected compilation" not in output, output
        return output

    sources = ["--std=c11", str(main), str(helper)]
    selection(sources, env, 0, "VERIFICATION SUCCESSFUL")
    output = selection(["--clang", str(old_clang)] + sources, env, 6, "needs clang 23")
    assert "pass --clang <path> or set CLANG" in output, output
    overridden = {**env, "CLANG": str(old_clang)}
    selection(sources, overridden, 6, "incompatible clang")
    selection(["--clang", str(tmp / "missing-clang")] + sources,
              env, 6, "pass --clang <path> or set CLANG")
    selection(["--clang", clang] + sources, overridden, 0, "VERIFICATION SUCCESSFUL")
    configured.unlink()
    selection(sources, env, 6, "incompatible clang")
    # PATH-selected clang must also select llvm-link beside that compiler.
    old_clang.unlink()
    old_clang.symlink_to(Path(clang).resolve())
    selection(sources, env, 0, "VERIFICATION SUCCESSFUL")
    old_clang.unlink()
    plain_ir = tmp / "plain.ll"
    plain_ir.write_text("define i32 @main() { ret i32 0 }\n")
    selection([str(plain_ir)], env, 0, "VERIFICATION SUCCESSFUL")
