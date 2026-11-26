# Cross-platform libc assertion failures

Use a shared name-and-arity predicate for __assert_fail, Darwin __assert_rtn,
Windows _wassert, the three- and four-argument BSD __assert forms, newlib
__assert_func and glibc __assert_perror_fail. The helper uses std::string_view
and lives in util so it introduces no LLVM dependency outside the permitted
stages. It checks exact names and conventional arities rather than prefixes.

Use that predicate consistently in IR preparation, loop analysis and walking.
Declaration pointer parameters receive captures(none) before function-attrs,
preventing message/file/function strings from appearing to escape. Loop analysis
treats the calls as assertion checks and recognizes their terminating exits.
The walker records an assertion when selected and always ends the path, even
when assertion reporting is disabled or the input omitted noreturn attributes.
Keep ordinary predicate assertions separate from these failure entry points.

The integration fixture generates raw LLVM IR for all seven combinations,
including both BSD arities. Each is checked in single- and multi-property mode,
with assertions disabled and with an unrelated selected property. A subsequent
zero division must never execute after a recognized failure. Dumped IR checks
all pointer capture attributes, and canonical array loops verify that each form
is eligible for a summary while preserving its failure. A wrong-arity _wassert
call is deliberately not intercepted. No new flags are needed.

Build and validation use LLVM 23.1.6 and rustc 1.100.0. The new integration check
passes in 3.96 seconds. The full Vereda suite completes in 32.66 seconds with
526 passes, the existing lib_sprintf_overflow known-bug skip and no failures.

Run the 15 imported fixtures mentioning these functions as well. The MSVC
failure fixture now correctly reports FAILED instead of its recorded SUCCESSFUL
verdict; promote it to CORE and confirm its rerun passes. The affected imported
fixtures account for five passes and ten skips after that promotion.
