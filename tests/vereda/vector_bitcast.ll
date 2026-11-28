; Bitcasts between a vector and a scalar keep the bits, lane 0 lowest:
; <2 x i32> to i64 and i64 to <4 x i16>, <4 x i1> to i4 and i8 to
; <8 x i1>, each with the vector crossing a call.
; RUN:
; EXPECT: ^VERIFICATION SUCCESSFUL$
declare i32 @__VERIFIER_nondet_int()
declare void @__VERIFIER_assert(i32)

define void @check(i1 %ok) {
  %c = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %c)
  ret void
}

define <2 x i32> @mk(i32 %x) noinline {
  %v = insertelement <2 x i32> <i32 0, i32 7>, i32 %x, i32 0
  ret <2 x i32> %v
}

define <4 x i16> @split(i64 %w) noinline {
  %v = bitcast i64 %w to <4 x i16>
  ret <4 x i16> %v
}

define <4 x i1> @mask(<4 x i32> %a) noinline {
  %c = icmp slt <4 x i32> %a, <i32 5, i32 5, i32 5, i32 5>
  ret <4 x i1> %c
}

define <8 x i1> @bits(i8 %b) noinline {
  %v = bitcast i8 %b to <8 x i1>
  ret <8 x i1> %v
}

define i32 @main() {
  %x = call i32 @__VERIFIER_nondet_int()
  %v = call <2 x i32> @mk(i32 %x)
  %w = bitcast <2 x i32> %v to i64
  %lo = trunc i64 %w to i32
  %hi64 = lshr i64 %w, 32
  %hi = trunc i64 %hi64 to i32
  %ok1 = icmp eq i32 %lo, %x
  call void @check(i1 %ok1)
  %ok2 = icmp eq i32 %hi, 7
  call void @check(i1 %ok2)
  %s = call <4 x i16> @split(i64 %w)
  %s0 = extractelement <4 x i16> %s, i32 0
  %x16 = trunc i32 %x to i16
  %ok3 = icmp eq i16 %s0, %x16
  call void @check(i1 %ok3)
  %s2 = extractelement <4 x i16> %s, i32 2
  %ok4 = icmp eq i16 %s2, 7
  call void @check(i1 %ok4)

  %a = insertelement <4 x i32> <i32 0, i32 9, i32 1, i32 9>, i32 %x, i32 0
  %m = call <4 x i1> @mask(<4 x i32> %a)
  %mi = bitcast <4 x i1> %m to i4
  %lt = icmp slt i32 %x, 5
  %want = select i1 %lt, i4 5, i4 4
  %ok7 = icmp eq i4 %mi, %want
  call void @check(i1 %ok7)

  %b = trunc i32 %x to i8
  %bv = call <8 x i1> @bits(i8 %b)
  %b3 = extractelement <8 x i1> %bv, i32 3
  %sh = lshr i8 %b, 3
  %bit3 = trunc i8 %sh to i1
  %ok8 = icmp eq i1 %b3, %bit3
  call void @check(i1 %ok8)
  ret i32 0
}
