# Displaying a function without a source location

Recorded 2026-11-25.

Library models have no debug information, so their loop-progress lines showed
`file <unknown> line 0 function strcmp`. SourceLoc::display now omits the file,
line and column when the file is empty or unknown, leaving the demangled
function name. Known source locations retain their existing spelling.
SourceLoc::str, used for internal names, is unchanged.

A mixed C/IR regression exercises both a source loop and a library-style loop
without debug information. It fails against the old binary on the unknown
location text and checks that the function is still demangled.

The LLVM 23 Release build passes the Vereda suite: 403 tests, with the
existing lib_sprintf_overflow skip. The imported ESBMC suite was not run.
