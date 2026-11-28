; llvm.maximum propagates NaN: the maximum of x and 1 is NaN only when x
; is NaN, so this assertion fails for that x only.
; RUN:
; RUN: --fp2bv
; EXPECT: ^VERIFICATION FAILED$
declare double @__VERIFIER_nondet_double()
declare void @__VERIFIER_assert(i32)
declare double @llvm.maximum.f64(double, double)

define i32 @main() {
  %x = call double @__VERIFIER_nondet_double()
  %m = call double @llvm.maximum.f64(double %x, double 1.0)
  %c = fcmp ord double %m, %m
  %a = zext i1 %c to i32
  call void @__VERIFIER_assert(i32 %a)
  ret i32 0
}
