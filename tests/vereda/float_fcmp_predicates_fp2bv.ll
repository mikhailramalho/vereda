; All ordered and unordered fcmp predicates on symbolic NaN, signed zeros,
; infinities and finite values under --fp2bv.
; RUN: --fp2bv
; EXPECT: ^VERIFICATION SUCCESSFUL$
declare void @__VERIFIER_assert(i32)
declare void @__VERIFIER_assume(i32)
declare double @__VERIFIER_nondet_double()

define internal void @assert_eq(i1 %got, i1 %want) {
  %ok = icmp eq i1 %got, %want
  %ok32 = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %ok32)
  ret void
}

define internal void @check(double %a, double %b,
                            i1 %e_oeq, i1 %e_ogt, i1 %e_oge, i1 %e_olt, i1 %e_ole, i1 %e_one, i1 %e_ord,
                            i1 %e_uno, i1 %e_ueq, i1 %e_ugt, i1 %e_uge, i1 %e_ult, i1 %e_ule, i1 %e_une) {
  %oeq = fcmp oeq double %a, %b
  %ogt = fcmp ogt double %a, %b
  %oge = fcmp oge double %a, %b
  %olt = fcmp olt double %a, %b
  %ole = fcmp ole double %a, %b
  %one = fcmp one double %a, %b
  %ord = fcmp ord double %a, %b
  %uno = fcmp uno double %a, %b
  %ueq = fcmp ueq double %a, %b
  %ugt = fcmp ugt double %a, %b
  %uge = fcmp uge double %a, %b
  %ult = fcmp ult double %a, %b
  %ule = fcmp ule double %a, %b
  %une = fcmp une double %a, %b
  call void @assert_eq(i1 %oeq, i1 %e_oeq)
  call void @assert_eq(i1 %ogt, i1 %e_ogt)
  call void @assert_eq(i1 %oge, i1 %e_oge)
  call void @assert_eq(i1 %olt, i1 %e_olt)
  call void @assert_eq(i1 %ole, i1 %e_ole)
  call void @assert_eq(i1 %one, i1 %e_one)
  call void @assert_eq(i1 %ord, i1 %e_ord)
  call void @assert_eq(i1 %uno, i1 %e_uno)
  call void @assert_eq(i1 %ueq, i1 %e_ueq)
  call void @assert_eq(i1 %ugt, i1 %e_ugt)
  call void @assert_eq(i1 %uge, i1 %e_uge)
  call void @assert_eq(i1 %ult, i1 %e_ult)
  call void @assert_eq(i1 %ule, i1 %e_ule)
  call void @assert_eq(i1 %une, i1 %e_une)
  ret void
}

define internal void @assume_i1(i1 %c) {
  %c32 = zext i1 %c to i32
  call void @__VERIFIER_assume(i32 %c32)
  ret void
}

define i32 @main() {
  %pos0 = call double @__VERIFIER_nondet_double()
  %is_p0_z = fcmp oeq double %pos0, 0.0
  call void @assume_i1(i1 %is_p0_z)
  %p0_bits = bitcast double %pos0 to i64
  %p0_pos = icmp sge i64 %p0_bits, 0
  call void @assume_i1(i1 %p0_pos)

  %neg0 = call double @__VERIFIER_nondet_double()
  %is_n0_z = fcmp oeq double %neg0, 0.0
  call void @assume_i1(i1 %is_n0_z)
  %n0_bits = bitcast double %neg0 to i64
  %n0_neg = icmp slt i64 %n0_bits, 0
  call void @assume_i1(i1 %n0_neg)

  %nan = call double @__VERIFIER_nondet_double()
  %is_nan = fcmp uno double %nan, %nan
  call void @assume_i1(i1 %is_nan)

  %pinf = call double @__VERIFIER_nondet_double()
  %is_pinf = fcmp oeq double %pinf, 0x7ff0000000000000
  call void @assume_i1(i1 %is_pinf)

  %ninf = call double @__VERIFIER_nondet_double()
  %is_ninf = fcmp oeq double %ninf, 0xfff0000000000000
  call void @assume_i1(i1 %is_ninf)

  %one = call double @__VERIFIER_nondet_double()
  %is_one = fcmp oeq double %one, 1.0
  call void @assume_i1(i1 %is_one)

  %two = call double @__VERIFIER_nondet_double()
  %is_two = fcmp oeq double %two, 2.0
  call void @assume_i1(i1 %is_two)

  call void @check(double %pos0, double %neg0, i1 1, i1 0, i1 1, i1 0, i1 1, i1 0, i1 1, i1 0, i1 1, i1 0, i1 1, i1 0, i1 1, i1 0)
  call void @check(double %neg0, double %pos0, i1 1, i1 0, i1 1, i1 0, i1 1, i1 0, i1 1, i1 0, i1 1, i1 0, i1 1, i1 0, i1 1, i1 0)
  call void @check(double %nan, double %one, i1 0, i1 0, i1 0, i1 0, i1 0, i1 0, i1 0, i1 1, i1 1, i1 1, i1 1, i1 1, i1 1, i1 1)
  call void @check(double %one, double %nan, i1 0, i1 0, i1 0, i1 0, i1 0, i1 0, i1 0, i1 1, i1 1, i1 1, i1 1, i1 1, i1 1, i1 1)
  call void @check(double %nan, double %nan, i1 0, i1 0, i1 0, i1 0, i1 0, i1 0, i1 0, i1 1, i1 1, i1 1, i1 1, i1 1, i1 1, i1 1)
  call void @check(double %pinf, double %ninf, i1 0, i1 1, i1 1, i1 0, i1 0, i1 1, i1 1, i1 0, i1 0, i1 1, i1 1, i1 0, i1 0, i1 1)
  call void @check(double %ninf, double %pinf, i1 0, i1 0, i1 0, i1 1, i1 1, i1 1, i1 1, i1 0, i1 0, i1 0, i1 0, i1 1, i1 1, i1 1)
  call void @check(double %pinf, double %pinf, i1 1, i1 0, i1 1, i1 0, i1 1, i1 0, i1 1, i1 0, i1 1, i1 0, i1 1, i1 0, i1 1, i1 0)
  call void @check(double %one, double %two, i1 0, i1 0, i1 0, i1 1, i1 1, i1 1, i1 1, i1 0, i1 0, i1 0, i1 0, i1 1, i1 1, i1 1)
  call void @check(double %two, double %one, i1 0, i1 1, i1 1, i1 0, i1 0, i1 1, i1 1, i1 0, i1 0, i1 1, i1 1, i1 0, i1 0, i1 1)
  ret i32 0
}
