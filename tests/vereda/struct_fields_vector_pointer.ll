; EXPECT: ^VERIFICATION SUCCESSFUL$
; Pointer-vector initializers copied through integer fields must retain provenance.
target datalayout = "e-p:64:64-i64:64-v128:128"
%source = type { <2 x ptr>, i64, i8 }
%raw = type { i64, i64, i64, i8 }
%pointer = type { ptr, i64, i64, i8 }
@target = global i32 7
@other = global i32 9
@a = global [2 x %source] [
  %source { <2 x ptr> <ptr getelementptr (i8, ptr @target, i64 4096), ptr null>, i64 0, i8 0 },
  %source { <2 x ptr> <ptr getelementptr (i8, ptr @target, i64 4096), ptr null>, i64 0, i8 0 }]
@b = global [2 x %raw] zeroinitializer
@c = global [2 x %pointer] zeroinitializer
declare void @llvm.memcpy.p0.p0.i64(ptr, ptr, i64, i1 immarg)
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
  %savedbits = ptrtoint ptr %saved to i64
  %i = call i64 @__VERIFIER_nondet_ulonglong()
  %within = icmp ult i64 %i, 2
  call void @__VERIFIER_assume(i1 %within)
  %ap = getelementptr [2 x %source], ptr @a, i64 0, i64 %i, i32 0
  %abits = load i64, ptr %ap
  %aok = icmp eq i64 %abits, %savedbits
  call void @__VERIFIER_assert(i1 %aok)
  %af = getelementptr [2 x %source], ptr @a, i64 0, i64 %i, i32 2
  %atag = load i8, ptr %af
  %atagok = icmp eq i8 %atag, 0
  call void @__VERIFIER_assert(i1 %atagok)
  call void @llvm.memcpy.p0.p0.i64(ptr @b, ptr @a, i64 64, i1 false)
  %bp = getelementptr [2 x %raw], ptr @b, i64 0, i64 %i, i32 0
  %bits = load i64, ptr %bp
  %bitsok = icmp eq i64 %bits, %savedbits
  call void @__VERIFIER_assert(i1 %bitsok)
  %bf = getelementptr [2 x %raw], ptr @b, i64 0, i64 %i, i32 3
  %tag = load i8, ptr %bf
  %tagok = icmp eq i8 %tag, 0
  call void @__VERIFIER_assert(i1 %tagok)
  call void @llvm.memcpy.p0.p0.i64(ptr @c, ptr @b, i64 64, i1 false)
  %cp = getelementptr [2 x %pointer], ptr @c, i64 0, i64 %i, i32 0
  %restored = load ptr, ptr %cp
  %same = icmp eq ptr %restored, %saved
  call void @__VERIFIER_assert(i1 %same)
  ret i32 0
}
