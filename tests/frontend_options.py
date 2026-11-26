#!/usr/bin/env python3
"""C standards reach every C compiler invocation and stay out of the verifier."""
from pathlib import Path
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
