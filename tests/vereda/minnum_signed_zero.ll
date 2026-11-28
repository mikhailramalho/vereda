; llvm.minnum and llvm.maxnum, and atomicrmw fmin and fmax, order -0 below
; +0, as LLVM 23's LangRef has it: for zeros x and y the minimum is -0 if
; either is and the maximum only if both are, which as bits is x | y and
; x & y. With nsz, and for fminf and fmaxf declared without a body, either
; zero may come back. Constant operands give the same as nondet ones.
; RUN:
; RUN: --fp2bv
; EXPECT: ^VERIFICATION SUCCESSFUL$
declare float @__VERIFIER_nondet_float()
declare void @__VERIFIER_assert(i32)
declare float @llvm.minnum.f32(float, float)
declare float @llvm.maxnum.f32(float, float)
declare float @fminf(float, float)
declare float @fmaxf(float, float)

define void @check(i1 %ok) {
  %c = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %c)
  ret void
}

define void @is(float %m, i32 %want) {
  %bits = bitcast float %m to i32
  %c = icmp eq i32 %bits, %want
  call void @check(i1 %c)
  ret void
}

define void @either(float %m, float %x, float %y) {
  %mb = bitcast float %m to i32
  %xb = bitcast float %x to i32
  %yb = bitcast float %y to i32
  %cx = icmp eq i32 %mb, %xb
  %cy = icmp eq i32 %mb, %yb
  %c = or i1 %cx, %cy
  call void @check(i1 %c)
  ret void
}

; Operands that are constants when walked, not in the IR.
define void @consts(float %a, float %b) {
  %mn = call float @llvm.minnum.f32(float %a, float %b)
  call void @is(float %mn, i32 -2147483648)
  %mx = call float @llvm.maxnum.f32(float %a, float %b)
  call void @is(float %mx, i32 0)
  ret void
}

define i32 @main() {
entry:
  %p = alloca float
  %x = call float @__VERIFIER_nondet_float()
  %y = call float @__VERIFIER_nondet_float()
  %xz = fcmp oeq float %x, 0.0
  %yz = fcmp oeq float %y, 0.0
  %zeros = and i1 %xz, %yz
  br i1 %zeros, label %zero, label %done

zero:
  %xb = bitcast float %x to i32
  %yb = bitcast float %y to i32
  %or = or i32 %xb, %yb
  %and = and i32 %xb, %yb
  %mn = call float @llvm.minnum.f32(float %x, float %y)
  call void @is(float %mn, i32 %or)
  %mx = call float @llvm.maxnum.f32(float %x, float %y)
  call void @is(float %mx, i32 %and)
  store float %x, ptr %p
  %o1 = atomicrmw fmin ptr %p, float %y seq_cst
  %r1 = load float, ptr %p
  call void @is(float %r1, i32 %or)
  store float %x, ptr %p
  %o2 = atomicrmw fmax ptr %p, float %y seq_cst
  %r2 = load float, ptr %p
  call void @is(float %r2, i32 %and)
  %n1 = call nsz float @llvm.minnum.f32(float %x, float %y)
  call void @either(float %n1, float %x, float %y)
  %n2 = call nsz float @llvm.maxnum.f32(float %x, float %y)
  call void @either(float %n2, float %x, float %y)
  %c1 = call float @fminf(float %x, float %y)
  call void @either(float %c1, float %x, float %y)
  %c2 = call float @fmaxf(float %x, float %y)
  call void @either(float %c2, float %x, float %y)
  call void @consts(float 0.0, float -0.0)
  call void @consts(float -0.0, float 0.0)
  br label %done

done:
  ret i32 0
}
