; llvm.ptrmask on a 32-bit target with an i32 mask.
; EXPECT: ^VERIFICATION SUCCESSFUL$
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-i128:128-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-unknown-linux-gnu"

declare ptr @llvm.ptrmask.p0.i32(ptr, i32)
declare void @__VERIFIER_assert(i32)

define i32 @main() {
entry:
  %a = alloca [16 x i8], align 8
  store i32 77, ptr %a, align 8
  %a3 = getelementptr i8, ptr %a, i32 3
  %am = call ptr @llvm.ptrmask.p0.i32(ptr %a3, i32 -8)
  %v = load i32, ptr %am, align 4
  %eq = icmp eq i32 %v, 77
  %z = zext i1 %eq to i32
  call void @__VERIFIER_assert(i32 %z)
  ret i32 0
}
