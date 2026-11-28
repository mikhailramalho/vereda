; frem is C's fmod, exact and truncating, unlike SMT's fp.rem: on constants
; it folds with APFloat, keeping the dividend's sign (-4 rem 2 is -0) and
; subnormals (tests/vereda/fmod_*.c cover other values). The operands are
; loaded from globals so that InstCombine leaves the frem to Vereda.
; EXPECT: ^VERIFICATION SUCCESSFUL$
@x = global [6 x double] [double 5.5, double -5.5, double -4.0, double 1.0e308, double 0x0000000000000001, double 0.1]
@y = global [6 x double] [double 2.0, double 2.0, double 2.0, double 3.0, double 1.0, double 0.03]
; fmod(x[i], y[i]) as bits, from the host's fmod
@r = global [6 x i64] [i64 u0x3ff8000000000000, i64 u0xbff8000000000000, i64 u0x8000000000000000, i64 u0x4000000000000000, i64 1, i64 u0x3f847ae147ae1480]

declare void @__VERIFIER_assert(i32)

define i32 @main() {
entry:
  br label %loop
loop:
  %i = phi i64 [ 0, %entry ], [ %next, %loop ]
  %px = getelementptr [6 x double], ptr @x, i64 0, i64 %i
  %py = getelementptr [6 x double], ptr @y, i64 0, i64 %i
  %pr = getelementptr [6 x i64], ptr @r, i64 0, i64 %i
  %a = load double, ptr %px
  %b = load double, ptr %py
  %want = load i64, ptr %pr
  %rem = frem double %a, %b
  %got = bitcast double %rem to i64
  %ok = icmp eq i64 %got, %want
  %okw = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %okw)
  %next = add i64 %i, 1
  %more = icmp ult i64 %next, 6
  br i1 %more, label %loop, label %done
done:
  ret i32 0
}
