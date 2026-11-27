; atomicrmw fadd stores the rounded sum: x + 1 is 3 when x is 2 (and for
; a few x just below 2 that round up to it), so this assertion fails for
; those x only.
; RUN:
; RUN: --fp2bv
; EXPECT: ^VERIFICATION FAILED$
declare float @__VERIFIER_nondet_float()
declare void @__VERIFIER_assert(i32)

define i32 @main() {
  %f = alloca float
  %x = call float @__VERIFIER_nondet_float()
  store float %x, ptr %f
  %o = atomicrmw fadd ptr %f, float 1.0 seq_cst
  %m = load float, ptr %f
  %c = fcmp une float %m, 3.0
  %a = zext i1 %c to i32
  call void @__VERIFIER_assert(i32 %a)
  ret i32 0
}
