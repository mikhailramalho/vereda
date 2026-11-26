# Count-zero intrinsics and undefined builtin arguments

Implement #262 with the opt-in `--ub-count-zero-check` flag. The name covers
both leading and trailing zero counts, while distinguishing undefined inputs
from ordinary arithmetic-overflow checks. Failures have the `count-zero`
property kind, independent of user assertions. Property-specific unreach-call
and no-overflow runs exclude this check, like division-by-zero checks, and
invalid paths terminate without discarding earlier failures.

The encoder handles LLVM 23 `ctlz` and `cttz` at arbitrary scalar integer
widths. A scan of the operand bits builds an ite expression for the count;
the defined-zero form returns the operand width modulo the result width.
Unchecked poison-on-zero results use a fresh value on zero, matching the
existing treatment of LLVM poison. Vector operations use the existing
scalarizer.

Preserve raw-IR poison-on-zero checks before optimization, including unused
results. Check all vector lanes and clear the intrinsic's poison flag after
capturing its precondition. Safe checks that become constant true after
mem2reg/InstCombine are removed, as for divisors. The loop-summary eligibility
test recognizes the new checks under the selected property.

For C inputs, enable Clang's builtin sanitizer with nonreturning handlers.
This captures source-level invalid builtin calls before they can disappear
or have their result masked. Clang's invalid-builtin handler distinguishes
zero arguments from false `__builtin_assume` conditions; the latter still
terminate the path as assumptions, without count-zero findings. Frontend flag
inspection follows last-flag-wins, while the binary remains responsible for
validating the option.

A raw poison-on-zero call can also implement a defined-zero source builtin:
Clang emits a select choosing the explicit fallback when the operand is zero.
Recognize this mask through casts and arithmetic, with a shared 32-node
search budget, and leave that call unchecked. This covers `clzg`/`ctzg` with
fallbacks and `ffs`. Source sanitizer checks still catch an ordinary clz/ctz
call whose result the C program masks later. Defined-zero LLVM intrinsics
never need a nonzero precondition.

Regression coverage compares symbolic 64-bit counts with independent shift
loops, tests scalar IR widths 1/7/8/16/32/64/128, and covers vector lanes,
constant and unused calls, fallbacks, both solver modes, incremental BMC,
property selection, assertion independence and preservation of earlier
failures. Map ESBMC's `--clz-zero-check` to the new flag in the importer.

Validation on LLVM 23.1.6 / camada 0.25: the Vereda suite gives 551 passes
and one existing skip in 35.64 seconds. After the final diagnostic wording
change, count-zero, check-selection, importer and test-registration integration
tests all pass (6.50 seconds together; the count-zero checks take 5.55 seconds
under that parallel run). Nine imported fixtures are promoted from FUTURE to
CORE. The imported clz-zero-default fixture remains FUTURE: unlike ESBMC's
chosen width result for an undefined zero input, Vereda leaves LLVM poison
unconstrained when this check is off.
