# The banner before compilation, with the build revision

Recorded 2026-11-25.

Moving the banner into the binary saved a process startup but put the header
after compiler warnings and linking progress. The front end now queries the
binary's version before compiling and tells the verification invocation not
to repeat the banner. The query is not echoed ahead of the banner under
verbose mode. Its failure stops the run with error code 6. The version printer
flushes its stream before later diagnostics can write to stderr.

The banner includes `commit <short hash>` immediately after the camada
version. A build target runs a small CMake script to read HEAD and generate a
header, rewritten only when its contents change. A new commit followed by an
ordinary build therefore updates the banner without a manual reconfigure;
only the driver needs recompiling. Builds without Git metadata use unknown.
The hash comes from the executable's build, not the front end's checkout.

The extra version-query startup is intentional: the banner must precede
compilation, including failed compiles, and must describe the selected binary.
Direct binary execution still prints its own banner. The internal no-banner
flag is hidden from normal help output.

The regression exercises compiler warnings, two linked sources, verbose mode,
a failed compile, IR input, direct binary execution, an invalid IR file, and
the front end's version command. Each must print exactly one banner first.
The version script was also checked without a Git repository and for stable
header timestamps when HEAD does not change.

The LLVM 23 Release build passes all 459 Vereda tests, with the existing
lib_sprintf_overflow skip. The three-file Picosat compilation was also checked
through dump-ir-only: its single banner precedes the undeclared-function
warning and every compilation/linking progress line. The imported ESBMC suite
was not rerun.
