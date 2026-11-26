# Count-zero check selection and loop assumptions

PR #284 review identified two interactions with property selection and loop
summaries. An excluded count-zero check still narrowed the path and could
hide a selected reachability or overflow failure on a zero input. Gate both
Clang's builtin instrumentation and the raw-IR instrumentation on property
`all`, so requesting the flag under `unreach-call` or `no-overflow` does not
change the input domain of those properties.

Clang also uses its invalid-builtin handler for `__builtin_assume(false)`.
The walker correctly treats that diagnostic kind as an assumption, but the
loop-summary eligibility test accepted it as a checked exit. Require a known
diagnostic kind below 2 before accepting the handler in a summary. Assumptions
that can stop an iteration must retain ordinary unrolling.

Defensively skip scalable-vector instrumentation and reject nonscalar count
intrinsics in the encoder rather than calling scalar LLVM APIs on vector
types. Restrict the masked-result search to casts and addition, the shapes
used for Clang's count fallbacks and ffs. Division/remainder consumers are not
safe mask propagation steps.

Add regressions for source and IR zero paths leading to selected failures,
an assumption that stops an array-writing loop, poison consumed by division
before a select, and a live scalable-vector intrinsic returning UNKNOWN.

Validation with LLVM 23.1.6 / camada 0.25: the four focused integration
checks pass in 6.60 seconds; the Vereda suite gives 551 passes and one existing
skip in 44.53 seconds. The imported count-bit set gives 14 passes and one
expected skip. Linking the old loop-analysis implementation into an otherwise
fixed verifier makes the assumption regression fail with a spurious reachable
`reach_error`, confirming the regression exercises the faulty summary path.
