; A vector of i1 crosses a call: the callee compares two vectors and
; returns the <4 x i1> the scalarizer rebuilds from its lanes, and the
; caller reads lanes 0 and 2 back. Lane 0 is x < 5, lane 2 is 2 < 1.
; RUN:
; EXPECT: ^VERIFICATION SUCCESSFUL$
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
  %e2 = extractelement <4 x i1> %c, i32 2
  %lt = icmp slt i32 %x, 5
  %ok0 = icmp eq i1 %e0, %lt
  %ok2 = xor i1 %e2, true
  %ok = and i1 %ok0, %ok2
  %z = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %z)
  ret i32 0
}
