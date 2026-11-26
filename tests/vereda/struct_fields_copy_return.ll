; EXPECT: ^VERIFICATION SUCCESSFUL$
; A memcpy result is an alias: writes through it must preserve pointer bytes
; and provenance even when the declared destination fields are integers.
target datalayout = "e-p:64:64-i64:64"
%raw = type { i64, i8 }
%pointer = type { ptr, i8 }
@a = global [16 x %raw] zeroinitializer
@b = global [16 x %raw] zeroinitializer
@c = global [16 x %raw] zeroinitializer
@d = global [16 x %pointer] zeroinitializer
@target = global i32 7
@other = global i32 9
declare ptr @memcpy(ptr, ptr, i64)
declare i64 @__VERIFIER_nondet_ulonglong()
declare void @__VERIFIER_assume(i1)
declare void @__VERIFIER_assert(i1)
define i32 @main() {
  %ti = ptrtoint ptr @target to i64
  %oi = ptrtoint ptr @other to i64
  %delta = sub i64 %oi, %ti
  %layout = icmp eq i64 %delta, 4096
  call void @__VERIFIER_assume(i1 %layout)
  %saved = getelementptr i8, ptr @target, i64 4096
  %i = call i64 @__VERIFIER_nondet_ulonglong()
  %within = icmp ult i64 %i, 16
  call void @__VERIFIER_assume(i1 %within)
  %initial = getelementptr [16 x %raw], ptr @a, i64 0, i64 %i, i32 0
  store i64 0, ptr %initial
  %alias = call ptr @memcpy(ptr @b, ptr @a, i64 256)
  %hidden = getelementptr %raw, ptr %alias, i64 %i, i32 0
  store ptr %saved, ptr %hidden
  %bp = getelementptr [16 x %raw], ptr @b, i64 0, i64 %i, i32 0
  %bits = load i64, ptr %bp
  %savedbits = ptrtoint ptr %saved to i64
  %bitsok = icmp eq i64 %bits, %savedbits
  call void @__VERIFIER_assert(i1 %bitsok)
  %flag = getelementptr [16 x %raw], ptr @b, i64 0, i64 %i, i32 1
  %f = load i8, ptr %flag
  %flagok = icmp eq i8 %f, 0
  call void @__VERIFIER_assert(i1 %flagok)
  call ptr @memcpy(ptr @c, ptr @b, i64 256)
  %ci = getelementptr [16 x %raw], ptr @c, i64 0, i64 %i, i32 0
  %cbits = load i64, ptr %ci
  %cbitsok = icmp eq i64 %cbits, %savedbits
  call void @__VERIFIER_assert(i1 %cbitsok)
  %cf = getelementptr [16 x %raw], ptr @c, i64 0, i64 %i, i32 1
  %ctag = load i8, ptr %cf
  %ctagok = icmp eq i8 %ctag, 0
  call void @__VERIFIER_assert(i1 %ctagok)
  call ptr @memcpy(ptr @d, ptr @c, i64 256)
  %cp = getelementptr [16 x %pointer], ptr @d, i64 0, i64 %i, i32 0
  %restored = load ptr, ptr %cp
  %same = icmp eq ptr %restored, %saved
  call void @__VERIFIER_assert(i1 %same)
  ret i32 0
}
