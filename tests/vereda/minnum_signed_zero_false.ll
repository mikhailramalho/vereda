; llvm.minnum orders -0 below +0, so the minimum of zeros y and x is +0 only
; when both are +0: asserting that it is +0 when y is +0 fails for x = -0
; only. An implementation that returns the first of two equal operands
; gives +0 there and passes.
; RUN:
; RUN: --fp2bv
; EXPECT: ^VERIFICATION FAILED$
declare float @__VERIFIER_nondet_float()
declare void @__VERIFIER_assert(i32)
declare float @llvm.minnum.f32(float, float)

define i32 @main() {
  %x = call float @__VERIFIER_nondet_float()
  %y = call float @__VERIFIER_nondet_float()
  %m = call float @llvm.minnum.f32(float %y, float %x)
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
