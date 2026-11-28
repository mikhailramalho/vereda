; Atomics on i1 in one thread: xchg returns the old bit and stores the new
; one, add on one bit is xor, and cmpxchg stores only when the bit equals
; the expected one.
; EXPECT: ^VERIFICATION SUCCESSFUL$
declare i32 @__VERIFIER_nondet_int()
declare void @__VERIFIER_assert(i32)

define void @check(i1 %ok) {
  %c = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %c)
  ret void
}

define i32 @main() {
  %b = alloca i1
  %n = call i32 @__VERIFIER_nondet_int()
  %v = icmp eq i32 %n, 0
  store i1 %v, ptr %b
  %o1 = atomicrmw xchg ptr %b, i1 true seq_cst
  %c1 = icmp eq i1 %o1, %v
  call void @check(i1 %c1)
  %m1 = load i1, ptr %b
  call void @check(i1 %m1)
  %o2 = atomicrmw add ptr %b, i1 %v seq_cst
  call void @check(i1 %o2)
  %m2 = load i1, ptr %b
  %c2 = xor i1 %m2, %v
  call void @check(i1 %c2)
  %r = cmpxchg ptr %b, i1 false, i1 true seq_cst seq_cst
  %old = extractvalue { i1, i1 } %r, 0
  %ok = extractvalue { i1, i1 } %r, 1
  %c3 = icmp eq i1 %old, %m2
  call void @check(i1 %c3)
  %c4 = icmp ne i1 %ok, %m2
  call void @check(i1 %c4)
  %m3 = load i1, ptr %b
  call void @check(i1 %m3)
  ret i32 0
}
