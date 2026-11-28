; C's fmaxf may give either of two zeros of different signs (C17 F.10.9.2),
; so declared without a body it becomes llvm.maxnum with nsz, and the
; maximum of zeros y and x need not be +0 when y is +0: asserting it fails
; for x = -0 only. Without nsz the maximum is +0 and the assertion holds.
; RUN:
; RUN: --fp2bv
; EXPECT: ^VERIFICATION FAILED$
declare float @__VERIFIER_nondet_float()
declare void @__VERIFIER_assert(i32)
declare float @fmaxf(float, float)

define i32 @main() {
  %x = call float @__VERIFIER_nondet_float()
  %y = call float @__VERIFIER_nondet_float()
  %m = call float @fmaxf(float %y, float %x)
  %bits = bitcast float %m to i32
  %pz = icmp eq i32 %bits, 0
  %ybits = bitcast float %y to i32
  %ynpz = icmp ne i32 %ybits, 0
  %xnz = fcmp une float %x, 0.0
  %skip = or i1 %ynpz, %xnz
  %ok = or i1 %skip, %pz
  %c = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %c)
  ret i32 0
}
