; cmpxchg on a pointer in one thread: it returns the pointer it found and
; whether that equalled the expected one, and stores the new pointer only
; then. The slot holds @a or @c; exchanging @a for @b leaves @b or @c.
; EXPECT: ^VERIFICATION SUCCESSFUL$
@a = global i32 1
@b = global i32 2
@c = global i32 3

declare i32 @__VERIFIER_nondet_int()
declare void @__VERIFIER_assert(i32)

define i32 @main() {
  %slot = alloca ptr
  %n = call i32 @__VERIFIER_nondet_int()
  %isA = icmp eq i32 %n, 0
  %init = select i1 %isA, ptr @a, ptr @c
  store ptr %init, ptr %slot
  %r = cmpxchg ptr %slot, ptr @a, ptr @b seq_cst seq_cst
  %old = extractvalue { ptr, i1 } %r, 0
  %ok = extractvalue { ptr, i1 } %r, 1
  %oldOk = icmp eq ptr %old, %init
  %c1 = zext i1 %oldOk to i32
  call void @__VERIFIER_assert(i32 %c1)
  %okOk = icmp eq i1 %ok, %isA
  %c2 = zext i1 %okOk to i32
  call void @__VERIFIER_assert(i32 %c2)
  %now = load ptr, ptr %slot
  %want = select i1 %isA, ptr @b, ptr @c
  %nowOk = icmp eq ptr %now, %want
  %c3 = zext i1 %nowOk to i32
  call void @__VERIFIER_assert(i32 %c3)
  %v = load i32, ptr %now
  %want.v = select i1 %isA, i32 2, i32 3
  %vOk = icmp eq i32 %v, %want.v
  %c4 = zext i1 %vOk to i32
  call void @__VERIFIER_assert(i32 %c4)
  ret i32 0
}
