; Each panic kind has its own per-function property numbering.
; RUN: --multi-property
; EXPECT: \[main\.assertion\.1\]
; EXPECT: \[main\.arithmetic-overflow\.1\]
; EXPECT: \[main\.array-bounds-violated\.1\]
; EXPECT: ^VERIFICATION FAILED$
declare i32 @__VERIFIER_nondet_int()
declare void @_RNvNtCs8uuN1nrGffB_4core9panicking5panic() noreturn
declare void @_ZN4core9panicking11panic_const24panic_const_add_overflow17h1234567890abcdefE() noreturn
declare void @_ZN4core9panicking18panic_bounds_check17h1234567890abcdefE() noreturn
define i32 @main() {
  %x = call i32 @__VERIFIER_nondet_int()
  switch i32 %x, label %assertion [ i32 1, label %overflow
                                     i32 2, label %bounds ]
assertion:
  call void @_RNvNtCs8uuN1nrGffB_4core9panicking5panic()
  unreachable
overflow:
  call void @_ZN4core9panicking11panic_const24panic_const_add_overflow17h1234567890abcdefE()
  unreachable
bounds:
  call void @_ZN4core9panicking18panic_bounds_check17h1234567890abcdefE()
  unreachable
}
