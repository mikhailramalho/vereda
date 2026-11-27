; A global vector of i1 holds element i in bit i, as clang reads a bool
; ext_vector: g's lanes 1, 0, 1, 1, 0, 0, 0, 1 are the byte 0x8d, and h's
; lanes 0, 1, 1, 0 are 6 in its low four bits. Each byte, bitcast back to
; a vector that crosses a call, gives the lane at a nondet index k. g's
; vector stored into out and loaded back, as a vector and as a byte, is
; the same.
; RUN:
; EXPECT: ^VERIFICATION SUCCESSFUL$
declare i32 @__VERIFIER_nondet_int()
declare void @__VERIFIER_assert(i32)

@g = global <8 x i1> <i1 true, i1 false, i1 true, i1 true, i1 false, i1 false, i1 false, i1 true>
@h = global <4 x i1> <i1 false, i1 true, i1 true, i1 false>
@out = global <8 x i1> zeroinitializer

define <8 x i1> @id8(<8 x i1> %v) {
  ret <8 x i1> %v
}

define <4 x i1> @id4(<4 x i1> %v) {
  ret <4 x i1> %v
}

define void @put(<8 x i1> %v) {
  store <8 x i1> %v, ptr @out
  ret void
}

define <8 x i1> @get() {
  %v = load <8 x i1>, ptr @out
  ret <8 x i1> %v
}

define void @check(i1 %ok) {
  %c = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %c)
  ret void
}

define i32 @main() {
  %gb = load i8, ptr @g
  %g.ok = icmp eq i8 %gb, -115
  call void @check(i1 %g.ok)
  %hb = load i8, ptr @h
  %h4 = trunc i8 %hb to i4
  %h.ok = icmp eq i4 %h4, 6
  call void @check(i1 %h.ok)
  %k = call i32 @__VERIFIER_nondet_int()
  %k8 = and i32 %k, 7
  %gv = bitcast i8 %gb to <8 x i1>
  %gw = call <8 x i1> @id8(<8 x i1> %gv)
  %ge = extractelement <8 x i1> %gw, i32 %k8
  %gs = trunc i32 %k8 to i8
  %gx = lshr i8 -115, %gs
  %gbit = trunc i8 %gx to i1
  %ge.ok = icmp eq i1 %ge, %gbit
  call void @check(i1 %ge.ok)
  %k4 = and i32 %k, 3
  %hv = bitcast i4 %h4 to <4 x i1>
  %hw = call <4 x i1> @id4(<4 x i1> %hv)
  %he = extractelement <4 x i1> %hw, i32 %k4
  %hs = trunc i32 %k4 to i4
  %hx = lshr i4 6, %hs
  %hbit = trunc i4 %hx to i1
  %he.ok = icmp eq i1 %he, %hbit
  call void @check(i1 %he.ok)
  call void @put(<8 x i1> %gw)
  %ob = load i8, ptr @out
  %ob.ok = icmp eq i8 %ob, -115
  call void @check(i1 %ob.ok)
  %ov = call <8 x i1> @get()
  %oe = extractelement <8 x i1> %ov, i32 %k8
  %oe.ok = icmp eq i1 %oe, %gbit
  call void @check(i1 %oe.ok)
  ret i32 0
}
