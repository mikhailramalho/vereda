; A vector of i1 crosses a call; lane 0 is x < 5, so asserting that it is
; x < 4 fails for x = 4 only.
; RUN:
; EXPECT: ^VERIFICATION FAILED$
declare i32 @__VERIFIER_nondet_int()
declare void @__VERIFIER_assert(i32)

define <4 x i1> @cmp(<4 x i32> %a, <4 x i32> %b) noinline {
  %c = icmp slt <4 x i32> %a, %b
  ret <4 x i1> %c
}

define i32 @main() {
  %x = call i32 @__VERIFIER_nondet_int()
  %v = insertelement <4 x i32> <i32 0, i32 1, i32 2, i32 3>, i32 %x, i32 0
  %c = call <4 x i1> @cmp(<4 x i32> %v, <4 x i32> <i32 5, i32 5, i32 1, i32 5>)
  %e0 = extractelement <4 x i1> %c, i32 0
  %lt = icmp slt i32 %x, 4
  %ok = icmp eq i1 %e0, %lt
  %z = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %z)
  ret i32 0
}
