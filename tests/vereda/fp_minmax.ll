; llvm.maximum and llvm.minimum give NaN when either operand is NaN;
; llvm.maximumnum, llvm.minimumnum and atomicrmw fmaximumnum and fminimumnum
; give the other operand, NaN only when both are. All four order -0 below +0.
; For any floats x and y each result is at least (at most) each ordered
; operand, equal to one of them, and of two zeros has the right sign.
; RUN:
; RUN: --fp2bv
; EXPECT: ^VERIFICATION SUCCESSFUL$
declare float @__VERIFIER_nondet_float()
declare void @__VERIFIER_assert(i32)
declare float @llvm.maximum.f32(float, float)
declare float @llvm.minimum.f32(float, float)
declare float @llvm.maximumnum.f32(float, float)
declare float @llvm.minimumnum.f32(float, float)

define void @check(i1 %ok) {
  %c = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %c)
  ret void
}

define void @same(float %a, float %b) {
  %a.bits = bitcast float %a to i32
  %b.bits = bitcast float %b to i32
  %c = icmp eq i32 %a.bits, %b.bits
  call void @check(i1 %c)
  ret void
}

; m is the maximum (%max) or minimum of x and y, NaN-propagating or, with
; %num, giving the number when only one operand is NaN.
define void @props(float %x, float %y, float %m, i1 %max, i1 %num) {
  %xnan = fcmp uno float %x, %x
  %ynan = fcmp uno float %y, %y
  %both = and i1 %xnan, %ynan
  %any = or i1 %xnan, %ynan
  %m.nan = fcmp uno float %m, %m
  %nan = select i1 %num, i1 %both, i1 %any
  %c1 = icmp eq i1 %m.nan, %nan
  call void @check(i1 %c1)
  %gex = fcmp uge float %m, %x
  %lex = fcmp ule float %m, %x
  %c2 = select i1 %max, i1 %gex, i1 %lex
  call void @check(i1 %c2)
  %gey = fcmp uge float %m, %y
  %ley = fcmp ule float %m, %y
  %c3 = select i1 %max, i1 %gey, i1 %ley
  call void @check(i1 %c3)
  %mx = fcmp oeq float %m, %x
  %my = fcmp oeq float %m, %y
  %one = or i1 %mx, %my
  %c4 = or i1 %one, %m.nan
  call void @check(i1 %c4)
  %xz = fcmp oeq float %x, 0.0
  %yz = fcmp oeq float %y, 0.0
  %zeros = and i1 %xz, %yz
  %x.bits = bitcast float %x to i32
  %y.bits = bitcast float %y to i32
  %m.bits = bitcast float %m to i32
  %xneg = icmp slt i32 %x.bits, 0
  %yneg = icmp slt i32 %y.bits, 0
  %mneg = icmp slt i32 %m.bits, 0
  %negand = and i1 %xneg, %yneg
  %negor = or i1 %xneg, %yneg
  %neg = select i1 %max, i1 %negand, i1 %negor
  %sign = icmp eq i1 %mneg, %neg
  %c5 = select i1 %zeros, i1 %sign, i1 true
  call void @check(i1 %c5)
  %ynum = xor i1 %ynan, true
  %xnum = xor i1 %xnan, true
  %xonly = and i1 %xnan, %ynum
  %yonly = and i1 %ynan, %xnum
  %num.x = and i1 %num, %xonly
  %num.y = and i1 %num, %yonly
  %is.y = icmp eq i32 %m.bits, %y.bits
  %is.x = icmp eq i32 %m.bits, %x.bits
  %c6 = select i1 %num.x, i1 %is.y, i1 true
  call void @check(i1 %c6)
  %c7 = select i1 %num.y, i1 %is.x, i1 true
  call void @check(i1 %c7)
  ret void
}

define float @rmw.fmaximumnum(float %x, float %y) {
  %p = alloca float
  store float %x, ptr %p
  %o = atomicrmw fmaximumnum ptr %p, float %y seq_cst
  call void @same(float %o, float %x)
  %m = load float, ptr %p
  ret float %m
}

define float @rmw.fminimumnum(float %x, float %y) {
  %p = alloca float
  store float %x, ptr %p
  %o = atomicrmw fminimumnum ptr %p, float %y seq_cst
  call void @same(float %o, float %x)
  %m = load float, ptr %p
  ret float %m
}

define i32 @main() {
  %x = call float @__VERIFIER_nondet_float()
  %y = call float @__VERIFIER_nondet_float()
  %mx = call float @llvm.maximum.f32(float %x, float %y)
  call void @props(float %x, float %y, float %mx, i1 true, i1 false)
  %mn = call float @llvm.minimum.f32(float %x, float %y)
  call void @props(float %x, float %y, float %mn, i1 false, i1 false)
  %mxn = call float @llvm.maximumnum.f32(float %x, float %y)
  call void @props(float %x, float %y, float %mxn, i1 true, i1 true)
  %mnn = call float @llvm.minimumnum.f32(float %x, float %y)
  call void @props(float %x, float %y, float %mnn, i1 false, i1 true)
  %rx = call float @rmw.fmaximumnum(float %x, float %y)
  call void @props(float %x, float %y, float %rx, i1 true, i1 true)
  %rn = call float @rmw.fminimumnum(float %x, float %y)
  call void @props(float %x, float %y, float %rn, i1 false, i1 true)
  ret i32 0
}
