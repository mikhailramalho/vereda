; Masking an unaligned pointer with -16 can change its address (offset < 0),
; so asserting the masked pointer equals the original must fail.
; EXPECT: ^VERIFICATION FAILED$
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

declare ptr @llvm.ptrmask.p0.i64(ptr, i64)
declare void @__VERIFIER_assert(i32)

define i32 @main() {
entry:
  %u = alloca [16 x i8], align 1
  %um = call ptr @llvm.ptrmask.p0.i64(ptr %u, i64 -16)
  %eq = icmp eq ptr %um, %u
  %z = zext i1 %eq to i32
  call void @__VERIFIER_assert(i32 %z)
  ret i32 0
}
