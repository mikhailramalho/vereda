# Check-selection review follow-up

Merge main after #256, keeping both frontend test registrations, standard
forwarding, and assertion/divisor opt-outs in the importer. The shared witness
fixture no longer lists either the implemented standard option or assertion
opt-out as missing. Source bodies and expectations are preserved.

## Constant and guarded divisors

Pre-optimization divisor instrumentation can leave a check of literal true when
mem2reg exposes a local initialized to 2. Such a call blocked array-loop summaries
when the selected property did not count divisor checks. Drop these calls after
InstCombine/scalarization, including their unused declaration, and accept a
constant-true divisor check in loop analysis as well. Regressions require a
1024-element array loop to summarize under both unreach-call and no-overflow,
and require its prepared IR to contain no divisor helper.

Avoid duplicate instrumentation where a dominating branch already establishes
a nonzero divisor. Use LLVM ValueTracking with a dominator tree, without relying
on instruction UB flags or choosing values for undef. Build a tree only for
functions containing divisions. Raw IR still gets native checks before
optimization; C code guarded by the compiler's zero test does not get a second
check at the arithmetic operation.

## Distinguishing zero division from overflow

Mapping every `llvm.ubsantrap(3)` to DivisionByZero was considered and rejected:
LLVM 23 shares this code with signed division overflow, including INT_MIN / -1.
Instead, the default C zero-divisor instrumentation uses the nonreturning
`__ubsan_handle_divrem_overflow_abort` handler, which retains the operands.
The walker emits DivisionByZero when the divisor is zero and ArithmeticOverflow
otherwise. Existing signed-overflow instrumentation remains in trap mode; clang
keeps its overflow trap separate from the zero-divisor handler when both are on.
The nonreturning handler ends the path in either case.

For operands wider than uintptr_t, UBSan ValueHandles point to temporary storage.
Decode the integer width from its constant TypeDescriptor in the IR stage and
read indirect operands through the memory model. This follows the integer and
BitInt ValueHandle representations in [compiler-rt's type descriptors](https://github.com/llvm/llvm-project/blob/main/compiler-rt/lib/ubsan/ubsan_value.h).
Test long long on 32/64-bit targets, __int128, and 12/129-bit BitInt types.
Reject unsupported descriptors rather
than treating an operand's address as its value. Separate claim keys keep zero
division and overflow properties distinct if a handler can report either.

Tests require C `7 / n` to have exactly one property with id
`main.division-by-zero.1`, require the same id for literal C `1 / 0`, and check
that the prepared C IR has no duplicate native divisor helper. Additional cases
combine signed overflow and zero division, preserve the signed-overflow
classification, and exercise both kinds through a raw LLVM handler call.

## Option ordering and imported verdict

Process repeated `--no-div-by-zero-check` flags in order in the frontend, matching
the binary's last-value behavior. Test both orders of bare true and explicit
false, with C and raw LLVM inputs.

Main's standard forwarding lets github_10786 run with C11. Default zero-divisor
checking changes its recorded verdict from SUCCESSFUL to FAILED. It remains
FUTURE: its second expected property is after an unconditional zero division,
whose failure ends the path. Preserve both expectations and record the new
verdict instead of weakening the fixture. The word-array SMT dump fixture disables
zero-divisor instrumentation explicitly: it checks exact memory object numbers,
which would otherwise shift when clang emits UBSan's diagnostic globals even
for its constant nonzero remainder divisor. Keep all array-shape expectations.

## Validation

Use the LLVM 23.1.6 compiler and its emitted IR as the authority for this ABI;
ToT sources are a reference rather than a substitute for testing the pinned
compiler. The focused tests pass for summaries, single C division property IDs,
literal zero divisions, mixed overflow checks, both boolean-option orders,
and direct/indirect integer operands. The full Release suite passes all 7,032
executions with zero failures in 243.80 seconds using eight workers.
