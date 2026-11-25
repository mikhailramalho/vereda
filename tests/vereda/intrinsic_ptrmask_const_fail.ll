; Writing through a pointer produced by llvm.ptrmask preserves the
; underlying const global's provenance, so pointer-check catches the write.
; EXPECT: dereference failure: write access to const object `limit'
; EXPECT: ^VERIFICATION FAILED$
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@limit = constant i32 10, align 8

declare ptr @llvm.ptrmask.p0.i64(ptr, i64)

define i32 @main() {
entry:
  %p = getelementptr i8, ptr @limit, i64 3
  %m = call ptr @llvm.ptrmask.p0.i64(ptr %p, i64 -8)
  store i32 20, ptr %m, align 4
  ret i32 0
}
