; EXPECT: ^VERIFICATION SUCCESSFUL$
; A non-inbounds GEP's arithmetic wraps at the original pointer width.
%entry = type { i8, i16 }
@a = global [256 x %entry] zeroinitializer
declare i64 @__VERIFIER_nondet_ulonglong()
declare void @__VERIFIER_assert(i1)
define i32 @main() {
  %initial = getelementptr [256 x %entry], ptr @a, i64 0, i64 0, i32 1
  store i16 42, ptr %initial
  %n = call i64 @__VERIFIER_nondet_ulonglong()
  %index = shl i64 %n, 62
  %p = getelementptr [256 x %entry], ptr @a, i64 0, i64 %index, i32 1
  %v = load i16, ptr %p
  %ok = icmp eq i16 %v, 42
  call void @__VERIFIER_assert(i1 %ok)
  ret i32 0
}
