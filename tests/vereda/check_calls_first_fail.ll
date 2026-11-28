; Without --multi-property a failed check Vereda inserts as a call ends its
; path: the division by zero is reported, and the checks after it, which
; fail on every path too, are not even checked.
; RUN: --ub-count-zero-check --ub-float-cast-overflow-check --ub-alignment-check
; EXPECT: ^  FAILED .*  division by zero$
; EXPECT: ^Properties: 1 checked, 0 passed, 1 failed, 0 not checked$
; EXPECT: ^VERIFICATION FAILED$
declare i32 @__VERIFIER_nondet_int()
declare double @__VERIFIER_nondet_double()
declare void @__VERIFIER_assume(i32)
declare i32 @llvm.cttz.i32(i32, i1)

define i32 @main() {
  %a = call i32 @__VERIFIER_nondet_int()
  %b = call i32 @__VERIFIER_nondet_int()
  %d = call double @__VERIFIER_nondet_double()
  %o = call i32 @__VERIFIER_nondet_int()
  %u = call i32 @__VERIFIER_nondet_int()
  %b0 = icmp eq i32 %b, 0
  %big = fcmp ogt double %d, 1.0e10
  %o1 = icmp eq i32 %o, 1
  %u0 = icmp eq i32 %u, 0
  %c1 = and i1 %b0, %big
  %c2 = and i1 %c1, %o1
  %c3 = and i1 %c2, %u0
  %c = zext i1 %c3 to i32
  call void @__VERIFIER_assume(i32 %c)
  %q = sdiv i32 %a, %b
  %z = call i32 @llvm.cttz.i32(i32 %u, i1 true)
  %y = fptosi double %d to i32
  %buf = alloca [2 x i32], align 4
  %p = getelementptr i8, ptr %buf, i32 %o
  %r = load i32, ptr %p, align 4
  %s1 = add i32 %q, %z
  %s2 = add i32 %s1, %y
  %s3 = add i32 %s2, %r
  ret i32 %s3
}
