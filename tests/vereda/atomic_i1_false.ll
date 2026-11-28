; atomicrmw xchg on i1 returns the bit it found, which is false when n is
; nonzero: claiming it is always true fails for those n only.
; EXPECT: ^VERIFICATION FAILED$
declare i32 @__VERIFIER_nondet_int()
declare void @__VERIFIER_assert(i32)

define i32 @main() {
  %b = alloca i1
  %n = call i32 @__VERIFIER_nondet_int()
  %v = icmp eq i32 %n, 0
  store i1 %v, ptr %b
  %o = atomicrmw xchg ptr %b, i1 true seq_cst
  %a = zext i1 %o to i32
  call void @__VERIFIER_assert(i32 %a)
  ret i32 0
}
