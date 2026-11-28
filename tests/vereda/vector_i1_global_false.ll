; Lane k of g, a global <8 x i1> whose lanes are the bits of 0x8d, asserted
; to be bit k of 0x9d: the two differ only in bit 4, so this fails for
; k = 4 only.
; RUN:
; EXPECT: ^VERIFICATION FAILED$
declare i32 @__VERIFIER_nondet_int()
declare void @__VERIFIER_assert(i32)

@g = global <8 x i1> <i1 true, i1 false, i1 true, i1 true, i1 false, i1 false, i1 false, i1 true>

define <8 x i1> @id8(<8 x i1> %v) {
  ret <8 x i1> %v
}

define i32 @main() {
  %gb = load i8, ptr @g
  %k = call i32 @__VERIFIER_nondet_int()
  %k8 = and i32 %k, 7
  %gv = bitcast i8 %gb to <8 x i1>
  %gw = call <8 x i1> @id8(<8 x i1> %gv)
  %ge = extractelement <8 x i1> %gw, i32 %k8
  %gs = trunc i32 %k8 to i8
  %gx = lshr i8 -99, %gs
  %gbit = trunc i8 %gx to i1
  %ok = icmp eq i1 %ge, %gbit
  %c = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %c)
  ret i32 0
}
