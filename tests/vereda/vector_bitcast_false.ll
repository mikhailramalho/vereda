; An i64 bitcast to <4 x i16> has the low 16 bits in lane 0: x's, so
; asserting that lane 0 is not 5 fails when x's low 16 bits are 5 only.
; RUN:
; EXPECT: ^VERIFICATION FAILED$
declare i32 @__VERIFIER_nondet_int()
declare void @__VERIFIER_assert(i32)

define <4 x i16> @split(i64 %w) noinline {
  %v = bitcast i64 %w to <4 x i16>
  ret <4 x i16> %v
}

define i32 @main() {
  %x = call i32 @__VERIFIER_nondet_int()
  %x64 = zext i32 %x to i64
  %w = or i64 %x64, 30064771072
  %s = call <4 x i16> @split(i64 %w)
  %s0 = extractelement <4 x i16> %s, i32 0
  %ok = icmp ne i16 %s0, 5
  %z = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %z)
  ret i32 0
}
