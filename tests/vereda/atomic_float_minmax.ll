; atomicrmw fmax, fmin, fmaximum and fminimum on any floats x and y (and
; fminimum on doubles) return the old value x and store the maximum or
; minimum. fmax and fmin give the other operand when one is NaN, NaN only
; when both are; fmaximum and fminimum give NaN when either is, and order
; -0 below +0.
; RUN:
; RUN: --fp2bv
; EXPECT: ^VERIFICATION SUCCESSFUL$
declare float @__VERIFIER_nondet_float()
declare double @__VERIFIER_nondet_double()
declare void @__VERIFIER_assert(i32)

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

define float @rmw.fmax(float %x, float %y) {
  %p = alloca float
  store float %x, ptr %p
  %o = atomicrmw fmax ptr %p, float %y seq_cst
  call void @same(float %o, float %x)
  %m = load float, ptr %p
  ret float %m
}

define float @rmw.fmin(float %x, float %y) {
  %p = alloca float
  store float %x, ptr %p
  %o = atomicrmw fmin ptr %p, float %y seq_cst
  call void @same(float %o, float %x)
  %m = load float, ptr %p
  ret float %m
}

define float @rmw.fmaximum(float %x, float %y) {
  %p = alloca float
  store float %x, ptr %p
  %o = atomicrmw fmaximum ptr %p, float %y seq_cst
  call void @same(float %o, float %x)
  %m = load float, ptr %p
  ret float %m
}

define float @rmw.fminimum(float %x, float %y) {
  %p = alloca float
  store float %x, ptr %p
  %o = atomicrmw fminimum ptr %p, float %y seq_cst
  call void @same(float %o, float %x)
  %m = load float, ptr %p
  ret float %m
}

define i32 @main() {
  %x = call float @__VERIFIER_nondet_float()
  %y = call float @__VERIFIER_nondet_float()
  %xnan = fcmp uno float %x, %x
  %ynan = fcmp uno float %y, %y
  %both = and i1 %xnan, %ynan
  %any = or i1 %xnan, %ynan
  %xz = fcmp oeq float %x, 0.0
  %yz = fcmp oeq float %y, 0.0
  %zeros = and i1 %xz, %yz
  %x.bits = bitcast float %x to i32
  %y.bits = bitcast float %y to i32
  %xneg = icmp slt i32 %x.bits, 0
  %yneg = icmp slt i32 %y.bits, 0
  %negand = and i1 %xneg, %yneg
  %negor = or i1 %xneg, %yneg

  %max = call float @rmw.fmax(float %x, float %y)
  %max.nan = fcmp uno float %max, %max
  %c1 = icmp eq i1 %max.nan, %both
  call void @check(i1 %c1)
  %c2 = fcmp uge float %max, %x
  call void @check(i1 %c2)
  %c3 = fcmp uge float %max, %y
  call void @check(i1 %c3)
  %max.x = fcmp oeq float %max, %x
  %max.y = fcmp oeq float %max, %y
  %max.xy = or i1 %max.x, %max.y
  %c4 = or i1 %max.xy, %both
  call void @check(i1 %c4)

  %min = call float @rmw.fmin(float %x, float %y)
  %min.nan = fcmp uno float %min, %min
  %c5 = icmp eq i1 %min.nan, %both
  call void @check(i1 %c5)
  %c6 = fcmp ule float %min, %x
  call void @check(i1 %c6)
  %c7 = fcmp ule float %min, %y
  call void @check(i1 %c7)
  %min.x = fcmp oeq float %min, %x
  %min.y = fcmp oeq float %min, %y
  %min.xy = or i1 %min.x, %min.y
  %c8 = or i1 %min.xy, %both
  call void @check(i1 %c8)

  %mx = call float @rmw.fmaximum(float %x, float %y)
  %mx.nan = fcmp uno float %mx, %mx
  %c9 = icmp eq i1 %mx.nan, %any
  call void @check(i1 %c9)
  %c10 = fcmp uge float %mx, %x
  call void @check(i1 %c10)
  %c11 = fcmp uge float %mx, %y
  call void @check(i1 %c11)
  %mx.x = fcmp oeq float %mx, %x
  %mx.y = fcmp oeq float %mx, %y
  %mx.xy = or i1 %mx.x, %mx.y
  %c12 = or i1 %mx.xy, %any
  call void @check(i1 %c12)
  %mx.bits = bitcast float %mx to i32
  %mx.neg = icmp slt i32 %mx.bits, 0
  %mx.sign = icmp eq i1 %mx.neg, %negand
  %c13 = select i1 %zeros, i1 %mx.sign, i1 true
  call void @check(i1 %c13)

  %mn = call float @rmw.fminimum(float %x, float %y)
  %mn.nan = fcmp uno float %mn, %mn
  %c14 = icmp eq i1 %mn.nan, %any
  call void @check(i1 %c14)
  %c15 = fcmp ule float %mn, %x
  call void @check(i1 %c15)
  %c16 = fcmp ule float %mn, %y
  call void @check(i1 %c16)
  %mn.x = fcmp oeq float %mn, %x
  %mn.y = fcmp oeq float %mn, %y
  %mn.xy = or i1 %mn.x, %mn.y
  %c17 = or i1 %mn.xy, %any
  call void @check(i1 %c17)
  %mn.bits = bitcast float %mn to i32
  %mn.neg = icmp slt i32 %mn.bits, 0
  %mn.sign = icmp eq i1 %mn.neg, %negor
  %c18 = select i1 %zeros, i1 %mn.sign, i1 true
  call void @check(i1 %c18)

  %d = call double @__VERIFIER_nondet_double()
  %e = call double @__VERIFIER_nondet_double()
  %q = alloca double
  store double %d, ptr %q
  %od = atomicrmw fminimum ptr %q, double %e seq_cst
  %od.ok = fcmp oeq double %od, %d
  %dnan = fcmp uno double %d, %d
  %c19 = or i1 %od.ok, %dnan
  call void @check(i1 %c19)
  %md = load double, ptr %q
  %md.nan = fcmp uno double %md, %md
  %enan = fcmp uno double %e, %e
  %danyn = or i1 %dnan, %enan
  %c20 = icmp eq i1 %md.nan, %danyn
  call void @check(i1 %c20)
  %c21 = fcmp ule double %md, %e
  call void @check(i1 %c21)
  %dz = fcmp oeq double %d, 0.0
  %ez = fcmp oeq double %e, 0.0
  %dzeros = and i1 %dz, %ez
  %d.bits = bitcast double %d to i64
  %e.bits = bitcast double %e to i64
  %md.bits = bitcast double %md to i64
  %dneg = icmp slt i64 %d.bits, 0
  %eneg = icmp slt i64 %e.bits, 0
  %mdneg = icmp slt i64 %md.bits, 0
  %dnegor = or i1 %dneg, %eneg
  %md.sign = icmp eq i1 %mdneg, %dnegor
  %c22 = select i1 %dzeros, i1 %md.sign, i1 true
  call void @check(i1 %c22)
  ret i32 0
}
