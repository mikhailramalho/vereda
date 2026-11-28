; A vector of pointers crosses a call; lane 0 points to x, so asserting
; that the value loaded through it is not 5 fails for x = 5 only.
; RUN:
; EXPECT: ^VERIFICATION FAILED$
declare i32 @__VERIFIER_nondet_int()
declare void @__VERIFIER_assert(i32)

define <2 x ptr> @pair(ptr %a, ptr %b) noinline {
  %v0 = insertelement <2 x ptr> poison, ptr %a, i32 0
  %v1 = insertelement <2 x ptr> %v0, ptr %b, i32 1
  ret <2 x ptr> %v1
}

define i32 @main() {
  %p = alloca i32
  %q = alloca i32
  %x = call i32 @__VERIFIER_nondet_int()
  store i32 %x, ptr %p
  store i32 7, ptr %q
  %v = call <2 x ptr> @pair(ptr %p, ptr %q)
  %a = extractelement <2 x ptr> %v, i32 0
  %la = load i32, ptr %a
  %ok = icmp ne i32 %la, 5
  %z = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %z)
  ret i32 0
}
