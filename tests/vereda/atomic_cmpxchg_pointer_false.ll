; As atomic_cmpxchg_pointer.ll, but claims the slot always ends up holding
; @b, which is wrong exactly when it held @c, so the exchange failed.
; EXPECT: ^VERIFICATION FAILED$
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
  %now = load ptr, ptr %slot
  %nowB = icmp eq ptr %now, @b
  %c = zext i1 %nowB to i32
  call void @__VERIFIER_assert(i32 %c)
  ret i32 0
}
