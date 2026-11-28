; Float atomics in one thread: xchg, fadd and fsub return the old value and
; store the new one, rounded as fadd and fsub round. x is any float; d and
; y are doubles in (-1e6, 1e6), so no NaN makes an equality false.
; RUN:
; RUN: --fp2bv
; EXPECT: ^VERIFICATION SUCCESSFUL$
declare float @__VERIFIER_nondet_float()
declare double @__VERIFIER_nondet_double()
declare void @__VERIFIER_assume(i32)
declare void @__VERIFIER_assert(i32)

define void @check(i1 %ok) {
  %c = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %c)
  ret void
}

define i32 @main() {
  %f = alloca float
  %x = call float @__VERIFIER_nondet_float()
  store float %x, ptr %f
  %o1 = atomicrmw xchg ptr %f, float 2.5 seq_cst
  %o1.bits = bitcast float %o1 to i32
  %x.bits = bitcast float %x to i32
  %c1 = icmp eq i32 %o1.bits, %x.bits
  call void @check(i1 %c1)
  %o2 = atomicrmw fadd ptr %f, float 1.0 seq_cst
  %c2 = fcmp oeq float %o2, 2.5
  call void @check(i1 %c2)
  %o3 = atomicrmw fsub ptr %f, float 0.25 seq_cst
  %c3 = fcmp oeq float %o3, 3.5
  call void @check(i1 %c3)
  %m = load float, ptr %f
  %c4 = fcmp oeq float %m, 3.25
  call void @check(i1 %c4)

  %p = alloca double
  %d = call double @__VERIFIER_nondet_double()
  %y = call double @__VERIFIER_nondet_double()
  %lo = fcmp ogt double %d, -1.0e6
  %hi = fcmp olt double %d, 1.0e6
  %in = and i1 %lo, %hi
  %ylo = fcmp ogt double %y, -1.0e6
  %yhi = fcmp olt double %y, 1.0e6
  %yin = and i1 %ylo, %yhi
  %both = and i1 %in, %yin
  %a = zext i1 %both to i32
  call void @__VERIFIER_assume(i32 %a)
  store double %d, ptr %p
  %o4 = atomicrmw fadd ptr %p, double %y seq_cst
  %c5 = fcmp oeq double %o4, %d
  call void @check(i1 %c5)
  %sum = fadd double %d, %y
  %m2 = load double, ptr %p
  %c6 = fcmp oeq double %m2, %sum
  call void @check(i1 %c6)
  %o5 = atomicrmw fsub ptr %p, double %y seq_cst
  %diff = fsub double %sum, %y
  %m3 = load double, ptr %p
  %c7 = fcmp oeq double %m3, %diff
  call void @check(i1 %c7)
  %o6 = atomicrmw xchg ptr %p, double %y seq_cst
  %c8 = fcmp oeq double %o6, %diff
  call void @check(i1 %c8)
  %m4 = load double, ptr %p
  %c9 = fcmp oeq double %m4, %y
  call void @check(i1 %c9)
  ret i32 0
}
