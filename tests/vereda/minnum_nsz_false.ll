; With nsz, llvm.minnum of two zeros of different signs may give either, so
; the minimum of zeros y and x need not be -0 when y is -0: asserting it
; fails for x = +0 only. An implementation that ignores nsz gives -0 there
; and passes.
; RUN:
; RUN: --fp2bv
; EXPECT: ^VERIFICATION FAILED$
declare float @__VERIFIER_nondet_float()
declare void @__VERIFIER_assert(i32)
declare float @llvm.minnum.f32(float, float)

define i32 @main() {
  %x = call float @__VERIFIER_nondet_float()
  %y = call float @__VERIFIER_nondet_float()
  %m = call nsz float @llvm.minnum.f32(float %y, float %x)
  %bits = bitcast float %m to i32
  %nz = icmp eq i32 %bits, -2147483648
  %ybits = bitcast float %y to i32
  %ynnz = icmp ne i32 %ybits, -2147483648
  %xnz = fcmp une float %x, 0.0
  %skip = or i1 %ynnz, %xnz
  %ok = or i1 %skip, %nz
  %c = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %c)
  ret i32 0
}
