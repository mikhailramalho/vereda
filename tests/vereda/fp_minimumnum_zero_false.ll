; llvm.minimumnum orders -0 below +0: the minimum of x and +0 is negative
; only when x is negative or -0 (a NaN gives +0), so asserting that a
; negative result comes from an x below 0 fails for x = -0 only.
; RUN:
; RUN: --fp2bv
; EXPECT: ^VERIFICATION FAILED$
declare double @__VERIFIER_nondet_double()
declare void @__VERIFIER_assert(i32)
declare double @llvm.minimumnum.f64(double, double)

define i32 @main() {
  %x = call double @__VERIFIER_nondet_double()
  %m = call double @llvm.minimumnum.f64(double %x, double 0.0)
  %bits = bitcast double %m to i64
  %pos = icmp sge i64 %bits, 0
  %below = fcmp olt double %x, 0.0
  %c = or i1 %pos, %below
  %a = zext i1 %c to i32
  call void @__VERIFIER_assert(i32 %a)
  ret i32 0
}
