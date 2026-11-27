; atomicrmw fmaximum orders -0 below +0: of x and +0 it stores x for every
; x >= 0 except -0, for which it stores +0. So asserting that it stores x
; when x >= 0 fails for x = -0 only, while an implementation that ignores
; the sign of zero and keeps the first of two equal operands passes.
; RUN:
; RUN: --fp2bv
; EXPECT: ^VERIFICATION FAILED$
declare double @__VERIFIER_nondet_double()
declare void @__VERIFIER_assert(i32)

define i32 @main() {
  %p = alloca double
  %x = call double @__VERIFIER_nondet_double()
  store double %x, ptr %p
  %o = atomicrmw fmaximum ptr %p, double 0.0 seq_cst
  %m = load double, ptr %p
  %mbits = bitcast double %m to i64
  %xbits = bitcast double %x to i64
  %same = icmp eq i64 %mbits, %xbits
  %neg = fcmp ult double %x, 0.0
  %ok = or i1 %neg, %same
  %a = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %a)
  ret i32 0
}
