#!/usr/bin/env python3
"""Libc assertion failures end paths, remain checks in summaries and do not capture strings."""
from pathlib import Path
import re
import subprocess
import sys
import tempfile

frontend, binary, clang = sys.argv[1:]
variants = {
    "__assert_fail": ["ptr", "ptr", "i32", "ptr"],
    "__assert_rtn": ["ptr", "ptr", "i32", "ptr"],
    "_wassert": ["ptr", "ptr", "i32"],
    "__assert": ["ptr", "i32", "ptr"],
    "__assert_func": ["ptr", "i32", "ptr", "ptr"],
    "__assert_perror_fail": ["i32", "ptr", "i32", "ptr"],
}

with tempfile.TemporaryDirectory() as directory:
    source = Path(directory) / "assert.ll"

    def check(flags, status, text):
        p = subprocess.run([sys.executable, frontend, "--bin", binary, "--clang", clang,
                            str(source)] + flags, capture_output=True, text=True, timeout=30)
        out = p.stdout + p.stderr
        assert p.returncode == status, out
        assert text in out, out
        return out

    cases = list(variants.items()) + [("__assert", ["ptr", "ptr", "i32", "ptr"])]
    for name, types in cases:
        declaration = f"declare void @{name}({', '.join(types)})\n"
        args = ", ".join(ty + (" @message" if ty == "ptr" else " 1") for ty in types)
        call = f"call void @{name}({args})\n"
        prefix = '@message = private constant [2 x i8] c"x\\00"\n' + declaration
        # No noreturn attribute: the walker must enforce termination itself.
        source.write_text(prefix + "define i32 @main() {\n" + call +
                          " %unused = udiv i32 1, 0\n ret i32 0\n}\n")
        for flags in ([], ["--multi-property"]):
            out = check(flags, 1, name)
            assert "division by zero" not in out, out
        check(["--no-assertions"], 0, "VERIFICATION SUCCESSFUL")
        check(["--property", "no-overflow"], 0, "VERIFICATION SUCCESSFUL")
        ir = check(["--dump-ir-only"], 0, "captures(none)")
        decl = next(line for line in ir.splitlines() if line.startswith("declare") and
                    "@" + name + "(" in line)
        assert len(re.findall(r"ptr captures\(none\)", decl)) == types.count("ptr"), decl

        source.write_text(prefix + '@a = global [256 x i32] zeroinitializer\n'
                          'define i32 @main() {\nentry:\n br label %loop\nloop:\n'
                          ' %i = phi i32 [0, %entry], [%next, %store]\n'
                          ' %more = icmp ult i32 %i, 256\n'
                          ' br i1 %more, label %body, label %exit\nbody:\n'
                          ' %bad = icmp eq i32 %i, 2\n br i1 %bad, label %fail, label %store\n'
                          'fail:\n' + call + ' unreachable\nstore:\n'
                          ' %p = getelementptr [256 x i32], ptr @a, i32 0, i32 %i\n'
                          ' store i32 %i, ptr %p\n %next = add i32 %i, 1\n'
                          ' br label %loop\nexit:\n ret i32 0\n}\n')
        # The slice would drop the store into @a, which nothing reads.
        check(["--no-slice"], 1, "Summarised 1 array loop(s)")

    # A similarly named function with the wrong arity is an ordinary call.
    source.write_text('declare void @_wassert(ptr)\ndefine i32 @main() {\n'
                      ' call void @_wassert(ptr null)\n %x = udiv i32 1, 0\n ret i32 0\n}\n')
    check([], 1, "division by zero")
