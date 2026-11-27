; atomicrmw fmaximum orders -0 below +0: the maximum of x and -0 is -0
; only when x is -0 (+0 gives +0, a NaN gives NaN), so this assertion fails
; for that x only.
; RUN:
; RUN: --fp2bv
; EXPECT: ^VERIFICATION FAILED$
declare double @__VERIFIER_nondet_double()
declare void @__VERIFIER_assert(i32)

define i32 @main() {
  %p = alloca double
  %x = call double @__VERIFIER_nondet_double()
  store double %x, ptr %p
  %o = atomicrmw fmaximum ptr %p, double -0.0 seq_cst
  %m = load double, ptr %p
  %bits = bitcast double %m to i64
  %c = icmp ne i64 %bits, -9223372036854775808
  %a = zext i1 %c to i32
  call void @__VERIFIER_assert(i32 %a)
  ret i32 0
}
