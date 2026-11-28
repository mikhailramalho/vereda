; atomicrmw fminimumnum orders -0 below +0 and gives the number when one
; operand is NaN: the minimum of x and +0 stored is negative only when x is
; negative or -0, so asserting that a negative result comes from an x
; below 0 fails for x = -0 only.
; RUN:
; RUN: --fp2bv
; EXPECT: ^VERIFICATION FAILED$
declare float @__VERIFIER_nondet_float()
declare void @__VERIFIER_assert(i32)

define i32 @main() {
  %p = alloca float
  %x = call float @__VERIFIER_nondet_float()
  store float %x, ptr %p
  %o = atomicrmw fminimumnum ptr %p, float 0.0 seq_cst
  %m = load float, ptr %p
  %bits = bitcast float %m to i32
  %pos = icmp sge i32 %bits, 0
  %below = fcmp olt float %x, 0.0
  %c = or i1 %pos, %below
  %a = zext i1 %c to i32
  call void @__VERIFIER_assert(i32 %a)
  ret i32 0
}
